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

int epf_ecap_rep(
	char * buf, unsigned int size,
	uint32_t cap,
	uint16_t * cell_ids,
	uint32_t nof_cells)
{
	int           i   = 0;
	ep_ecap_rep * rep = (ep_ecap_rep *)buf;

	rep->cap       = htonl(cap);
	rep->nof_cells = htonl(nof_cells);

	for(i = 0; i < nof_cells && i < EP_ENCAP_MAX_CELLS; i++) {
		rep->cell_ids[i] = htons(cell_ids[i]);
	}

	return sizeof(ep_ecap_rep);
}

int epp_ecap_rep(
	char * buf, unsigned int size,
	uint32_t * cap,
	uint16_t * cell_ids,
	uint32_t * nof_cells)
{
	int           i   = 0;
	ep_ecap_rep * rep = (ep_ecap_rep *)buf;

	*cap       = ntohl(rep->cap);
	*nof_cells = ntohl(rep->nof_cells);

	for(i = 0; i < *nof_cells && i < EP_ENCAP_MAX_CELLS; i++) {
		cell_ids[i] = ntohs(rep->cell_ids[i]);
	}

	return EP_SUCCESS;
}

int epf_ecap_req(char * buf, unsigned int size)
{
	ep_ecap_req * rep = (ep_ecap_req *)buf;

	rep->dummy = 0;

	return sizeof(ep_ecap_req);
}

int epp_ecap_req(char * buf, unsigned int size)
{
	return EP_SUCCESS;
}

/******************************************************************************
 * Public API                                                                 *
 ******************************************************************************/

int epf_single_ecap_rep(
	char *     buf, unsigned int size,
	uint32_t   enb_id,
	uint16_t   cell_id,
	uint32_t   mod_id,
	uint32_t   cap_mask,
	uint16_t * cell_ids,
	uint32_t   nof_cells)
{
	int ms = 0;

	ms += epf_head(buf, size, EP_TYPE_SINGLE_MSG, enb_id, cell_id, mod_id);
	ms += epf_single(buf + ms, size - ms, EP_SIN_ECAP_MSG, EP_DIR_REPLY);
	ms += epf_ecap_rep(buf + ms, size - ms, cap_mask, cell_ids, nof_cells);

	return ms;
}

int epp_single_ecap_rep(
	char * buf, unsigned int size,
	uint32_t * cap_mask,
	uint16_t * cell_ids,
	uint32_t * nof_cells)
{
	return epp_ecap_rep(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size,
		cap_mask,
		cell_ids,
		nof_cells);
}

int epf_single_ecap_req(
	char * buf, unsigned int size,
	uint32_t enb_id,
	uint16_t cell_id,
	uint32_t mod_id)
{
	int ms = 0;

	ms += epf_head(buf, size, EP_TYPE_SINGLE_MSG, enb_id, cell_id, mod_id);
	ms += epf_single(buf + ms, size - ms, EP_SIN_ECAP_MSG, EP_DIR_REQUEST);
	ms += epf_ecap_req(buf + ms, size - ms);

	return ms;
}

int epp_single_ecap_req(char * buf, unsigned int size)
{
	return epp_ecap_req(
		buf + sizeof(ep_hdr) + sizeof(ep_s_hdr),
		size);
}
