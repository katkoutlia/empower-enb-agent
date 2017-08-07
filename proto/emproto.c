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

/*
 * Protocols for communication with EmPOWER technologies.
 */

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <emage/pb/main.pb-c.h>

#include "emproto.h"

/* Converts a 32bits number containing a PLMN Id into and allocated string. */
void plmn_to_string(uint32_t plmn, char * str)
{
	int i = 0;

	str[i++] = 0x30 + ((plmn & 0xf00000) >> 20);
	str[i++] = 0x30 + ((plmn & 0x0f0000) >> 16);
	str[i++] = 0x30 + ((plmn & 0x00f000) >> 12);

	if(plmn & 0x00f0000 < 0xa) {
		str[i++] = 0x30 + ((plmn & 0x000f000) >> 8);
	}
	str[i++] = 0x30 + ((plmn & 0x0000f0) >> 4);
	str[i++] = 0x30 + (plmn & 0x00000f);
}

/******************************************************************************
 * Library routines:                                                          *
 ******************************************************************************/

int emp_format_empty_RRC_conf(
	uint32_t enb_id, uint32_t module_id,
	uint16_t rnti, uint16_t freq, EmageMsg ** ret)
{
	EmageMsg * msg         = malloc(sizeof(EmageMsg));
	Header * hdr           = malloc(sizeof(Header));
	TriggerEvent * te      = malloc(sizeof(TriggerEvent));
	UeRrcMeasConf * mc     = malloc(sizeof(UeRrcMeasConf));
	UeRrcMeasConfRepl * cr = malloc(sizeof(UeRrcMeasConfRepl));
	UeCapabilities * uc    = malloc(sizeof(UeCapabilities));

	if(!msg || !hdr || !te || !mc || !cr || !uc) {
		goto err;
	}

	memset(msg, 0, sizeof(EmageMsg));
	memset(hdr, 0, sizeof(Header));
	memset(te,  0, sizeof(TriggerEvent));
	memset(mc,  0, sizeof(UeRrcMeasConf));
	memset(cr,  0, sizeof(UeRrcMeasConfRepl));
	memset(uc,  0, sizeof(UeCapabilities));

	emage_msg__init(msg);
	header__init(hdr);
	trigger_event__init(te);
	ue_rrc_meas_conf__init(mc);
	ue_rrc_meas_conf_repl__init(cr);
	ue_capabilities__init(uc);

	uc->band   = 0;
	uc->n_band = 0;

	uc->has_a5_a4_events           = 0;
	uc->has_interfreq_meas         = 0;
	uc->has_interfreq_ref_per_meas = 0;
	uc->has_interfreq_si_acq       = 0;
	uc->has_intrafreq_ref_per_meas = 0;
	uc->has_intrafreq_si_acq       = 0;
	uc->has_release_3gpp           = 0;

	cr->capabilities = uc;
	cr->status       = CONF_REQ_STATUS__CREQS_SUCCESS;
	cr->rnti         = rnti;

	cr->has_freq = 1;
	cr->freq     = freq;

	cr->has_meas_gap_config_sf_offset = 0;
	cr->has_meas_gap_patt = 0;
	cr->has_pcell_dd      = 0;
	cr->has_ue_rrc_state  = 0;
	cr->n_m_obj           = 0;
	cr->n_meas_id         = 0;
	cr->n_r_conf          = 0;

	mc->repl = cr;
	mc->ue_rrc_meas_conf_m_case = UE_RRC_MEAS_CONF__UE_RRC_MEAS_CONF_M_REPL;

	te->mue_rrc_meas_conf = mc;
	te->events_case = TRIGGER_EVENT__EVENTS_M_UE_RRC_MEAS_CONF;

	hdr->t_id = module_id;
	hdr->b_id = enb_id;

	msg->head = hdr;
	msg->event_types_case = EMAGE_MSG__EVENT_TYPES_TE;
	msg->te   = te;

	*ret = msg;

	return 0;

err:
	if(cr) {
		if(cr->capabilities) {
			if(cr->capabilities->band) {
				free(cr->capabilities->band);
			}

			free(cr->capabilities);
		}

		free(cr);
	}

	if(mc) {
		free(mc);
	}

	if(te) {
		free(te);
	}

	if(hdr) {
		free(hdr);
	}

	if(msg) {
		free(msg);
	}

	return -1;
}

