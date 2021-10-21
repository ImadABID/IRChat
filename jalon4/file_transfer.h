#ifndef __FILE_TRANSFER_H__
#define __FILE_TRANSFER_H__

#include <pthread.h>

#include "common.h"
#include "client_list.h"

enum file_transfer_status{
    PROPOSED, //REQEUSTED
    REJECTED,
    TRANSFERING
};

struct file{
    enum file_transfer_status transfer_status;
    unsigned char progress;
    char name[STR_MAX_SIZE]; //path
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
void file_list_add(struct file_list *filiste, char *name, char *other_side_client_nick);

// Free
void file_free(struct file *f);
void file_recursif_free(struct file *f);
void list_file_free(struct file_list *filiste);

// Getters

struct file *file_list_get_by_filename(struct file_list * filiste, char *filename);

// display
void file_list_print_hist(struct file_list * filiste_in, struct file_list * filiste_out);

#endif