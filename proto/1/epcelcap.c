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

#include <netinet/in.h>

#include <emproto.h>

int epf_ccap_rep(
	char *        buf,
	unsigned int  size,
	ep_cell_det * cell)
{
	ep_ccap_rep * rep = (ep_ccap_rep *)buf;

	if(!cell) {
		rep->cap        = 0;
		rep->DL_earfcn  = 0;
		rep->DL_prbs    = 0;
		rep->UL_earfcn  = 0;
		rep->UL_prbs    = 0;
	} else {
		rep->cap        = htonl(cell->cap);
		rep->DL_earfcn  = htons(cell->DL_earfcn);
		rep->DL_prbs    = cell->DL_prbs;
		rep->UL_earfcn  = htons(cell->UL_earfcn);
		rep->UL_prbs    = cell->UL_prbs;
	}

	return sizeof(ep_ccap_rep);
}

int epp_ccap_rep(
	char *        buf,
	unsigned int  size,
	ep_cell_det * cell)
{
	ep_ccap_rep * rep = (ep_ccap_rep *)buf;

	cell->cap        = ntohl(rep->cap);
	cell->DL_earfcn  = ntohs(rep->DL_earfcn);
	cell->DL_prbs    = rep->DL_prbs;
	cell->UL_earfcn  = ntohs(rep->UL_earfcn);
	cell->UL_prbs    = rep->UL_prbs;

	return EP_SUCCESS;
}

int epf_ccap_req(char * buf, unsigned int size)
{
	ep_ccap_req * rep = (ep_ccap_req *)buf;

	rep->dummy = 0;

	return sizeof(ep_ccap_req);
}

int epp_ccap_req(char * buf, unsigned int size)
{
	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_single_ccap_rep_fail(
	char *        buf,
	unsigned int  size,
	uint32_t      enb_id,
	uint16_t      cell_id,
	uint32_t      mod_id)
{
	int ms = 0;

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		EP_ACT_CCAP,
		EP_OPERATION_FAIL,
		EP_DIR_REPLY);

	ms += epf_ccap_rep(buf + ms, size - ms, 0);
	epf_msg_length(buf, size, ms);

	return ms;
}

int epf_single_ccap_rep(
	char *        buf,
	unsigned int  size,
	uint32_t      enb_id,
	uint16_t      cell_id,
	uint32_t      mod_id,
	ep_cell_det * cell)
{
	int ms = 0;

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		EP_ACT_CCAP,
		EP_OPERATION_UNSPECIFIED,
		EP_DIR_REPLY);

	ms += epf_ccap_rep(buf + ms, size - ms, cell);
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_single_ccap_rep(
	char *        buf, unsigned int size,
	ep_cell_det * cell)
{
	return epp_ccap_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size,
		cell);
}

int epf_single_ccap_req(
	char * buf, unsigned int size,
	uint32_t enb_id,
	uint16_t cell_id,
	uint32_t mod_id)
{
	int ms = 0;

	ms += epf_head(
		buf,
		size,
		EP_TYPE_SINGLE_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_single(
		buf + ms,
		size - ms,
		EP_ACT_CCAP,
		EP_OPERATION_UNSPECIFIED,
		EP_DIR_REQUEST);

	ms += epf_ccap_req(buf + ms, size - ms);
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_single_ccap_req(char * buf, unsigned int size)
{
	return epp_ccap_req(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size);
}
