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

/*    TRIGGER-EVENT MESSAGES
 *
 * These types of messages defines an event which can occurs at any time and at
 * any desired frequency. This means that is possible which to occurs 0, 1 or
 * many times, every which its own independent interval.
 */

#ifndef __EMAGE_PROTOCOLS_TRIGGER_H
#define __EMAGE_PROTOCOLS_TRIGGER_H

#include <stdint.h>

#include "epop.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
 * Possible types of schedule messages that can occurs
 */


/*
 * Schedule-event messages header
 */

typedef struct __ep_trigger_header {
	uint8_t type;
	uint8_t dir;  /* Direction of the message, see epdir.h */
	uint8_t op;   /* Operation type, see epop.h */
}__attribute__((packed)) ep_t_hdr;

/* Format a trigger-event message */
int epf_trigger(
	char * buf, unsigned int size,
	ep_act_type type,
	ep_op_type  op,
	ep_dir_type dir);

/* Extracts the direction on an Empower trigger message */
ep_dir_type epp_trigger_dir(char * buf, unsigned int size);

/* Parse the operation type for a trigger message */
ep_op_type  epp_trigger_op(char * buf, unsigned int size);

/* Extracts the trigger type from an Empower message */
ep_act_type  epp_trigger_type(char * buf, unsigned int size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_PROTOCOLS_SCHEDULE_H */
