#ifndef __MSG_IO_H__
#define __MSG_IO_H__

#include <stdlib.h>

#include "msg_struct.h"

void send_msg(
    int socket_fd,
    size_t pld_len,
    char *nick_sender,
    enum msg_type type,
    char *infos,
    void *data
);


struct message receive_msg(int socket_fd, void **data);
/*
    free data after use.
*/


#endif