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
 * Empower Agent internal scheduler logic.
 */

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <emage.h>
#include <emlog.h>
#include <emproto.h>

#include "agent.h"
#include "sched.h"

#include "msg.h"
#include "emlist.h"
#include "net.h"

/*
 * Some states when processing jobs
 */

#define JOB_NET_ERROR                          -1
#define JOB_CONSUMED                            0
#define JOB_NOT_ELAPSED                         1
#define JOB_RESCHEDULE                          2

/* Dif "b-a" two timespec structs and return such value in ms*/
#define ts_diff_to_ms(a, b)                     \
	(((b->tv_sec - a->tv_sec) * 1000) +     \
	 ((b->tv_nsec - a->tv_nsec) / 1000000))

/******************************************************************************
 * Utilities                                                                  *
 ******************************************************************************/

/* Fix the last details and send the message */
int sched_send_msg(struct agent * a, char * msg, unsigned int size)
{
	char     buf[EM_BUF_SIZE];
	uint32_t ms = htonl(size);

	if(size + EP_PROLOGUE_SIZE > EM_BUF_SIZE) {
		EMLOG("Message too long, msg=%u, limit=%u!",
			size + sizeof(uint32_t),
			EM_BUF_SIZE);

		return JOB_CONSUMED;
	}

	/* How long will be the message? */
	memcpy(buf, &ms, EP_PROLOGUE_SIZE);

	/* Insert the correct sequence number before sending */
	epf_seq(msg, size, net_next_seq(&a->net));

	memcpy(buf + EP_PROLOGUE_SIZE, msg, size);

	EMDBG("Sending a message of %d bytes...", size);

	if(net_send(&a->net, buf, size + EP_PROLOGUE_SIZE) < 0) {
		return JOB_NET_ERROR; /* On error */
	} else {
		return JOB_CONSUMED;  /* On success */
	}
}

/******************************************************************************
 * Jobs                                                                       *
 ******************************************************************************/

int sched_perform_send(struct agent * a, struct sched_job * job)
{
	return sched_send_msg(a, job->args, job->size);
}

int sched_perform_cell_setup(struct agent * a, struct sched_job * job)
{
	uint16_t cell_id = 0;

	/* Extracts only the cell id field */
	epp_head((char *)job->args, job->size, 0, 0, &cell_id, 0);

	if(a->ops && a->ops->cell_setup_request) {
		a->ops->cell_setup_request(cell_id);
	}

	return JOB_CONSUMED;
}

int sched_perform_enb_setup(struct agent * a, struct sched_job * job)
{
	if(a->ops && a->ops->enb_setup_request) {
		a->ops->enb_setup_request();
	}

	return JOB_CONSUMED;
}

int sched_perform_ue_report(struct agent * a, struct sched_job * job)
{
	uint32_t         mod = 0;
	struct trigger * t   = 0;

	if(a->ops && a->ops->ue_report) {
		epp_head((char *)job->args, job->size, 0, 0, 0, &mod);

		t = tr_has_trigger(&a->trig, mod, EM_TRIGGER_UE_REPORT);

		if(t) {
			a->ops->ue_report(mod, t->id, t->type);
		}
	}

	return JOB_CONSUMED;
}

#if 0
int sched_perform_enb_cells(struct agent * a, struct sched_job * job)
{
	EmageMsg * msg   = (EmageMsg *)job->args;
	EmageMsg * reply = 0;

	if(a->ops->eNB_cells_report) {
		a->ops->eNB_cells_report(msg, &reply);
	}

	/* Something to say at the controller? */
	if(reply) {
		sched_send_msg(a, reply);
		emage_msg__free_unpacked(reply, 0);
	}

	return JOB_CONSUMED;
}

int sched_perform_ran_sh(struct agent * a, struct sched_job * job) {
	EmageMsg * msg   = (EmageMsg *)job->args;
	EmageMsg * reply = 0;

	if(a->ops->ran_sharing_control) {
		a->ops->ran_sharing_control(msg, &reply);
	}

	/* Something to say at the controller? */
	if(reply) {
		sched_send_msg(a, reply);
		emage_msg__free_unpacked(reply, 0);
	}

	return JOB_CONSUMED;
}


