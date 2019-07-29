/*
* Copyright 2016 SiFive, Inc.
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* 
* http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License. 
*/

/**
 * @ingroup     cpu_sifive_common
 * @ingroup     cpu
**/

#ifndef _SIFIVE_GPIO_H
#define _SIFIVE_GPIO_H

#define GPIO_INPUT_VAL  (0x00)
#define GPIO_INPUT_EN   (0x04)
#define GPIO_OUTPUT_EN  (0x08)
#define GPIO_OUTPUT_VAL (0x0C)
#define GPIO_PULLUP_EN  (0x10)
#define GPIO_DRIVE      (0x14)
#define GPIO_RISE_IE    (0x18)
#define GPIO_RISE_IP    (0x1C)
#define GPIO_FALL_IE    (0x20)
#define GPIO_FALL_IP    (0x24)
#define GPIO_HIGH_IE    (0x28)
#define GPIO_HIGH_IP    (0x2C)
#define GPIO_LOW_IE     (0x30)
#define GPIO_LOW_IP     (0x34)
#define GPIO_IOF_EN     (0x38)
#define GPIO_IOF_SEL    (0x3C)
#define GPIO_OUTPUT_XOR    (0x40)

#endif /* _SIFIVE_GPIO_H */
