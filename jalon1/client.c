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

char echo_client(int sockfd) {
	/*

	Return value :
	0 : every thing is ok
	1 : connection closed by client
	2 : connection closed by server

	*/

	struct pollfd pollfds[2];
	pollfds[0].fd = STDIN_FILENO;
	pollfds[0].events = POLLIN;
	pollfds[0].revents = 0;
	pollfds[1].fd = sockfd;
	pollfds[1].events = POLLIN;
	pollfds[1].revents = 0;

	char buff_stdin[MSG_LEN];
	int buff_stdin_i;

	char buff_sockin[MSG_LEN];

	// Getting message from client
	printf("\nMessage:\n"); fflush(stdout);

	while (1) {

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

			// sending msg
			if (send(sockfd, buff_stdin, buff_stdin_i, 0) <= 0) {
				perror("send");
				exit(EXIT_FAILURE);
			}
			printf("Message sent!\n");

			// Verify if /quit
			if(strcmp(buff_stdin, "/quit") == 0){
				return 1;
			}

			// Getting message from client
			printf("\nMessage:\n"); fflush(stdout);

			pollfds[0].revents = 0;
		}

		if(pollfds[1].revents & POLLIN){
			// Cleaning memory
			memset(buff_sockin, 0, MSG_LEN);

			// Receiving message
			if (recv(sockfd, buff_sockin, MSG_LEN, 0) <= 0) {
				perror("recv");
				exit(EXIT_FAILURE);
			}

			if(strcmp(buff_sockin, "/quit") == 0){
				return 2;
			}

			printf("Received: %s\n", buff_sockin);

			pollfds[1].revents = 0;
		}

		if(pollfds[1].revents & POLLHUP){
			return 2;
		}
		
	}

	return 0;
}

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

	int sfd;
	sfd = handle_connect(argv[1], argv[2]);
	
	switch(echo_client(sfd)){
		case 1:
			printf("Connection closed succesfully\n");
			break;

		case 2:
			printf("Connection closed by the server\n");
			break;

		default :
			break;
	}
	
	close(sfd);
	return EXIT_SUCCESS;
}