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

int epf_uerep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t        nof_ues,
	uint32_t        max_ues,
	ep_ue_details * ues)
{
	int i = -1;

	ep_uerep_rep * rep = (ep_uerep_rep *)buf;
	ep_uerep_det * det = (ep_uerep_det *)(buf + sizeof(ep_uerep_rep));

	rep->nof_ues = htonl(nof_ues);

	if(ues) {
		for(i = 0; i < nof_ues && i < max_ues; i++) {
			det[i].pci  = htons(ues[i].pci);
			det[i].rnti = htons(ues[i].rnti);
			det[i].plmn = htonl(ues[i].plmn);
			det[i].imsi = htobe64(ues[i].imsi);
		}
	}

	return sizeof(ep_uerep_rep) + (sizeof(ep_uerep_det) * (i + 1));
}

int epp_uerep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t *      nof_ues,
	uint32_t        max_ues,
	ep_ue_details * ues)
{
	int i;

	ep_uerep_rep * rep = (ep_uerep_rep *)buf;
	ep_uerep_det * det = (ep_uerep_det *)(buf + sizeof(ep_uerep_rep));

	*nof_ues = ntohl(rep->nof_ues);

	if(ues) {
		for(i = 0; i < *nof_ues && i < max_ues; i++) {
			ues[i].pci  = ntohs(det[i].pci);
			ues[i].rnti = ntohs(det[i].rnti);
			ues[i].plmn = ntohl(det[i].plmn);
			ues[i].imsi = be64toh(det[i].imsi);
		}
	}

	return EP_SUCCESS;
}

int epf_uerep_req(char * buf, unsigned int size)
{
	ep_uerep_req * req = (ep_uerep_req *)buf;

	req->dummy = 0;

	return sizeof(ep_uerep_req);
}

int epp_uerep_req(char * buf, unsigned int size)
{
	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_trigger_uerep_rep_fail(
	char *          buf,
	unsigned int    size,
	uint32_t        enb_id,
	uint16_t        cell_id,
	uint32_t        mod_id)
{
	int ms = 0;

	ms += epf_head(
		buf,
		size,
		EP_TYPE_TRIGGER_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_trigger(
		buf + ms,
		size - ms,
		EP_ACT_UE_REPORT,
		EP_OPERATION_FAIL,
		EP_DIR_REPLY);

	ms += epf_uerep_rep(buf + ms, size - ms, 0, 0, 0);
	epf_msg_length(buf, size, ms);

	return ms;
}

int epf_trigger_uerep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t        enb_id,
	uint16_t        cell_id,
	uint32_t        mod_id,
	uint32_t        nof_ues,
	uint32_t        max_ues,
	ep_ue_details * ues)
{
	int ms = 0;

	ms += epf_head(
		buf,
		size,
		EP_TYPE_TRIGGER_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_trigger(
		buf + ms,
		size - ms,
		EP_ACT_UE_REPORT,
		EP_OPERATION_SUCCESS,
		EP_DIR_REPLY);

	ms += epf_uerep_rep(buf + ms, size - ms, nof_ues, max_ues, ues);
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_trigger_uerep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t *      nof_ues,
	uint32_t        max_ues,
	ep_ue_details * ues)
{
	return epp_uerep_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_t_hdr),
		size,
		nof_ues,
		max_ues,
		ues);
}

int epf_trigger_uerep_req(
	char *       buf,
	unsigned int size,
	uint32_t     enb_id,
	uint16_t     cell_id,
	uint32_t     mod_id,
	ep_op_type   op)
{
	int ms = 0;

	ms += epf_head(
		buf,
		size,
		EP_TYPE_TRIGGER_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_trigger(
		buf + ms,
		size - ms,
		EP_ACT_UE_REPORT,
		op,
		EP_DIR_REQUEST);

	ms += epf_uerep_req(buf + ms, size - ms);
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_trigger_uerep_req(char * buf, unsigned int size)
{
	return epp_uerep_req(buf, size);
}