/* This has to be used only in case of trigger events. */
int sched_perform_cell_tstats(struct agent * a, struct sched_job * job) {
	struct trigger * t = (struct trigger*)job->args;
	EmageMsg * reply = 0;

	if(a->ops->cell_statistics_report) {
		a->ops->cell_statistics_report(t->req, &reply, t->id);
	}

	/* Something to say at the controller? */
	if(reply) {
		sched_send_msg(a, reply);
		emage_msg__free_unpacked(reply, 0);
	}

	return JOB_CONSUMED;
}

int sched_perform_ctrl_cmd(struct agent * a, struct sched_job * job) {
	EmageMsg * msg   = (EmageMsg *)job->args;
	EmageMsg * reply = 0;

	switch(msg->se->mctrl_cmds->req->ctrl_cmd_case) {
	case CONTROLLER_COMMANDS_REQ__CTRL_CMD_CTRL_HO:
		/* Inform the stack of the hand-over request. */
		if(a->ops->handover_request) {
			a->ops->handover_request(msg, &reply);
		}

		if(reply) {
			sched_send_msg(a, reply);
			emage_msg__free_unpacked(reply, 0);
		}

		break;
	default:
		EMDBG("Unknown controller command detected");
	}

	return JOB_CONSUMED;
}
#endif

int sched_perform_hello(struct agent * a, struct sched_job * job) {
	char buf[EM_BUF_SIZE];
	int blen = 0;
	int sent = 0;
	int ret  = JOB_CONSUMED;

	blen = epf_single_hello_req(buf, EM_BUF_SIZE, a->b_id, 0, 0, 0);
	ret  = sched_send_msg(a, buf, blen);

	return ret;
}

#if 0
int sched_perform_RRC_mcon(struct agent * a, struct sched_job * job) {
	struct trigger * t = (struct trigger*)job->args;
	EmageMsg * reply = 0;

	/* Inform the stack about the activation of the logging mechanism. */
	if(a->ops->RRC_meas_conf) {
		a->ops->RRC_meas_conf(t->req, &reply, t->id);
	}

	/* Something to say at the controller? */
	if(reply) {
		sched_send_msg(a, reply);
		emage_msg__free_unpacked(reply, 0);
	}

	return JOB_CONSUMED;
}

int sched_perform_RRC_meas(struct agent * a, struct sched_job * job) {
	struct trigger * t = (struct trigger*)job->args;
	EmageMsg * reply = 0;

	/* Inform the stack about the activation of the logging mechanism. */
	if(a->ops->RRC_measurements) {
		a->ops->RRC_measurements(t->req, &reply, t->id);
	}

	/* Something to say at the controller? */
	if(reply) {
		sched_send_msg(a, reply);
		emage_msg__free_unpacked(reply, 0);
	}

	return JOB_CONSUMED;
}

int sched_perform_UEs_report(struct agent * a, struct sched_job * job) {
	struct trigger * t = (struct trigger*)job->args;
	EmageMsg * reply = 0;

	/* Inform the stack about the activation of the logging mechanism. */
	if(a->ops->UEs_ID_report) {
		a->ops->UEs_ID_report(t->req, &reply, t->id);
	}

	/* Something to say at the controller? */
	if(reply) {
		sched_send_msg(a, reply);
		emage_msg__free_unpacked(reply, 0);
	}

	return JOB_CONSUMED;
}
#endif

int sched_release_job(struct sched_job * job)
{
	EMDBG("Releasing a %d job", job->type);

	if(job->args) {
		if(job->args) {
			free(job->args);
			job->args = 0;
		}
	}

	free(job);
	return 0;
}

/******************************************************************************
 * Generic procedures:                                                        *
 ******************************************************************************/

int sched_add_job(struct sched_job * job, struct sched_context * sched) {
	int status = 0;

	clock_gettime(CLOCK_REALTIME, &job->issued);

/****** LOCK ******************************************************************/
	pthread_spin_lock(&sched->lock);

	/* Perform the job if the context is not stopped. */
	if(!sched->stop) {
		list_add(&job->next, &sched->jobs);
	} else {
		status = -1;
	}

	pthread_spin_unlock(&sched->lock);
/****** UNLOCK ****************************************************************/

	EMDBG("Scheduled a %d job for %d msec", job->type, job->elapse);

	return status;
}

