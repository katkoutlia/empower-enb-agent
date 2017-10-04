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

#include <emproto.h>

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

ep_dir_type epp_schedule_dir(char * buf, unsigned int size)
{
	ep_c_hdr * h = (ep_c_hdr *)(buf + sizeof(ep_hdr));

	return (ep_dir_type)h->dir;
}

ep_act_type epp_schedule_type(char * buf, unsigned int size)
{
	ep_c_hdr * h = (ep_c_hdr *)(buf + sizeof(ep_hdr));

	if(size < sizeof(ep_hdr) + sizeof(ep_c_hdr)) {
		return EP_TYPE_INVALID_MSG;
	}

	return (ep_act_type)h->type;
}
