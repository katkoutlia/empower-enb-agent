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

#ifndef __EMAGE_H
#define __EMAGE_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Possible triggers which can be installed in the agent. */
typedef enum em_trigger_type {
	EM_TRIGGER_NONE = 0,
	/* UE report trigger. */
	EM_TRIGGER_UE_REPORT,
#if 0
	/* RRC measurements trigger. */
	EM_RRC_MEAS_TRIGGER,
	/* RRC measurements configuration trigger. */
	EM_RRC_MEAS_CONF_TRIGGER,
	/* Cell statistics trigger. */
	EM_CELL_STATS_TRIGGER,
#endif
} EM_TRIGGER_TYPE;

/* Defines the operations that can be customized depending on the technology
 * where you want to embed the agent to. Such procedures will be called by the
 * agent main logic while responding to the controller orders or events
 * triggered by the local system.
 */
struct em_agent_ops {
	/* Perform custom initialization for the technology abstraction layer.
	 *
	 * Reporting an error during initialization stages cause the agent to
	 * fail.
	 *
	 * Returns 0 on success, a negative error code otherwise.
	 */
	int (* init) (void);

	/* Perform custom initialization for the technology abstraction layer.
	 * Regardless of error returns codes, the agent will be stopped.
	 *
	 * Returns 0 on success, a negative error code otherwise.
	 */
	int (* release) (void);

	/*
	 * Setup messages:
	 */

	/* The controller requested to this agent the setup of the base station,
	 * which means how it is configured, what operation supports and so on.
	 *
	 * Filling the reply field with an Empower message will cause the reply
	 * to be sent back to the controller.
	 *
	 * Returns 0 on success, a negative error code otherwise.
	 */
	int (* enb_setup_request) (void);

	/*
	 * Capabilities-related procedures:
	 */

	/* Informs the stack that a log for UE activity has been required by the
	 * controller. The wrapper shall perform operations to enable such
	 * functionality into the base station.
	 *
	 * Mod. represent the module which requested for such report.
	 * The id given has to be used to check for its existence later.
	 *
	 * Returns 0 on success, a negative error code otherwise.
	 */
	int (* ue_report) (unsigned int mod, int trig_id, int trig_type);
};

/* Peek the triggers of the given agent and check if a trigger is enabled or
 * not. This is useful to avoid doing some heavy operation and just being denied
 * at the end.
 *
 * Returns 1 if the trigger is enabled, 0 otherwise.
 */
int em_has_trigger(int enb_id, int tid, int ttype);

/* Check if the agent is currently connected to a controller.
 *
 * Returns 1 if the agent is connected to a controller, 0 otherwise.
 */
int em_is_connected(int enb_id);

/* Send a message to the connected controller, if any controller is attached.
 * This operations is only possible if the agent for that particular id has
 * already been created.
 *
 * Returns 0 if the message is successfully sent, a negative error code
 * otherwise.
 */
int em_send(int enb_id, char * msg, unsigned int size);

/* Start the Empower Agent logic. This will cause the agent to start interacting
 * with a remote controller or to local events. You need to pass the technology
 * dependant callbacks and the base station identifier.
 *
 * Information about controller address and characteristics are taken by the
 * configuration file which must be present in your machine.
 *
 * Returns 0 on success, or a negative error code on failure.
 */
int em_start(struct em_agent_ops * ops, int enb_id);

/* Stop the Empower Agent logic. This will cause the agent to stop to all the
 * controller commands and local events.
 *
 * Returns 0 on success, or a negative error code on failure.
 */
int em_stop(void);

/* Terminate a single agent instance using its ID.
 *
 * Always returns 0.
 */
int em_terminate_agent(int b_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EMAGE_H */
