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

#ifndef USE_DMX_EMULATION

#include "rocketDMX.h"
#include "periph_cpu.h"
#include "vendor/plic_driver.h"
#include "thread.h"

static rocketdmx_t* DRIVERS[RDMX_INTERFACES];
static uint32_t* const BASEADDRS[RDMX_INTERFACES] = RDMX_MMIO_BASEADDRS;
static const long IRQS[RDMX_INTERFACES] = RDMX_IRQS;

static void rdmx_interrupt_callback(long irq) {
	for (int i = 0; i < RDMX_INTERFACES; i++) {
		if (irq == DRIVERS[i]->irq) {
			switch (DRIVERS[i]->status) {
			case RDMX_FLIPPING_0_TO_1:
				DRIVERS[i]->status = RDMX_ACTIVE_PAGE1;
				break;
			case RDMX_FLIPPING_1_TO_0:
				DRIVERS[i]->status = RDMX_ACTIVE_PAGE0;
				break;
			case RDMX_SHUTTING_DOWN:
				DRIVERS[i]->status = RDMX_INACTIVE;
				break;
			default:
				break;
			}
			return;
		}
	}
}

int rdmx_init(rocketdmx_t* dev, unsigned int id) {

	if (id >= RDMX_INTERFACES) {
		return RDMX_ERR_INVAL;
	}

	dev->id = id;

	mutex_init(&dev->hwMutex);
	mutex_lock(&dev->hwMutex);
	dev->baseaddr = BASEADDRS[id];
	dev->irq = IRQS[id];
	dev->controlRegInt = 0x001F0000;
	dev->baseaddr[512] = 0x001F0000;
	dev->startByte = 0;

	while(dev->baseaddr[512] & 0x40000000) {
		thread_yield();
	}

	set_external_isr_cb(IRQS[id], rdmx_interrupt_callback);
	PLIC_set_priority(dev->irq, RDMX_IRQ_PRIO);
	PLIC_enable_interrupt(dev->irq);

	DRIVERS[id] = dev;

	dev->status = RDMX_INACTIVE;

	mutex_unlock(&dev->hwMutex);
	return RDMX_OK;
}

int rdmx_is_busy(rocketdmx_t* dev) {
	return (dev->status == RDMX_SHUTTING_DOWN
			|| dev->status == RDMX_FLIPPING_0_TO_1
			|| dev->status == RDMX_FLIPPING_1_TO_0);
}

int rdmx_begin_single_buffered(rocketdmx_t* dev, unsigned int channelCount) {

	if (channelCount == 0 || channelCount > 512) {
		return RDMX_ERR_INVAL;
	}

	mutex_lock(&dev->hwMutex);

	if (dev->status != RDMX_INACTIVE) {
		mutex_unlock(&dev->hwMutex);
		return RDMX_ERR_BUSY;
	}

	dev->controlRegInt &= 0x0000FFFF;
	dev->controlRegInt |= ((uint32_t)(channelCount - 1)) << 16;
	dev->baseaddr[512] = dev->controlRegInt;
	dev->controlRegInt |= (1u << 31);
	dev->baseaddr[512] = dev->controlRegInt;
	dev->status = RDMX_ACTIVE_SB;
	mutex_unlock(&dev->hwMutex);
	return RDMX_OK;
}

int rdmx_begin_double_buffered(rocketdmx_t* dev, unsigned int channelCount) {

	if (channelCount == 0 || channelCount > 512) {
		return RDMX_ERR_INVAL;
	}

	mutex_lock(&dev->hwMutex);

	if (dev->status != RDMX_INACTIVE) {
		mutex_unlock(&dev->hwMutex);
		return RDMX_ERR_BUSY;
	}

	dev->controlRegInt &= 0x0000FFFF;
	dev->controlRegInt |= ((uint32_t)(channelCount - 1)) << 16;
	dev->controlRegInt |=  (1u << 31) | (1u << 27) | (1u << 26);
	dev->baseaddr[512] = dev->controlRegInt;
	dev->status = RDMX_ACTIVE_PAGE0;

	mutex_unlock(&dev->hwMutex);
	return RDMX_OK;
}

