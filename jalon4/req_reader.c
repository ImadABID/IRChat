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

        size_t req_index = 5;

        while(req[req_index] != ' '){
            if(req[req_index++] == '\0'){
                *data = NULL;
                return UNICAST_SEND;
            }
        }

        struct_msg->pld_len = (strlen(req+req_index)+1)*sizeof(char);
        strcpy(struct_msg->nick_sender, nick_name);
        struct_msg->type = UNICAST_SEND;
        strncpy(struct_msg->infos, req+5, req_index-5);
        struct_msg->infos[req_index-5] = '\0';

        *data = malloc(struct_msg->pld_len);
        strcpy(*data, req+req_index);
        
        return UNICAST_SEND;
    }

    if(strncmp(req, "/create ", 8) == 0){

        struct_msg->pld_len = 0;
        strcpy(struct_msg->nick_sender, nick_name);
        struct_msg->type = MULTICAST_CREATE;
        strcpy(struct_msg->infos, req+8);

        *data = NULL;

        return MULTICAST_CREATE;
        
    }

    if(strcmp(req, "/channel_list") == 0){

        struct_msg->pld_len = 0;
        strcpy(struct_msg->nick_sender, nick_name);
        struct_msg->type = MULTICAST_LIST;
        strcpy(struct_msg->infos, "");

        *data = NULL;

        return MULTICAST_LIST;
    }

    if(strncmp(req, "/join ", 6) == 0){

        struct_msg->pld_len = 0;
        strcpy(struct_msg->nick_sender, nick_name);
        struct_msg->type = MULTICAST_JOIN;
        strcpy(struct_msg->infos, req+6);

        *data = NULL;
        
        return MULTICAST_JOIN;
    }

    if(strncmp(req, "/quit ", 6) == 0){

        *data = NULL;
        struct_msg->pld_len = 0;
        strcpy(struct_msg->nick_sender, nick_name);
        struct_msg->type = MULTICAST_QUIT;
        strcpy(struct_msg->infos, req+6);
        return MULTICAST_QUIT;
    }

    if(strncmp(req, "/send ", 6) == 0){

        /*
            /send file_name receiver_nickname
        */

        size_t receiver_index = 6;

        while(req[receiver_index] != ' '){
            if(req[receiver_index++] == '\0'){
                *data = NULL;
                return FILE_REQUEST;
            }
        }

        req[receiver_index] = '\0';

        
        struct_msg->pld_len = (strlen(req+6)+1)*sizeof(char);
        if(struct_msg->pld_len == 1){
            // file name is an empty string
            *data = NULL;
            return FILE_REQUEST;
        }

        if(strlen(req+receiver_index+1) == 0){
            // receiver nickname is an empty string
            *data = NULL;
            return FILE_REQUEST;
        }


        *data = malloc(struct_msg->pld_len);
        strcpy((char *) *data, req+6);

        strcpy(struct_msg->nick_sender, nick_name);
        struct_msg->type = FILE_REQUEST;
        strcpy(struct_msg->infos, req+receiver_index+1);

        return FILE_REQUEST;

    }

    if(strcmp(req, "/quit") == 0){

        *data = NULL;
        struct_msg->pld_len = 0;
        strcpy(struct_msg->nick_sender, nick_name);
        struct_msg->type = CLIENT_QUIT;
        strcpy(struct_msg->infos,"");
        return CLIENT_QUIT;
        
    }

    struct_msg->pld_len = strlen(req)+1;
    *data = malloc(struct_msg->pld_len*sizeof(char));
    strcpy(*data, req);
    strcpy(struct_msg->nick_sender, nick_name);

    if(strlen(salon_name) == 0){
        //ECHO_SEND 
        struct_msg->type = ECHO_SEND;
        strcpy(struct_msg->infos, "");
        return ECHO_SEND;
    }else{
        struct_msg->type = MULTICAST_SEND;
        strcpy(struct_msg->infos, salon_name);
        return MULTICAST_SEND;
    }

}