#include "client_list.h"

#include <stdlib.h>

struct client *client_new(){
    struct client *c = malloc(sizeof(struct client));
    c->prev = NULL;
    c->next = NULL;

    return c;
}

void client_free(struct client *c){
    free(c);
}

void client_recursive_free(struct client *c){
    if(c != NULL){
        if(c->prev != NULL){
           client_recursive_free(c->prev);
        }else{
            if(c->next == NULL){
                client_free(c);
            }else{
                c->next->prev = NULL;
                client_recursive_free(c->next);
                client_free(c);
            }
        }
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
    c->prev = NULL;
    c->next = cl->first_client;

    cl->first_client = c;
}

void client_list_drop_client(struct client_list *cl, struct client *c){

    if(c->prev == NULL){
        cl->first_client = c->next->next;
    }else{
        c->prev->next = c->next->next;
    }

    if(c->next != NULL){
        c->next->prev = c->prev;
    }

    client_free(c);
}