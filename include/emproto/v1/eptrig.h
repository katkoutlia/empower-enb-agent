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

/*
 * Possible types of schedule messages that can occurs
 */

typedef enum __ep_trigger_type {
	EP_TR_INVALID_MSG = 0,
	EP_TR_HELLO_MSG,
	EP_TR_EXTENDED = 0xff
} ep_tr_type;

/*
 * Schedule-event messages header
 */

typedef struct __ep_trigger_header {
	uint8_t type;
	uint8_t dir;  /* Direction of the message, see epdir.h */
	uint8_t op;   /* Operation type, see epop.h */
}__attribute__((packed)) ep_t_hdr;

#endif /* __EMAGE_PROTOCOLS_SCHEDULE_H */
