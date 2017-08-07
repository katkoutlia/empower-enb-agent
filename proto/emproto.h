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
 * Protocols for communication with EmPOWER technologies.
 */

#ifndef __EMPOWER_PROTOCOLS_H
#define __EMPOWER_PROTOCOLS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Structure holding UE RRC measurements.
 */

#define EMP_ENB_MAX_RRC_UE_NEIGH 8

typedef struct emp_ue_RRC_neigh_meas {
	uint16_t pci;
	float    rsrp;
	float    rsrq;
} emp_UERN;

typedef struct emp_ue_RRC_measurement {
	uint8_t  id;

	uint16_t freq;
	float    rsrp;
	float    rsrq;

	int      nof_neigh;
	emp_UERN neighs[EMP_ENB_MAX_RRC_UE_NEIGH];
} emp_UERM;

/*
 * Structure holding eNB information.
 */

#define EMP_ENB_MAX_CELLS	6

typedef struct emp_cell_info {
	uint32_t pci;
	uint32_t freq;
	uint32_t prb_dl;
	uint32_t prb_ul;
} emp_cell;

typedef struct emp_eNB_info {
	uint32_t nof_cells;
	emp_cell cells[EMP_ENB_MAX_CELLS];
} emp_eNB;

/*
 * Structure holding UE information.
 */

typedef struct emp_UE_info {
	uint16_t rnti;
	uint64_t imsi;
	uint32_t plmn;
} emp_UE;

/* Format an empty message which contains an UE RRC configurations.
 *
 * Returns 0 on success, and the ret pointer is filled with the desired message.
 * On failure, a negative error number is returned.
 *
 * NOTE: You are responsible for deallocating the message once you finished you
 * operations with it.
 */
int emp_format_empty_RRC_conf(
	uint32_t enb_id, uint32_t module_id,
	uint16_t rnti, uint16_t freq, EmageMsg ** ret);

/* Format a message containing information about the configuration of a base
 * station.
 *
 * Returns 0 on success, and the ret pointer is filled with the desired message.
 * On failure, a negative error number is returned.
 *
 * NOTE: You are responsible for deallocating the message once you finished you
 * operations with it.
 */
int emp_format_enb_report(
	uint32_t enb_id, uint32_t module_id, emp_eNB * info, EmageMsg ** ret);

/* Format an EmAGE message to contain an UE report based on the given input.
 *
 * Returns 0 on success, and the ret pointer is filled with the desired message.
 * On failure, a negative error number is returned.
 *
 * NOTE: You are responsible for deallocating the message once you finished you
 * operations with it.
 */
int emp_format_UE_report(
	uint32_t enb_id, uint32_t module_id,
	emp_UE * active_UEs, unsigned int nof_active_UEs,
	emp_UE * inactive_UEs, unsigned int nof_inactive_UEs,
	EmageMsg ** ret);

/* Format an EmAGE message to contain an UE measurements of carrier signal
 * quality and neighbor cell signals power.
 *
 * Returns 0 on success, and the ret pointer is filled with the desired message.
 * On failure, a negative error number is returned.
 *
 * NOTE: You are responsible for deallocating the message once you finished you
 * operations with it.
 */
int emp_format_UE_RRC_meas(
	uint32_t enb_id, uint32_t module_id,
	uint16_t rnti, emp_UERM * info,
	EmageMsg ** ret);

#ifdef __cplusplus
}
#endif

#endif /* __EMPOWER_PROTOCOLS_H */
