/*
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 *
 * @author      Frederik Haxel <haxel@fzi.de>
 * @author      Leon Hielscher <hielscher@fzi.de>
 *
 */

#include <string.h>
#include <limits.h>
#include <errno.h>

#include "rocketif.h"
#include "rocketif_int.h"
#include "net/netdev.h"
#include "net/netdev/eth.h"
#include "net/ethernet/hdr.h"
#include "vendor/plic_driver.h"
#include "periph_cpu.h"
#include "thread.h"

#include <stdio.h>

#define ENABLE_DEBUG (0)
#include "debug.h"

static rocketif_t INTERFACES[RIF_INTERFACES];

static netdev_driver_t DEVDRIVERS[RIF_INTERFACES];
static struct rocketif_inpkgbuf RXBUFFERS[RIF_INTERFACES];

static const long TXIRQS[RIF_INTERFACES] = RIF_TX_IRQS;
static const long RXIRQS[RIF_INTERFACES] = RIF_RX_IRQS;
static const uint32_t* BASEADDRS[RIF_INTERFACES] = RIF_MMIO_BASE_ADDRS;
static const uint8_t MACADDRS[6 * RIF_INTERFACES] = RIF_MAC_ADDRS;

static void rif_isr(netdev_t *dev);

static void rocketif_interrupt_callback(long intnum) {
  for(int i = 0; i < RIF_INTERFACES; i++) {
    if (intnum == RXIRQS[i] || intnum == TXIRQS[i]) {
       rocketif_t *rocketIf = &INTERFACES[i];

       if (intnum == TXIRQS[i]) {
         // -- TX ISR ---
         volatile uint32_t* devPtr = rocketIf->baseAddr;
         if (devPtr[TXCTRL] & (1 << TXC_REG_ERR_BIT)) {
           rocketIf->txStatus = RIF_TX_ERROR;
         } else {
           rocketIf->txStatus = RIF_TX_COMPLETE;
         }
       }    

       rocketIf->netdev.event_callback(&rocketIf->netdev, NETDEV_EVENT_ISR);
    }
  }
}

static int rif_init(netdev_t *dev) {
  
  rocketif_t *rocketIf = (rocketif_t*)dev;
  volatile uint32_t* devPtr = rocketIf->baseAddr;

  mutex_lock(&rocketIf->txMutex);
  mutex_lock(&rocketIf->rxMutex);
  
  devPtr[RXCTRL] = 0x00;
  devPtr[TXCTRL] = 0;

  set_external_isr_cb(RXIRQS[rocketIf->id], rocketif_interrupt_callback);
  PLIC_set_priority(RXIRQS[rocketIf->id], RIF_RX_IRQ_PRIO);

  set_external_isr_cb(TXIRQS[rocketIf->id], rocketif_interrupt_callback);
  PLIC_set_priority(TXIRQS[rocketIf->id], RIF_TX_IRQ_PRIO);
  
  PLIC_enable_interrupt(TXIRQS[rocketIf->id]);
  PLIC_enable_interrupt(RXIRQS[rocketIf->id]);

  uint32_t rxReg = rocketIf->receiverFlags | (1 << RXC_REG_EN_BIT);

  /* cppcheck-suppress redundantAssignment
   * (reason: register is read by hw) */
  devPtr[TXCTRL] = (1 << TXC_REG_EN_BIT);
  devPtr[RXCTRL] = rxReg;
  
  if (devPtr[RXCTRL] == rxReg && 
        (devPtr[TXCTRL] & (1 << TXC_REG_EN_BIT)) != 0) {
    rocketIf->receiverFlags = rxReg;
    rocketIf->txStatus = RIF_TX_IDLE;
    mutex_unlock(&rocketIf->rxMutex);
    mutex_unlock(&rocketIf->txMutex);
    return RIF_ERR_OK;
  } else {
    devPtr[RXCTRL] = 0x00;
    devPtr[TXCTRL] = 0;
    rocketIf->txStatus = RIF_TX_UNINITIALIZED;

    PLIC_disable_interrupt(TXIRQS[rocketIf->id]);
    PLIC_disable_interrupt(RXIRQS[rocketIf->id]);

    mutex_unlock(&rocketIf->rxMutex);
    mutex_unlock(&rocketIf->txMutex);
    return RIF_ERR_HW_ERROR;
  }
}

