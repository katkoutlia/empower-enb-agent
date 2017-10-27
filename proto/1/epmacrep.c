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

int epf_macrep_rep(
	char *          buf,
	unsigned int    size,
	ep_macrep_det * report)
{
	ep_macrep_rep * rep = (ep_macrep_rep *)buf;

	rep->DL_prbs_avg    = htons(report->DL_prbs_avg);
	rep->DL_prbs_in_use = report->DL_prbs_in_use;
	rep->DL_prbs_total  = report->DL_prbs_total;

	rep->UL_prbs_avg    = htons(report->UL_prbs_avg);
	rep->UL_prbs_in_use = report->UL_prbs_in_use;
	rep->UL_prbs_total  = report->UL_prbs_total;

	return sizeof(ep_macrep_rep);
}

int epp_macrep_rep(
	char *          buf,
	unsigned int    size,
	ep_macrep_det * report)
{
	ep_macrep_rep * rep = (ep_macrep_rep *)buf;

	rep->DL_prbs_avg    = ntohs(report->DL_prbs_avg);
	rep->DL_prbs_in_use = report->DL_prbs_in_use;
	rep->DL_prbs_total  = report->DL_prbs_total;

	rep->UL_prbs_avg    = ntohs(report->UL_prbs_avg);
	rep->UL_prbs_in_use = report->UL_prbs_in_use;
	rep->UL_prbs_total  = report->UL_prbs_total;

	return EP_SUCCESS;
}

int epf_macrep_req(char * buf, unsigned int size, uint16_t interval)
{
	ep_macrep_req * req = (ep_macrep_req *)buf;

	req->interval = htons(interval);

	return sizeof(ep_macrep_req);
}

int epp_macrep_req(char * buf, unsigned int size, uint16_t * interval)
{
	ep_macrep_req * req = (ep_macrep_req *)buf;

	*interval = ntohs(req->interval);

	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_trigger_macrep_rep_fail(
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
		EP_TYPE_TRIGGER_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_trigger(
		buf + ms,
		size - ms,
		EP_ACT_MAC_REPORT,
		EP_OPERATION_FAIL,
		EP_DIR_REPLY);

	ms += epf_macrep_rep(buf + ms, size - ms, 0);

	return ms;
}

int epf_trigger_macrep_rep_ns(
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
		EP_TYPE_TRIGGER_MSG,
		enb_id,
		cell_id,
		mod_id);

	ms += epf_trigger(
		buf + ms,
		size - ms,
		EP_ACT_MAC_REPORT,
		EP_OPERATION_NOT_SUPPORTED,
		EP_DIR_REPLY);

	ms += epf_macrep_rep(buf + ms, size - ms, 0);

	return ms;
}

int epf_trigger_macrep_rep(
	char *          buf,
	unsigned int    size,
	uint32_t        enb_id,
	uint16_t        cell_id,
	uint32_t        mod_id,
	ep_macrep_det * det)
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
		EP_ACT_MAC_REPORT,
		EP_OPERATION_SUCCESS,
		EP_DIR_REPLY);

	ms += epf_macrep_rep(buf + ms, size - ms, det);

	return ms;
}

int epp_trigger_macrep_rep(
	char *          buf,
	unsigned int    size,
	ep_macrep_det * det)
{
	return epp_macrep_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_t_hdr),
		size,
		det);
}

int epf_trigger_macrep_req(
	char *          buf,
	unsigned int    size,
	uint32_t        enb_id,
	uint16_t        cell_id,
	uint32_t        mod_id,
	uint16_t        interval)
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
		EP_ACT_MAC_REPORT,
		EP_OPERATION_UNSPECIFIED,
		EP_DIR_REPLY);

	ms += epf_macrep_req(buf + ms, size - ms, interval);

	return ms;
}

int epp_trigger_macrep_req(
	char *          buf,
	unsigned int    size,
	uint16_t *      interval)
{
	return epp_macrep_req(
		buf + sizeof(ep_hdr) + sizeof(ep_t_hdr),
		size,
		interval);
}
