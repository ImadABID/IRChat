#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "msg_IO.h"
#include "socket_IO.h"

void send_msg(int socket_fd, size_t pld_len, char *nick_sender, enum msg_type type, char *infos, void *data){
    char *pi;
    int err;
    size_t send;

    struct message msg_struct_val;
    struct message *msg_struct = &msg_struct_val;
    msg_struct->pld_len = pld_len;
    strcpy(msg_struct->nick_sender, nick_sender);
    msg_struct->type = type;
    strcpy(msg_struct->infos, infos);

    pi = (char *) msg_struct;
    send = 0;
    while (send != sizeof(struct message)){
        err = write(socket_fd, pi+send, sizeof(struct message)-send);
        if(err == -1){
            perror("write");
            exit(EXIT_FAILURE);
        }else{
            send += err;
        }
    }

    if(pld_len > 0){
        pi = (char *) data;
        send = 0;
        while (send != pld_len){
            err = write(socket_fd, pi+send, pld_len-send);
            if(err == -1){
                perror("write");
                exit(EXIT_FAILURE);
            }else{
                send += err;
            }
        }
    }
}

struct message receive_msg(int socket_fd, void **data){
    
    struct message msg_struct_val;
    struct message *msg_struct = &msg_struct_val;

    const unsigned int max_read_attempt = 10;
    unsigned int attempt_nbr;
    char *pi;
    int err;
    size_t received;

    pi = (char *) msg_struct;
    received = 0;
    attempt_nbr = 0;
    while (received != sizeof(struct message)){
        err = read(socket_fd, pi+received, sizeof(struct message)-received);
        if(err == -1){
            perror("write");
            exit(EXIT_FAILURE);
        }else if(err == 0){
            if(attempt_nbr++ < max_read_attempt){
                fprintf(stderr, "receive_msg : max read attempt reachted.\n");
                exit(EXIT_FAILURE);
            }
        }else{
            received += err;
        }
    }

    if(msg_struct->pld_len > 0){
        pi = (char *) malloc(msg_struct->pld_len);
        if(pi == NULL){
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        received = 0;
        attempt_nbr = 0;
        while (received != msg_struct->pld_len){
            err = read(socket_fd, pi+received, msg_struct->pld_len-received);
            if(err == -1){
                perror("write");
                exit(EXIT_FAILURE);
            }else if(err == 0){
                if(attempt_nbr++ < max_read_attempt){
                    fprintf(stderr, "receive_msg : max read attempt reachted.\n");
                    exit(EXIT_FAILURE);
                }
            }else{
                received += err;
            }
        }

        *data = (void *) pi;
    }else{
        *data = NULL;
    }

    return msg_struct_val;
}