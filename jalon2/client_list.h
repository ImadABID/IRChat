#ifndef __CLIENTS_LIST_H__
#define __CLIENTS_LIST_H__

#include "msg_struct.h"

struct client{

    int fd;
    char *host;
    unsigned short port;

    char *nickname;
    
    struct client *next;
    
};

struct client_list{
    struct client *first_client;
};

// Client
struct client *client_new();
void client_free(struct client *c);
void client_recursive_free(struct client *c);

// Client List
struct client_list *client_list_new();
void client_list_free(struct client_list *cl);

// Insert / Drop
void client_list_insert(struct client_list *cl, struct client *c);
void client_list_drop_client_by_fd(struct client_list *cl, int fd);

// Getters
struct client *client_list_get_client_by_fd(struct client_list *cl, int fd);

//nickname check
char client_list_nickname_already_used(struct client_list *cl, int fd, char *nikname);


#endif