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

/*    MAC REPORT
 *
 * This message provides details about MAC layer reporting
 */

#ifndef __EMAGE_CELL_MAC_REPORT_H
#define __EMAGE_CELL_MAC_REPORT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
 * Cell MAC report messages:
 */

typedef struct __ep_cell_mac_report_reply {
	uint8_t  DL_prbs_total;
	uint32_t DL_prbs_used;
	uint8_t  UL_prbs_total;
	uint32_t UL_prbs_used;
}__attribute__((packed)) ep_macrep_rep;

typedef struct __ep_cell_mac_report_request {
	uint16_t interval;
}__attribute__((packed)) ep_macrep_req;

/******************************************************************************
 * Opaque structures                                                          *
 ******************************************************************************/

typedef struct ep_cell_mac_report_details {
	uint8_t  DL_prbs_total;
	uint32_t DL_prbs_used;
	uint8_t  UL_prbs_total;
	uint32_t UL_prbs_used;
} ep_macrep_det;

/******************************************************************************
 * Operation on single-event messages                                         *
 ******************************************************************************/

/******************************************************************************
 * Operation on schedule-event messages                                       *
 ******************************************************************************/

/******************************************************************************
 * Operation on trigger-event messages                                        *
 ******************************************************************************/

/* Format a MAC report negative reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_macrep_rep_fail(
	char *        buf,
	unsigned int  size,
	uint32_t      enb_id,
	uint16_t      cell_id,
	uint32_t      mod_id);

/* Format a MAC report not-supported reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_macrep_rep_ns(
	char *        buf,
	unsigned int  size,
	uint32_t      enb_id,
	uint16_t      cell_id,
	uint32_t      mod_id);

/* Format a MAC report reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_macrep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t        enb_id,
	uint16_t        cell_id,
	uint32_t        mod_id,
	ep_macrep_det * det);

/* Parse a MAC report reply looking for the desired fields */
int epp_trigger_macrep_rep(
	char *          buf,
	unsigned int    size,
	ep_macrep_det * det);

/* Format a MAC report request.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_macrep_req(
	char *          buf,
	unsigned int    size,
	uint32_t        enb_id,
	uint16_t        cell_id,
	uint32_t        mod_id,
	/* Interval for statistic measurements, in 'ms' */
	uint16_t        interval);

/* Parse a MAC report request looking for the desired fields */
int epp_trigger_macrep_req(
	char *          buf,
	unsigned int    size,
	uint16_t *      interval);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_CELL_MAC_REPORT_H */
