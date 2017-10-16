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

#ifndef __EMAGE_UE_MEASUREMENT_H
#define __EMAGE_UE_MEASUREMENT_H

#include <endian.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
 * UE measurements messages
 */

/* PCI not specified; eNB decide which measure to pass */
#define EP_UE_MEAS_ANY_PCI    0xffff
/* Frequency not specified; eNB decide which measure to pass */
#define EP_UE_MEAS_ANY_EARFCN 0xffff

typedef struct __ep_ue_measurements_details {
	uint8_t  meas_id; /* Id assigned for this measurement */
	int16_t  pci;     /* Physical Cell Id measured */
	int16_t  rsrp;    /* Reference Signal Received Power */
	int16_t  rsrq;    /* Reference Signal Received Quality */
}__attribute__((packed)) ep_uemeas_det;

/* Structure of ep_uemeas_rep:
 *      At the end of the UE measure reply message there will be listed the
 *      details of the UEs up to 'max_meas' specified in the request.
 *
 *      || nof_meas | meas 0 | meas 1 | ..... | meas [nof_meas - 1] ||
 *      Where: 1 <= nof_meas <= max_meas
 */
typedef struct __ep_ue_measurement_reply {
	uint32_t nof_meas; /* Number of measurements listed  */
	/* Multiple ep_uemeas_det listed here at the end */
}__attribute__((packed)) ep_uemeas_rep;

/* Request of a measurement
 *
 * max_cells and max_meas accepts -1, which indicates that the agent can decide
 * the amount of cells and measure to take into account.
 */
typedef struct __ep_ue_measurement_request {
	uint8_t  meas_id;  /* Id assigned for this measurement */
	uint16_t rnti;     /* RNTI which have to perform the measurement */
	uint16_t earfcn;   /* Frequency to measure */
	uint16_t interval; /* Interval (in ms) of the measure */
	int16_t  max_cells;/* Up to this number of cells  */
	int16_t  max_meas; /* Up to this number of measurements */
}__attribute__((packed)) ep_uemeas_req;

/******************************************************************************
 * Operation on single-event messages                                         *
 ******************************************************************************/

/******************************************************************************
 * Operation on schedule-event messages                                       *
 ******************************************************************************/

/******************************************************************************
 * Operation on trigger-event messages                                        *
 ******************************************************************************/

typedef struct __ep_ue_measure {
	uint8_t  meas_id; /* Id assigned for this measurement */
	uint16_t pci;     /* Physical Cell Id measured */
	uint16_t rsrp;    /* Reference Signal Received Power */
	uint16_t rsrq;    /* Reference Signal Received Quality */
} ep_ue_measure;

/* Format an UE measurement reply failure.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_uemeas_rep_fail(
	char *          buf,
	unsigned int    size,
	uint32_t        enb_id,
	uint16_t        cell_id,
	uint32_t        mod_id);

/* Format an UE measurement reply.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_uemeas_rep(
	char *          buf,
	unsigned int    size,
	uint32_t        enb_id,
	uint16_t        cell_id,
	uint32_t        mod_id,
	uint32_t        nof_meas,
	uint32_t        max,
	ep_ue_measure * meas);

/* Parse an UE measurement reply looking for the desired fields */
int epp_trigger_uemeas_rep(
	char *          buf,
	unsigned int    size,
	uint32_t *      nof_meas,
	uint32_t        max,
	ep_ue_measure * meas);

/* Format an UE measurement request.
 * Returns the size of the message, or a negative error number.
 */
int epf_trigger_uemeas_req(
	char *       buf,
	unsigned int size,
	uint32_t     enb_id,
	uint16_t     cell_id,
	uint32_t     mod_id,
	ep_op_type   op,
	uint8_t      meas_id,
	uint16_t     rnti,
	uint16_t     earfcn,
	uint16_t     interval,
	int16_t      max_cells,
	int16_t      max_meas);

/* Parse an UE measurement request for the desired fields */
int epp_trigger_uemeas_req(
	char *       buf,
	unsigned int size,
	uint8_t  *   meas_id,
	uint16_t *   rnti,
	uint16_t *   earfcn,
	uint16_t *   interval,
	int16_t  *   max_cells,
	int16_t  *   max_meas);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_UE_REPORT_H */
