#ifndef __FILE_TRANSFER_H__
#define __FILE_TRANSFER_H__

#include <pthread.h>

#include "common.h"
#include "client_list.h"

enum file_transfer_status{
    PROPOSED, //REQEUSTED
    ACCEPTED,
    REJECTED,
    TRANSFERING,
    TRANSFERED
};

struct file{
    enum file_transfer_status transfer_status;
    char name[STR_MAX_SIZE]; //path
    unsigned int id; // Unique for this client only
    pthread_t thread;
    struct client other_side_client;

    struct file *next;
};

struct file_list{
    struct file *first_file;

    unsigned int file_nbr;
};

// init
struct file_list *file_list_init();
int file_list_assigne_id(struct file_list *filiste, char *name);

// Free
void file_free(struct file *f);
void file_recursif_free(struct file *f);
void list_file_free(struct file_list *filiste);

#endif