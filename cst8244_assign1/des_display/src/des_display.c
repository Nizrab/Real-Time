/**************************************************
 * File: des_inputs.c
 * Authors:
 * Barzin Farahani (040922695)
 * Frederic Desjardins (040941359)
 *
 * Description:
 * This file is the client function of the program,
 * and takes in the inputs that are given to the user it will also prompt
 * a user on what to enter based on what is inputed
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

void displayState(Person* person);

int main(void) {
    Person person;
    Display display;
    int rcvid;
    int chid;

    chid = ChannelCreate(0);

    if(chid == -1){
        printf("ERROR ON CHANNEL CREATE");
        exit(EXIT_FAILURE);
    }

    printf("Display PID = %d\n", getpid());
    //Recives the msg

    while(1){
    	rcvid = MsgReceive(chid,&person,sizeof(person),NULL);
        //Checks if the message recive fails
        if(rcvid  == -1){
            printf("MSG RECIVED FAILED");
            exit(EXIT_FAILURE);
        }

        displayState(&person);

        if(MsgReply(rcvid, EOK, &display, sizeof(display))==-1){
			printf("[Display] - Error replying to Controller");/* reply EOK back to controller */
		}

        if(person.state == EXIT_STATE){
        	printf("[Display] - Exiting...\n");
            break;
        }

    }
    ChannelDestroy(chid);
    return EXIT_SUCCESS;
}

void displayState(Person* person){
    switch(person->state){
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
        //printf("Invalid Input\n");
        break;
    }
}
