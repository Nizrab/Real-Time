/**************************************************
 * File: metronome.h
 * Author:
 * Barzin Farahani (040922695)
 *
 * Description:
 * This is the H file for the metronome program
 *
 **************************************************/
#ifndef SRC_METRONOME_H_
#define SRC_METRONOME_H_
struct ioattr_t;
#define IOFUNC_ATTR_T struct ioattr_t
struct metro_ocb;
#define IOFUNC_OCB_T struct metro_ocb
//INCLUDES FOR FILE
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <math.h>
#include <sys/types.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
//PULSE CODES
#define METRONOME_PULSE_CODE _PULSE_CODE_MINAVAIL     //0
#define PAUSE_PULSE_CODE (_PULSE_CODE_MINAVAIL +1) 	  //1
#define START_PULSE_CODE (_PULSE_CODE_MINAVAIL +2)    //2
#define STOP_PULSE_CODE  (_PULSE_CODE_MINAVAIL +3)    //3
#define QUIT_PULSE_CODE  (_PULSE_CODE_MINAVAIL +4)    //4
#define SET_PULSE_CODE   (_PULSE_CODE_MINAVAIL +5)    //5
//CONNECTION STRING
#define METRO_CONNECT  "metronome"
//TIMER STATUS
#define START 0
#define STOPPED 1
#define PAUSED 2
//PULSE AND ERROR CODE
#define PULSE 0
#define ERROR -1
//UNION STRUCT
typedef union {
	struct _pulse pulse;
	char msg[255];
} my_message_t;
//ROWS FOR DATA TABLE
struct DataTableRow {
	int tst;
	int tsb;
	int intervals;
	char pattern[16];
};
//METRONOME TABLE
struct DataTableRow t[] = { { 2, 4, 4, "|1&2&" }, { 3, 4, 6, "|1&2&3&" }, { 4,
		4, 8, "|1&2&3&4&" }, { 5, 4, 10, "|1&2&3&4-5-" },
		{ 3, 8, 6, "|1-2-3-" }, { 6, 8, 6, "|1&a2&a" },
		{ 9, 8, 9, "|1&a2&a3&a" }, { 12, 8, 12, "|1&a2&a3&a4&a" } };
//TIMER PROPERTIES STRUCT
struct Timer_Properties {
	double bps;
	double measure;
	double interval;
	double nano_sec;

}typedef timer_props_t;
//METRONOME PROPERTIES STRUCT
struct Metronome_Properties {
	int bpm;
	int tst;
	int tsb;
}typedef metro_props_t;
//METRONOME STRUCT
struct Metronome {
	metro_props_t m_props;
	timer_props_t t_props;
}typedef Metronome_t;

#define DEVICES 1
#define METRONOME 0

char *devnames[DEVICES] = { "/dev/local/metronome" };
//OVERIDE FUNCTION
typedef struct ioattr_t {
	iofunc_attr_t attr;
	int device;
} ioattr_t;
typedef struct metro_ocb {
	iofunc_ocb_t ocb;
	char buffer[50];
} metro_ocb_t;
//FUNCTION DECLERATIONS
int io_read(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb);
int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);
int io_open(resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle,
		void *extra);
void *metronome_thread(void*metronome);
void set_timer(Metronome_t * Metronome);
void stop_timer(struct itimerspec * itime, timer_t timer_id);
void start_timer(struct itimerspec * itime, timer_t timer_id,
		Metronome_t* Metronome);
int search_idx_table(Metronome_t * Metronome);
metro_ocb_t * ocb_calloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *mtattr);
void ocb_free(IOFUNC_OCB_T *mocb);

#endif /* METRONOME_H_ */
