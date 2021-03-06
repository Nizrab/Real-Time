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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <unistd.h>
#include "../../des_controller/src/des.h"
void getUsrPrompt(char input[4], Person* person);
void getUsrID(Person* person);
void getUsrWeight(Person* person);
int main(int argc, char* argv[]) {


    Person person;
    Display display;
    int errno;

    int coid; //Connection Id

    pid_t controller_pid = atoi(argv[1]);


    // Phase 1
    if (argc != 2) {
        printf("ERROR WITH ARG\n");
        exit(EXIT_FAILURE);
    }
    person.state = START_STATE; //Default State
    coid = ConnectAttach(ND_LOCAL_NODE, controller_pid, 1, _NTO_SIDE_CHANNEL, 0);
    if(coid == -1){
        printf("ERROR WITH CONNECTION\n");
        exit(EXIT_FAILURE);
    }

    printf("Enter a command [ls= left scan, rs= right scan, ws= weight scale, lo =left open, ro=right open, lc = left closed, rc = right closed , gru = guard right unlock, grl = guard right lock, gll=guard left lock, glu = guard left unlock, exit]\n");
    // Phase 2
    while(1) {

        char input[4];

        scanf("%s",&input);

        //Validate user input
        getUsrPrompt(input, &person);


        // Check if Msg.send Went through
        if(MsgSend(coid, &person, sizeof(person), &display, sizeof(display)) == -1){

			 if(person.state  == EXIT_STATE){
				printf("[Inputs] - Exiting...\n");
				break;
			}
            printf("ERROR IN MSG SEND\n");
            exit(EXIT_FAILURE);
        }

        //Checks if Exit State
        if(person.state  == EXIT_STATE){
        	//printf("[Inputs] - Exiting...\n");
            break;
        }
    }

    sleep(2);
    ConnectDetach(coid);
    return EXIT_SUCCESS;
}
void getUsrPrompt(char input[4], Person* person) {

	//printf("[Inputs] - State: %s\n", input);

	//Check if the user input is a valid command
    if(strcmp(input,inMessage[LS_INPUT]) == 0){

            person->state = LS_STATE;
            getUsrID(person);
    }
    else if(strncmp(input,inMessage[RS_INPUT],strlen(inMessage[RS_INPUT])) == 0){
        person->state = RS_STATE;
        getUsrID(person);
    }
    else if(strncmp(input,inMessage[WS_INPUT],strlen(inMessage[WS_INPUT])) == 0){
        person->state = WS_STATE;
        getUsrWeight(person);
    }
    else if(strncmp(input,inMessage[LO_INPUT],strlen(inMessage[LO_INPUT])) == 0){
        person->state = LO_STATE;
    }
    else if(strncmp(input,inMessage[RO_INPUT],strlen(inMessage[RO_INPUT])) == 0){
        person->state = RO_STATE;
    }
    else if(strncmp(input,inMessage[LC_INPUT],strlen(inMessage[LC_INPUT])) == 0){
        person->state = LC_STATE;
    }
    else if(strncmp(input,inMessage[RC_INPUT],strlen(inMessage[RC_INPUT])) == 0){
        person->state = RC_STATE;
    }
    else if(strncmp(input,inMessage[GLU_INPUT],strlen(inMessage[GLU_INPUT])) == 0){
        person->state = GLU_STATE;
    }
    else if(strncmp(input,inMessage[GRU_INPUT],strlen(inMessage[GRU_INPUT])) == 0){
        person->state = GRU_STATE;
    }
    else if(strncmp(input,inMessage[GLL_INPUT],strlen(inMessage[GLL_INPUT])) == 0){
        person->state = GLL_STATE;
    }
    else if(strncmp(input,inMessage[GRL_INPUT],strlen(inMessage[GRL_INPUT])) == 0){
        person->state = GRL_STATE;
    }
    else if(strncmp(input,inMessage[EXIT_INPUT],strlen(inMessage[EXIT_INPUT])) == 0){
        person->state = EXIT_STATE;
    }

}

//Prompts for Users ID
void getUsrID(Person* person){
    printf("Enter ID: \n");
    scanf("%d",&person->id);
}

//Prompts for Users Weight
void getUsrWeight(Person* person){
    printf("Enter Weight: \n");
    scanf("%d",&person->weight);
}
