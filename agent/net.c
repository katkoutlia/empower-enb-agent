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
 * Empower Agent internal network listener logic.
 */

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <emage.h>
#include <emlog.h>
#include <emproto.h>

#include "agent.h"
#include "net.h"
#include "sched.h"

#include "emlist.h"
#include "sched.h"

#define NET_RECONNECT_TIME      1000000 /* 1sec in usec */
#define NET_WAIT_TIME           16000   /* 16ms in usec */

/******************************************************************************
 * Network procedures.                                                        *
 ******************************************************************************/

/* Common operations done when it successfully connects again. */
int net_connected(struct net_context * net) {
	struct agent * a = container_of(net, struct agent, net);
	struct sched_job * h = 0;

	EMDBG("Connected to controller %s:%d", net->addr, net->port);
	net->status = EM_STATUS_CONNECTED;

	h = malloc(sizeof(struct sched_job));

	if(!h) {
		EMLOG("No more memory!");
		return -1;
	}

	INIT_LIST_HEAD(&h->next);
	h->elapse     = 1000;
	h->type       = JOB_TYPE_HELLO;
	h->reschedule = -1;

	/* Add the Hello message. */
	sched_add_job(h, &a->sched);

	return 0;
}

unsigned int net_next_seq(struct net_context * net) {
	int ret = 0;

/****** LOCK ******************************************************************/
	pthread_spin_lock(&net->lock);
	ret = net->seq++;
	pthread_spin_unlock(&net->lock);
/****** UNLOCK ****************************************************************/

	return ret;
}

