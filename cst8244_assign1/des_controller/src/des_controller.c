/**************************************************
 * File: des_controller.c
 * Authors:
 * Frederic Desjardins (040941359)
 * Barzin Farahani (040922695)
 *
 *
 * Description:
 * This program asts as the server to receive user input
 *
 **************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "des.h"

#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <errno.h>

Person person;

int prev_state = START_STATE;
int direction_entered = -1; //Person has not entered yet

void start();
void left_scan();
void right_scan();
void weight_scan();
void left_open();
void right_open();
void left_close();
void right_close();
void guard_left_unlock();
void guard_right_unlock();
void guard_left_lock();
void guard_right_lock();
void exit_state();

void (*transitionStates[13])() = {
	start,
	left_scan,
	right_scan,
	weight_scan,
	left_open,
	right_open,
	left_close,
	right_close,
	guard_left_unlock,
	guard_right_unlock,
	guard_left_lock,
	guard_right_lock,
	exit_state
	};

	int main(int argc, char* argv[]) {

		Display display;

		int inputs_chid; //channel id
		int rcvid;  //MsgReceive return
		int display_coid; //connection id

		pid_t pid; //process id

		pid_t display_pid;

		int progOn = 0;

		if (argv[1] != NULL) {
			display_pid = atoi(argv[1]);
			printf("Controller PID: %d\n", getpid());
			//printf("Display PID: %d\n", display_pid);
		} else {
			puts("Invalid argument: Must supply a PID\n");
		}

		inputs_chid = ChannelCreate(0);

		if (inputs_chid == -1) {
			printf("Error creating channel...\n");
			exit(EXIT_FAILURE);
		} else {
			//printf("Channel created successfully.\n");
		}

		display_coid = ConnectAttach(ND_LOCAL_NODE, display_pid, 1,
		_NTO_SIDE_CHANNEL, 0);

		if (display_coid == -1) {
			printf("Error calling CallingAttach()\n");
			//exit(EXIT_FAILURE);
		} else {
			//printf("ConnectAttach() successful\n");
		}

		printf("[Controller] - Waiting for person...\n");

		/*
		 person.id = 69;
		 person.weight = 420;

		 */
		while (progOn == 0) {

			rcvid = MsgReceive(inputs_chid, &person, sizeof(person), NULL);

			//sleep(500);

			if (rcvid == -1) { /* receive message from client */
				printf("Message Received failed\n"); /* ON FAIL */
			}

			transitionStates[person.state]();

			if (MsgSend(display_coid, &person, sizeof(person), &display,
					sizeof(display)) == -1) {
				printf("[Controller] - Error sending message to display\n");
			}

			if (person.state != EXIT_STATE) {
				MsgReply(rcvid, EOK, &display, sizeof(display));
			} else {
				printf("[Controller] - Exiting...\n");
				break;
			}

		}

		puts("Controller Successful\n"); /* prints Hello World!!! */
		ConnectDetach(display_coid); /* Detach from display */
		ChannelDestroy(inputs_chid);/* destroy channel */
		return EXIT_SUCCESS;
	}

	void start() {
		printf("[Controller] - Waiting for person...\n");
		prev_state = START_STATE;
	}
	void left_scan() {

		if (prev_state != START_STATE) {
			person.state = prev_state;
			printf("[Controller] - Invalid command, State Returned To: %s\n",
					debugStates[person.state]);

		} else {
			/*printf("[Controller] -  Transitioned from %s to %s\n",
			 debugStates[prev_state], debugStates[person.state]);*/
			prev_state = LS_STATE;
			direction_entered = LS_STATE;
		}
	}

	void guard_left_unlock() {

		if (direction_entered == -1) {
			person.state = START_STATE;
			printf("[Controller] - Invalid command, State Returned To: %s\n",
					debugStates[person.state]);
			return;
		}

		if (direction_entered == LS_STATE) {

			if (prev_state != LS_STATE) {
				person.state = prev_state;
				printf(
						"[Controller] - Invalid command, State Returned To: %s\n",
						debugStates[person.state]);
			} else {
				/*printf("[Controller] -  Transitioned from %s to %s\n",
				 debugStates[prev_state], debugStates[person.state]);*/
				prev_state = GLU_STATE;
			}
		} else if (direction_entered == RS_STATE) {
			if (prev_state != GRL_STATE) {
				person.state = prev_state;
				printf(
						"[Controller] - Invalid command, State Returned To: %s\n",
						debugStates[person.state]);
			} else {
				/*printf("[Controller] -  Transitioned from %s to %s\n",
				 debugStates[prev_state], debugStates[person.state]);*/
				prev_state = GLU_STATE;
			}
		}
	}

	void guard_left_lock() {

		if (direction_entered == -1) {
			person.state = START_STATE;
			printf("[Controller] - Invalid command, State Returned To: %s\n",
					debugStates[person.state]);
			return;
		}

		if (direction_entered == LS_STATE) {
			if (prev_state != LC_STATE) {
				person.state = prev_state;
				printf(
						"[Controller] - Invalid command, State Returned To: %s\n",
						debugStates[person.state]);
			} else {
				/*printf("[Controller] -  Transitioned from %s to %s\n",
				 debugStates[prev_state], debugStates[person.state]);*/
				prev_state = GLL_STATE;
			}
		} else if (direction_entered == RS_STATE) {
			if (prev_state != LC_STATE) {
				person.state = prev_state;
				printf(
						"[Controller] - Invalid command, State Returned To: %s\n",
						debugStates[person.state]);
			} else {
				printf("[Controller] -  Person has exited the building");
				person.state = START_STATE;
				prev_state = START_STATE;
			}
		}

	}

	void left_open() {

		if (direction_entered == -1) {
			person.state = START_STATE;
			printf("[Controller] - Invalid command, State Returned To: %s\n",
					debugStates[person.state]);
			return;
		}

		if (prev_state != GLU_STATE) {
			person.state = prev_state;
			printf("[Controller] - Invalid command, State Returned To: %s\n",
					debugStates[person.state]);
		} else {
			/*printf("[Controller] -  Transitioned from %s to %s\n",
			 debugStates[prev_state], debugStates[person.state]);*/
			prev_state = LO_STATE;

		}
	}

	void left_close() {

		if (direction_entered == -1) {
			person.state = START_STATE;
			printf("[Controller] - Invalid command, State Returned To: %s\n",
					debugStates[person.state]);
			return;
		}

		if (direction_entered == LS_STATE) {

			if (prev_state != WS_STATE) {
				person.state = prev_state;
				printf(
						"[Controller] - Invalid command, State Returned To: %s\n",
						debugStates[person.state]);
			} else {
				/*printf("[Controller] -  Transitioned from %s to %s\n",
				 debugStates[prev_state], debugStates[person.state]);
				 */
				prev_state = LC_STATE;
			}
		} else if (direction_entered == RS_STATE) {
			if (prev_state != LO_STATE) {
				person.state = prev_state;
				printf(
						"[Controller] - Invalid command, State Returned To: %s\n",
						debugStates[person.state]);
			} else {
				/*printf("[Controller] -  Transitioned from %s to %s\n",
				 debugStates[prev_state], debugStates[person.state]);*/
				prev_state = LC_STATE;
			}
		} else {
			person.state = prev_state;
			printf("[Controller] - Invalid command, State Returned To: %s\n",
					debugStates[person.state]);
		}
	}

	void weight_scan() {

		if (direction_entered == -1) {
			person.state = START_STATE;
			printf("[Controller] - Invalid command, State Returned To: %s\n",
					debugStates[person.state]);
			return;
		}

		if (direction_entered == LS_STATE) {
			if (prev_state != LO_STATE) {

				person.state = prev_state;
				printf(
						"[Controller] - Invalid command, State Returned To: %s\n",
						debugStates[person.state]);

			} else {
				/*printf("[Controller] -  Transitioned from %s to %s\n",
				 debugStates[prev_state], debugStates[person.state]);*/
				prev_state = WS_STATE;
			}
		} else if (direction_entered == RS_STATE) {
			if (prev_state != RO_STATE) {

				person.state = prev_state;
				printf(
						"[Controller] - Invalid command, State Returned To: %s\n",
						debugStates[person.state]);

			} else {
				/*printf("[Controller] -  Transitioned from %s to %s\n",
				 debugStates[prev_state], debugStates[person.state]);*/
				prev_state = WS_STATE;
			}
		}
	}

	void right_scan() {

		if (prev_state != START_STATE) {
			person.state = prev_state;
			printf("[Controller] - Invalid command, State Returned To: %s\n",
					debugStates[person.state]);
		} else {
			/*printf("[Controller] -  Transitioned from %s to %s\n",
			 debugStates[prev_state], debugStates[person.state]);*/
			prev_state = RS_STATE;
			direction_entered = RS_STATE;
		}
	}

	void guard_right_unlock() {

		if (direction_entered == -1) {
			person.state = START_STATE;
			printf("[Controller] - Invalid command, State Returned To: %s\n",
					debugStates[person.state]);
			return;
		}

		if (direction_entered == LS_STATE) {
			if (prev_state != GLL_STATE) {
				person.state = prev_state;
				printf(
						"[Controller] - Invalid command, State Returned To: %s\n",
						debugStates[person.state]);
			} else {
				/*printf("[Controller] -  Transitioned from %s to %s\n",
				 debugStates[prev_state], debugStates[person.state]);*/
				prev_state = GRU_STATE;

			}
		} else if (direction_entered == RS_STATE) {
			if (prev_state != RS_STATE) {
				person.state = prev_state;
				printf(
						"[Controller] - Invalid command, State Returned To: %s\n",
						debugStates[person.state]);
			} else {
				/*printf("[Controller] -  Transitioned from %s to %s\n",
				 debugStates[prev_state], debugStates[person.state]);*/
				prev_state = GRU_STATE;
			}
		}

	}

	void right_open() {

		if (direction_entered == -1) {
			person.state = START_STATE;
			printf("[Controller] - Invalid command, State Returned To: %s\n",
					debugStates[person.state]);
			return;
		}

		if (prev_state != GRU_STATE) {
			person.state = prev_state;
			printf("[Controller] - Invalid command, State Returned To: %s\n",
					debugStates[person.state]);
		} else {
			/*printf("[Controller] -  Transitioned from %s to %s\n",
			 debugStates[prev_state], debugStates[person.state]);*/
			prev_state = RO_STATE;
		}

	}

	void right_close() {

		if (direction_entered == -1) {
			person.state = START_STATE;
			printf("[Controller] - Invalid command, State Returned To: %s\n",
					debugStates[person.state]);
			return;
		}

		if (direction_entered == RS_STATE) {

			if (prev_state != WS_STATE) {
				person.state = prev_state;
				printf(
						"[Controller] - Invalid command, State Returned To: %s\n",
						debugStates[person.state]);
			} else {
				/*printf("[Controller] -  Transitioned from %s to %s\n",
				 debugStates[prev_state], debugStates[person.state]);*/
				prev_state = RC_STATE;
			}
		} else if (direction_entered == LS_STATE) {
			if (prev_state != RO_STATE) {
				person.state = prev_state;
				printf(
						"[Controller] - Invalid command, State Returned To: %s\n",
						debugStates[person.state]);
			} else {
				/*printf("[Controller] -  Transitioned from %s to %s\n",
				 debugStates[prev_state], debugStates[person.state]);*/
				prev_state = RC_STATE;
			}
		} else {
			person.state = prev_state;
			printf("[Controller] - Invalid command, State Returned To: %s\n",
					debugStates[person.state]);
		}
	}

	void guard_right_lock() {

		if (direction_entered == -1) {
			person.state = START_STATE;
			printf("[Controller] - Invalid command, State Returned To: %s\n",
					debugStates[person.state]);
			return;
		}
		if (direction_entered == RS_STATE) {

			if (prev_state != RC_STATE) {
				person.state = prev_state;
				printf(
						"[Controller] - Invalid command, State Returned To: %s\n",
						debugStates[person.state]);
			} else {
				/*printf("[Controller] -  Transitioned from %s to %s\n",
				 debugStates[prev_state], debugStates[person.state]);*/
				prev_state = GRL_STATE;
			}
		} else if (direction_entered == LS_STATE) {
			if (prev_state != RC_STATE) {
				person.state = prev_state;
				printf(
						"[Controller] - Invalid command, State Returned To: %s\n",
						debugStates[person.state]);
			} else {
				prev_state = START_STATE;
				/*printf("[Controller] -  Transitioned from %s to %s\n",
				 debugStates[prev_state], debugStates[person.state]);*/

			}
		}

	}

	void exit_state() {

		//printf(	"Exiting... All person data and states will be erase. Thanks for using this program:)\n");

	}
