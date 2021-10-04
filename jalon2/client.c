#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "common.h"
#include "msg_IO.h"
#include "req_reader.h"

int handle_connect(char host[], char port[]){
	struct addrinfo hints, *result, *rp;
	int sfd;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(host, port, &hints, &result) != 0) {
		perror("getaddrinfo()");
		exit(EXIT_FAILURE);
	}
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}
		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
			break;
		}
		close(sfd);
	}
	if (rp == NULL) {
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(result);
	return sfd;
}

int main(int argc, char *argv[]) {

	if(argc < 3){
		fprintf(stderr, "Usage : client host port\n");
		exit(EXIT_FAILURE);
	}

	int socket_fd = handle_connect(argv[1], argv[2]);

	struct pollfd pollfds[2];
	pollfds[0].fd = STDIN_FILENO;
	pollfds[0].events = POLLIN;
	pollfds[0].revents = 0;
	pollfds[1].fd = socket_fd;
	pollfds[1].events = POLLIN;
	pollfds[1].revents = 0;

	char buff_stdin[MSG_LEN];
	int buff_stdin_i;

	// Getting message from client
	printf("\nMessage:\n"); fflush(stdout);

	while(1){

		int n_active;
		if(-1 == (n_active = poll(pollfds, 2, POLL_TIMEOUT))){
			perror("Poll");
		}

		if(pollfds[0].revents & POLLIN){

			// buffer index
			memset(buff_stdin, 0, MSG_LEN);
			buff_stdin_i = 0;

			char c = getchar();
			while(c != '\n'){
				buff_stdin[buff_stdin_i++] = c;
				c = getchar();
			}

			buff_stdin[buff_stdin_i++] = '\0';

			// Understaing request
			struct message struct_msg;
			void *data;
			switch (req_reader(buff_stdin, &struct_msg, &data)){

				case ECHO_SEND:
					send_msg(socket_fd, &struct_msg, data);
					printf("Message sent : %s\n", (char *) data);
					free(data);
					break;
				
				case CLIENT_QUIT:

					send_msg(socket_fd, &struct_msg, data);

					close(socket_fd);
					printf("Deconnected\n");
					exit(EXIT_SUCCESS);
					break;

				default:
					break;
			}

			// Getting message from client
			printf("\nMessage:\n"); fflush(stdout);

			pollfds[0].revents = 0;
		}

		if(pollfds[1].revents & POLLIN){

			// Receiving message
			void *data = NULL;
			struct message msg_struct;

			switch(receive_msg(socket_fd, &msg_struct, &data)){
				
				case ECHO_SEND:
					printf("Message received : %s\n", (char *) data);
					break;

				case CLIENT_QUIT:
					close(socket_fd);
					printf("Server Deconnected\n");
					exit(EXIT_SUCCESS);
					break;

				default:
					break;
			}

			if(data != NULL){
				free(data);
			}

			pollfds[1].revents = 0;
		}

		if(pollfds[1].revents & POLLHUP){
			close(socket_fd);
			printf("Server Deconnected\n");
			exit(EXIT_SUCCESS);
		}
		
	}
	
	close(socket_fd);
	return EXIT_SUCCESS;
}