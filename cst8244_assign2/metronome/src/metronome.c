/**************************************************
 * File: metronome.c
 * Authors:
 * Barzin Farahani (040922695)
 *
 * Description:
 * This program is used to implement a metrenome by using a device driven resource manager and by
 * taking in the userinputs via the command line. In this program the user is also able to
 * "stop","start", and "pause" the program all through the commandline and via the metronomes resource manager.
 *
 **************************************************/
//Includes for file
#include "metronome.h"
//Global Variables
name_attach_t * attach;
Metronome_t Metronome;
int server_coid;
char data[255];
int main(int argc, char *argv[]) {
	int id;
	//Thread ID
	int server_coid;
	dispatch_t * dpp;
	//IO functions Read/Write
	resmgr_io_funcs_t io_funcs;
	//Connection functions Open
	resmgr_connect_funcs_t connect_funcs;
	ioattr_t ioattr[DEVICES];
	//Thread Atttibutes
	pthread_attr_t thread_attrib;
	dispatch_context_t * ctp;
	//Checks for correct amount of arguments
	if (argc != 4) {
		printf("Error incorrect amount of arguments\n");
		printf(
				"Command: ./metronome <beats/minute> <time-signature-top> <time-signature-bottom>\n");
		exit(EXIT_FAILURE);
	}
	iofunc_funcs_t ocb_funcs = {
	_IOFUNC_NFUNCS, ocb_calloc, ocb_free };
	//Mounts fucntions  to OCB
	iofunc_mount_t mount = { 0, 0, 0, 0, &ocb_funcs };
	//get/process command line agrguments
	Metronome.m_props.bpm = atoi(argv[1]);
	Metronome.m_props.tst = atoi(argv[2]);
	Metronome.m_props.tsb = atoi(argv[3]);
	//Attach dispach handler
	dpp = dispatch_create();
	if (dpp == NULL) {
		fprintf(stderr, "%s:  Error in dispatch_create()\n", argv[0]);
		return (EXIT_FAILURE);
	}
	//Fucntion Tables
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS,
			&io_funcs);
	connect_funcs.open = io_open;
	io_funcs.read = io_read;
	io_funcs.write = io_write;
	for (int i = 0; i < DEVICES; i++) {
		iofunc_attr_init(&ioattr[i].attr, S_IFCHR | 0666, NULL, NULL);
		ioattr[i].device = i;
		ioattr[i].attr.mount = &mount;
		//Attatch path to pathname
		if ((id = resmgr_attach(dpp, NULL, devnames[i], _FTYPE_ANY, 0,
				&connect_funcs, &io_funcs, &ioattr[i])) == -1) {
			fprintf(stderr, "%s:  Unable to attach name.\n", argv[0]);
			return (EXIT_FAILURE);
		}
	}
	//return to dispatch context
	ctp = dispatch_context_alloc(dpp);
	//Intialize thread attributes
	pthread_attr_init(&thread_attrib);
	pthread_create(NULL, &thread_attrib, &metronome_thread, &Metronome);
	while (1) {
		if ((ctp = dispatch_block(ctp))) {
			dispatch_handler(ctp);
		} else
			printf("ERROR in while\n");
	}
	//Destroy thread Atributes
	pthread_attr_destroy(&thread_attrib);
	//Detatch from namespace
	name_detach(attach, 0);
	//Close server connection
	name_close(server_coid);
	//Exit
	return EXIT_SUCCESS;

}
/**
 *  Function: io_read
 *  Description:
 *  This I/O function is used to Read for
 *  the metronome
 */
int io_read(resmgr_context_t *ctp, io_read_t *msg, metro_ocb_t *mocb) {
	//Member variables
	int index;
	int nb;
	nb = strlen(data);
	if (data == NULL) {
		return 0;
	}
	//Searches the metrnome table
	index = search_idx_table(&Metronome);
	//the timer properties are calculated and set here
	sprintf(data,
			"[metronome: %d beats/min, time signature: %d/%d, sec-per-interval: %.2f, nanoSecs: %.0lf]\n",
			Metronome.m_props.bpm, t[index].tst, t[index].tsb,
			Metronome.t_props.interval, Metronome.t_props.nano_sec);
	nb = strlen(data);
	//checks to see if i have already sent the mesg
	if (mocb->ocb.offset == nb) {
		return 0;
	}
	nb = min(nb, msg->i.nbytes);
	//sets the number of bytes
	_IO_SET_READ_NBYTES(ctp, nb);
	//copys data to buffer
	SETIOV(ctp->iov, data, nb);
	mocb->ocb.offset += nb;
	if (nb > 0) {
		mocb->ocb.flags |= IOFUNC_ATTR_ATIME;
	}
	return (_RESMGR_NPARTS(1));
}
/**
 *  Function: io_write
 *  Description:
 *  This I/O function is used to write for
 *  the metronome
 */
