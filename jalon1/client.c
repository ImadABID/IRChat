#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common.h"

char echo_client(int sockfd) {
	/*

	Return value :
	0 : every thing is ok
	1 : connection closed by client
	2 : connection closed by server

	*/
	char buff[MSG_LEN];
	int n;
	while (1) {
		// Cleaning memory
		memset(buff, 0, MSG_LEN);
		// Getting message from client
		printf("Message: ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n') {} // trailing '\n' will be sent
		// Sending message (ECHO)
		if (send(sockfd, buff, strlen(buff), 0) <= 0) {
			break;
		}

		if(strcmp(buff, "/quit\n") == 0){
			return 1;
		}

		printf("Message sent!\n");
		// Cleaning memory
		memset(buff, 0, MSG_LEN);
		// Receiving message
		if (recv(sockfd, buff, MSG_LEN, 0) <= 0) {
			break;
		}

		if(strcmp(buff, "/quit\n") == 0){
			return 2;
		}

		printf("Received: %s", buff);
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