/*
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 *
 * @author      Leon Hielscher <hielscher@fzi.de>
 *
 */

#ifndef BOARD_H
#define BOARD_H


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);



#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
