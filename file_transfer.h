#ifndef __FILE_TRANSFER_H__
#define __FILE_TRANSFER_H__

#include <pthread.h>
#include <sys/types.h>

#include "common.h"
#include "client_list.h"

enum file_transfer_status{
    PROPOSED, //REQEUSTED
    REJECTED,
    TRANSFERING,
    COMPLETED
};

struct file{
    enum file_transfer_status transfer_status;
    size_t progress; // in bytes
    char name[STR_MAX_SIZE]; //path
    int src_file_fd; // Used in file_out_list
    //pthread_t thread;
    struct client other_side_client;

    struct file *next;
};

struct file_list{
    struct file *first_file;

    unsigned int file_nbr;
};

// init
struct file_list *file_list_init();
void file_list_add(
    struct file_list *filiste,
    char *name,
    char *other_side_client_nick,
    int src_file_fd
);

// Free
void file_free(struct file *f);
void file_recursif_free(struct file *f);
void list_file_free(struct file_list *filiste);

// Getters

struct file *file_list_get_by_filename(struct file_list * filiste, char *filename);

// display
void file_list_print_hist_launch_thread(
    struct file_list *filiste_in,
    struct file_list *filiste_out,
    pthread_mutex_t *mutex_stdin,
    pthread_mutex_t *mutex_server_socket
);
void *file_list_print_hist(void *);

// Receive
struct file_transfer_conn_info{
    char hostname[STR_MAX_SIZE];
    u_short port;
};

u_short file_receive_launche_thread(
    struct file *trans_file,
    char *receiver_nickname,
    char *file_name
);
void *file_receive(void * void_p_args);

//send
void file_send_launche_thread(
    struct file_transfer_conn_info conn_info,
    struct file *trans_file,
    pthread_mutex_t *sock_server_mutexe,
    int sock_server_fd
);
void *file_send(void *void_p_args);

#endif