int emp_format_enb_report(
	uint32_t enb_id, uint32_t module_id, emp_eNB * info, EmageMsg ** ret)
{
	int i;

	EmageMsg * msg         = malloc(sizeof(EmageMsg));
	Header * hdr           = malloc(sizeof(Header));
	SingleEvent * se       = malloc(sizeof(SingleEvent));
	ENBCells * ec          = malloc(sizeof(ENBCells));
	ENBCellsRepl * cr      = malloc(sizeof(ENBCellsRepl));

	if(!msg || !hdr || !se || !ec || !cr) {
		goto err;
	}

	memset(msg, 0, sizeof(EmageMsg));
	memset(hdr, 0, sizeof(Header));
	memset(se,  0, sizeof(SingleEvent));
	memset(ec,  0, sizeof(ENBCells));
	memset(cr,  0, sizeof(ENBCellsRepl));

	emage_msg__init(msg);
	header__init(hdr);
	single_event__init(se);
	e_nb_cells__init(ec);
	e_nb_cells_repl__init(cr);

	cr->status   = CONF_REQ_STATUS__CREQS_SUCCESS;
	cr->n_cells  = info->nof_cells;

	if(cr->n_cells > 0) {
		cr->cells = malloc(sizeof(CellInformation *) * cr->n_cells);

		if(!cr->cells) {
			goto err;
		}

		for(i = 0; i < cr->n_cells; i++) {
			cr->cells[i] = malloc(sizeof(CellInformation));

			if(!cr->cells[i]) {
				goto err;
			}

			memset(cr->cells[i], 0, sizeof(CellInformation));
			cell_information__init(cr->cells[i]);

			cr->cells[i]->phys_cell_id = info->cells[i].pci;
			cr->cells[i]->carrier_freq = info->cells[i].freq;

			cr->cells[i]->has_num_rbs_dl = 1;
			cr->cells[i]->num_rbs_dl = info->cells[i].prb_dl;
			cr->cells[i]->has_num_rbs_ul = 1;
			cr->cells[i]->num_rbs_ul = info->cells[i].prb_ul;

			cr->cells[i]->n_res_alloc_type_supp_dl = 0;
			cr->cells[i]->n_res_alloc_type_supp_ul = 0;
		}
	}

	ec->e_nb_cells_m_case = E_NB_CELLS__E_NB_CELLS_M_REPL;
	ec->repl = cr;

	se->menb_cells  = ec;
	se->events_case = SINGLE_EVENT__EVENTS_M_ENB_CELLS;

	hdr->t_id = module_id;
	hdr->b_id = enb_id;

	msg->head = hdr;
	msg->event_types_case = EMAGE_MSG__EVENT_TYPES_SE;
	msg->se   = se;

	*ret = msg;

	return 0;

err:
	if(cr) {
		if(!cr->cells) {
			goto s1;
		}

		for(i = 0; i < cr->n_cells; i++) {
			if(cr->cells[i]) {
				free(cr->cells[i]);
			}
		}
s1:
		free(cr->cells);
	}

	if(ec) {
		free(ec);
	}

	if(se) {
		free(se);
	}

	if(hdr) {
		free(hdr);
	}

	if(msg) {
		free(msg);
	}

	return -1;
}

