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

#ifndef ROCKETDMX_H
#define ROCKETDMX_H

#include <stdint.h>
#include <stdbool.h>
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif


#define RDMX_INTERFACES 1

#define RDMX_MMIO_BASEADDRS { (uint32_t*) 0x9000 }
#define RDMX_IRQS { 5 }
#define RDMX_IRQ_PRIO 5

#define RDMX_OK 0
#define RDMX_ERR_INVAL -1
#define RDMX_ERR_BUSY -2
#define RDMX_ERR_HW -3


typedef enum {
    RDMX_INACTIVE,
    RDMX_ACTIVE_SB,
	RDMX_ACTIVE_PAGE0,
	RDMX_FLIPPING_0_TO_1,
	RDMX_FLIPPING_1_TO_0,
	RDMX_ACTIVE_PAGE1,
	RDMX_SHUTTING_DOWN
} rdmx_txstat_t;

#ifndef USE_DMX_EMULATION

/**
 * RocketDMX Instantiation struct
 */
typedef struct {
	volatile uint32_t* baseaddr;  /**< base address of RocketDMX */
	unsigned int id;			  /**< driver ID */
	rdmx_txstat_t status;         /**< last read status */
	mutex_t hwMutex;			  /**< mutex for concurrent access to hw */
	long irq;					  /**< interrupt request */
	uint32_t controlRegInt;		  /**< last controll register access*/
	uint8_t startByte;            /**< start byte for dmx */
} rocketdmx_t;

#else

#include <X11/Xlib.h>
#define MAX_PAGES 2
#define MAX_PAGE_SIZE 512

typedef struct {
	unsigned int id;
	uint8_t framebuffer[MAX_PAGES][MAX_PAGE_SIZE];
	uint8_t enabled;
	uint8_t mode;
	uint8_t curPage;
	uint8_t displaySize;
	uint8_t frameSize;
	uint8_t startByte[MAX_PAGES];
	Display* dmxDisplay;
	Window   dmxWindow;
} rocketdmx_t;

#endif

int rdmx_init(rocketdmx_t* dev, unsigned int id);
int rdmx_is_busy(rocketdmx_t* dev);
int rdmx_begin_single_buffered(rocketdmx_t* dev, unsigned int channelCount);
int rdmx_begin_double_buffered(rocketdmx_t* dev, unsigned int channelCount);
int rdmx_flip_page(rocketdmx_t* dev);
int rdmx_shutdown(rocketdmx_t* dev);
int rdmx_set_start_byte(rocketdmx_t* dev, uint8_t sbyte);
int rdmx_set_channel(rocketdmx_t* dev, unsigned int channel, uint8_t value);
int rdmx_set_channels(rocketdmx_t* dev, unsigned int start, unsigned int count, uint8_t* values);


#ifdef __cplusplus
}
#endif

#endif /* ROCKETDMX_H */
