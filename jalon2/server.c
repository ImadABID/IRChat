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
#include "client_list.h"
#include "msg_IO.h"

void echo_server(int socket_fd, struct message struct_msg, void *data) {
	/*

	Return value :
	0 : every thing is ok
	1 : connection closed by client

	*/

	// Handling connection closing
	/*
	if(strcmp(buff, "/quit") == 0){
		free(buff);
		return 1;
	}
	*/

	printf("\tReceived: %s\n", (char *)data);
	
	// Sending message (ECHO)
	send_msg(
		socket_fd,
		struct_msg.pld_len,
		"server",
		ECHO_SEND,
		"Noinfo",
		data
	);

	printf("\tMessage sent!\n");
}

int handle_bind(char port[]) {
	struct addrinfo hints, *result, *rp;
	int sfd;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, port, &hints, &result) != 0) {
		perror("getaddrinfo()");
		exit(EXIT_FAILURE);
	}
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,
		rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}
		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
			break;
		}
		close(sfd);
	}
	if (rp == NULL) {
		fprintf(stderr, "Could not bind\n");
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(result);
	return sfd;
}

int main(int argc, char *argv[]) {

	if(argc < 2){
		fprintf(stderr, "Usage : server port\n");
		exit(EXIT_FAILURE);
	}

	int sfd = handle_bind(argv[1]);
	if ((listen(sfd, SOMAXCONN)) != 0) {
			perror("listen()\n");
			exit(EXIT_FAILURE);
	}
	
	struct client_list *client_list = client_list_new();
	struct pollfd pollfds[MAX_CLIENTS];

	pollfds[0].fd = sfd;
	pollfds[0].events = POLLIN;
	pollfds[0].revents = 0;
	for(int i=1; i<MAX_CLIENTS; i++){
		pollfds[i].fd = -1;
		pollfds[i].events = 0;
		pollfds[i].revents = 0;
	}

	while(1){
		int n_active;
		if(-1 == (n_active = poll(pollfds, MAX_CLIENTS, POLL_TIMEOUT))){
			perror("Poll");
		}

		for(int i = 0; i<MAX_CLIENTS; i++){

			if(pollfds[i].fd == sfd && pollfds[i].revents & POLLIN){
				// Accept
				struct sockaddr cli;
				int connfd;
				socklen_t len = sizeof(cli);
				
				if ((connfd = accept(sfd, (struct sockaddr*) &cli, &len)) < 0) {
					perror("accept()\n");
					exit(EXIT_FAILURE);
				}
				

				//store info in pollfds
				for(int j = 0; j<MAX_CLIENTS; j++){
					if(pollfds[j].fd == -1){
						pollfds[j].fd = connfd;
						pollfds[j].events = POLLIN;
						pollfds[j].revents = 0;
						break;
					}
				}

				//store client info in list chainee
				struct client *c = client_new();
				struct sockaddr_in *client_in = (struct sockaddr_in *) &cli;
				c->fd = connfd;
				c->port = htons(client_in->sin_port);
				char *ip_str = inet_ntoa(client_in->sin_addr);
				c->host = malloc((strlen(ip_str)+1)*sizeof(char));
				strcpy(c->host, ip_str);
				
				client_list_insert(client_list, c);

				// Display Client info
				printf("%s:%d :\n\tConnection accepted.\n", c->host, c->port);

				// set pollfd[i].revent = 0
				pollfds[i].revents = 0;

			}else if(pollfds[i].fd != sfd && pollfds[i].revents & POLLHUP){
				// display message on terminal
				printf("Client in fd = %d deconnected\n", pollfds[i].fd);
				
				//close(pollfd[i].fd)
				close(pollfds[i].fd);
				client_list_drop_client_by_fd(client_list, pollfds[i].fd);
				pollfds[i].fd = -1;
				pollfds[i].events = 0;
				pollfds[i].revents = 0;

				// set pollfds[i].event = 0
				pollfds[i].events = 0;

				//set pollfds[i].revent = 0
				pollfds[i].revents = 0;
			}else if(pollfds[i].fd != sfd && pollfds[i].revents & POLLIN){
				
				struct client *c = client_list_get_client_by_fd(client_list, pollfds[i].fd);
				printf("\n%s:%d :\n", c->host, c->port);

				void *data = NULL;
				struct message struct_msg = receive_msg(pollfds[i].fd, &data);

				switch (struct_msg.type){
					case ECHO_SEND:
						/*
						switch(echo_server(pollfds[i].fd)){
							case 1:
								close(pollfds[i].fd);
								client_list_drop_client_by_fd(client_list, pollfds[i].fd);
								pollfds[i].fd = -1;
								pollfds[i].events = 0;
								pollfds[i].revents = 0;
								printf("\tConnection was closed by the client.\n");
								break;

							default :
								break;
						}
						*/
						echo_server(pollfds[i].fd, struct_msg, data);

						break;
					
					default:
						break;
				}

				if(data != NULL){
					free(data);
				}

			}
		}
	}

	client_list_free(client_list);
	close(sfd);

	return EXIT_SUCCESS;
}