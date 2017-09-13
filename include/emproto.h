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

/*
 *    EMPOWER AGENT PROTOCOLS
 *
 * Naming has not be done randomly:
 *     epf - Empower Protocol format, when you need to insert info
 *     epp - Empower Protocol parse, when you need to extract info
 *
 */

#ifndef __EMAGE_PROTOCOLS_H
#define __EMAGE_PROTOCOLS_H

#include "emproto/v1/epdefs.h"

/******************************************************************************
 * Operation on master header                                                 *
 ******************************************************************************/

/* Extracts the type from an Empower message */
ep_msg_type epp_msg_type(char * buf, unsigned int size);

/* Extracts the sequence number from the message */
uint32_t epp_seq(char * buf, unsigned int size);

/* Inject a sequence number in the header. */
int epf_seq(char * buf, unsigned int size, uint32_t seq);

/******************************************************************************
 * Operation on schedule-event messages                                       *
 ******************************************************************************/

/* Extracts the direction on an Empower schedule message */
ep_dir_type epp_schedule_dir(char * buf, unsigned int size);

/* Extracts the schedule type from an Empower message */
ep_sch_type epp_schedule_type(char * buf, unsigned int size);

/******************************************************************************
 * Operation on single-event messages                                         *
 ******************************************************************************/

/* Format an Hello request message with the desired fields.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_hello_req(
	char * buf, unsigned int size,
	uint32_t enb_id,
	uint16_t cell_id,
	uint32_t mod_id,
	uint32_t id);

/* Extracts the direction on an Empower single message */
ep_dir_type epp_single_dir(char * buf, unsigned int size);

/* Extracts the type from an Empower single message */
ep_sin_type epp_single_type(char * buf, unsigned int size);

/******************************************************************************
 * Operation on trigger-event messages                                        *
 ******************************************************************************/

/* Extracts the direction on an Empower trigger message */
ep_dir_type epp_trigger_dir(char * buf, unsigned int size);

/* Extracts the trigger type from an Empower message */
ep_tr_type  epp_trigger_type(char * buf, unsigned int size);

#endif /* __EMAGE_PROTOCOLS_H */
