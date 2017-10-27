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

/*    ENB CAPABILITIES MESSAGE
 *
 * This message provides the request/reply for/from eNB capabilities, which are
 * the possible operation which can be done within the base station.
 */

#ifndef __EMAGE_ENB_CAPABILITIES_H
#define __EMAGE_ENB_CAPABILITIES_H

#include <stdint.h>

#include "epcelcap.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define EP_ENCAP_MAX_CELLS      8

/* NOTE: Elements here must have power-of-two values */
typedef enum __ep_enb_capabilities_types {
	/* Can't do anything except present himself */
	EP_ECAP_NOTHING    = 0,
	/* Can report UEs */
	EP_ECAP_UE_REPORT  = 1,
	/* Can report UEs signal power */
	EP_ECAP_UE_MEASURE = 2,
	/* Can hand-over UE to other eNBs */
	EP_ECAP_HANDOVER   = 4,
} ep_ecap_type;

/*
 * eNB capabilities message
 */

typedef struct __ep_enb_capabilities_reply {
	uint32_t cap;                          /* eNB capabilities */
	uint32_t nof_cells;                    /* Number of cells  */
	/* Cells details are appended here, at the end of eNB message */
}__attribute__((packed)) ep_ecap_rep;

typedef struct __ep_enb_capabilities_request {
	uint8_t dummy;
}__attribute__((packed)) ep_ecap_req;

/******************************************************************************
 * Operation on single-event messages                                         *
 ******************************************************************************/

/* Format an eNB capabilities negative reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ecap_rep_fail(
	char *        buf,
	unsigned int  size,
	uint32_t      enb_id,
	uint16_t      cell_id,
	uint32_t      mod_id);

/* Format an eNB capabilities reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ecap_rep(
	char *        buf,
	unsigned int  size,
	uint32_t      enb_id,
	uint16_t      cell_id,
	uint32_t      mod_id,
	uint32_t      cap_mask,
	ep_cell_det * cells,
	uint32_t      nof_cells);

/* Parse an eNB capabilities reply looking for the desired fields */
int epp_single_ecap_rep(
	char *        buf,
	unsigned int  size,
	uint32_t *    cap_mask,
	ep_cell_det * cells,
	uint32_t *    nof_cells);

/* Format an eNB capabilities request.
 * Returns the size of the message, or a negative error number.
 */
int epf_single_ecap_req(
	char *       buf,
	unsigned int size,
	uint32_t     enb_id,
	uint16_t     cell_id,
	uint32_t     mod_id);

/* Parse an eNB capabilities request for the desired fields */
int epp_single_ecap_req(char * buf, unsigned int size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_ENB_CAPABILITIES_H */