int rdmx_flip_page(rocketdmx_t* dev) {
	mutex_lock(&dev->hwMutex);

	if (dev->status == RDMX_ACTIVE_PAGE0) {
		dev->controlRegInt &= 0xFBFFFFFF; // Mask Interrupt
		dev->baseaddr[512] = dev->controlRegInt;	
		dev->status = RDMX_FLIPPING_0_TO_1;
	} else if (dev->status == RDMX_ACTIVE_PAGE1) {
		dev->controlRegInt &= 0xFBFFFFFF; // Mask Interrupt
		dev->baseaddr[512] = dev->controlRegInt;	
		dev->status = RDMX_FLIPPING_1_TO_0;
	} else {
		mutex_unlock(&dev->hwMutex);
		return RDMX_ERR_BUSY;
	}

	// Unmask interrupt and flip
	dev->controlRegInt |= 0x04000000; // Unmask Interrupt
	dev->baseaddr[512] = dev->controlRegInt | 0x10000000; 

	mutex_unlock(&dev->hwMutex);
	return RDMX_OK;
}

int rdmx_shutdown(rocketdmx_t* dev) {

	mutex_lock(&dev->hwMutex);

	if (dev->status == RDMX_INACTIVE) {
		mutex_unlock(&dev->hwMutex);
		return RDMX_ERR_INVAL;
	}

	if (rdmx_is_busy(dev)) {
		mutex_unlock(&dev->hwMutex);
		return RDMX_ERR_BUSY;
	}

	dev->controlRegInt &= 0xFBFFFFFF; // Mask Interrupt
	dev->baseaddr[512] = dev->controlRegInt;
	dev->status = RDMX_SHUTTING_DOWN;
	dev->controlRegInt |= 0x04000000;  // Unmask interrupt 
	dev->controlRegInt &= 0x7FFFFFFF;  // Disable
	dev->baseaddr[512] = dev->controlRegInt;

	mutex_unlock(&dev->hwMutex);
	return RDMX_OK;
}

int rdmx_set_start_byte(rocketdmx_t* dev, uint8_t sbyte) {
	mutex_lock(&dev->hwMutex);

	if (rdmx_is_busy(dev)) {
		mutex_unlock(&dev->hwMutex);
		return RDMX_ERR_BUSY;
	}

	if (dev->status == RDMX_ACTIVE_PAGE0) {
		dev->controlRegInt &= 0xFFFF00FF;
		dev->controlRegInt |= sbyte << 8;
	} else {
		dev->controlRegInt &= 0xFFFFFF00;
		dev->controlRegInt |= sbyte;
	}

	dev->baseaddr[512] = dev->controlRegInt;
	dev->startByte = sbyte;

	mutex_unlock(&dev->hwMutex);
	return RDMX_OK;
}

int rdmx_set_channel(rocketdmx_t* dev, unsigned int channel, uint8_t value) {

	if (channel > 511) {
		return RDMX_ERR_INVAL;
	}

	mutex_lock(&dev->hwMutex);

	if (rdmx_is_busy(dev)) {
		mutex_unlock(&dev->hwMutex);
		return RDMX_ERR_BUSY;
	}

	dev->baseaddr[channel] = (uint32_t) value;

	mutex_unlock(&dev->hwMutex);
	return RDMX_OK;
}

int rdmx_set_channels(rocketdmx_t* dev, unsigned int start, unsigned int count,
		uint8_t* values) {

	if (start + count > 512) {
		return RDMX_ERR_INVAL;
	}

	mutex_lock(&dev->hwMutex);

	if (rdmx_is_busy(dev)) {
		mutex_unlock(&dev->hwMutex);
		return RDMX_ERR_BUSY;
	}

	for (unsigned int i = 0; i < count; i++) {
		dev->baseaddr[i] = (uint32_t) values[i];
	}

	mutex_unlock(&dev->hwMutex);
	return RDMX_OK;
}

#else
typedef int dont_be_pedantic;
#endif
