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
 * Empower Agent.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <pthread.h>

#include <emage.h>
#include <emlog.h>

#include "config.h"
#include "agent.h"
#include "emlist.h"
#include "net.h"
#include "sched.h"

#include <emproto.h>

/* Static location of the configuration file. */
#define EM_CONFIG_FILE			"/etc/empower/agent.conf"

/* For the first run only. */
int initialized = 0;

/* Configuration of the agent. */
struct config_profile em_conf = {0};

/* Agents which are actually active. */
LIST_HEAD(em_agents);
/* Lock for handling the agents list. */
pthread_spinlock_t em_agents_lock;

/******************************************************************************
 * Misc.                                                                      *
 ******************************************************************************/

/* Schedule the send of a message. */
int add_send_job(struct agent * a, char * msg, unsigned int size) {
	char * buf;
	struct sched_job * s = 0;

	int status = -1;

	s = malloc(sizeof(struct sched_job));

	if(!s) {
		EMLOG("No more memory!");
		return -1;
	}

	buf = malloc(sizeof(char) * size);

	if(!buf) {
		EMLOG("No more memory!");

		free(s);
		return -1;
	}

	memcpy(buf, msg, sizeof(char) * size);

	INIT_LIST_HEAD(&s->next);
	s->args       = buf;
	s->size       = size;
	s->elapse     = 1;
	s->type       = JOB_TYPE_SEND;
	s->reschedule = 0;

	status = sched_add_job(s, &a->sched);

	/* Some error occurs?*/
	if(status) {
		free(buf);
		free(s);
	}

	return status;
}

int em_has_trigger(int enb_id, int tid, int ttype) {
	struct agent * a = 0;

	int found = 0;
	struct trigger * t = 0;

/****** LOCK ******************************************************************/
	pthread_spin_lock(&em_agents_lock);
	list_for_each_entry(a, &em_agents, next) {
		if(a->b_id == enb_id) {
			t = tr_has_trigger(&a->trig, tid, ttype);
			break;
		}
	}
	pthread_spin_unlock(&em_agents_lock);
/****** UNLOCK ****************************************************************/

	return t ? 1 : 0;
}

int em_is_connected(int enb_id) {
	struct agent * a = 0;
	int found = 0;

/****** LOCK ******************************************************************/
	pthread_spin_lock(&em_agents_lock);
	list_for_each_entry(a, &em_agents, next) {
		if(a->b_id == enb_id) {
			/* 1/0 evaluation operation. */
			found = (a->net.status == EM_STATUS_CONNECTED);
			break;
		}
	}
	pthread_spin_unlock(&em_agents_lock);
/****** UNLOCK ****************************************************************/

	return found;
}


/* Read the configuration file and set the globals properly. Returns a negative
 * error number on error.
 */
int em_load_config(char * config_path, struct config_profile * config) {
	char buf[4096];
	int br = 0;

	char * token = 0;

	int fd = open(EM_CONFIG_FILE, O_RDONLY);

	if(fd < 0) {
		EMLOG("Failed to load config file %s.", config_path);
		return -1;
	}

	br = read(fd, buf, 4096);

	if(br <= 0) {
		EMLOG("Nothing to be read from the %s config file!",
			config_path);

		close(fd);
		return -1;
	}

	token = strtok(buf, " ");

	if(!token) {
		EMLOG("Configuration file address is malformed.");
		close(fd);
		return -1;
	}

	strncpy(config->ctrl_ipv4_addr, token, 16);
	token = strtok(NULL, " ");

	if(!token) {
		EMLOG("Configuration file port is malformed.");
		close(fd);
		return -1;
	}

	config->ctrl_port = atoi(token);

	close(fd);
	return 0;
}

int em_init(void) {
	if(!initialized) {
		/* Initialize locking. */
		pthread_spin_init(&em_agents_lock, 0);

		if(em_load_config(EM_CONFIG_FILE, &em_conf)) {
			EMLOG("No configuration file...");
			return -1;
		}

		EMDBG("Configuration file loaded successfully...");

		/* Don't perform initialization again. */
		initialized = 1;
	}

	return 0;
}

int em_release_agent(struct agent * a) {
	free(a);

	return 0;
}

int em_send(int enb_id, char * msg, unsigned int size) {
	struct agent * a = 0;

	int found = 0;
	int status = 0;

/****** LOCK ******************************************************************/
	pthread_spin_lock(&em_agents_lock);
	list_for_each_entry(a, &em_agents, next) {
		if(a->b_id == enb_id) {
			status = add_send_job(a, msg, size);

			break;
		}
	}
	pthread_spin_unlock(&em_agents_lock);
/****** UNLOCK ****************************************************************/

	return status;
}