int emp_format_UE_report(
	uint32_t enb_id, uint32_t module_id,
	emp_UE * active_UEs,   unsigned int nof_active_UEs,
	emp_UE * inactive_UEs, unsigned int nof_inactive_UEs,
	EmageMsg ** ret)
{
	EmageMsg *     msg  = 0;
	Header *       hdr  = 0;
	TriggerEvent * te   = 0;
	UesId *        uid  = 0;
	UesIdRepl *    urpl = 0;

	int i = 0;

	char * plmn = 0;
	char * imsi = 0;

	msg  = malloc(sizeof(EmageMsg));
	hdr  = malloc(sizeof(Header));
	te   = malloc(sizeof(TriggerEvent));
	uid  = malloc(sizeof(UesId));
	urpl = malloc(sizeof(UesIdRepl));

	if(!msg || !hdr || !te || !uid || !urpl) {
		goto err;
	}

	memset(msg,  0, sizeof(EmageMsg));
	memset(hdr,  0, sizeof(Header));
	memset(te,   0, sizeof(TriggerEvent));
	memset(uid,  0, sizeof(UesId));
	memset(urpl, 0, sizeof(UesIdRepl));

	emage_msg__init(msg);
	header__init(hdr);
	trigger_event__init(te);
	ues_id__init(uid);
	ues_id_repl__init(urpl);

	/*
	 * Formatting up the active UEs.
	 */

	urpl->n_active_ue_id = nof_active_UEs;

	if(urpl->n_active_ue_id > 0) {
		urpl->active_ue_id = malloc(
			sizeof(ActiveUe *) * urpl->n_active_ue_id);

		if(!urpl->active_ue_id) {
			goto err;
		}

		memset(
			urpl->active_ue_id,
			0,
			sizeof(ActiveUe *) * urpl->n_active_ue_id);

		for(i = 0; i < nof_active_UEs; i++) {
			urpl->active_ue_id[i] = malloc(sizeof(ActiveUe));

			if(!urpl->active_ue_id[i]) {
				goto err;
			}

			plmn = malloc(sizeof(char) * 8);
			imsi = malloc(sizeof(char) * 16);

			if(!plmn || !imsi) {
				goto err;
			}

			memset(plmn, 0, sizeof(char) * 8);
			memset(imsi, 0, sizeof(char) * 16);

			plmn_to_string(active_UEs[i].plmn, plmn);
			sprintf(imsi, "%"PRIu64"", active_UEs[i].imsi);

			active_ue__init(urpl->active_ue_id[i]);

			urpl->active_ue_id[i]->rnti    = active_UEs[i].rnti;
			urpl->active_ue_id[i]->plmn_id = plmn;
			urpl->active_ue_id[i]->imsi    = imsi;
		}
	} else {
		urpl->active_ue_id = 0;
	}

	/*
	 * Formatting up the inactive UEs.
	 */

	urpl->n_inactive_ue_id = nof_inactive_UEs;

	if(urpl->n_inactive_ue_id > 0) {
		urpl->inactive_ue_id = malloc(
			sizeof(InactiveUe *) * urpl->n_inactive_ue_id);

		if(!urpl->inactive_ue_id) {
			goto err;
		}

		memset(
			urpl->inactive_ue_id,
			0,
			sizeof(InactiveUe *) * urpl->n_inactive_ue_id);

		for(i = 0; i < nof_inactive_UEs; i++) {
			urpl->inactive_ue_id[i] = malloc(sizeof(InactiveUe));

			if(!urpl->inactive_ue_id[i]) {
				goto err;
			}

			plmn = malloc(sizeof(char) * 8);
			imsi = malloc(sizeof(char) * 16);

			if(!plmn || !imsi) {
				goto err;
			}

			memset(plmn, 0, sizeof(char) * 8);
			memset(imsi, 0, sizeof(char) * 16);

			plmn_to_string(inactive_UEs[i].plmn, plmn);
			sprintf(imsi, "%"PRIu64"", inactive_UEs[i].imsi);

			inactive_ue__init(urpl->inactive_ue_id[i]);

			urpl->inactive_ue_id[i]->rnti    = inactive_UEs[i].rnti;
			urpl->inactive_ue_id[i]->plmn_id = plmn;
			urpl->inactive_ue_id[i]->imsi    = imsi;
		}
	} else {
		urpl->inactive_ue_id = 0;
	}

	urpl->status = CONF_REQ_STATUS__CREQS_SUCCESS;

	uid->ues_id_m_case = UES_ID__UES_ID_M_REPL;
	uid->repl = urpl;

	te->events_case = TRIGGER_EVENT__EVENTS_M_UES_ID;
	te->mues_id = uid;

	hdr->b_id = enb_id;
	hdr->vers = 1;
	hdr->seq  = 0;
	hdr->t_id = module_id;

	msg->event_types_case = EMAGE_MSG__EVENT_TYPES_TE;
	msg->te = te;
	msg->head = hdr;

	*ret = msg;

	return 0;

err:
	if(plmn) {
		free(plmn);
	}

	if(imsi) {
		free(imsi);
	}

	if(urpl) {
		if(!urpl->active_ue_id) {
			goto s1;
		}

		for(i = 0; i < urpl->n_active_ue_id; i++) {
			if(urpl->active_ue_id[i]) {
				if(urpl->active_ue_id[i]->imsi) {
					free(urpl->active_ue_id[i]->imsi);
				}

				if(urpl->active_ue_id[i]->plmn_id) {
					free(urpl->active_ue_id[i]->plmn_id);
				}

				free(urpl->active_ue_id[i]);
			}
		}

		free(urpl->active_ue_id);
s1:
		if(!urpl->inactive_ue_id) {
			goto s2;
		}

		for(i = 0; i < urpl->n_inactive_ue_id; i++) {
			if(urpl->inactive_ue_id[i]) {
				if(urpl->inactive_ue_id[i]->imsi) {
					free(urpl->inactive_ue_id[i]->imsi);
				}

				if(urpl->inactive_ue_id[i]->plmn_id) {
					free(urpl->inactive_ue_id[i]->plmn_id);
				}

				free(urpl->inactive_ue_id[i]);
			}
		}

		free(urpl->inactive_ue_id);
s2:
		free(urpl);
	}

	if(uid) {
		free(uid);
	}

	if(te) {
		free(te);
	}

	if(hdr) {
		free(hdr);
	}

	if(msg) {
		free(msg);
	}

	return -1;
}

