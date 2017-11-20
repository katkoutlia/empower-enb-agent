/* Copyright (c) 2016 Kewin Rausch
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
 * Empower Agent internal triggers logic.
 */

#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include <emlog.h>
#include <emage/emproto.h>

#include "triggers.h"

struct trigger * tr_add(
	struct tr_context * tc,
	int id, int mod, int type, int instance,
	char * req, unsigned char size)
{
	struct trigger * t = tr_has_trigger_ext(tc, mod, type, instance);

	if(t) {
		EMDBG("Trigger %d already exists", type);
		return t;
	}

	t = malloc(sizeof(struct trigger));

	if(!t) {
		EMLOG("Not enough memory for new trigger!");
		return 0;
	}

	memset(t, 0, sizeof(struct trigger));

	if(req) {
		t->req = malloc(sizeof(char) * size);

		if(!t->req) {
			EMLOG("Not enough memory for new trigger!");
			free(t);
			return 0;
		}

		memcpy(t->req, req, size);
		t->size = size;
	}

	INIT_LIST_HEAD(&t->next);
	t->id       = id;
	t->mod      = mod;
	t->type     = type;
	t->instance = instance;

	pthread_spin_lock(&tc->lock);
	list_add(&t->next, &tc->ts);
	pthread_spin_unlock(&tc->lock);

	EMDBG("New trigger enabled, id=%d, type=%d", id, type);

	return t;
}

int tr_del(struct tr_context * tc, int mod, int type, int instance)
{
	struct trigger * t = 0;
	struct trigger * u = 0;
	int found = 0;

	pthread_spin_lock(&tc->lock);
	list_for_each_entry_safe(t, u, &tc->ts, next) {
		if(t->type == type &&
			t->mod == mod &&
			t->instance == instance) {

			list_del(&t->next);
			pthread_spin_unlock(&tc->lock);

			if(t->req) {
				free(t->req);
			}

			tr_free(t);
			return 0;
		}
	}
	pthread_spin_unlock(&tc->lock);

	return -1;
}

struct trigger * tr_find(struct tr_context * tc, int id)
{
	struct trigger * t = 0;
	int found = 0;

	pthread_spin_lock(&tc->lock);
	list_for_each_entry(t, &tc->ts, next) {
		if(t->id == id) {
			pthread_spin_unlock(&tc->lock);
			return t;
		}
	}
	pthread_spin_unlock(&tc->lock);

	return 0;
}

struct trigger * tr_has_trigger(struct tr_context * tc, int id)
{
	struct trigger * t = 0;
	int found = 0;

	pthread_spin_lock(&tc->lock);
	list_for_each_entry(t, &tc->ts, next) {
		if(t->id == id) {
			found = 1;
			break;
		}
	}
	pthread_spin_unlock(&tc->lock);

	if(!found) {
		return 0;
	}

	return t;
}

struct trigger * tr_has_trigger_ext(
	struct tr_context * tc, int mod, int type, int instance)
{
	struct trigger * t = 0;
	int found = 0;

	pthread_spin_lock(&tc->lock);
	list_for_each_entry(t, &tc->ts, next) {
		if(t->mod == mod &&
			t->type == type &&
			t->instance == instance) {

			found = 1;
			break;
		}
	}
	pthread_spin_unlock(&tc->lock);

	if(!found) {
		return 0;
	}

	return t;
}

int tr_flush(struct tr_context * tc)
{
	struct trigger * t = 0;
	struct trigger * u = 0;

	pthread_spin_lock(&tc->lock);
	list_for_each_entry_safe(t, u, &tc->ts, next) {
		EMDBG("Flushing out trigger %d", t->id);

		list_del(&t->next);
		tr_free(t);
	}
	pthread_spin_unlock(&tc->lock);

	return 0;
}

void tr_free(struct trigger * t)
{
	if(t) {
		if(t->req) {
			free(t->req);
		}

		free(t);
	}
}

int tr_next_id(struct tr_context * tc)
{
	struct trigger * t = 0;
	int              n = 0;

	/* Select a random trigger ID which is not already present */
	do {
		/* Rand generate values from 0 to RAND_MAX */
		n = rand();

		if(!n) {
			n++;
		}

		pthread_spin_lock(&tc->lock);
		list_for_each_entry(t, &tc->ts, next) {
			if(n == t->id) {
				n = 0;
				break;
			}
		}
		n = tc->next++;
		pthread_spin_unlock(&tc->lock);
	} while(!n);

	return n;
}

int tr_rem(struct tr_context * tc, int id, int type)
{
	struct trigger * t = 0;
	struct trigger * u = 0;

	pthread_spin_lock(&tc->lock);
	list_for_each_entry_safe(t, u, &tc->ts, next) {
		if(t->id == id) {
			EMDBG("Removing trigger %d", t->id);

			list_del(&t->next);
			break;
		}
	}
	pthread_spin_unlock(&tc->lock);

	return 0;
}