/* Turn the socket in an non-blocking one. */
int net_noblock_socket(int sockfd) {
	int flags = fcntl(sockfd, F_GETFL, 0);

	if(flags < 0) {
		return -1;
	}

	return fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

/* Apply the desired personalization to the socket. */
int net_nodelay_socket(int sockfd) {
	int flag = 1; /* Enable no delay... */
	int result = setsockopt(
		sockfd,
		SOL_TCP,
		TCP_NODELAY,
		(char *) &flag,
		sizeof(int));

	if (result < 0) {
		EMLOG("Could not personalize the socket!");
		perror("setsockopt");

		return -1;
	}

	return 0;
}

int net_not_connected(struct net_context * net) {
	EMDBG("No more connected with controller!");

	if(net->sockfd > 0) {
		close(net->sockfd);
		net->sockfd = -1;
	}

	net->status = EM_STATUS_NOT_CONNECTED;
	net->seq = 0;

	return 0;
}

/* Connect to the controller. Returns the open socket file descriptor if a
 * socket for the controller has been open, otherwise loop until a connection
 * has been established.
 *
 * Returns a negative number on error.
 */
int net_connect_to_controller(struct net_context * net) {
	int status = 0;

	struct sockaddr_in srvaddr = {0};
	struct hostent * ctrli = 0;

	if(net->sockfd < 0) {
		status = socket(AF_INET, SOCK_STREAM, 0);
	}

	if(status < 0) {
		EMLOG("Could not create the socket, error=%d", net->sockfd);
		perror("socket");

		return -1;
	}
	/* Socket has been created now. */
	else if (status > 0) {
		net->sockfd = status;
		net_nodelay_socket(net->sockfd);
	}

	EMDBG("Connecting to %s:%d...", net->addr, net->port);

	ctrli = gethostbyname(net->addr);

	if(!ctrli) {
		EMLOG("Could not resolve controller!");
		perror("gethostbyname");

		return -1;
	}

	srvaddr.sin_family = AF_INET;
	memcpy(
		&srvaddr.sin_addr.s_addr,
		ctrli->h_addr,
		ctrli->h_length);
	srvaddr.sin_port = htons(net->port);

	status = connect(
		net->sockfd,
		(struct sockaddr *)&srvaddr,
		sizeof(struct sockaddr));

	if(status < 0) {
		EMDBG("Error while connecting to %s, error=%d",
			net->addr,
			status);

		return -1;
	}

	return 0;
}

/* Receive data. */
int net_recv(struct net_context * context, char * buf, unsigned int size) {
	return recv(context->sockfd, buf, size, MSG_DONTWAIT | MSG_NOSIGNAL);
}

/* Send data. */
int net_send(struct net_context * context, char * buf, unsigned int size) {
#ifdef EM_DEBUG_SEND
	int i = 0;

	for(i = 0; i < size; i++) {
		if(i % 16 == 0) {
			printf("\n");
		}

		printf("%02x ", (unsigned char)buf[i]);
	}

	printf("\n");
#endif

	/* NOTE:
	 * Since sending on a dead socket can cause a signal to be issued to the
	 * application (SIGPIPE), we don't want that the host get disturbed by
	 * this, and so we ask not to notify the error.
	 */
	return send(context->sockfd, buf, size, MSG_DONTWAIT | MSG_NOSIGNAL);
}

int net_sched_job(
	struct agent * a,
	unsigned int id,
	int type,
	int interval,
	int res,
	void * args,
	unsigned int size) {

	struct sched_job * job = malloc(sizeof(struct sched_job));

	if(!job) {
		EMLOG("Not enough memory!");
		return -1;
	}

	memset(job, 0, sizeof(struct sched_job));

	if(args) {
		job->args = malloc(sizeof(char) * size);

		if(!job->args) {
			free(job);
			EMLOG("Not enough memory!");
			return -1;
		}

		memcpy(job->args, args, sizeof(char) * size);
	}

	INIT_LIST_HEAD(&job->next);
	job->type       = type;
	job->size       = size;
	job->id         = id;
	job->elapse     = interval;
	job->reschedule = res;

	sched_add_job(job, &a->sched);

	return 0;
}

/******************************************************************************
 * Message specific procedures.                                               *
 ******************************************************************************/

int net_se_cell_setup(struct net_context * net, char * msg, int size)
{
	uint32_t       seq;
	struct agent * a = container_of(net, struct agent, net);

	seq = epp_seq(msg, size);

	return net_sched_job(a, seq, JOB_TYPE_CELL_SETUP, 1, 0, msg, size);
}

int net_se_enb_setup(struct net_context * net, char * msg, int size)
{
	uint32_t       seq;
	struct agent * a = container_of(net, struct agent, net);

	seq = epp_seq(msg, size);

	return net_sched_job(a, seq, JOB_TYPE_ENB_SETUP, 1, 0, msg, size);
}

int net_te_ue_report(struct net_context * net, char * msg, int size)
{
	uint32_t         mod;
	uint32_t         seq;
	uint32_t         op;

	struct trigger * t;
	struct agent *   a = container_of(net, struct agent, net);

	epp_head(msg, size, 0, 0, 0, &mod);

	seq = epp_seq(msg, size);
	op  = epp_trigger_op(msg, size);

	if(op == EP_OPERATION_ADD) {
		t = tr_add(&a->trig, mod, EM_TRIGGER_UE_REPORT, 0, 0);

	} else {
		return tr_del(&a->trig, mod, EM_TRIGGER_UE_REPORT);
	}

	return net_sched_job(a, seq, JOB_TYPE_UE_REPORT, 1, 0, msg, size);
}

/******************************************************************************
 * Top-level message handlers.                                                *
 ******************************************************************************/

int net_process_sched_event(
	struct net_context * net, char * msg, unsigned int size)
{
	ep_act_type s = epp_schedule_type(msg, size);

	if(s == EP_ACT_INVALID) {
		EMDBG("Malformed schedule-event message received!\n");
		return -1;
	}

	switch(s) {
	case EP_ACT_HELLO:
		/* Don't really care about the hello reply now */
		break;
	default:
		EMDBG("Unknown scheduled event, type=%d", s);
		break;
	}

	return 0;
}

int net_process_single_event(
	struct net_context * net, char * msg, unsigned int size)
{
	ep_act_type s = epp_single_type(msg, size);

	if(s == EP_ACT_INVALID) {
		EMDBG("Malformed single-event message received!\n");
		return -1;
	}

	switch(s) {
	case EP_ACT_HELLO:
		if(epp_single_dir(msg, size) == EP_DIR_REPLY) {
			EMDBG("Hello reply received!");
		}
		break;
	case EP_ACT_ECAP:
		if(epp_single_dir(msg, size) == EP_DIR_REQUEST) {
			EMDBG("eNB capabilities request received!");
			return net_se_enb_setup(net, msg, size);
		}
		break;
	default:
		EMDBG("Unknown single event, type=%d", s);
		break;
	}

	return 0;
}

int net_process_trigger_event(
	struct net_context * net, char * msg, unsigned int size)
{
	ep_act_type t = epp_trigger_type(msg, size);

	if(t == EP_ACT_INVALID) {
		EMDBG("Malformed trigger-event message received!\n");
		return -1;
	}

	switch(t) {
	case EP_ACT_HELLO:
		/* Don't really care about the hello reply now */
		break;
	case EP_ACT_UE_REPORT:
		return net_te_ue_report(net, msg, size);
#if 0
	case TRIGGER_EVENT__EVENTS_M_UES_ID:
		return net_te_usid(net, msg);
	case TRIGGER_EVENT__EVENTS_M_RRC_MEAS:
		return net_te_rrc_meas(net, msg);
	case TRIGGER_EVENT__EVENTS_M_UE_RRC_MEAS_CONF:
		return net_te_rrc_mcon(net, msg);
	case TRIGGER_EVENT__EVENTS_M_CELL_STATS:
		return net_te_cell_stats(net, msg);
#endif
	default:
		EMDBG("Unknown trigger event, type=%d", t);
		break;
	}

	return 0;
}

/* Process incoming messages. */
int net_process_message(struct net_context * net, char * msg, unsigned int size)
{
	ep_msg_type mt = epp_msg_type(msg, size);

	switch(mt) {
	/* Single events messages. */
	case EP_TYPE_SINGLE_MSG:
		return net_process_single_event(net, msg, size);
	/* Scheduled events messages. */
	case EP_TYPE_SCHEDULE_MSG:
		return net_process_sched_event(net, msg, size);
	/* Triggered events messages. */
	case EP_TYPE_TRIGGER_MSG:
		return net_process_trigger_event(net, msg, size);
	default:
		EMDBG("Unknown message received, size=%d", size);
		break;
	}

	return 0;
}

/******************************************************************************
 * Network listener logic.                                                    *
 ******************************************************************************/

void * net_loop(void * args)
{
	struct net_context * net = (struct net_context *)args;

	int op;
	int bread;
	int mlen  = 0;

	char buf[EM_BUF_SIZE] = {0};

	unsigned int wi = net->interval;
	struct timespec wt = {0};	/* Wait time. */
	struct timespec td = {0};

	/* Convert the wait interval in a timespec struct. */
	while(wi >= 1000) {
		wi -= 1000;
		wt.tv_sec += 1;
	}
	wt.tv_nsec = wi * 1000000;

	while(!net->stop) {
next:
		if(net->status == EM_STATUS_NOT_CONNECTED) {
			if(net_connect_to_controller(net) == 0) {
				net_connected(net);
			}

			/* Relax the CPU. */
			usleep(NET_RECONNECT_TIME);
			continue;
		}

		bread = 0;

		/* Continue until EP_PROLOGUE_SIZE bytes have been collected */
		while(bread < EP_PROLOGUE_SIZE) {
			op = net_recv(
				net, buf + bread, EP_PROLOGUE_SIZE - bread);

			if(op <= 0) {
				if(errno == EAGAIN) {
					/* Relax the CPU. */
					usleep(NET_WAIT_TIME);
					continue;
				}

				net_not_connected(net);
				goto next;
			}

			bread += op;
		}

		memcpy(&mlen, buf, bread);
		mlen = ntohl(mlen);

		EMDBG("Receiving a message of size %d", mlen);

		bread = 0;

		/* Continue until the entire message has been collected */
		while(bread < mlen) {
			op = net_recv(net, buf + bread, mlen - bread);

			if(op <= 0) {
				if(errno == EAGAIN) {
					/* Relax the CPU. */
					usleep(NET_WAIT_TIME);
					continue;
				}

				net_not_connected(net);
				goto next;
			}

			bread += op;
		}

		/* Finally we collected the entire message; process it! */
		net_process_message(net, buf, bread);
	}

	EMDBG("Listening loop is terminating...");

	/*
	 * If you need to release 'net' specific resources, do it here!
	 */

	return 0;
}

int net_start(struct net_context * net)
{
	/* 1 second interval by default. */
	net->interval = 100;
	net->sockfd   = -1;

	pthread_spin_init(&net->lock, 0);

	/* Create the context where the agent scheduler will run on. */
	if(pthread_create(
		(pthread_t *)&net->thread, NULL, net_loop, net)) {

		EMLOG("Failed to create the listener agent thread.");
		return -1;
	}

	return 0;
}

int net_stop(struct net_context * net)
{
	/* Stop and wait for it... */
	net->stop = 1;
	pthread_join(net->thread, 0);

	pthread_spin_destroy(&net->lock);

	return 0;
}
