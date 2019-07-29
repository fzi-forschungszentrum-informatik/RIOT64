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
 *
 */

#ifdef USE_DMX_EMULATION

#include "rocketDMX.h"
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define WIN_HEIGHT 512
#define WIN_WIDTH 512

#define MODE_UNDEFINED -1
#define MODE_SB 0
#define MODE_DB 1

static int8_t _getCurPage(rocketdmx_t* dev) {
	if (dev->mode == MODE_DB) {
		return (MAX_PAGES + dev->curPage - 1) % MAX_PAGES;
	}
	return dev->curPage;
}

static void _initDisplay(rocketdmx_t* dev) {
	if (!dev->enabled) {
		dev->enabled = 1;
		dev->dmxDisplay = XOpenDisplay(NULL);
		if (dev->dmxDisplay == NULL) {
			fprintf(stderr, "Cannot open display\n");
			exit(1);
		}

		int s = DefaultScreen(dev->dmxDisplay);
		dev->dmxWindow = XCreateSimpleWindow(dev->dmxDisplay, RootWindow(dev->dmxDisplay, s), 20, 20, WIN_WIDTH, WIN_HEIGHT,
				1, BlackPixel(dev->dmxDisplay, s), WhitePixel(dev->dmxDisplay, s));
		XSelectInput(dev->dmxDisplay, dev->dmxWindow, ExposureMask | KeyPressMask);
		XMapWindow(dev->dmxDisplay, dev->dmxWindow);

		XEvent e;
		while (1) {
			XNextEvent(dev->dmxDisplay, &e);
			if (e.type == Expose || e.type == KeyPress) {
				XWindowAttributes wa;
				int s = DefaultScreen(dev->dmxDisplay);
				GC gc = DefaultGC(dev->dmxDisplay, s);
				XGetWindowAttributes(dev->dmxDisplay, dev->dmxWindow, &wa);
				XSetForeground(dev->dmxDisplay, gc, 0x00);
				XFillRectangle(dev->dmxDisplay, dev->dmxWindow, gc, 0, 0, wa.width, wa.height);
				break;
			}
		}
	}

}

uint8_t PALETTE[16][3] = {
	{0x00, 0x00, 0x00}, //0
	{0xFF, 0x00, 0x00}, //1
	{0xFF, 0x2A, 0x00}, //2
	{0xFF, 0x55, 0x00}, //3
	{0xFF, 0x80, 0x00}, //4
	{0xFF, 0xAA, 0x00}, //5
	{0xFF, 0xD5, 0x00}, //6
	{0xFF, 0xFF, 0x00}, //7
	{0xD4, 0xFF, 0x00}, //8
	{0xAA, 0xFF, 0x00}, //9
	{0x80, 0xFF, 0x00}, //10
	{0x55, 0xFF, 0x00}, //11
	{0x2B, 0xFF, 0x00}, //12
	{0x00, 0xFF, 0x00}, //13
	{0xFF, 0x00, 0x00}, //14
	{0xFF, 0xFF, 0x00}, //15
};

static void _updateDisplay(rocketdmx_t* dev) {
	if (dev->enabled) {
	    XWindowAttributes wa;
		int s = DefaultScreen(dev->dmxDisplay);
	    GC gc = DefaultGC(dev->dmxDisplay, s);
		XGetWindowAttributes(dev->dmxDisplay, dev->dmxWindow, &wa);
		float xSize = (wa.width * 1.0f) / dev->displaySize;
		float ySize = (wa.height * 1.0f) / dev->displaySize;
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;

		uint8_t pos = 0;
		uint8_t* framePtr = dev->framebuffer[_getCurPage(dev)];

		for (int yInc = 0; yInc < dev->displaySize; ++yInc) {
			float yS = yInc * ySize;
			float yE = (yInc + 1) * ySize;
			for (int xInc = 0; xInc < dev->displaySize; ++xInc) {
				float xS;
				float xE;
				xS = xInc * xSize;
				xE = (xInc + 1) * xSize;

//				if (yInc % 2 == 0) {
//					xS = xInc * xSize;
//					xE = (xInc + 1) * xSize;
//				} else {
//					xS = xSize - xInc * xSize - 1;
//					xE = xSize - (xInc + 1) * xSize - 1;
//				}

				uint8_t c;
				if (pos == 0) {
					c = *framePtr >> 0x4;
				} else {
					c = *framePtr & 0x0f;
				}
				r = PALETTE[c][0];
				g = PALETTE[c][1];
				b = PALETTE[c][2];
				if (c < 14) {
					r /= 1.4;
					g /= 1.4;
					b /= 1.4;
				}

				if (pos != 0) {
					++framePtr;
				}

				pos = !pos;

				XSetForeground(dev->dmxDisplay, gc, r << 16 | g << 8 | b << 0);
				XFillRectangle(dev->dmxDisplay, dev->dmxWindow, gc, xS, yS, xE, yE);
			}
		}

	}
}

