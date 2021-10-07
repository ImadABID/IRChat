#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "req_reader.h"

enum msg_type req_reader(char *req, struct message *struct_msg, void **data){
    // /nick, /who, /whois , /msgall , /msg, /quit

    if(strncmp(req, "/nick ", 6) == 0){

        struct_msg->pld_len = 0;
        strcpy(struct_msg->nick_sender, nick_name);
        struct_msg->type = NICKNAME_NEW;
        strcpy(struct_msg->infos, req+6);

        *data = NULL;

        return NICKNAME_NEW;
        
    }

    if(strcmp(req, "/who") == 0){

        struct_msg->pld_len = 0;
        strcpy(struct_msg->nick_sender, nick_name);
        struct_msg->type = NICKNAME_LIST;
        strcpy(struct_msg->infos, "");

        *data = NULL;

        return NICKNAME_LIST;

    }

    if(strncmp(req, "/whois ", 7) == 0){
        struct_msg->pld_len = 0;
        strcpy(struct_msg->nick_sender, nick_name);
        struct_msg->type = NICKNAME_INFOS;
        strcpy(struct_msg->infos, req+7);

        *data = NULL;

        return NICKNAME_INFOS;
    }

    if(strncmp(req, "/msgall ", 8) == 0){

        struct_msg->pld_len = (strlen(req+8)+1)*sizeof(char);
        strcpy(struct_msg->nick_sender, nick_name);
        struct_msg->type = BROADCAST_SEND;
        strcpy(struct_msg->infos, "");

        *data = malloc(struct_msg->pld_len);
        strcpy(*data, req+8);
        
        return BROADCAST_SEND;
    }

    if(strncmp(req, "/msg ", 5) == 0){
        
    }

    if(strncmp(req, "/quit", 5) == 0){
        *data = NULL;
        struct_msg->pld_len = 0;
        strcpy(struct_msg->nick_sender,"");
        struct_msg->type = CLIENT_QUIT;
        strcpy(struct_msg->infos,"");
        return CLIENT_QUIT;
    }

    //ECHO_SEND
    *data = malloc((strlen(req)+1)*sizeof(char));
    strcpy(*data, req);
    struct_msg->pld_len = strlen(req)+1;
    strcpy(struct_msg->nick_sender, nick_name);
    struct_msg->type = ECHO_SEND;
    strcpy(struct_msg->infos, "");
    return ECHO_SEND;

}