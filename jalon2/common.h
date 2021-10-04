#ifndef __COMMON_H__
#define __COMMON_H__

#define MSG_LEN 1024
#define SERV_PORT "8080"
#define SERV_ADDR "127.0.0.1"

#define MAX_CLIENTS 100
#define POLL_TIMEOUT 3000

struct info{
    short s;
    long l;
};

#endif