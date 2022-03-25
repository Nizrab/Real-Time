#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#define MY_PULSE_CODE _PULSE_CODE_MINAVAIL

typedef union {
	struct _pulse pulse;
	char msg[255];
} my_message_t;

int main(void) {
	name_attach_t *attach;
	FILE *fp;
	char status[10];
	my_message_t msg;
	int rcvid;
	attach = name_attach(NULL, "mydevice", 0);
	fp = fopen("/dev/local/mydevice", "r");
	if (attach == NULL) {
		exit(EXIT_FAILURE);
	}
	if (fp == NULL) {
		printf("Error opening file\n");
		exit(EXIT_FAILURE);
	}
	fscanf(fp, "%s %s", status, msg.msg);
	if (strcmp(status, "status") == 0) {
		printf("Status: %s\n", msg.msg);
		fclose(fp);
	}
	if (strcmp(msg.msg, "closed") == 0) {
		name_detach(attach, 0);
		fclose(fp);
		exit(EXIT_SUCCESS);
	}
	while (1) {
		if ((rcvid = MsgReceivePulse(attach->chid, &msg.msg, sizeof(msg.msg),
				NULL)) == -1) {
			printf("Error receiving pulse\n");
			exit(EXIT_FAILURE);
		}

		if (rcvid == 0 && msg.pulse.code == MY_PULSE_CODE) {
			printf("Small Integer: %d\n", msg.pulse.value.sival_int);

			if ((fp = fopen("/dev/local/mydevice", "r")) == NULL) {
				printf("Error opening file\n");
				exit(EXIT_FAILURE);
			}

			fscanf(fp, "%s %s", status, msg.msg);

			if (strcmp(status, "status") == 0) {
				printf("Status: %s\n", msg.msg);
			}

			if (strcmp(msg.msg, "closed") == 0) {
				name_detach(attach, 0);
				fclose(fp);
				break;
			}
		} else {
			printf("Error receiving pulse\n");
		}
	}
	return EXIT_FAILURE;
}
