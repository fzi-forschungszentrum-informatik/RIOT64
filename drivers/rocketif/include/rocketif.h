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

#ifndef ROCKETIF_H
#define ROCKETIF_H

#include <stdint.h>
#include "net/netdev.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RIF_ERR_OK          0
#define RIF_ERR_HW_ERROR    -1
#define RIF_ERR_NOT_READY   -2
#define RIF_ERR_OVERFLOW   -10


#define RIF_INTERFACES 1

#define RIF_MMIO_BASE_ADDRS { (uint32_t*)0x8000 }
#define RIF_MAC_ADDRS { 0xCA, 0xFE, 0xCA, 0xFE, 0xCA, 0xFE }
#define RIF_RX_IRQS { 3 }
#define RIF_TX_IRQS { 4 }

#define RIF_TX_IRQ_PRIO 2
#define RIF_RX_IRQ_PRIO 4

typedef enum {
    RIF_TX_UNINITIALIZED,
    RIF_TX_IDLE,
    RIF_TX_BUSY,
    RIF_TX_COMPLETE,
    RIF_TX_ERROR
} rocketif_txstat_t;

/**
 * RocketIF Instantiation struct
 */
typedef struct rocketif_drv {
    netdev_t netdev;				/**< RIOT device */
    unsigned int id;				/**< driver id */
    volatile uint32_t* baseAddr;    /**< hardware base register */
    uint8_t macAddr[6];             /**< NIC mac address */
    uint8_t receiverFlags;          /**< receiver flags */
    volatile rocketif_txstat_t txStatus;  /**< current status */
    mutex_t txMutex;               /**< transceiver mutex -- Always lock TX mutex first when both are needed!*/
    mutex_t rxMutex;			    /**< receiver mutex -- Always lock TX mutex first when both are needed!*/
    volatile uint8_t txBusy;        /**< current transceiver busy flag*/
} rocketif_t;

rocketif_t *rocketif_create(unsigned int id);
rocketif_t *rocketif_get_by_id(unsigned int id);

#ifdef __cplusplus
}
#endif

#endif /* ROCKETIF_H */
