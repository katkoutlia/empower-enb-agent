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

/*    MASTER HEADER
 *
 * Modifying the master header will cause a break in the protocol; in case where
 * it is necessary to add/remove some elements, dispatch a new version. Adding
 * new types does not break the protocol, and allows to maintain backward
 * compatibility.
 */

#ifndef __EMAGE_PROTOCOLS_HEADER_H
#define __EMAGE_PROTOCOLS_HEADER_H

#include <stdint.h>

#include "eptype.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Size of the header */
#define EP_HEADER_SIZE          sizeof(ep_hdr)

typedef struct __ep_header_id {
	uint32_t enb_id;        /* Base station identifier */
	uint16_t cell_id;       /* Physical cell id */
	uint32_t mod_id;        /* Module id */
}__attribute__((packed)) ep_hdrid;

typedef struct __ep_header {
	uint8_t  type;
	uint8_t  vers;
	ep_hdrid id;
	/* Length of the whole packet (headers + data) */
	uint16_t length;
	uint32_t seq;           /* Sequence number */
}__attribute__((packed)) ep_hdr;

/* Format a master header with the desired fields.
 * Returns the size of the message, or a negative error number.
 */
int epf_head(
	char * buf, unsigned int size,
	ep_msg_type type,
	uint32_t enb_id,
	uint16_t cell_id,
	uint32_t mod_id);

/* Parse a master header extracting the valuable fields.
 * Returns EP_SUCCESS, or an error code on failure.
 */
int epp_head(
	char * buf, unsigned int size,
	ep_msg_type * type,
	uint32_t * enb_id,
	uint16_t * cell_id,
	uint32_t * mod_id);

/* Extracts the type from an Empower message */
ep_msg_type epp_msg_type(char * buf, unsigned int size);

/* Extracts the sequence number from the message */
uint32_t    epp_seq(char * buf, unsigned int size);

/* Extracts the message length in the header. */
uint16_t    epp_msg_length(char * buf, unsigned int size);

/* Inject a sequence number in the header. */
int         epf_seq(char * buf, unsigned int size, uint32_t seq);

/* Inject the message length in the header. */
int         epf_msg_length(char * buf, unsigned int size, uint16_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_PROTOCOLS_HEADER_H */
