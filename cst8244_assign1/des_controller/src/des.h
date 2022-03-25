/*
 * des.h
 *
 *  Created on: Nov 9, 2021
 *      Author: Barzin Farahani
 */
#ifndef DOOR_ENTRY_SYSTEM_H_
#define DOOR_ENTRY_SYSTEM_H_

#define NUM_STATES 13

typedef void *(*FState)();

//Person struct
typedef struct {
	int id;
	int weight;
	int state;
} Person;

//Display struct
typedef struct {
	Person person;
	int statusCode;
	char errMsg[128];
} Display;

const char *debugStates[NUM_STATES] = { "start", "ls",  //LEFT SCAN INPUT
		"rs",  //RIGHT SCAN INPUT
		"ws",  //WEIGHT STATE INPUT
		"lo",  //LEFT OPEN INPUT
		"ro",  //RIGHT OPEN INPUT
		"lc",  //LEFT SCAN INPUT
		"rc",  //RIGHT SCAN INPUT
		"glu", //GAURD LEFT UNLOCK INPUT
		"gru", //GAURD RIGHT UNLOCK INPUT
		"gll", //GAURD LEFT LOCK INPUT
		"grl", //GAURD RIGHT LOCK INPUT
		"exit" //EXIT INPUT
		};

//STATES

typedef enum {
	START_STATE = 0, //START STATE
	LS_STATE = 1, 	//LEFT SCAN
	RS_STATE = 2, 	//RIGHT SCAN
	WS_STATE = 3, 	//WEIGHT STATE
	LO_STATE = 4, 	//LEFT OPEN
	RO_STATE = 5, 	//RIGHT OPEN
	LC_STATE = 6, 	//LEFT CLOSE
	RC_STATE = 7, 	//RIGHT CLOSE
	GLU_STATE = 8,	//GARD LEFT UNLOCK
	GRU_STATE = 9,	//GAURD RIGHT UNLOCK
	GLL_STATE = 10,	//GAURD LEFT LOCK
	GRL_STATE = 11,	//GARD RIGHT LOCK
	EXIT_STATE = 12	//EXIT STATE
} State;

//INPUTS
#define NUM_INPUTS 12
typedef enum {
	LS_INPUT = 0, 	//LEFT SCAN
	RS_INPUT = 1, 	//RIGHT SCAN
	WS_INPUT = 2, 	//WEIGHT STATE
	LO_INPUT = 3, 	//LEFT OPEN
	RO_INPUT = 4, 	//RIGHT OPEN
	LC_INPUT = 5, 	//LEFT CLOSE
	RC_INPUT = 6, 	//RIGHT CLOSE
	GLU_INPUT = 7,	//GARD LEFT UNLOCK
	GRU_INPUT = 8,	//GAURD RIGHT UNLOCK
	GLL_INPUT = 9,	//GAURD LEFT LOCK
	GRL_INPUT = 10,	//GARD RIGHT LOCK
	EXIT_INPUT = 11	//EXIT STATE
} Input;

//INPUT COMMANDS
const char *inMessage[NUM_INPUTS] = { "ls",  //LEFT SCAN INPUT
		"rs",  //RIGHT SCAN INPUT
		"ws",  //WEIGHT STATE INPUT
		"lo",  //LEFT OPEN INPUT
		"ro",  //RIGHT OPEN INPUT
		"lc",  //LEFT SCAN INPUT
		"rc",  //RIGHT SCAN INPUT
		"glu", //GAURD LEFT UNLOCK INPUT
		"gru", //GAURD RIGHT UNLOCK INPUT
		"gll", //GAURD LEFT LOCK INPUT
		"grl", //GAURD RIGHT LOCK INPUT
		"exit" //EXIT INPUT
		};

//OUTPUTS
#define NUM_OUTPUTS 13
typedef enum {
	START_OUTPUT = 0, //START OUTPUT
	LS_OUTPUT = 1,   //LEFT SCAN OUTPUT
	RS_OUTPUT = 2,   //RIGHT SCAN OUTPUT
	WS_OUTPUT = 3,   //WEIGHT SCALE OUTPUT
	LO_OUTPUT = 4,	 //LEFT OPEN OUTPUT
	RO_OUTPUT = 5,	 //RIGHT OPEN OUTPUT
	LC_OUTPUT = 6,   //LEFT CLOSED OUTPUT
	RC_OUTPUT = 7,   //RIGHT CLOSED OUTPUT
	GRL_OUTPUT = 8,  //GAURD RIGHT LOCK OUTPUT
	GRU_OUTPUT = 9,  //GAURD RIGHT UNLOCK OUTPUT
	GLL_OUTPUT = 10, //GAURD LEFT LOCK OUTPUT
	GLU_OUTPUT = 11, //GAURD LEFT UNLOCK OUTPUT
	EXIT_OUTPUT = 12 //EXIT OUTPUT
} Output;

//OUTPUT MSGS
const char *outMessage[NUM_OUTPUTS] =
		{
				"Enter the device (ls= left scan, rs= right scan, ws= weight scale, lo =left open, ro=right open, lc = left closed, rc = right closed , gru = guard right unlock, grl = guard right lock, gll=guard left lock, glu = guard left unlock)", //START
				"Person scanned ID, ID = ", //LEFT SCAN OUTPUT
				"Person scanned ID, ID = ", //RIGHT SCAN OUTPUT
				"Person weight, Weight = ", //WEIGHT OUTPUT
				"Left door open.", //LEFT OPEN OUTPUT
				"Right door open.", //RIGHT OPEN OUTPUT
				"Left door closed.", //LEFT CLOSE OUTPUT
				"Right door closed.", //RIGHT CLOSED OUTPUT
				"Right door locked by Guard.", //GAURD RIGHT LOCK OUTPUT
				"Right door unlocked by Guard.", //GAURD RIGHT UNLOCKED OUTPUT
				"left door locked by Guard.", //GAURD LEFT LOCKED OUTPUT
				"left door unlocked by Guard.", //GAURD LEFT UNLOCKED OUTPUT
				"Exiting Door System." //EXIT OUTPUT
		};

#endif /* DOOR_ENTRY_SYSTEM_H_ */
