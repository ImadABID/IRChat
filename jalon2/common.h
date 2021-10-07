#ifndef __COMMON_H__
#define __COMMON_H__

#define MSG_LEN 1024
#define SERV_PORT "8080"
#define SERV_ADDR "127.0.0.1"

#define MAX_CLIENTS 100
#define POLL_TIMEOUT 3000

#define STR_MAX_SIZE 124

struct info{
    short s;
    long l;
};

struct whois_data{
    char nickname[STR_MAX_SIZE];
    char address[STR_MAX_SIZE];
    unsigned short port;
    char date[STR_MAX_SIZE];
};

#endif