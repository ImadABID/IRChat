#include <string.h>

#include "req_reader.h"

enum msg_type req_reader(char *req, struct message *struct_msg, void **data){
    // /nick, /who, /whois , /msgall , /msg, /quit

    if(strncmp( req, "/nick ", 6) == 0){

    }

    if(strncmp( req, "/who ", 5) == 0){
        
    }

    if(strncmp( req, "/whois ", 7) == 0){
        
    }

    if(strncmp( req, "/msgall ", 8) == 0){
        
    }

    if(strncmp( req, "/msg ", 5) == 0){
        
    }

    if(strncmp( req, "/quit", 5) == 0){
        *data = NULL;
        struct_msg->pld_len = 0;
        strcpy(struct_msg->nick_sender,"");
        struct_msg->type = CLIENT_QUIT;
        strcpy(struct_msg->infos,"");
        return CLIENT_QUIT;
    }

    return UKNOWN;

}