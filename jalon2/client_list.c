#include "client_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct client *client_new(){
    struct client *c = malloc(sizeof(struct client));

    c->next = NULL;

    return c;
}

void client_free(struct client *c){
    if(c->host != NULL)
        free(c->host);

    if(c->nickname != NULL)
        free(c->nickname);

    if(c->connecion_time != NULL)
        free(c->connecion_time);

    free(c);
}

void client_recursive_free(struct client *c){
    if(c != NULL){
        client_recursive_free(c->next);
        client_free(c);
    }
}


struct client_list *client_list_new(){
    struct client_list *cl = malloc(sizeof(struct client_list));
    cl->first_client = NULL;
    cl->client_nbr = 0;

    return cl;
}

void client_list_free(struct client_list *cl){
    client_recursive_free(cl->first_client);
    free(cl);
}

void client_list_insert(struct client_list *cl, struct client *c){
    c->next = cl->first_client;
    cl->first_client = c;
    cl->client_nbr++;
}

void client_list_drop_client_by_fd(struct client_list *cl, int fd){

    struct client *c = cl->first_client;
    struct client *c_prev = NULL;

    // Finding prev c
    while(c != NULL){

        if(c->fd == fd){
            break;
        }

        c_prev = c;
        c = c->next;
    }

    if(c == NULL){
        fprintf(stderr, "client_list_drop_client_by_fd : Can't find client with fd = %d.\n", fd);
        exit(EXIT_FAILURE);
    }

    // deleting client
    if(c_prev == NULL){
        cl->first_client = c->next;
    }else{
        c_prev->next = c->next;
    }

    client_free(c);

    cl->client_nbr--;

}

// Getters
struct client *client_list_get_client_by_fd(struct client_list *cl, int fd){
    struct client *c = cl->first_client;

    while(c != NULL){
        if(c->fd == fd){
            return c;
        }
        c = c->next;
    }

    fprintf(stderr, "client_list_get_client_by_fd : No such client with fd = %d.\n", fd);
    exit(EXIT_FAILURE);
    
}

struct client *client_list_get_client_by_nickname(struct client_list *cl, char *nickname_){
    struct client *c = cl->first_client;

    while(c != NULL){
        if(strcmp(c->nickname, nickname_) == 0){
            return c;
        }
        c = c->next;
    }

    // No such client with nickname = nickname_
    return NULL;
}

//nickname check
char client_list_nickname_already_used(struct client_list *cl, int fd, char *nikname){
    struct client *c = cl->first_client;

    while(c != NULL){
        if(c->fd != fd && strcmp(c->nickname, nikname) == 0){
            return 1;
        }
        c = c->next;
    }

    return 0;
}

// Get nicknames in an array
void client_list_niknames_as_array(struct client_list *cl, char *buff){
    int i = 0;
    struct client *c = cl->first_client;
    while(c != NULL){
        strcpy(buff+i*NICK_LEN, c->nickname);
        c = c->next;
        i++;
    }
}