int emp_format_UE_RRC_meas(
	uint32_t enb_id, uint32_t module_id,
	uint16_t rnti, emp_UERM * info,
	EmageMsg ** ret)
{
	EmageMsg *               msg = malloc(sizeof(EmageMsg));
	Header *                 hdr = malloc(sizeof(Header));
	TriggerEvent *           te  = malloc(sizeof(TriggerEvent));
	RrcMeas *                rm  = malloc(sizeof(RrcMeas));
	RrcMeasRepl *            rmr = malloc(sizeof(RrcMeasRepl));
	NeighCellsMeasurements * nc  = malloc(sizeof(NeighCellsMeasurements));

	int i;

	if(!msg || !hdr || !te || !rm || !rmr || !nc) {
		goto err;
	}

	emage_msg__init(msg);
	header__init(hdr);
	trigger_event__init(te);
	rrc_meas__init(rm);
	rrc_meas_repl__init(rmr);
	neigh_cells_measurements__init(nc);

	nc->n_eutra_meas = info->nof_neigh;
	nc->eutra_meas = malloc(sizeof(EUTRAMeasurements *) * nc->n_eutra_meas);

	if(!nc->eutra_meas) {
		goto err;
	}

	memset(
		nc->eutra_meas,
		0,
		sizeof(EUTRAMeasurements *) * nc->n_eutra_meas);

	for(i = 0; i < nc->n_eutra_meas; i++) {
		nc->eutra_meas[i] = malloc(sizeof(EUTRAMeasurements));

		if(!nc->eutra_meas[i]) {
			goto err;
		}

		eutra_measurements__init(nc->eutra_meas[i]);

		nc->eutra_meas[i]->has_phys_cell_id = 1;
		nc->eutra_meas[i]->phys_cell_id = info->neighs[i].pci;
		nc->eutra_meas[i]->cgi_meas = 0;

		nc->eutra_meas[i]->meas_result =
			malloc(sizeof(EUTRARefSignalMeas));

		if(!nc->eutra_meas[i]->meas_result) {
			goto err;
		}

		eutra_ref_signal_meas__init(nc->eutra_meas[i]->meas_result);

		nc->eutra_meas[i]->meas_result->has_rsrp = 1;
		nc->eutra_meas[i]->meas_result->rsrp =
			info->neighs[i].rsrp;
		nc->eutra_meas[i]->meas_result->has_rsrq = 1;
		nc->eutra_meas[i]->meas_result->rsrq =
			info->neighs[i].rsrq;
	}

	rmr->rnti           = rnti;
	rmr->neigh_meas     = nc;
	rmr->status         = STATS_REQ_STATUS__SREQS_SUCCESS;
	rmr->has_measid     = 1;
	rmr->measid         = info->id;
	rmr->has_freq       = 1;
	rmr->freq           = info->freq;
	rmr->has_pcell_rsrp = 1;
	rmr->pcell_rsrp     = info->rsrp;
	rmr->has_pcell_rsrq = 1;
	rmr->pcell_rsrq     = info->rsrq;

	rm->rrc_meas_m_case = RRC_MEAS__RRC_MEAS_M_REPL;
	rm->repl = rmr;

	te->events_case = TRIGGER_EVENT__EVENTS_M_RRC_MEAS;
	te->mrrc_meas = rm;

	hdr->b_id = enb_id;
	hdr->vers = 1;
	hdr->seq  = 0;
	hdr->t_id = module_id;

	msg->event_types_case = EMAGE_MSG__EVENT_TYPES_TE;
	msg->te = te;
	msg->head = hdr;

	*ret = msg;

	return 0;

err:
	if(nc) {
		if(nc->eutra_meas) {
			for(i = 0; i < nc->n_eutra_meas; i++) {
				if(!nc->eutra_meas[i]) {
					continue;
				}

				if(nc->eutra_meas[i]->meas_result) {
					free(nc->eutra_meas[i]->meas_result);
				}

				free(nc->eutra_meas[i]);
			}

			free(nc->eutra_meas);
		}

		free(nc);
	}

	if(rmr) {
		free(rmr);
	}

	if(rm) {
		free(rm);
	}

	if(te) {
		free(te);
	}

	if(hdr) {
		free(hdr);
	}

	if(msg) {
		free(msg);
	}

	return -1;
}
