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
#include <pthread.h>

#include "common.h"
#include "msg_IO.h"
#include "req_reader.h"
#include "file_transfer.h"

char name_validate(char nick_name_[]){

	size_t nikname_len = 0;
	char c = nick_name_[0];
	while(c != '\0'){
		if(!(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9'))){
			printf("The name shouldn't contain special characters. Try again.\n");
			return 0;
		}
		c = nick_name_[nikname_len++];
		if(nikname_len > NICK_LEN-1){
			printf("The name too long. Try again.\n");
			return 0;
		}
	}
	nikname_len++;

	if(nikname_len==1){
		printf("The name can't be an empty string. Try again.\n");
		return 0;
	}

	return 1;
}

void nickname_set_1st_time(int socket_fd){

	char buff_stdin[MSG_LEN];
	int buff_stdin_i;
	char c;

	printf("Choose a nikname. Ex : /nick Bond007\n");
	c = getchar();
	buff_stdin_i = 0;
	while(c != '\n'){
		buff_stdin[buff_stdin_i++] = c;
		c = getchar();
	}
	buff_stdin[buff_stdin_i++] = '\0';

	struct message struct_msg;
	void *data;
	char repeat = 1;
	while(repeat){
		while(req_reader(buff_stdin, &struct_msg, &data) != NICKNAME_NEW){
			if(data != NULL){
				free(data);
				data = NULL;
			}
			printf("Choose a nickname. Ex : /nick Bond007\n");
			c = getchar();
			buff_stdin_i = 0;
			while(c != '\n'){
				buff_stdin[buff_stdin_i++] = c;
				c = getchar();
			}
			buff_stdin[buff_stdin_i++] = '\0';
		}
		if(name_validate(struct_msg.infos)){
			repeat = 0;
			send_msg(socket_fd, &struct_msg, data);
		}else{
			c = getchar();
			buff_stdin_i = 0;
			while(c != '\n'){
				buff_stdin[buff_stdin_i++] = c;
				c = getchar();
			}
			buff_stdin[buff_stdin_i++] = '\0';
		}

	}
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

	char buff_stdin[MSG_LEN];
	int buff_stdin_i;
	char c;

	int socket_fd = handle_connect(argv[1], argv[2]);

	// Set File I/O Lists
	struct file_list *file_in_list = file_list_init();
	struct file_list *file_out_list = file_list_init();

	//Set Nickname
	strcpy(nick_name, "");
	nickname_set_1st_time(socket_fd);

	// Set Salon Name
	strcpy(salon_name, "");

	//file_hist's mutexs def & init
	pthread_mutex_t mutex_file_hist_stdin;
	pthread_mutex_t mutex_file_hist_server_socket;

	pthread_mutex_init(&mutex_file_hist_stdin, NULL);
	pthread_mutex_init(&mutex_file_hist_server_socket, NULL);

	struct pollfd pollfds[2];
	pollfds[0].fd = STDIN_FILENO;
	pollfds[0].events = POLLIN;
	pollfds[0].revents = 0;
	pollfds[1].fd = socket_fd;
	pollfds[1].events = POLLIN;
	pollfds[1].revents = 0;

	while(1){

		int n_active;
		if(-1 == (n_active = poll(pollfds, 2, POLL_TIMEOUT))){
			perror("Poll");
		}

		if(pollfds[0].revents & POLLIN){ // stdin

			if(pthread_mutex_trylock(&mutex_file_hist_stdin) == 0){
				
				//-printf("[stdin mutex] taken by main.\n");

				// buffer index
				memset(buff_stdin, 0, MSG_LEN);
				buff_stdin_i = 0;

				c = getchar();
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
						if(strlen((char *) data) > 0){
							send_msg(socket_fd, &struct_msg, data);
							printf("[%s]->[Server] : %s\n", nick_name, (char *) data);
						}
						break;

					case NICKNAME_NEW:
						if(name_validate(struct_msg.infos)){
							send_msg(socket_fd, &struct_msg, data);
						}
						break;
					
					case NICKNAME_LIST:
						send_msg(socket_fd, &struct_msg, data);
						break;

					case NICKNAME_INFOS:
						send_msg(socket_fd, &struct_msg, data);
						break;

					case BROADCAST_SEND:
						send_msg(socket_fd, &struct_msg, data);
						printf("[%s]->[All] : %s\n\n", nick_name, (char *) data);
						break;

					case UNICAST_SEND:
						if(data == NULL){
							printf("Syntax Error. /msg Syntax :\n\t/msg receiverNickname the message you want to send\n");
						}else{
							printf("[%s]->[%s] : %s\n\n", nick_name, struct_msg.infos, (char *) data);
							send_msg(socket_fd, &struct_msg, data);
						}

						break;

					case MULTICAST_CREATE :
						if(name_validate(struct_msg.infos)){
							send_msg(socket_fd, &struct_msg, data);
						}
						break;

					case MULTICAST_LIST:
						send_msg(socket_fd, &struct_msg, data);
						break;
					
					case MULTICAST_JOIN:
						send_msg(socket_fd, &struct_msg, data);
						printf("Joining channel : %s\n\n", struct_msg.infos);
						break;

					case MULTICAST_QUIT:
						if(strcmp(struct_msg.infos, salon_name) != 0){
							printf("Please type one of the following requests :\n\t /quit %s : to quit the channel\n\t /quit to exit the programme\n", salon_name);
						}else{
							send_msg(socket_fd, &struct_msg, data);
							printf("Quiting channel : %s\n\n", struct_msg.infos);
							strcpy(salon_name, "");
						}
						
						break;

					case MULTICAST_SEND:
						if(strlen((char *) data) > 0){
							send_msg(socket_fd, &struct_msg, data);
							printf("[%s]=>[%s] : %s\n\n", nick_name, struct_msg.infos, (char *) data);
						}
						break;

					case CLIENT_QUIT:

						send_msg(socket_fd, &struct_msg, data);

						printf("Deconnected\n");
						goto quitter;

					case FILE_REQUEST:

						if(data == NULL){
							printf("Please Respect this format : /send file_name receiver_nickname\n");
						}else{
							char new_file_name[STR_MAX_SIZE];
							sprintf(new_file_name, "%s_%ld", (char *) data, time(NULL));
							
							free(data);

							struct_msg.pld_len = (strlen(new_file_name)+1) * sizeof(char);

							data = malloc(struct_msg.pld_len);
							strcpy((char *) data, new_file_name);

							file_list_add(file_out_list, (char *) data, struct_msg.infos);
							send_msg(socket_fd, &struct_msg, data);
							printf("File new name is %s.\nYou are going to be notified when %s responds.\n", (char *) data, struct_msg.infos);
						}

						break;

					case FILE_REJECT:{
						struct file *f = file_list_get_by_filename(file_in_list, (char *) data);
						if(f == NULL){
							printf("You didn't received any file with this name. Retry with /file_reject a_valid_filename.\n");
							printf("Suggestion :\n\t /file_hist : to display file transfer history.");
						}else{
							f->transfer_status = REJECTED;
							strcpy(struct_msg.infos, f->other_side_client.nickname);
							printf("Rejection was sent to %s.\n", struct_msg.infos);
							send_msg(socket_fd, &struct_msg, data);
						}
						break;
					}

					case FILE_HIST:{

						file_list_print_hist_launch_thread(
							file_in_list,
							file_out_list,
							&mutex_file_hist_stdin,
							&mutex_file_hist_server_socket
						);

						break;
					}

					default:
						break;
				}

				if(data != NULL){
					free(data);
					data = NULL;
				}

				pollfds[0].revents = 0;

				//-printf("[stdin mutex] realesed by main.\n");
				pthread_mutex_unlock(&mutex_file_hist_stdin);
			}
		}

		if(pollfds[1].revents & POLLIN){ // socket

			pthread_mutex_lock(&mutex_file_hist_server_socket);
			//-printf("[socket mutex] taken by main.\n");

			// Receiving message
			void *data = NULL;
			struct message msg_struct;
			struct whois_data *client_data = NULL;

			switch(receive_msg(socket_fd, &msg_struct, &data)){
				
				case ECHO_SEND:
					printf("[server] %s\n", (char *) data);
					break;

				case NICKNAME_NEW:
					if(strcmp(msg_struct.infos, "AlreadyUsed") == 0){
						if(strcmp(nick_name, "") == 0){
							printf("Nickname already used. Try again.\n");
							nickname_set_1st_time(socket_fd);
						}else{
							printf("Nickname already used. Operation rejected.\n");
						}
					}else{
						strcpy(nick_name, msg_struct.infos);
						printf("Nickname accepted : %s.\n", nick_name);
					}
					break;

				case NICKNAME_LIST:
					printf("[Server] : Online users are :\n");
					for(int i =0; i < msg_struct.pld_len; i+=NICK_LEN){
						printf("\t%s\n", ((char *) data)+i);
					}
					break;

				case NICKNAME_INFOS:

					if(strncmp((char *)data, "Error : ", 8) == 0){
						printf("\t%s\n", (char *)data);
					}else{

					client_data = (struct whois_data *) data;

					printf("[Server] :\n");
					printf("\tNickname \t\t: %s\n", client_data->nickname);
					printf("\tConnected since \t: %s\n", client_data->date);
					printf("\tAddress \t\t: %s\n", client_data->address);
					printf("\tPort \t\t\t: %d\n", client_data->port);

					}
					
					break;

				case BROADCAST_SEND:
					printf("[%s]->[All] %s\n", msg_struct.nick_sender, (char *) data);
					break;

				case UNICAST_SEND:
					printf("[%s]->[%s] %s\n", msg_struct.nick_sender, nick_name, (char *) data);
					break;

				case MULTICAST_CREATE:
					if(strcmp(msg_struct.infos, "AlreadyUsed") == 0){
						printf("This channel name is already used. Operation rejected.\n");
					}else{
						strcpy(salon_name, msg_struct.infos);
						printf("Channel created with the name : %s.\n", salon_name);
					}
					break;

				case MULTICAST_LIST:
					printf("[Server] : Active channels are :\n");
					for(int i =0; i < msg_struct.pld_len; i+=NICK_LEN){
						printf("\t%s\n", ((char *) data)+i);
					}
					break;

				case MULTICAST_JOIN:
					if(strlen(msg_struct.infos)==0){
						printf("There is no channel with this name. Join Rejected\n");
					}else{
						strcpy(salon_name, msg_struct.infos);
						printf("Join accepted\n");
					}
					break;

				case MULTICAST_SEND:
					printf("\n[%s]=>[%s] : %s\n\n", msg_struct.nick_sender, msg_struct.infos, (char *) data);
					break;

				case FILE_REQUEST:
					file_list_add(file_in_list, (char *) data, msg_struct.infos);
					printf("[%s] requests to send you %s.\n You can accept it or rejected any time you want by Typing : \n", msg_struct.infos, (char *) data);
					printf("\tTo accept it, type :\t/file_accept %s\n\tTo reject it, type :\t/file_reject %s\n", (char *) data, (char *) data);
					break;

				case FILE_REJECT:{

					struct file * f = file_list_get_by_filename(file_out_list, (char *) data);
					if(f != NULL){
						f->transfer_status = REJECTED;
						if(strcmp(msg_struct.nick_sender, "Server") == 0){
							printf("[%s] No user with such nickname. %s rejected.\n", msg_struct.nick_sender, (char *) data);
						}else{
							printf("[%s] %s was rejected by its receiver.\n", msg_struct.nick_sender, (char *) data);
						}
					}

					break;
				}



				case CLIENT_QUIT:
					printf("Server Deconnected\n");
					goto quitter;

				default:
					break;
			}

			if(data != NULL){
				free(data);
				data = NULL;
			}

			printf("\n");

			pollfds[1].revents = 0;

			//- printf("[socket mutex] realesed by main.\n");
			pthread_mutex_unlock(&mutex_file_hist_server_socket);
		}

		if(pollfds[1].revents & POLLHUP){
			close(socket_fd);
			printf("Server Deconnected\n");
			goto quitter;
		}
		
	}
	
	quitter :
	close(socket_fd);
	pthread_mutex_destroy(&mutex_file_hist_stdin);
	pthread_mutex_destroy(&mutex_file_hist_server_socket);
	list_file_free(file_in_list);
	list_file_free(file_out_list);

	return EXIT_SUCCESS;
}