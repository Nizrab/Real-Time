/**************************************************
 * File: des_display.c
 * Authors:
 * Barzin Farahani (040922695)
 * Frederic Desjardins (040941359)
 *
 * Description:
 * This file is the display file for the program which communicates with the controller file
 * and is used to obtain the users inputs and data from there. With the given data it will figure out what state
 * the user is in and based off that will output a message.
 *
 **************************************************/
//Inclues for the file
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <unistd.h>
#include <sys/dispatch.h>
#include "../../des_controller/src/des.h"
//Function declaration
void displayState(Person* person);
/* Function: Main
 * Parameters: Int, Char*
 * Description:
 * This is the main function for the display class this class has a infinite loop which will run
 * the function displayState which will display the output message based on what state the user is in which is passed in via the controller.
 * this function will also receive the date from des_inputs and display who has entered the building based off id
 */
int main(void) {
	//Member Variables
	Person person;
	Display display;
	int rcvid;
	name_attach_t *attach;
	if ((attach = name_attach(NULL, "display_point", 0)) == NULL) {
		printf("Error in Attatch\n");
		exit(EXIT_FAILURE);
	}
	//Displays the pid
	printf("Display PID = %d\n", getpid());

	while (1) {
		//receives the message from the client
		rcvid = MsgReceive(attach->chid, &person, sizeof(person), NULL);
		//Checks if the message received fails
		if (rcvid == -1) {
			printf("MSG RECIVED FAILED");
			exit(EXIT_FAILURE);
		}
		//Sends message to display state
		displayState(&person);
		//Checks if there is a error replying to the controller
		if (MsgReply(rcvid, EOK, &display, sizeof(display)) == -1) {
			printf("[Display] - Error replying to Controller");
		}
		//If exit state exit
		if (person.state == EXIT_STATE) {
			break;
		}
	}
	//Disconnects from the channel
	name_detach(attach, 0);
	return EXIT_SUCCESS;
}
/**********************************
 * Function: display_current_state
 * Parameters: Person
 * Description:
 * This function is used to display the output for the user based on what there current state is.
 * This function gets its output messages  from the des.h file
 *********************************/
void displayState(Person* person) {
	switch (person->state) {
	case LS_STATE:
		printf("%s%d\n", outMessage[LS_OUTPUT], person->id);
		break;
	case RS_STATE:
		printf("%s%d\n", outMessage[RS_OUTPUT], person->id);
		break;
	case WS_STATE:
		printf("%s%d\n", outMessage[WS_OUTPUT], person->weight);
		break;
	case LO_STATE:
		printf("%s\n", outMessage[LO_OUTPUT]);
		break;
	case RO_STATE:
		printf("%s\n", outMessage[RO_OUTPUT]);
		break;
	case LC_STATE:
		printf("%s\n", outMessage[LC_OUTPUT]);
		break;
	case RC_STATE:
		printf("%s\n", outMessage[RC_OUTPUT]);
		break;
	case GLU_STATE:
		printf("%s\n", outMessage[GLU_OUTPUT]);
		break;
	case GRU_STATE:
		printf("%s\n", outMessage[GRU_OUTPUT]);
		break;
	case GLL_STATE:
		printf("%s\n", outMessage[GLL_OUTPUT]);
		break;
	case GRL_STATE:
		printf("%s\n", outMessage[GRL_OUTPUT]);
		break;
	case EXIT_STATE:
		printf("%s\n", outMessage[EXIT_OUTPUT]);
		break;
	default:
		// printf("Invalid Input\n");
		break;
	}
}
