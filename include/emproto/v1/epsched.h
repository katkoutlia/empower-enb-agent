/* Copyright (c) 2017 Kewin Rausch
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*    SCHEDULE-EVENT MESSAGES
 *
 * These types of messages defines an event which occurs every certain interval
 * of time.
 */

#ifndef __EMAGE_PROTOCOLS_SCHEDULE_H
#define __EMAGE_PROTOCOLS_SCHEDULE_H

#include <stdint.h>

#include "epop.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
 * Schedule-event messages header
 */

typedef struct __ep_schedule_header {
	uint8_t  type;
	uint8_t  dir;       /* Direction of the message, see epdir.h */
	uint8_t  op;        /* Operation type, see epop.h */
	uint32_t interval;  /* Interval of time in ms */
}__attribute__((packed)) ep_c_hdr;

/* Format a schedule-event message */
int epf_schedule(
	char * buf, unsigned int size,
	ep_act_type type,
	ep_op_type  op,
	ep_dir_type dir,
	uint32_t    interval);

/* Extracts the direction on an Empower schedule message */
ep_dir_type epp_schedule_dir(char * buf, unsigned int size);

/* Extracts the interval on an Empower schedule message */
uint32_t    epp_sched_interval(char * buf, unsigned int size);

/* Extracts the schedule type from an Empower message */
ep_act_type epp_schedule_type(char * buf, unsigned int size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_PROTOCOLS_SCHEDULE_H */