static void _closeDisplay(rocketdmx_t* dev) {
	if (dev->enabled) {
		dev->enabled = 0;
		XCloseDisplay(dev->dmxDisplay);
	}
}

int rdmx_init(rocketdmx_t* dev, unsigned int id) {
	if (id >= RDMX_INTERFACES) {
		return RDMX_ERR_INVAL;
	}

	for (int i = 0; i < MAX_PAGES; ++i) {
		memset(&dev->framebuffer[i], 0, MAX_PAGE_SIZE);
	}

	dev->mode = MODE_UNDEFINED;
	dev->curPage = 0;
	dev->displaySize = 0;
	dev->frameSize = 0;
	dev->enabled = 0;

	_initDisplay(dev);

	return RDMX_OK;
}

int rdmx_is_busy(rocketdmx_t* dev) {
	(void) dev;
	return RDMX_OK;
}

int rdmx_begin_single_buffered(rocketdmx_t* dev, unsigned int channelCount) {
	if (channelCount == 0 || channelCount > MAX_PAGE_SIZE) {
		return RDMX_ERR_INVAL;
	}
	dev->curPage = MODE_SB;
	dev->displaySize = sqrt(channelCount*2);
	dev->frameSize = channelCount*2;

	_initDisplay(dev);
	_updateDisplay(dev);
	return RDMX_OK;
}

int rdmx_begin_double_buffered(rocketdmx_t* dev, unsigned int channelCount) {
	if (channelCount == 0 || channelCount > MAX_PAGE_SIZE) {
		return RDMX_ERR_INVAL;
	}
	dev->curPage = MODE_DB;
	dev->displaySize = sqrt(2*channelCount);
	dev->frameSize = 2*channelCount;

	_initDisplay(dev);
	_updateDisplay(dev);

	return RDMX_OK;
}

int rdmx_flip_page(rocketdmx_t* dev) {
	dev->curPage = (dev->curPage + 1) % MAX_PAGES;
	_updateDisplay(dev);
	return RDMX_OK;
}

int rdmx_shutdown(rocketdmx_t* dev) {
	dev->mode = MODE_UNDEFINED;
	_closeDisplay(dev);
	return RDMX_OK;
}

int rdmx_set_start_byte(rocketdmx_t* dev, uint8_t sbyte) {
	dev->startByte[_getCurPage(dev)] = sbyte;
	return RDMX_OK;
}

int rdmx_set_channel(rocketdmx_t* dev, unsigned int channel, uint8_t value) {
	int8_t curPage = _getCurPage(dev);
	if (channel >= MAX_PAGE_SIZE || curPage == -1) {
		return RDMX_ERR_INVAL;
	}

	dev->framebuffer[curPage][channel] = value;
	return RDMX_OK;
}

int rdmx_set_channels(rocketdmx_t* dev, unsigned int start, unsigned int count,
		uint8_t* values) {
	int8_t curPage = _getCurPage(dev);
	if (start + count > MAX_PAGE_SIZE  || curPage == -1) {
		return RDMX_ERR_INVAL;
	}

	uint8_t* dataPtr = values;
	for (unsigned int i = start; i < start + count; ++i) {
		rdmx_set_channel(dev, i, *dataPtr++);
	}

	return RDMX_OK;
}

#else
typedef int dont_be_pedantic;
#endif
