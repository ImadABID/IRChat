#ifndef __REQ_READER_H__
#define __REQ_READER_H__

#include <stdlib.h>

#include "msg_IO.h"
#include "msg_struct.h"

char nick_name[NICK_LEN];
char salon_name[NICK_LEN];

enum msg_type req_reader(char *req, struct message *struct_msg, void **data);
/*
    free data after use.
*/

#endif