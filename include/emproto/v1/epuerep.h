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

/*    UE REPORT MESSAGE
 *
 * This message provides the request/reply for/from eNB capabilities, which are
 * the possible operation which can be done within the base station.
 */

#ifndef __EMAGE_UE_REPORT_H
#define __EMAGE_UE_REPORT_H

#include <endian.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
 * UE report messages
 */

typedef struct __ep_ue_report_details {
	uint16_t pci;     /* Cell on which the UE is attached */
	uint32_t plmn;    /* Public Land Mobile Network identifier */
	uint16_t rnti;    /* Radio Network Temporary Identifier */
	uint64_t imsi;    /* International Mobile Subscriber Identity */
}__attribute__((packed)) ep_uerep_det;

/* Structure of ep_uerep_rep:
 *      At the end of the UE report reply message there will be listed the
 *      details of the UEs up to max_ues.
 *
 *      || nof_ues | UE0 | UE1| ..... | UE[nof_ues - 1] ||
 *      Where: 0 <= nof_ues <= max_ues
 */
typedef struct __ep_ue_report_reply {
	uint32_t nof_ues; /* Number of UEs listed  */
	/*
	 * Here at the end of the message there will be listed the UEs details
	 */
}__attribute__((packed)) ep_uerep_rep;

typedef struct __ep_ue_report_request {
	uint8_t dummy;
}__attribute__((packed)) ep_uerep_req;

/******************************************************************************
 * Operation on single-event messages                                         *
 ******************************************************************************/

/******************************************************************************
 * Operation on schedule-event messages                                       *
 ******************************************************************************/

/******************************************************************************
 * Operation on trigger-event messages                                        *
 ******************************************************************************/

typedef struct __ep_ue_details {
	uint16_t pci;   /* Cell used by this UE to attach */

	uint32_t plmn;  /* Public Land Mobile Network identifier */
	uint16_t rnti;  /* Radio Network Temporary Identifier */
	uint64_t imsi;  /* International Mobile Subscriber Identity */

	/* TODO:
	 *   - RRC state?
	 */
} ep_ue_details;

/* Format an UE report reply failure.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_uerep_rep_fail(
	char *          buf,
	unsigned int    size,
	uint32_t        enb_id,
	uint16_t        cell_id,
	uint32_t        mod_id);

/* Format an UE report reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_uerep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t        enb_id,
	uint16_t        cell_id,
	uint32_t        mod_id,
	uint32_t        nof_ues,
	uint32_t        max_ues,
	ep_ue_details * ues);

/* Parse an UE report reply looking for the desired fields */
int epp_trigger_uerep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t *      nof_ues,
	uint32_t        max_ues,
	ep_ue_details * ues);

/* Format an UE report request.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_uerep_req(
	char *       buf,
	unsigned int size,
	uint32_t     enb_id,
	uint16_t     cell_id,
	uint32_t     mod_id,
	ep_op_type   op);

/* Parse an UE report request for the desired fields */
int epp_trigger_uerep_req(char * buf, unsigned int size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_UE_REPORT_H */
