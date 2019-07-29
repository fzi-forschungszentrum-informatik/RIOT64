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

#ifndef PLIC_DRIVER_H
#define PLIC_DRIVER_H


typedef uint32_t plic_source;
typedef uint32_t plic_priority;
typedef uint32_t plic_threshold;


void PLIC_init (
                uintptr_t base_addr,
                uint32_t num_sources,
                uint32_t num_priorities
                );

void PLIC_set_threshold (plic_threshold threshold);
  
void PLIC_enable_interrupt (plic_source source);

void PLIC_disable_interrupt (plic_source source);
  
void PLIC_set_priority (
			plic_source source,
			plic_priority priority);

plic_source PLIC_claim_interrupt(void);

void PLIC_complete_interrupt(plic_source source);



#endif
