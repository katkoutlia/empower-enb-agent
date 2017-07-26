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

/* Converts a 32bits number containing a plmn id into and allocated string. */
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

int emp_format_UE_report(
	uint32_t enb_id, uint32_t module_id,
	emp_UE * active_UEs, unsigned int nof_active_UEs,
	emp_UE * inactive_UEs, unsigned int nof_inactive_UEs,
	EmageMsg ** ret)
{
	EmageMsg *     msg  = 0;
	Header *       hdr  = 0;
	TriggerEvent * te   = 0;
	UesId *        uid  = 0;
	UesIdRepl *    urpl = 0;

	int i = 0;
	int u = 0;

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

			plmn = malloc(sizeof(char) * 7);
			imsi = malloc(sizeof(char) * 16);

			if(!plmn || !imsi) {
				goto err;
			}

			memset(plmn, 0, sizeof(char) * 8);
			memset(imsi, 0, sizeof(char) * 16);

			plmn_to_string(active_UEs[i].plmn, plmn);
			//sprintf(plmn, "%06d", active_UEs[i].plmn);
			sprintf(imsi, "%"PRIu64"", active_UEs[i].imsi);

			active_ue__init(urpl->active_ue_id[u]);

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

			plmn = malloc(sizeof(char) * 7);
			imsi = malloc(sizeof(char) * 16);

			if(!plmn || !imsi) {
				goto err;
			}

			memset(plmn, 0, sizeof(char) * 8);
			memset(imsi, 0, sizeof(char) * 16);

			plmn_to_string(inactive_UEs[i].plmn, plmn);
			//sprintf(plmn, "%06d", inactive_UEs[i].plmn);
			sprintf(imsi, "%"PRIu64"", inactive_UEs[i].imsi);

			inactive_ue__init(urpl->inactive_ue_id[u]);

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
		if(urpl->active_ue_id) {
			for(i = 0; i < urpl->n_active_ue_id; i++) {
				if(urpl->active_ue_id[i]) {
					if(urpl->active_ue_id[i]->imsi) {
						free(urpl->active_ue_id[i]->
							imsi);
					}

					if(urpl->active_ue_id[i]->plmn_id) {
						free(urpl->active_ue_id[i]->
							plmn_id);
					}

					free(urpl->active_ue_id[i]);
				}
			}

			free(urpl->active_ue_id);
		}

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
