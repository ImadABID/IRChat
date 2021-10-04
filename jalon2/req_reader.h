#ifndef __REQ_READER_H__
#define __REQ_READER_H__

#include <stdlib.h>

#include "msg_IO.h"

enum msg_type req_reader(char *req, struct message *struct_msg, void **data);
/*
    free data after use.
*/

#endif