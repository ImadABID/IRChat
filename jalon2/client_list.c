#include "client_list.h"

#include <stdio.h>
#include <stdlib.h>

struct client *client_new(){
    struct client *c = malloc(sizeof(struct client));

    c->next = NULL;

    return c;
}

void client_free(struct client *c){
    free(c->host);
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

    return cl;
}

void client_list_free(struct client_list *cl){
    client_recursive_free(cl->first_client);
    free(cl);
}

void client_list_insert(struct client_list *cl, struct client *c){
    c->next = cl->first_client;
    cl->first_client = c;
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