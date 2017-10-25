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

/*    CELL CAPABILITIES MESSAGE
 *
 * This message provides the request/reply for/from cell capabilities.
 */

#ifndef __EMAGE_CELL_CAPABILITIES_H
#define __EMAGE_CELL_CAPABILITIES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Capabilities for a single Cell */
typedef enum __ep_cell_capabilities_types {
	/* Can't do anything except present himself */
	EP_CCAP_NOTHING    = 0,
	/* Cell can report about PHY layer status */
	EP_CCAP_PHY_REPORT = 1,
	/* Cell can report about MAC layer status */
	EP_CCAP_MAC_REPORT = 2,
} ep_ccap_type;

/*
 * Cell capabilities message
 */

typedef struct __ep_cell_capabilities_reply {
	uint16_t pci;       /* Physical cell id */
	uint32_t cap;       /* Cell capabilities */
	uint16_t DL_earfcn; /* Frequency at which the cell operates in DL */
	uint8_t  DL_prbs;   /* Physical Resource Block available in the DL */
	uint16_t UL_earfcn; /* Frequency at which the cell operates in UL */
	uint8_t  UL_prbs;   /* Physical Resource Block available in the UL */
}__attribute__((packed)) ep_ccap_rep;

typedef struct __ep_cell_capabilities_request {
	uint8_t dummy;
}__attribute__((packed)) ep_ccap_req;

/******************************************************************************
 * Operation on single-event messages                                         *
 ******************************************************************************/

typedef struct el_cell_details {
	uint16_t pci;
	uint32_t cap;
	uint16_t DL_earfcn;
	uint16_t UL_earfcn;
	uint8_t  DL_prbs;
	uint8_t  UL_prbs;
} ep_cell_det;

/* Format a cell capabilities negative reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ccap_rep_fail(
	char *        buf,
	unsigned int  size,
	uint32_t      enb_id,
	uint16_t      cell_id,
	uint32_t      mod_id);

/* Format a cell capabilities reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ccap_rep(
	char *        buf,
	unsigned int  size,
	uint32_t      enb_id,
	uint16_t      cell_id,
	uint32_t      mod_id,
	ep_cell_det * cell);

/* Parse a cell capabilities reply looking for the desired fields */
int epp_single_ccap_rep(
	char *        buf,
	unsigned int  size,
	ep_cell_det * cell);

/* Format a cell capabilities request.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ccap_req(
	char *       buf,
	unsigned int size,
	uint32_t     enb_id,
	uint16_t     cell_id,
	uint32_t     mod_id);

/* Parse a cell capabilities request for the desired fields */
int epp_single_ccap_req(char * buf, unsigned int size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_CELL_CAPABILITIES_H */
