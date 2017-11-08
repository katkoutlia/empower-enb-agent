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

int epf_ho_rep(char * buf, unsigned int size)
{
	return sizeof(ep_ho_rep);
}

int epp_ho_rep(char * buf, unsigned int size)
{
	return EP_SUCCESS;
}

int epf_ho_req(
	char *       buf,
	unsigned int size,
	uint16_t     rnti,
	uint32_t     enb,
	uint16_t     pci,
	uint8_t      cause)
{
	ep_ho_req * req = (ep_ho_req *)buf;

	req->rnti       = htons(rnti);
	req->target_eNB = htonl(enb);
	req->target_pci = htons(pci);
	req->cause      = cause;

	return sizeof(ep_ho_req);
}

int epp_ho_req(
	char *       buf,
	unsigned int size,
	uint16_t *   rnti,
	uint32_t *   enb,
	uint16_t *   pci,
	uint8_t *    cause)
{
	ep_ho_req * req = (ep_ho_req *)buf;

	if(rnti) {
		*rnti  = ntohs(req->rnti);
	}

	if(enb) {
		*enb   = ntohl(req->target_eNB);
	}

	if(pci) {
		*pci   = ntohs(req->target_pci);
	}

	if(cause) {
		*cause = req->cause;
	}

	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_single_ho_rep_fail(
	char *       buf,
	unsigned int size,
	uint32_t     enb_id,
	uint16_t     cell_id,
	uint32_t     mod_id)
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
		EP_ACT_HANDOVER,
		EP_OPERATION_FAIL,
		EP_DIR_REPLY);

	ms += epf_ho_rep(buf + ms, size - ms);
	epf_msg_length(buf, size, ms);

	return ms;
}

int epf_single_ho_rep_ns(
	char *       buf,
	unsigned int size,
	uint32_t     enb_id,
	uint16_t     cell_id,
	uint32_t     mod_id)
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
		EP_ACT_HANDOVER,
		EP_OPERATION_NOT_SUPPORTED,
		EP_DIR_REPLY);

	ms += epf_ho_rep(buf + ms, size - ms);
	epf_msg_length(buf, size, ms);

	return ms;
}

int epf_single_ho_rep(
	char *       buf,
	unsigned int size,
	uint32_t     enb_id,
	uint16_t     cell_id,
	uint32_t     mod_id)
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
		EP_ACT_HANDOVER,
		EP_OPERATION_SUCCESS,
		EP_DIR_REPLY);

	ms += epf_ho_rep(buf + ms, size - ms);
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_single_ho_rep(char * buf, unsigned int  size)
{
	return epp_ho_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size);
}

int epf_single_ho_req(
	char *       buf,
	unsigned int size,
	uint32_t     enb_id,
	uint16_t     cell_id,
	uint32_t     mod_id,
	uint16_t     rnti,
	uint32_t     enb,
	uint16_t     pci,
	uint8_t      cause)
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
		EP_ACT_HANDOVER,
		EP_OPERATION_UNSPECIFIED,
		EP_DIR_REQUEST);

	ms += epf_ho_req(buf + ms, size - ms, rnti, enb, pci, cause);
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_single_ho_req(
	char *       buf,
	unsigned int size,
	uint16_t *   rnti,
	uint32_t *   enb,
	uint16_t *   pci,
	uint8_t *    cause)
{
	return epp_ho_req(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size,
		rnti,
		enb,
		pci,
		cause);
}
