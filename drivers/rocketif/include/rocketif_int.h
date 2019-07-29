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

#ifndef ROCKETIF_INT_H
#define ROCKETIF_INT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TXCTRL 0
#define RXCTRL 1
#define TXDATA 2
#define TXFIFO 3
#define RXDATA 4
#define RXFIFO 5

#define TXCNTR 6

#define RXC_REG_EN_BIT 3
#define TXC_REG_EN_BIT 2
#define TXC_REG_ERR_BIT 1

#define TX_FIFO_EOF 0x01
#define TX_FIFO_PAD 0x04

#define RXBUF_PAYLOAD_SIZE 1536
#define TX_MAX_PAYLOAD_SIZE 1536

#define RXBUF_FLAG_FCS_OK         0x001
#define RXBUF_FLAG_CORRUPTED      0x002
#define RXBUF_FLAG_PADDING        0x004
#define RXBUF_FLAG_FIFO_OVERFLOW  0x008
#define RXBUF_FLAG_MAC_MISSMATCH  0x100
#define RXBUF_FLAG_BUF_OVERFLOW   0x200
#define RXBUF_FLAG_HW_ERROR       0x400

#define RX_INIT_MODE 0x02

typedef uint32_t rocketif_rxinfo_t;
/**
 * RocketIF package buffer
 */
struct rocketif_inpkgbuf {
    uint8_t payload[RXBUF_PAYLOAD_SIZE]; 	/**< payload buffer of the package */
    uint32_t size;							/**< payload size */
    uint32_t readPos;						/**< current read position */
    rocketif_rxinfo_t flags;				/**< package flags */
} __attribute__ ((aligned(4)));

#ifdef __cplusplus
}
#endif

#endif /* ROCKETIF_INT_H */
