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

/*    ENB FUNCTIONALITY MESSAGE
 *
 * This message request the switch On/Off of base station functionalities.
 */

#ifndef __EMAGE_ENB_FUNCTIONALITY_H
#define __EMAGE_ENB_FUNCTIONALITY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
 * eNB functionality message
 */

typedef struct __ep_enb_functionality_reply {
	uint8_t dummy;   /* Just a dummy field */
}__attribute__((packed)) ep_efun_rep;

typedef struct __ep_enb_functionality_request {
	uint32_t cap;    /* eNB capabilities */
}__attribute__((packed)) ep_efun_req;

/******************************************************************************
 * Operation on single-event messages                                         *
 ******************************************************************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_ENB_FUNCTIONALITY_H */
