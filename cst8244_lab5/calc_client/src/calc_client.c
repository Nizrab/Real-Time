#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include "../../calc_server/src/calc_message.h"

int main(int argc, char* argv[]) {
	//client_send_t msg_send;
	server_response_t msg_receive;
	int errno;
	int coid;

	if (argc != 5) {
		printf(
				"Usage: ./calc_client <Calc-Server-PID left_oerand operator right_operand");
		return EXIT_FAILURE;
	}

	pid_t serverpid = atoi(argv[1]);
	int chid = ChannelCreate(0);
	coid = ConnectAttach(ND_LOCAL_NODE, serverpid, chid, _NTO_SIDE_CHANNEL, 0);
	msg_send.left_hand = atoi(argv[2]);
	msg_send.operator = argv[3][0];
	msg_send.right_hand = atoi(argv[4]);

	MsgSend(coid, &msg_send, sizeof(msg_send), &msg_receive,
			sizeof(msg_receive));
	if (-1L
			== MsgSend(coid, &msg_send, sizeof(msg_send), &msg_receive,
					sizeof(msg_receive))) {
		printf("MsgSend had an error.\n");
		exit(EXIT_FAILURE);
	}
	if (msg_receive.statusCode == SRVR_OK) {
		printf("The server has calculated the result of %d %c %d as %.2f\n",
				msg_send.left_hand, msg_send.operator, msg_send.right_hand,
				msg_receive.answer);
	} else if (msg_receive.statusCode == SRVR_INVALID_OPERATOR) {
		printf("Error message from server: %s %c\n", msg_receive.errorMsg,
				msg_send.operator);
	} else if (msg_receive.statusCode == SRVR_OVERFLOW) {
		printf("Error message from server: %s %d %c %d\n", msg_receive.errorMsg,
				msg_send.left_hand, msg_send.operator, msg_send.right_hand);
	} else if (msg_receive.statusCode == SRVR_UNDEFINED) {
		printf("Error message from server: %s\n", msg_receive.errorMsg);
	} else {
		printf("MsgSend had an error.\n");
	}

	ConnectDetach(coid);
	return EXIT_SUCCESS;
}