static void rif_restart_rx(rocketif_t *rocketIf) {
  volatile uint32_t* devPtr = rocketIf->baseAddr;
  mutex_lock(&rocketIf->rxMutex);
  PLIC_disable_interrupt(RXIRQS[rocketIf->id]);
  
  while(devPtr[RXCTRL] != 0) {
    devPtr[RXCTRL] = 0;
  }

  PLIC_enable_interrupt(RXIRQS[rocketIf->id]);
  devPtr[RXCTRL] = rocketIf->receiverFlags | (1 << RXC_REG_EN_BIT);
  mutex_unlock(&rocketIf->rxMutex);
}

static int rif_send(netdev_t *dev, const iolist_t *iolist) {
  rocketif_t *rocketIf = (rocketif_t*)dev;
  volatile uint32_t* devPtr = rocketIf->baseAddr;
 // int err;

  size_t totalSize = iolist_size(iolist);
  if (totalSize > TX_MAX_PAYLOAD_SIZE) {
    return RIF_ERR_OVERFLOW;
  }

  while (!mutex_trylock(&rocketIf->txMutex)) {
    // Wait for ongoing transaction to finish and check for incoming data.
    thread_yield();
    rif_isr(dev);
  }

  if (rocketIf->txStatus != RIF_TX_IDLE) {
    mutex_unlock(&rocketIf->txMutex);
    dev->event_callback(dev, NETDEV_EVENT_TX_MEDIUM_BUSY);
    return RIF_ERR_NOT_READY;
  }

  rocketIf->txStatus = RIF_TX_BUSY;

  devPtr[TXCTRL] = 0;

  int shiftPos = 0;
  uint16_t totCnt = 0;
  uint32_t wrdBuf = 0;

  const iolist_t *iol;
  for (iol = iolist; iol != NULL; iol = iol->iol_next) {

	  for(size_t byte = 0; byte < iol->iol_len; byte++) {
		if (shiftPos == 0) {
			wrdBuf = 0;
		}
		wrdBuf += (uint32_t)(((uint8_t*)iol->iol_base)[byte] << (shiftPos * 8)); // Stupidly inefficient. TODO: Optimize in HW or just go for DMA.
		shiftPos = (shiftPos + 1) % 4;
		totCnt++;
      if (shiftPos == 0) {
        devPtr[TXDATA] = wrdBuf;
        devPtr[TXFIFO] = 0; // Push
      }
	  }

  }

	// Padding
	if (totCnt < 60) {
		totCnt += (4 - shiftPos) % 4;
		devPtr[TXDATA] = wrdBuf;
		devPtr[TXFIFO] = 0; // Push
		shiftPos = 0;
		wrdBuf = 0;

		devPtr[TXDATA] = 0;
		while (totCnt < 60) {
			devPtr[TXFIFO] = 0; // Push
			totCnt += 4;
		}
	}

 // signal that packet should be sent
  uint32_t ctrlNibble = 0;
  ctrlNibble = ~((1 << shiftPos) - 1);

  if (shiftPos == 0) {
  		wrdBuf = 0;
  }
  wrdBuf += (uint32_t)(TX_FIFO_EOF << (shiftPos * 8));
  shiftPos = (shiftPos + 1) % 4;

  while (shiftPos != 0) {
	  wrdBuf += (uint32_t)(TX_FIFO_PAD << (shiftPos * 8));
	  shiftPos = (shiftPos + 1) % 4;
  }
  devPtr[TXDATA] = wrdBuf;
  devPtr[TXFIFO] = ctrlNibble;
  devPtr[TXCTRL] = 0x04;

  return (int)totalSize;
}


