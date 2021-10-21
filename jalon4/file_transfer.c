#include "file_transfer.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>


// init & destroy mutexs
void file_list_mutexs_init(){
    pthread_mutex_init(&mutex_file_hist_stdin, NULL);
    pthread_mutex_init(&mutex_file_hist_server_socket, NULL);
}

void file_list_mutexs_destroy(){
    pthread_mutex_destroy(&mutex_file_hist_stdin);
    pthread_mutex_destroy(&mutex_file_hist_server_socket);
}

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
void *file_list_print_hist(void *filistes_ptrs){

    // for address computing
    struct file_list **file_lists= (struct file_list **) filistes_ptrs;

    struct file_list *filiste_in = file_lists[0];
    struct file_list *filiste_out = file_lists[1];

    int display_periode = 500;

    int nfds =1;
    struct pollfd pollstdin[nfds];
    pollstdin[0].fd = STDIN_FILENO;
    pollstdin[0].revents = 0;
    pollstdin[0].events = 1;

    struct file *f;

    char ref_char = '-';

    pthread_mutex_lock(&mutex_file_hist_stdin);

    while(1){

        f = filiste_in->first_file;
        while(f != NULL){
            switch(f->transfer_status){
                case PROPOSED:
                    printf("%s : you <- %s REQEUSTED\n", f->name, f->other_side_client.nickname);
                    break;
                
                case REJECTED:
                    printf("%s : you <- %s REJECTED\n", f->name, f->other_side_client.nickname);
                    break;

                default:
                    printf("%s : you <- %s  %d%%\n", f->name, f->other_side_client.nickname, f->progress);
                    break;
            }
            f = f->next;
        }
        f = filiste_out->first_file;
        while(f != NULL){
            switch(f->transfer_status){
                case PROPOSED:
                    printf("%s : you -> %s REQEUSTED\n", f->name, f->other_side_client.nickname);
                    break;
                
                case REJECTED:
                    printf("%s : you -> %s REJECTED\n", f->name, f->other_side_client.nickname);
                    break;

                default:
                    printf("%s : you -> %s  %d%%\n", f->name, f->other_side_client.nickname, f->progress);
                    break;
            }
            f = f->next;
        }

        printf("%c Refreshing. Click [Entre] to quit file transfer history\n", ref_char);
        if(ref_char ==  '-'){
            ref_char = '|';
        }else{
            ref_char = '-';
        }

        if(poll(pollstdin, nfds, display_periode) != 0){
            break;
        }else{
            printf("\033[%dF", filiste_in->file_nbr+filiste_out->file_nbr+1);
        }
    }

    // Empty stdin
    char c;
    c = getchar();
    while(c != '\n'){
        c = getchar();
    }

    pthread_mutex_unlock(&mutex_file_hist_stdin);

    return NULL;

}