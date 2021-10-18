#ifndef __CLIENTS_LIST_H__
#define __CLIENTS_LIST_H__

#include "msg_struct.h"
#include "common.h"

struct client{

    int *fd;
    char *host;
    unsigned short *port;

    char *nickname;

    char *connecion_time;
    
    struct client *next;
    
};

struct client_list{
    struct client *first_client;

    unsigned int client_nbr;
};

// Client
struct client *client_new();

/* client_copy
    /!\ Important
        pointer always point to the same data
*/
struct client *client_copy(struct client *c);

// Client free
void client_free(struct client *c);
void client_recursive_free(struct client *c);

// Client List
struct client_list *client_list_new();
void client_list_free(struct client_list *cl);

// Insert / Drop
void client_list_insert(struct client_list *cl, struct client *c);
int client_list_drop_client_by_fd(struct client_list *cl, int fd);
int client_list_detache_client_by_fd(struct client_list *cl, int fd);

// Getters
struct client *client_list_get_client_by_fd(struct client_list *cl, int fd);
struct client *client_list_get_client_by_nickname(struct client_list *cl, char *nickname_);

//nickname check
char client_list_nickname_already_used(struct client_list *cl, int fd, char *nikname);

// Get nicknames in an array
void client_list_niknames_as_array(struct client_list *cl, char *buff);

#endif