// Lock rxMutex before calling!
static void update_input_buffer(rocketif_t *rocketIf) {
  uint32_t statWrd;
  struct rocketif_inpkgbuf *rxbuf = &RXBUFFERS[rocketIf->id];
  int padCount;

  volatile uint32_t* devPtr = rocketIf->baseAddr;
  rxbuf->flags = 0;
  rxbuf->readPos = 0;
  rxbuf->size = 0;

  statWrd = devPtr[RXFIFO];

  if (!(statWrd & 0x30)) { // Empty
    return;
  }

  if (statWrd & 0x20) {
    // MAC missmatch
	 if (rocketIf->receiverFlags & (1 << 3)) {
		 // Receive anyway
		 rxbuf->flags |= RXBUF_FLAG_MAC_MISSMATCH;
	 } else {
		 // Forget about it, the HW should already have discarded it.
		 return;
	 }
  }

  uint32_t len = 0;
  uint32_t *pldPtr = (uint32_t*)rxbuf->payload;

  while ((statWrd & 0xF) == 0) {
    
    if (statWrd & 0x30) {                         // Data present
      if (len < (RXBUF_PAYLOAD_SIZE / 4) - 1) {
        pldPtr[len] = devPtr[RXDATA];
        len++;
      } else {
        // Buffer overflow
        rxbuf->flags |= RXBUF_FLAG_BUF_OVERFLOW;
      }
      devPtr[RXFIFO] = 0; // Pop
    }
    statWrd = devPtr[RXFIFO];
	}

	uint32_t lastWrd = devPtr[RXDATA];
	pldPtr[len] = lastWrd;
	devPtr[RXFIFO] = 0; // Pop
	len *= 4;

  padCount = 3;
  while(!(statWrd & 1)) {
      len++;
      padCount--;
      statWrd >>= 1;
      lastWrd >>= 8;
    }

  rxbuf->flags |= (lastWrd & 0xFF);

  // Verify padding
  for (; padCount > 0; padCount--) {
    lastWrd >>= 8;
    if ((lastWrd & 0xFF) != RXBUF_FLAG_PADDING) {
      rxbuf->flags |= RXBUF_FLAG_HW_ERROR;
    }
  }

  // assert(len <= RXBUF_PAYLOAD_SIZE);

#if (ENABLE_DEBUG != 0)
  if (rxbuf->flags != RXBUF_FLAG_FCS_OK) {
    printf("\r\n\r\n --- LEN = %ul, FLAGS = 0x%04x--- \r\n", len, rxbuf->flags);
    for(size_t i = 0; i < (len / 4) + 1; i++) {
    printf( "%lu - \t 0x%02x 0x%02x 0x%02x 0x%02x \r\n" , i, (unsigned int) rxbuf->payload[(i * 4)], 
                                                    (unsigned int) rxbuf->payload[(i * 4) + 1],
                                                    (unsigned int) rxbuf->payload[(i * 4) + 2],
                                                    (unsigned int) rxbuf->payload[(i * 4) + 3] );
    }
  }
 #endif

  rxbuf->size = len;
}


static int rif_recv(netdev_t *dev, void *buf, size_t len, void *info) {
  rocketif_t *rocketIf = (rocketif_t*)dev;
  struct rocketif_inpkgbuf *rxbuf = &RXBUFFERS[rocketIf->id];

  size_t bytesRemaining = (size_t)(rxbuf->size - rxbuf->readPos);

  if (info != NULL) {
    *((rocketif_rxinfo_t*)info) = rxbuf->flags;
  }

  int bytesToRead = (bytesRemaining > INT_MAX) ? INT_MAX : (int)bytesRemaining; // size_t vs. int. Silly!

  if (buf == NULL) {
    if (len != 0) {
      // Drop
      rxbuf->size = 0;
      rxbuf->flags = 0;
      rxbuf->readPos = 0;
    }
    return bytesToRead;
  }

  if (len < (size_t)INT_MAX && (size_t)bytesToRead > len) {
    bytesToRead = (int)len;
  }

  memcpy(buf, &(rxbuf->payload[rxbuf->readPos]), (size_t)bytesToRead);

  if (rxbuf->readPos + bytesToRead >= rxbuf->size) {
    rxbuf->size = 0;
    rxbuf->flags = 0;
    rxbuf->readPos = 0;
  } else {
    rxbuf->readPos += bytesToRead;
  }

  return bytesToRead;
}

