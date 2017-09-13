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

/*    FIRST-LEVEL TYPE OF MESSAGES
 *
 * This type is directly present into the master header and provides the first
 * differentiation of the possible type of message.
 */

#ifndef __EMAGE_PROTOCOLS_MESSAGE_TYPE_H
#define __EMAGE_PROTOCOLS_MESSAGE_TYPE_H

typedef enum __ep_message_type {
	EP_TYPE_INVALID_MSG = 0,
	EP_TYPE_SINGLE_MSG,
	EP_TYPE_SCHEDULE_MSG,
	EP_TYPE_TRIGGER_MSG,
	EP_TYPE_EXTENDED = 0xff
} ep_msg_type;

#endif /* __EMAGE_PROTOCOLS_MESSAGE_TYPE_H */
