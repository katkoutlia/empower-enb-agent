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

int epf_uemeas_rep(
	char * buf, unsigned int size,
	uint32_t        nof_ues,
	ep_ue_measure * ues)
{
	int i = -1; /* On ('ues' = 0) returns (x + (y * (0)))*/

	ep_uemeas_rep * rep = (ep_uemeas_rep *) buf;
	ep_uemeas_det * det = (ep_uemeas_det *)(buf + sizeof(ep_uemeas_rep));

	rep->nof_ues = htonl(nof_ues);

	if(ues) {
		for(i = 0; i < nof_ues && i < EP_UE_REPORT_MAX_UES; i++) {
			det[i].meas_id = ues[i].meas_id;
			det[i].rnti    = htons(ues[i].rnti);
			det[i].pci     = htons(ues[i].pci);
			det[i].earfcn  = htons(ues[i].earfcn);
			det[i].rsrp    = htons(ues[i].rsrp);
			det[i].rsrq    = htons(ues[i].rsrq);
		}
	}

	return sizeof(ep_uemeas_rep) + (sizeof(ep_uemeas_det) * (i + 1));
}

int epp_uemeas_rep(
	char * buf, unsigned int size,
	uint32_t *      nof_ues,
	ep_ue_measure * ues)
{
	int i;

	ep_uemeas_rep * rep = (ep_uemeas_rep *) buf;
	ep_uemeas_det * det = (ep_uemeas_det *)(buf + sizeof(ep_uemeas_rep));

	*nof_ues = ntohl(rep->nof_ues);

	if(ues) {
		for(i = 0; i < *nof_ues && i < EP_UE_REPORT_MAX_UES; i++) {
			ues[i].meas_id = det[i].meas_id;
			ues[i].rnti    = ntohs(det[i].rnti);
			ues[i].pci     = ntohs(det[i].pci);
			ues[i].earfcn  = ntohs(det[i].earfcn);
			ues[i].rsrp    = ntohs(det[i].rsrp);
			ues[i].rsrq    = ntohs(det[i].rsrq);
		}
	}

	return EP_SUCCESS;
}

int epf_uemeas_req(
	char * buf,
	unsigned int size,
	uint8_t  meas_id,
	uint16_t pci,
	uint16_t earfcn)
{
	ep_uemeas_req * req = (ep_uemeas_req *)buf;

	req->meas_id = meas_id;
	req->pci     = htons(pci);
	req->earfcn  = htons(earfcn);

	return sizeof(ep_uemeas_req);
}

int epp_uemeas_req(
	char * buf, unsigned int size,
	uint8_t  * meas_id,
	uint16_t * pci,
	uint16_t * earfcn)
{
	ep_uemeas_req * req = (ep_uemeas_req *) buf;

	if(meas_id) {
		*meas_id = ntohs(req->meas_id);
	}

	if(pci) {
		*pci = ntohs(req->pci);
	}

	if(earfcn) {
		*earfcn = ntohs(req->earfcn);
	}

	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_trigger_uemeas_rep_fail(
	char * buf, unsigned int size,
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
		EP_TR_UE_MEASURE_MSG,
		EP_OPERATION_FAIL,
		EP_DIR_REPLY);

	ms += epf_uemeas_rep(buf + ms, size - ms, 0, 0);

	return ms;
}

int epf_trigger_uemeas_rep(
	char * buf, unsigned int size,
	uint32_t        enb_id,
	uint16_t        cell_id,
	uint32_t        mod_id,
	uint32_t        nof_ues,
	ep_ue_measure * ues)
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
		EP_TR_UE_MEASURE_MSG,
		EP_OPERATION_SUCCESS,
		EP_DIR_REPLY);

	ms += epf_uemeas_rep(buf + ms, size - ms, nof_ues, ues);

	return ms;
}

int epp_trigger_uemeas_rep(
	char * buf, unsigned int size,
	uint32_t *      nof_ues,
	ep_ue_measure * ues)
{
	return epp_uemeas_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_t_hdr),
		size,
		nof_ues,
		ues);
}

int epf_trigger_uemeas_req(
	char * buf, unsigned int size,
	uint32_t   enb_id,
	uint16_t   cell_id,
	uint32_t   mod_id,
	ep_op_type op,
	uint8_t    meas_id,
	uint16_t   pci,
	uint16_t   earfcn)
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
		EP_TR_UE_MEASURE_MSG,
		op,
		EP_DIR_REQUEST);

	ms += epf_uemeas_req(buf + ms, size - ms, meas_id, pci, earfcn);

	return ms;
}

int epp_trigger_uemeas_req(
	char * buf, unsigned int size,
	uint8_t  * meas_id,
	uint16_t * pci,
	uint16_t * earfcn)
{
	return epp_uemeas_req(buf, size, meas_id, pci, earfcn);
}