static void rif_isr(netdev_t *dev) {
  rocketif_t *rocketIf = (rocketif_t*)dev;
  struct rocketif_inpkgbuf *rxbuf = &RXBUFFERS[rocketIf->id];

  if (rxbuf->size == 0 && rxbuf->flags == 0) {
    // Let's see if there is new data
    mutex_lock(&rocketIf->rxMutex);
    update_input_buffer(rocketIf);
    mutex_unlock(&rocketIf->rxMutex);
  }

  if (rxbuf->size != 0 || rxbuf->flags != 0) { // We've got something
    if (rxbuf->size != 0 && rxbuf->flags == RXBUF_FLAG_FCS_OK) {  // Frame valid
      dev->event_callback(dev, NETDEV_EVENT_RX_COMPLETE);  
    } else  { // Something went wrong
      DEBUG("RX ERROR: Size = %ul, Flags = 0x%08x.\r\n.", rxbuf->size, rxbuf->flags );
      // Discard data
      rxbuf->size = 0;
      rxbuf->flags = 0;
      rxbuf->readPos = 0;
      rif_restart_rx(rocketIf);
      dev->event_callback(dev, NETDEV_EVENT_CRC_ERROR); // Or any other kind of error...
    }
    rocketIf->netdev.event_callback(&rocketIf->netdev, NETDEV_EVENT_ISR); // Schedule a new turn, there might be more.
  }

  if (rocketIf->txStatus == RIF_TX_COMPLETE) {
    rocketIf->txStatus = RIF_TX_IDLE;
    mutex_unlock(&rocketIf->txMutex);
    dev->event_callback(dev, NETDEV_EVENT_TX_COMPLETE);
  } else if (rocketIf->txStatus == RIF_TX_ERROR) {
    // TODO: Do some error handling
    rocketIf->txStatus = RIF_TX_IDLE;
    volatile uint32_t* devPtr = rocketIf->baseAddr;
    devPtr[TXCTRL] = 0;
    mutex_unlock(&rocketIf->txMutex);
    dev->event_callback(dev, NETDEV_EVENT_TX_MEDIUM_BUSY);
  }
}

static int rif_get(netdev_t *dev, netopt_t opt, void *value, size_t max_len) {
  int res = 0;
  rocketif_t *rocketIf = (rocketif_t*)dev;

  switch (opt) {
      case NETOPT_ADDRESS:
          if (max_len < ETHERNET_ADDR_LEN) {
              res = -EINVAL;
          }
          else {
              memcpy((uint8_t*)value, &(rocketIf->macAddr), 6);
              res = ETHERNET_ADDR_LEN;
          }
          break;
      default:
          res = netdev_eth_get(dev, opt, value, max_len);
          break;
  }

  return res;
}

static int rif_set(netdev_t *dev, netopt_t opt, const void *value, size_t value_len) {
  // TODO: Whatever...
  return netdev_eth_set(dev, opt, value, value_len);
}

rocketif_t *rocketif_create(unsigned int id) {

  if (id >= RIF_INTERFACES) {
    return NULL;
  }

  INTERFACES[id].id = id;
  INTERFACES[id].txStatus = RIF_TX_UNINITIALIZED;
  INTERFACES[id].baseAddr = (volatile uint32_t*)BASEADDRS[id];
  memcpy(INTERFACES[id].macAddr, &MACADDRS[6*id], 6);

  INTERFACES[id].receiverFlags = RX_INIT_MODE;

  mutex_init(&(INTERFACES[id].txMutex));
  mutex_init(&(INTERFACES[id].rxMutex));
  
  RXBUFFERS[id].size = 0;
  RXBUFFERS[id].flags = 0;

  DEVDRIVERS[id].init = rif_init;
  DEVDRIVERS[id].send = rif_send;
  DEVDRIVERS[id].recv = rif_recv;
  DEVDRIVERS[id].isr = rif_isr;
  DEVDRIVERS[id].set = rif_set;
  DEVDRIVERS[id].get = rif_get;
  INTERFACES[id].netdev.driver = &DEVDRIVERS[id];

  return &INTERFACES[id];
}

rocketif_t *rocketif_get_by_id(unsigned int id) {
  if (id >= RIF_INTERFACES) {
    return NULL;
  } else {
    return &INTERFACES[id];
  }
}




