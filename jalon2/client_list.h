#ifndef __CLIENTS_LIST_H__
#define __CLIENTS_LIST_H__


struct client{

    int fd;
    char *host;
    unsigned short port;
    
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

#endif