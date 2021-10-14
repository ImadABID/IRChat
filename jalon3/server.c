#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>

#include "common.h"
#include "msg_IO.h"
#include "client_list.h"
#include "salon_list.h"


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
	
	struct salon_list *salon_list = salon_list_new();

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
				c->nickname = malloc(NICK_LEN * sizeof(char));
				strcpy(c->nickname, "");
				
				time_t timestamp = time(NULL);
    			struct tm * pTime = localtime(&timestamp); 
				char date_str[STR_MAX_SIZE];
    			strftime(date_str, STR_MAX_SIZE, "%d/%m/%Y %H:%M:%S", pTime);

				c->connecion_time = malloc(sizeof(char)*(strlen(date_str)+1));
				strcpy(c->connecion_time, date_str);

				client_list_insert(client_list, c);

				// Display Client info
				printf("%s:%d :\n\tConnection accepted at %s.\n", c->host, c->port, c->connecion_time);

				// set pollfd[i].revent = 0
				pollfds[i].revents = 0;

			}else if(pollfds[i].fd != sfd && pollfds[i].revents & POLLHUP){
				// display message on terminal
				printf("Client in fd = %d deconnected\n", pollfds[i].fd);
				
				//close(pollfd[i].fd)
				close(pollfds[i].fd);
				if(client_list_drop_client_by_fd(client_list, pollfds[i].fd) == -1){
					fprintf(stderr, "client_list_drop_client_by_fd : can't find a client with fd = %d\n", pollfds[i].fd);
				}
				pollfds[i].fd = -1;

				// set pollfds[i].event = 0
				pollfds[i].events = 0;

				//set pollfds[i].revent = 0
				pollfds[i].revents = 0;
			}else if(pollfds[i].fd != sfd && pollfds[i].revents & POLLIN){
				
				struct client *c = client_list_get_client_by_fd(client_list, pollfds[i].fd);
				printf("\n%s@%s:%d :\n", c->nickname, c->host, c->port);

				void *data = NULL;
				struct message struct_msg;
				
				struct client *client_rcv;

				struct salon *salon;

				switch (receive_msg(pollfds[i].fd, &struct_msg, &data)){

					case ECHO_SEND:
						printf("\tMessage received : %s\n", (char *) data);
						send_msg(pollfds[i].fd, &struct_msg, data);
						printf("\tSending message back : %s\n", (char *) data);
						break;

					case CLIENT_QUIT:
						// display message on terminal
						printf("\tDeconnected\n");
						
						//close(pollfd[i].fd)
						close(pollfds[i].fd);
						if(client_list_drop_client_by_fd(client_list, pollfds[i].fd) == -1){
							fprintf(stderr, "client_list_drop_client_by_fd : can't find a client with fd = %d\n", pollfds[i].fd);
						}
						pollfds[i].fd = -1;

						// set pollfds[i].event = 0
						pollfds[i].events = 0;

						break;

					case NICKNAME_NEW:
						printf("\tSetting nickname : \n\t %s -> %s\n", struct_msg.nick_sender, struct_msg.infos);
						if(client_list_nickname_already_used(client_list, pollfds[i].fd, struct_msg.infos)){
							strcpy(struct_msg.infos, "AlreadyUsed");
							printf("\tOperation rejected : nick name already used.\n");
						}else{
							strcpy(c->nickname, struct_msg.infos);
							printf("\tOperation accepted.\n");
						}
						
						send_msg(pollfds[i].fd, &struct_msg, data);

						break;

					case NICKNAME_LIST:
						printf("\t/who\n");
						struct_msg.pld_len = client_list->client_nbr * NICK_LEN;
						strcpy(struct_msg.nick_sender, "Server");
						// Just in case the client send data
						if(data != NULL){
							free(data);
						}

						data = malloc(struct_msg.pld_len * sizeof(char));
						client_list_niknames_as_array(client_list, (char *) data);

						send_msg(pollfds[i].fd, &struct_msg, data);
						printf("\tResponse was sent.\n");

						break;
						
					case NICKNAME_INFOS:
						printf("\t/whois %s\n", struct_msg.infos);

						// Just in case the client send data
						if(data != NULL){
							free(data);
						}

						struct_msg.pld_len = sizeof(struct whois_data);
						strcpy(struct_msg.nick_sender, "Server");

						struct client *target_client = client_list_get_client_by_nickname(client_list, struct_msg.infos);
						if(target_client == NULL){

							char msg_error[] = "Error : No user with such nickname.";
							data = malloc(sizeof(char)*(strlen(msg_error)+1));
							strcpy(data, msg_error);

						}else{

							struct whois_data *client_data = malloc(sizeof(struct whois_data));
							strcpy(client_data->nickname, target_client->nickname);
							strcpy(client_data->date, target_client->connecion_time);
							strcpy(client_data->address, target_client->host);
							client_data->port = target_client->port;

							data = (void *) client_data;
						}

						send_msg(pollfds[i].fd, &struct_msg, data);
						printf("\tResponse was sent.\n");

						break;

					case BROADCAST_SEND:

						printf("\t/msgall %s\n", (char *) data);

						client_rcv = client_list->first_client;
						while(client_rcv != NULL){

							if(client_rcv == c){
								client_rcv = client_rcv->next;
								continue;
							}

							send_msg(client_rcv->fd, &struct_msg, data);

							client_rcv = client_rcv->next;
						}

						break;

					case UNICAST_SEND:

						printf("\t/msg %s %s\n", struct_msg.infos, (char *) data);
						client_rcv = client_list_get_client_by_nickname(client_list, struct_msg.infos);
						if(client_rcv == NULL){
							if(data != NULL){
								free(data);
							}
							
							char msg_error[] = "Error : No user with such nickname.";

							struct_msg.pld_len = sizeof(char)*(strlen(msg_error)+1);
							strcpy(struct_msg.nick_sender,"Server");

							data = malloc(struct_msg.pld_len);
							strcpy(data, msg_error);

							send_msg(pollfds[i].fd, &struct_msg, data);
							printf("\t%s\n", (char *) data);

						}else{
							send_msg(client_rcv->fd, &struct_msg, data);
							printf("\tMessage sent to %s\n", client_rcv->nickname);
						}

						break;

					case MULTICAST_CREATE:
						printf("\tCreating a channel with the name : %s\n", struct_msg.infos);
						if(salon_list_name_already_used(salon_list, struct_msg.infos)){
							strcpy(struct_msg.infos, "AlreadyUsed");
							printf("\tOperation rejected : The name is already used.\n");
						}else{
							salon = salon_new(struct_msg.infos, c);
							salon_list_insert(salon_list, salon);
							printf("\tOperation accepted.\n");
						}
						
						send_msg(pollfds[i].fd, &struct_msg, data);

						break;

					default:
						break;
				}

				if(data != NULL){
					free(data);
				}

				//set pollfds[i].revent = 0
				pollfds[i].revents = 0;
			}
		}
	}

	client_list_free(client_list);
	salon_list_free(salon_list);
	close(sfd);

	return EXIT_SUCCESS;
}