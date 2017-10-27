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

/*    MESSAGE TYPE
 *
 * This type is directly present into the master header and provides the first
 * differentiation of the possible type of message. This types provides
 * information on how to treat the type of action carried, so if it's a single
 * event, a scheduled or a trigger one.
 *
 *    ACTION TYPE
 * After the first classification, you find the type of action performed; this
 * is unique among all the possible types, and defines what request to perform
 * or which response is reported back.
 */

#ifndef __EMAGE_PROTOCOLS_MESSAGE_TYPE_H
#define __EMAGE_PROTOCOLS_MESSAGE_TYPE_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Type of the message */
typedef enum __ep_message_type {
	EP_TYPE_INVALID_MSG  = 0,
	EP_TYPE_SINGLE_MSG   = 1,
	EP_TYPE_SCHEDULE_MSG = 2,
	EP_TYPE_TRIGGER_MSG  = 3,
	EP_TYPE_EXTENDED     = 0xff
} ep_msg_type;

/* Action operated by the message */
typedef enum __ep_action_type {
	EP_ACT_INVALID       = 0, /* Invalid */
	EP_ACT_HELLO         = 1, /* Hello */
	EP_ACT_ECAP          = 2, /* eNB capabilities */
	EP_ACT_CCAP          = 3, /* Cell capabilities */
	EP_ACT_UE_REPORT     = 4, /* UE report */
	EP_ACT_UE_MEASURE    = 5, /* UE RRC measurements */
	EP_ACT_MAC_REPORT    = 6, /* Report coming from MAC layer */
	EP_ACT_HANDOVER      = 7, /* Hand an UE over another eNB */
} ep_act_type;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_PROTOCOLS_MESSAGE_TYPE_H */