int io_write(resmgr_context_t *ctp, io_write_t *msg, metro_ocb_t *mocb) {
	int nb = 0;
	if (msg->i.nbytes == ctp->info.msglen - (ctp->offset + sizeof(*msg))) {
		//Buffer
		char *buf;
		//Used for message pause
		char * pause_msg;
		//Used for set command
		char * set_msg;
		int i;
		//Value for pause
		int small_integer = 0;
		//checks for pause request
		buf = (char *) (msg + 1);
		if (strstr(buf, "pause") != NULL) {
			for (i = 0; i < 2; i++) {
				pause_msg = strsep(&buf, " ");
			}
			small_integer = atoi(pause_msg);
			if (small_integer >= 1 && small_integer <= 9) {
				MsgSendPulse(server_coid, SchedGet(0, 0, NULL),
						PAUSE_PULSE_CODE, small_integer);
			} else {
				printf("Integer is not between 1 and 9\n");
			}
		} else if (strstr(buf, "quit") != NULL) {
			MsgSendPulse(server_coid, SchedGet(0, 0, NULL), QUIT_PULSE_CODE,
					small_integer);
		} else if (strstr(buf, "start") != NULL) {
			MsgSendPulse(server_coid, SchedGet(0, 0, NULL), START_PULSE_CODE,
					small_integer);
		} else if (strstr(buf, "stop") != NULL) {
			MsgSendPulse(server_coid, SchedGet(0, 0, NULL), STOP_PULSE_CODE,
					small_integer);
		} else if (strstr(buf, "set") != NULL) {
			//loops trhough and splits the string
			for (i = 0; i < 4; i++) {
				set_msg = strsep(&buf, " ");
				//1st string input
				if (i == 1) {
					//sets it to bpm
					Metronome.m_props.bpm = atoi(set_msg);
					//2rd string input
				} else if (i == 2) {
					//sets it to tst
					Metronome.m_props.tst = atoi(set_msg);
					//3rd string input
				} else if (i == 3) {
					//sets it to tsb
					Metronome.m_props.tsb = atoi(set_msg);
				}
			}
			MsgSendPulse(server_coid, SchedGet(0, 0, NULL), SET_PULSE_CODE,
					small_integer);
			//When the user enters a invalid command line input
		} else {
			printf("Invalid Input\n");
			strcpy(data, buf);
		}
		nb = msg->i.nbytes;
	}
	_IO_SET_WRITE_NBYTES(ctp, nb);

	if (msg->i.nbytes > 0) {
		mocb->ocb.flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;
	}
	return (_RESMGR_NPARTS(0));
}
/**
 *  Function: io_open
 *  Description:
 *  This function is used to connect to the metronome
 *  device via the METRO_CONNECT string
 */
int io_open(resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle,
		void *extra) {
	server_coid = name_open(METRO_CONNECT, 0);
	//Checks if server_coid returns a error
	if (server_coid == -1) {
		perror("Open failed -> in I_Open Func");
		return EXIT_FAILURE;
	}
	return (iofunc_open_default(ctp, msg, &handle->attr, extra));
}
/**
 *  Function: metronome_thread
 *  Description:
 *  This function is used to in a sence drive the
 *  program this function will recive a pulse from the timer, create a thread, and recvice
 *  pulses from both the timer and the iowrite for "quit" and "pause"
 */