int sched_perform_job(
	struct agent * a, struct sched_job * job, struct timespec * now) {

	int status = JOB_CONSUMED;
	struct timespec * is = &job->issued;

	/* Job not to be performed now. */
	if(ts_diff_to_ms(is, now) < job->elapse) {
		return JOB_NOT_ELAPSED;
	}

	EMDBG(" **************** Performing a job %d", job->type);

	switch(job->type) {
	case JOB_TYPE_SEND:
		status = sched_perform_send(a, job);
		break;
	case JOB_TYPE_HELLO:
		status = sched_perform_hello(a, job);
		break;
	case JOB_TYPE_ENB_SETUP:
		status = sched_perform_enb_setup(a, job);
		break;
	case JOB_TYPE_CELL_SETUP:
		status = sched_perform_cell_setup(a, job);
		break;
	case JOB_TYPE_UE_REPORT:
		status = sched_perform_ue_report(a, job);
		break;
#if 0
	case JOB_TYPE_UEs_LOG_TRIGGER:
		status = sched_perform_UEs_report(a, job);
		break;
	case JOB_TYPE_RRC_MEAS_TRIGGER:
		status = sched_perform_RRC_meas(a, job);
		break;
	case JOB_TYPE_RRC_MCON_TRIGGER:
		status = sched_perform_RRC_mcon(a, job);
		break;
	case JOB_TYPE_CELL_STATS_TRIGGER:
		status = sched_perform_cell_tstats(a, job);
		break;
	case JOB_TYPE_CTRL_COMMAND:
		status = sched_perform_ctrl_cmd(a, job);
		break;
	case JOB_TYPE_ENB_CELLS:
		status = sched_perform_enb_cells(a, job);
		break;
	case JOB_TYPE_RAN_SHARING:
		status = sched_perform_ran_sh(a, job);
		break;
#endif
	default:
		EMDBG("Unknown job cannot be performed, type=%d", job->type);
	}

	/* The job has to be rescheduled? */
	if(status == 0 && job->reschedule != 0) {
		return JOB_RESCHEDULE;
	}

	return status;
}

int sched_consume(struct sched_context * sched) {
	struct agent * a = container_of(sched, struct agent, sched);
	struct net_context * net = &a->net;
	struct sched_job * job = 0;
	struct sched_job * tmp = 0;
	struct timespec now;

	int op = 0;
	int nj = 1;	/* New job to consume. */
	int ne = 0;	/* Network error. */

	while(nj) {
/****** LOCK ******************************************************************/
		pthread_spin_lock(&sched->lock);

		/* Nothing to to? Go to sleep. */
		if(list_empty(&sched->jobs)) {
			nj = 0;
		}

		if(nj) {
			job = list_first_entry(
				&sched->jobs,
				struct sched_job,
				next);

			list_del(&job->next);
		}

		pthread_spin_unlock(&sched->lock);
/****** UNLOCK ****************************************************************/

		/* Nothing to do... out! */
		if(!nj) {
			break;
		}

		clock_gettime(CLOCK_REALTIME, &now);

		op = sched_perform_job(a, job, &now);

/****** LOCK ******************************************************************/
		pthread_spin_lock(&sched->lock);

		/* Possible outcomes. */
		switch(op) {
		case JOB_NOT_ELAPSED:
			list_add(&job->next, &sched->todo);
			break;
		case JOB_RESCHEDULE:
			job->issued.tv_sec  = now.tv_sec;
			job->issued.tv_nsec = now.tv_nsec;
			list_add(&job->next, &sched->todo);

			/* Consume one reschedule credit. */
			if(job->reschedule > 0) {
				job->reschedule--;
			}

			break;
		case JOB_CONSUMED:
			sched_release_job(job);
			break;
		case JOB_NET_ERROR:
			sched_release_job(job);
			ne = 1;
			break;
		}

		if(ne) {
			/* Dump job to process again. */
			list_for_each_entry_safe(job, tmp, &sched->todo, next) {
				list_del(&job->next);
				sched_release_job(job);
			}

			/* Free ANY remaining job still to process. */
			list_for_each_entry_safe(job, tmp, &sched->jobs, next) {
				list_del(&job->next);
				sched_release_job(job);
			}

			pthread_spin_unlock(&sched->lock);
/****** UNLOCK ****************************************************************/

			tr_flush(&a->trig);

			/* Signal the connection down now.
			 * We do it here since we are sure we cleaned all the
			 * jobs, and eventual new job(from a new successful
			 * connection) don't get deleted.
			 */
			net_not_connected(net);

			return 0;
		}

		pthread_spin_unlock(&sched->lock);
/****** UNLOCK ****************************************************************/
	}

	/* All the jobs marked as to process again are moved to the official
	 * job queue.
	 */
/****** LOCK ******************************************************************/
	/* Dump all the rescheduled jobs in the queue again. */
	pthread_spin_lock(&sched->lock);
	list_for_each_entry_safe(job, tmp, &sched->todo, next) {
		list_del(&job->next);
		list_add(&job->next, &sched->jobs);
	}
	pthread_spin_unlock(&sched->lock);
/****** UNLOCK ****************************************************************/

	return 0;
}