int em_terminate_agent(int b_id) {
	struct agent * a = 0;
	struct agent * b = 0;

	int found = 0;
	int status = 0;

/****** LOCK ******************************************************************/
	pthread_spin_lock(&em_agents_lock);
	list_for_each_entry_safe(a, b, &em_agents, next) {
		if(a->b_id == b_id) {
			list_del(&a->next);
			found = 1;
			break;
		}
	}
	pthread_spin_unlock(&em_agents_lock);
/****** UNLOCK ****************************************************************/

	if(found) {
		if(a->ops->release) {
			status = a->ops->release();
		}

		tr_flush(&a->trig);
		pthread_spin_destroy(&a->trig.lock);

		net_stop(&a->net);
		sched_stop(&a->sched);

		EMDBG("Releasing agent for base station %d", a->b_id);
		em_release_agent(a);
	}

	return status;
}

/******************************************************************************
 * Entry/exit points for the Agent.                                           *
 ******************************************************************************/

int em_start(struct em_agent_ops * ops, int b_id) {
	struct agent * a = 0;

	int status = 0;
	int running = 0;	/* Already running? */
	int nm = 0;		/* No mem? */

	if(em_init()) {
		return -1;
	}

	/* Any check for necessary call-backs here. For the moment you can also
	 * implement no call-backs: your agent will simply do nothing.
	 */
	if(!ops) {
		EMLOG("Invalid set of operations...");
		return -1;
	}

	pthread_spin_lock(&em_agents_lock);
	/* Find for an already present agent */
	list_for_each_entry(a, &em_agents, next) {
		if(a->b_id == b_id) {
			running = 1;
			break;
		}
	}

	if(!running) {
		a = malloc(sizeof(struct agent));

		if(a) {
			memset(a, 0, sizeof(struct agent));
			INIT_LIST_HEAD(&a->next);
			a->b_id = b_id;

			list_add(&a->next, &em_agents);
		} else {
			nm = 1;
		}

	}
	pthread_spin_unlock(&em_agents_lock);

	if(running) {
		EMLOG("Agent for base station %d is already running...",
			b_id);

		return -1;
	}

	if(nm) {
		EMLOG("Not enough memory!");
		return -1;
	}

	EMDBG("New agent for %d created", b_id);

	memcpy(a->net.addr, em_conf.ctrl_ipv4_addr, 16);
	a->net.port = em_conf.ctrl_port;
	a->ops = ops;

	a->trig.next = 1;
	pthread_spin_init(&a->trig.lock, 0);
	INIT_LIST_HEAD(&a->trig.ts);

	if (a->ops->init) {
		status = a->ops->init();

		/* On error, do not launch the agent */
		if (status < 0) {
			EMLOG("Custom initialization failed with error %d",
				status);

			pthread_spin_lock(&em_agents_lock);
			list_del(&a->next);
			pthread_spin_unlock(&em_agents_lock);

			em_release_agent(a);

			return status;
		}
	}

	/*
	 * Start this agent scheduler
	 */

	if(sched_start(&a->sched)) {
		pthread_spin_lock(&em_agents_lock);
		list_del(&a->next);
		pthread_spin_unlock(&em_agents_lock);

		EMLOG("Failed to create the agent scheduler thread.");
		em_release_agent(a);

		return -1;
	}

	/*
	 * Start this agent networking operations
	 */

	if(net_start(&a->net)) {
		pthread_spin_lock(&em_agents_lock);
		list_del(&a->next);
		pthread_spin_unlock(&em_agents_lock);

		EMLOG("Failed to create the listener agent thread.");
		sched_stop(&a->sched);
		em_release_agent(a);

		return -1;
	}

	return 0;
}

int em_stop(void) {
	struct agent * a = 0;

	while(!list_empty(&em_agents)) {
		pthread_spin_lock(&em_agents_lock);
		a = list_first_entry(&em_agents, struct agent, next);
		list_del(&a->next);
		pthread_spin_unlock(&em_agents_lock);

		if(a->ops->release) {
			a->ops->release();
		}

		tr_flush(&a->trig);
		pthread_spin_destroy(&a->trig.lock);

		net_stop(&a->net);
		sched_stop(&a->sched);

		EMDBG("Releasing agent for base station %d", a->b_id);
		em_release_agent(a);
	}

	EMLOG("Shut down...");

	return 0;
}
