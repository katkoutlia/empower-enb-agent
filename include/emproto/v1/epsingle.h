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

/*    SINGLE-EVENT MESSAGES
 *
 * These types of messages defines an event which occurs only once, with a
 * request/reply couple or even just one interaction. It's a fire-and-forget
 * message style.
 */

#ifndef __EMAGE_PROTOCOLS_SINGLE_H
#define __EMAGE_PROTOCOLS_SINGLE_H

#include <stdint.h>

/*
 * Possible types of single messages that can occurs
 */

typedef enum __ep_single_type {
	EP_SIN_INVALID_MSG = 0,
	EP_SIN_HELLO_MSG,
	EP_SIN_ECAP_MSG,
	EP_SIN_EXTENDED = 0xff
} ep_sin_type;

/*
 * Single-event messages header
 */

typedef struct __ep_single_header {
	uint8_t type;
	uint8_t dir;   /* Direction of the message, see epdir.h */
}__attribute__((packed)) ep_s_hdr;

/* Format a single-event message */
int epf_single(
	char * buf, unsigned int size, ep_sin_type type, ep_dir_type dir);

#endif /* __EMAGE_PROTOCOLS_SINGLE_H */