int sched_remove_job(unsigned int id, int type, struct sched_context * sched) {
	int found = 0;

	struct sched_job * job = 0;
	struct sched_job * tmp = 0;

	/* Dump the job from wherever it could be listed. */
/****** LOCK ******************************************************************/
	pthread_spin_lock(&sched->lock);
	list_for_each_entry_safe(job, tmp, &sched->jobs, next) {
		if(job->id == id && job->type == type) {
			found = 1;
			list_del(&job->next);

			/* There can be multiple jobs with the same id in case
			 * of cancellation events, so remove everything.
			 */
		}
	}

	/* Where is it? Already performed? */
	if(!found) {
		list_for_each_entry_safe(job, tmp, &sched->todo, next) {
			if(job->id == id && job->type == type) {
				found = 1;
				list_del(&job->next);

				/* There can be multiple jobs with the same id
				 * in case of cancellation events, so remove
				 * everything.
				 */
			}
		}
	}
	pthread_spin_unlock(&sched->lock);
/****** UNLOCK ****************************************************************/

	if(!found) {
		EMDBG("Job %d NOT found!", job->id);
		return -1;
	}

	EMDBG("Job %d removed from the scheduler", job->id);

	sched_release_job(job);

	return 0;
}

/******************************************************************************
 * Scheduler procedures.                                                      *
 ******************************************************************************/

void * sched_loop(void * args) {
	struct sched_context * s = (struct sched_context *)args;

	unsigned int wi = s->interval;
	struct timespec wt = {0};
	struct timespec td = {0};

	struct sched_job * job = 0;
	struct sched_job * tmp = 0;

	/* Convert the wait interval in a timespec struct. */
	while(wi >= 1000) {
		wi -= 1000;
		wt.tv_sec += 1;
	}
	wt.tv_nsec = wi * 1000000;

	EMDBG("Scheduling loop starting, interval=%d", s->interval);

	while(!s->stop) {
		/* Job scheduling logic. */
		sched_consume(s);

		/* Relax the CPU. */
		nanosleep(&wt, &td);
	}

/****** LOCK ******************************************************************/
	pthread_spin_lock(&s->lock);
	/* Dump job to process again. */
	list_for_each_entry_safe(job, tmp, &s->todo, next) {
		list_del(&job->next);
		sched_release_job(job);
	}

	/* Free ANY remaining job still to process. */
	list_for_each_entry_safe(job, tmp, &s->jobs, next) {
		list_del(&job->next);
		sched_release_job(job);
	}
	pthread_spin_unlock(&s->lock);
/****** UNLOCK ****************************************************************/

	/*
	 * If execution arrives here, then a stop has been issued.
	 */
out:
	EMDBG("Scheduling loop is terminating...\n");
	return 0;
}

int sched_start(struct sched_context * sched) {
	sched->interval = 1;

	INIT_LIST_HEAD(&sched->jobs);
	INIT_LIST_HEAD(&sched->todo);
	pthread_spin_init(&sched->lock, 0);

	/* Create the context where the agent scheduler will run on. */
	if(pthread_create(&sched->thread, NULL, sched_loop, sched)) {
		EMLOG("Failed to create the scheduler thread.");
		return -1;
	}

	return 0;
}

int sched_stop(struct sched_context * sched) {
	/* Stop and wait for it... */
	sched->stop = 1;
	pthread_join(sched->thread, 0);

	pthread_spin_destroy(&sched->lock);

	return 0;
}
