#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include "calc_message.h"

int main() {
	int chid = ChannelCreate(0);
	int rcvid;
	client_send_t client_message;
	server_response_t response;
	printf("Server PID is %d\tAnd the Channel ID (chid) is: %d\n", getpid(),
			chid);
	while (1) {
		rcvid = MsgReceive(chid, &client_message, sizeof(client_message), NULL);
		response.statusCode = SRVR_OK;
		switch (client_message.operator) {
		case '+':
			response.answer = client_message.left_hand
					+ client_message.right_hand;
			if (client_message.left_hand > 0 && client_message.right_hand > 0
					&& response.answer < 0) {
				strcpy(response.errorMsg, "OVERFLOW:");
				response.statusCode = SRVR_OVERFLOW;
			}
			if (client_message.left_hand < 0 && client_message.right_hand < 0
					&& response.answer > 0) {
				strcpy(response.errorMsg, "OVERFLOW:");
				response.statusCode = SRVR_OVERFLOW;
			}
			break;
		case '-':
			response.answer = client_message.left_hand
					- client_message.right_hand;
			break;
		case 'x':
			response.answer = client_message.left_hand
					* client_message.right_hand;
			break;
		case '/':
			if (0 == client_message.right_hand) {
				strcpy(response.errorMsg, "UNDEFINED: 2 / 0");
				response.statusCode = SRVR_UNDEFINED;
			} else {
				response.answer = ((double) client_message.left_hand
						/ (double) client_message.right_hand);
			}
			break;
		default:
			strcpy(response.errorMsg, "INVALID OPERATOR:");
			response.statusCode = SRVR_INVALID_OPERATOR;
			break;
		}
		MsgReply(rcvid, EOK, &response, sizeof(response));
	}
	ChannelDestroyed(chid);
	return EXIT_SUCCESS;
}
