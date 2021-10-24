#include "file_transfer.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <err.h>


// init

struct file_list *file_list_init(){
    struct file_list *filiste = malloc(sizeof(struct file_list));
    filiste->file_nbr = 0;
    filiste->first_file = NULL;
    return filiste;
}

void file_list_add(struct file_list *filiste, char *name, char *other_side_client_nick){

    
    struct file *f = malloc(sizeof(struct file));
    

    f->transfer_status = PROPOSED;
    strcpy(f->name, name);

    f->other_side_client.nickname = malloc(STR_MAX_SIZE * sizeof(char));
    strcpy(f->other_side_client.nickname, other_side_client_nick);

    f->other_side_client.fd = NULL;
    f->other_side_client.host = NULL;
    f->other_side_client.port = NULL;
    f->other_side_client.connecion_time = NULL;
    f->other_side_client.next = NULL;


    f->next = filiste->first_file;
    filiste->first_file = f;
    filiste->file_nbr++;
    
    
}

// Free

void file_free(struct file *f){
    if(f != NULL){
        client_free_fields_only(f->other_side_client);
        free(f);
        f = NULL;
    }
}

void file_recursif_free(struct file *f){
    if(f != NULL){
        file_recursif_free(f->next);
        file_free(f);
    }
}

void list_file_free(struct file_list *filiste){
    if(filiste != NULL){
        file_recursif_free(filiste->first_file);
        free(filiste);
        filiste = NULL;
    }
}


// Getters

struct file *file_list_get_by_filename(struct file_list * filiste, char *filename){
    struct file *f = filiste->first_file;

    while(f!=NULL){
        if(strcmp(f->name, filename) == 0){
            return f;
        }
        f = f->next;
    }

    return NULL;
}

// display

struct file_list_print_hist_args{
    struct file_list *filiste_in;
    struct file_list *filiste_out;
    pthread_mutex_t *mutex_stdin;
    pthread_mutex_t *mutex_server_socket;
};

void file_list_print_hist_launch_thread(
    struct file_list *filiste_in,
    struct file_list *filiste_out,
    pthread_mutex_t *mutex_stdin,
    pthread_mutex_t *mutex_server_socket
){
    struct file_list_print_hist_args *args_struct = malloc(sizeof(struct file_list_print_hist_args));

    args_struct->filiste_in = filiste_in;
    args_struct->filiste_out = filiste_out;
    args_struct->mutex_stdin = mutex_stdin;
    args_struct->mutex_server_socket = mutex_server_socket;

    pthread_t file_hist_thread;
    pthread_create(&file_hist_thread, NULL, file_list_print_hist, (void *) args_struct);
    pthread_detach(file_hist_thread);

}

void *file_list_print_hist(void *void_p_args){
    
    struct file_list_print_hist_args *args_struct= (struct file_list_print_hist_args *) void_p_args;

    struct file_list *filiste_in = args_struct->filiste_in;
    struct file_list *filiste_out = args_struct->filiste_out;
    pthread_mutex_t *mutex_stdin = args_struct->mutex_stdin;
    pthread_mutex_t *mutex_server_socket = args_struct->mutex_server_socket;

    free(void_p_args);

    int display_periode = 500;

    int nbr_line_to_delete;

    int nfds =1;
    struct pollfd pollstdin[nfds];
    pollstdin[0].fd = STDIN_FILENO;
    pollstdin[0].revents = 0;
    pollstdin[0].events = 1;

    struct file *f;

    char ref_char = '-';
    pthread_mutex_lock(mutex_stdin);
    //-printf("[stdin mutex] taken by display_func.\n");
    while(1){

        pthread_mutex_lock(mutex_server_socket);
        //-printf("[socket mutex] taken by display_func.\n");

        printf("\n########## %c File Transfer History %c\n", ref_char, ref_char);

        f = filiste_in->first_file;
        while(f != NULL){
            switch(f->transfer_status){
                case PROPOSED:
                    printf("%s\t: you <- %s REQEUSTED\n", f->name, f->other_side_client.nickname);
                    break;
                
                case REJECTED:
                    printf("%s\t: you <- %s REJECTED\n", f->name, f->other_side_client.nickname);
                    break;

                default:
                    printf("%s\t: you <- %s  %d%%\n", f->name, f->other_side_client.nickname, f->progress);
                    break;
            }
            f = f->next;
        }
        f = filiste_out->first_file;
        while(f != NULL){
            switch(f->transfer_status){
                case PROPOSED:
                    printf("%s\t: you -> %s REQEUSTED\n", f->name, f->other_side_client.nickname);
                    break;
                
                case REJECTED:
                    printf("%s\t: you -> %s REJECTED\n", f->name, f->other_side_client.nickname);
                    break;

                default:
                    printf("%s\t: you -> %s  %d%%\n", f->name, f->other_side_client.nickname, f->progress);
                    break;
            }
            f = f->next;
        }

        printf("########## Click [Entre] to quit file transfer history.\n");
        if(ref_char ==  '-'){
            ref_char = '|';
        }else{
            ref_char = '-';
        }


        if(poll(pollstdin, nfds, display_periode) != 0){
            break;
        }
        nbr_line_to_delete = filiste_in->file_nbr+filiste_out->file_nbr+3;
        printf("\033[%dF", nbr_line_to_delete);
        for(int i = 0; i < nbr_line_to_delete; i++){
            printf("                                                                   \n");
        }
        printf("\033[%dF", nbr_line_to_delete);

        //-printf("[socket mutex] released by display_func.\n");
        pthread_mutex_unlock(mutex_server_socket);
        usleep(100);
    }

    // Empty stdin
    char c;
    c = getchar();
    while(c != '\n'){
        c = getchar();
    }

    //-printf("[socket mutex] released by display_func.\n");
    //-printf("[stdin mutex] released by display_func.\n");
    pthread_mutex_unlock(mutex_server_socket);
    pthread_mutex_unlock(mutex_stdin);

    return NULL;

}

// Receive & sent

u_short file_receive_lunche_thread(){


    //Création de la socket
    int listen_fd = -1;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_fd == -1){
        perror("Socket");
        exit(EXIT_FAILURE);
    }

    //construction de struct sockaddrs
    struct addrinfo hints, *res, *tmp;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV | AI_PASSIVE;

    int err = getaddrinfo("127.0.0.1", NULL, &hints, &res);
    if(err){
        errx(1, "%s", gai_strerror(err));
    }

    tmp = res;
    while(tmp != NULL){
        if(tmp->ai_addr->sa_family == AF_INET){
            //bind
            err = bind(listen_fd, tmp->ai_addr, tmp->ai_addrlen);
            if(err == -1){
                perror("bind");
                exit(EXIT_FAILURE);
            }
            break;
        }
        tmp = tmp->ai_next;
    }

    // get port number
    struct sockaddr addr;
    socklen_t addrlen;
    if(getsockname(listen_fd, &addr, &addrlen) == -1){
        perror("getsockname");
        exit(EXIT_FAILURE);
    }
    u_short *p = (u_short *) addr.sa_data;

    u_short port_nbr = ntohs(*p);

    // listen
    if(listen(listen_fd, 1) == -1){
        perror("Listen");
    }

    // Thread_launch

    return port_nbr;

}

void *file_send(void *void_p_args){

    struct file_transfer_conn_info receiver_conn_info = *((struct file_transfer_conn_info *) void_p_args);
    free(void_p_args);

    printf("sending file to %s:%hu.\n", receiver_conn_info.hostname, receiver_conn_info.port);

    return NULL;
}