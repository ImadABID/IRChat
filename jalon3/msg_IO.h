#ifndef __MSG_IO_H__
#define __MSG_IO_H__

#include <stdlib.h>

#include "msg_struct.h"

void send_msg(int socket_fd, struct message *msg_struct, void *data);


enum msg_type receive_msg(int socket_fd, struct message *msg_struct, void **data);
/*
    free data after use.
*/


#endif