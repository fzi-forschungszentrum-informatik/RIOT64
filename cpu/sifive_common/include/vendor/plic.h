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

#ifndef PLIC_H
#define PLIC_H


// 32 bits per source
#define PLIC_PRIORITY_OFFSET            (0x0000)
#define PLIC_PRIORITY_SHIFT_PER_SOURCE  2
// 1 bit per source (1 address)
#define PLIC_PENDING_OFFSET             (0x1000)
#define PLIC_PENDING_SHIFT_PER_SOURCE   0

//0x80 per target
#define PLIC_ENABLE_OFFSET              (0x2000)
#define PLIC_ENABLE_SHIFT_PER_TARGET    7


#define PLIC_THRESHOLD_OFFSET           (0x200000)
#define PLIC_CLAIM_OFFSET               (0x200004)
#define PLIC_THRESHOLD_SHIFT_PER_TARGET 12
#define PLIC_CLAIM_SHIFT_PER_TARGET     12

#define PLIC_MAX_SOURCE                 1023
#define PLIC_SOURCE_MASK                0x3FF

#define PLIC_MAX_TARGET                 15871
#define PLIC_TARGET_MASK                0x3FFF

#endif /* PLIC_H */
