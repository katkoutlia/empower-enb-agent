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

typedef struct empower_proto_UE {
	uint16_t rnti;
	uint64_t imsi;
	uint32_t plmn;
} emp_UE;

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

#ifdef __cplusplus
}
#endif

#endif /* __EMPOWER_PROTOCOLS_H */