void *metronome_thread(void * ta) {
	//Method variables
	struct sigevent event;
	struct itimerspec itime;
	timer_t timer_id;
	my_message_t msg;
	int rcvid;
	int i = 0;
	char * tp;
	int timer_status;
	//Phase I
	attach = name_attach( NULL, METRO_CONNECT, 0);
	//If there is a error in attach i catch it
	if (attach == NULL) {
		printf("Error metronomeThread attatch\n");
		exit(EXIT_FAILURE);
	}
	//Pulse Timer
	event.sigev_notify = SIGEV_PULSE;
	//Attatch Connection
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, attach->chid,
			_NTO_SIDE_CHANNEL, 0);
	//Priority of events
	event.sigev_priority = SIGEV_PULSE_PRIO_INHERIT;
	//Sets pulse code to metronome pulse code
	event.sigev_code = METRONOME_PULSE_CODE;
	//Create Timer
	timer_create(CLOCK_REALTIME, &event, &timer_id);
	//Search in current table
	i = search_idx_table(&Metronome);
	//Set timer
	set_timer(&Metronome);
	//Start timer
	start_timer(&itime, timer_id, &Metronome);
	tp = t[i].pattern;
	/*Phase II
	 Check for pulse*/
	for (;;) {
		//If message recived returns a error break
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
		if (rcvid == ERROR) {
			printf("Error on MessageReceived in Metronome() \n");
			exit(EXIT_FAILURE);
		}
		//If is Pulse Received
		if (rcvid == PULSE) {
			switch (msg.pulse.code) {
			case METRONOME_PULSE_CODE:
				//Checks first Char
				if (*tp == '|') {
					printf("%.2s", tp);
					tp = (tp + 2);
					//Checks if end of the String
				} else if (*tp == '\0') {
					printf("\n");
					//Restart
					tp = t[i].pattern;
				} else {
					printf("%c", *tp++);
				}

				break;
				//Pause Pulse code
			case PAUSE_PULSE_CODE:
				if (timer_status == START) {
					itime.it_value.tv_sec = msg.pulse.value.sival_int;
					timer_settime(timer_id, 0, &itime, NULL);
				}
				break;
				//Quit Pulse
			case QUIT_PULSE_CODE:
				timer_delete(timer_id);
				name_detach(attach, 0);
				name_close(server_coid);
				exit(EXIT_SUCCESS);
				//Set Pulse
			case SET_PULSE_CODE:
				i = search_idx_table(&Metronome);
				tp = t[i].pattern;
				set_timer(&Metronome);
				start_timer(&itime, timer_id, &Metronome);
				printf("\n");
				break;
				//Start Pulse Code
			case START_PULSE_CODE:
				if (timer_status == STOPPED) {
					start_timer(&itime, timer_id, &Metronome);
					//Set the timer Status to start
					timer_status = START;
				}
				break;
				//Stop Pulse Code
			case STOP_PULSE_CODE:
				if (timer_status == START || timer_status == PAUSED) {
					stop_timer(&itime, timer_id);
					//Set the timer Status to stopped
					timer_status = STOPPED;
				}
				break;
			}
		}
		//Returns a standard output
		fflush(stdout);
	}
	return NULL;
}
/**
 *  Function: set_timer
 *  Description:
 *  This function is used to set the timer properties
 *  and do the needed math for each variable
 *
 */
void set_timer(Metronome_t * Metronome) {
	//Beats per minute which equal seconds per beeat
	Metronome->t_props.bps = (double) 60 / Metronome->m_props.bpm;
	//beat mesure
	Metronome->t_props.measure = Metronome->t_props.bps * 2;
	//seconds in intervals
	Metronome->t_props.interval = Metronome->t_props.measure
			/ Metronome->m_props.tsb;
	//nano seconds in intervals
	Metronome->t_props.nano_sec = (Metronome->t_props.interval
			- (int) Metronome->t_props.interval) * 1e+9;
}
/**
 *  Function: search_idx_table
 *  Description:
 *  This function is uesed to search through the
 *  table comparing it to what in giving by the users input
 *
 */
int search_idx_table(Metronome_t * Metronome) {
	int index = 0;
	//Searchs throught the table
	for (index = 0; index < 8; index++) {
		if (t[index].tsb == Metronome->m_props.tsb
				&& t[index].tst == Metronome->m_props.tst) {
			//when found returns the index
			return index;
		}
	}
	//if there is a error returns -1 error code
	return (-1);
}
/**
 *  Function: start_timer
 *  Description:
 *  This function is used to start
 *  the current timer
 */
void start_timer(struct itimerspec * itime, timer_t timer_id,
		Metronome_t* Metronome) {
	itime->it_value.tv_sec = 1;
	itime->it_value.tv_nsec = 0;
	itime->it_interval.tv_sec = Metronome->t_props.interval;
	itime->it_interval.tv_nsec = Metronome->t_props.nano_sec;
	timer_settime(timer_id, 0, itime, NULL);
}
/**
 *  Function: stop_timer
 *  Description:
 *  This function is used to stop
 *  the current timer
 */
void stop_timer(struct itimerspec * itime, timer_t timer_id) {
	//Sets the timers value to 0 to start
	itime->it_value.tv_sec = 0;
	//Then will set the new time
	timer_settime(timer_id, 0, itime, NULL);
}
/**
 * Implemented from the example program
 */
metro_ocb_t * ocb_calloc(resmgr_context_t *ctp, ioattr_t *mattr) {
	metro_ocb_t *mocb;
	mocb = calloc(1, sizeof(metro_ocb_t));
	mocb->ocb.offset = 0;
	return (mocb);
}
/**
 * Implemented from the example program
 */
void ocb_free(metro_ocb_t *mocb) {
	free(mocb);
}

