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

int epf_uemeas_rep(
	char *          buf,
	unsigned int    size,
	uint32_t        nof_meas,
	uint32_t        max,
	ep_ue_measure * ues)
{
	int i = -1;

	ep_uemeas_rep * rep = (ep_uemeas_rep *) buf;
	ep_uemeas_det * det = (ep_uemeas_det *)(buf + sizeof(ep_uemeas_rep));

	rep->nof_meas = htonl(nof_meas);

	if(ues) {
		for(i = 0; i < nof_meas && i < max; i++) {
			det[i].meas_id = ues[i].meas_id;
			det[i].pci     = htons(ues[i].pci);
			det[i].rsrp    = htons(ues[i].rsrp);
			det[i].rsrq    = htons(ues[i].rsrq);
		}
	}

	return sizeof(ep_uemeas_rep) + (sizeof(ep_uemeas_det) * i);
}

int epp_uemeas_rep(
	char *          buf,
	unsigned int    size,
	uint32_t *      nof_meas,
	uint32_t        max,
	ep_ue_measure * ues)
{
	int i;

	ep_uemeas_rep * rep = (ep_uemeas_rep *) buf;
	ep_uemeas_det * det = (ep_uemeas_det *)(buf + sizeof(ep_uemeas_rep));

	*nof_meas = ntohl(rep->nof_meas);

	if(ues) {
		for(i = 0; i < *nof_meas && i < max; i++) {
			ues[i].meas_id = det[i].meas_id;
			ues[i].pci     = ntohs(det[i].pci);
			ues[i].rsrp    = ntohs(det[i].rsrp);
			ues[i].rsrq    = ntohs(det[i].rsrq);
		}
	}

	return EP_SUCCESS;
}

int epf_uemeas_req(
	char *        buf,
	unsigned int  size,
	uint8_t       meas_id,
	uint16_t      rnti,
	uint16_t      earfcn,
	uint16_t      interval,
	int16_t       max_cells,
	int16_t       max_meas)
{
	ep_uemeas_req * req = (ep_uemeas_req *)buf;

	req->meas_id   = meas_id;
	req->rnti      = htons(rnti);
	req->earfcn    = htons(earfcn);
	req->interval  = htons(interval);
	req->max_cells = htons(max_cells);
	req->max_meas  = htons(max_meas);

	return sizeof(ep_uemeas_req);
}

int epp_uemeas_req(
	char * buf, unsigned int size,
	uint8_t  * meas_id,
	uint16_t * rnti,
	uint16_t * earfcn,
	uint16_t * interval,
	int16_t  * max_cells,
	int16_t  * max_meas)
{
	ep_uemeas_req * req = (ep_uemeas_req *) buf;

	if(meas_id) {
		*meas_id = req->meas_id;
	}

	if(rnti) {
		*rnti = ntohs(req->rnti);
	}

	if(earfcn) {
		*earfcn = ntohs(req->earfcn);
	}

	if(interval) {
		*interval = ntohs(req->interval);
	}

	if(max_cells) {
		*max_cells = ntohs(req->max_cells);
	}

	if(max_meas) {
		*max_meas = ntohs(req->max_meas);
	}

	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_trigger_uemeas_rep_fail(
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
		EP_ACT_UE_MEASURE,
		EP_OPERATION_FAIL,
		EP_DIR_REPLY);

	ms += epf_uemeas_rep(buf + ms, size - ms, 0, 0, 0);
	epf_msg_length(buf, size, ms);

	return ms;
}

int epf_trigger_uemeas_rep(
	char *          buf,
	unsigned int    size,
	uint32_t        enb_id,
	uint16_t        cell_id,
	uint32_t        mod_id,
	uint32_t        nof_meas,
	uint32_t        max,
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
		EP_ACT_UE_MEASURE,
		EP_OPERATION_SUCCESS,
		EP_DIR_REPLY);

	ms += epf_uemeas_rep(buf + ms, size - ms, nof_meas, max, ues);
	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_trigger_uemeas_rep(
	char *          buf,
	unsigned int    size,
	uint32_t *      nof_ues,
	uint32_t        max,
	ep_ue_measure * ues)
{
	return epp_uemeas_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_t_hdr),
		size,
		nof_ues,
		max,
		ues);
}

int epf_trigger_uemeas_req(
	char *        buf,
	unsigned int  size,
	uint32_t      enb_id,
	uint16_t      cell_id,
	uint32_t      mod_id,
	ep_op_type    op,
	uint8_t       meas_id,
	uint16_t      rnti,
	uint16_t      earfcn,
	uint16_t      interval,
	int16_t       max_cells,
	int16_t       max_meas)
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
		EP_ACT_UE_MEASURE,
		op,
		EP_DIR_REQUEST);

	ms += epf_uemeas_req(
		buf + ms,
		size - ms,
		meas_id,
		rnti,
		earfcn,
		interval,
		max_cells,
		max_meas);

	epf_msg_length(buf, size, ms);

	return ms;
}

int epp_trigger_uemeas_req(
	char *       buf,
	unsigned int size,
	uint8_t  *   meas_id,
	uint16_t *   rnti,
	uint16_t *   earfcn,
	uint16_t *   interval,
	int16_t  *   max_cells,
	int16_t  *   max_meas)
{
	return epp_uemeas_req(
		buf + sizeof(ep_hdr) + sizeof(ep_t_hdr),
		size,
		meas_id,
		rnti,
		earfcn,
		interval,
		max_cells,
		max_meas);
}
