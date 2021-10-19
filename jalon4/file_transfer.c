#include "file_transfer.h"

#include <stdlib.h>
#include <string.h>

// init

struct file_list *file_list_init(){
    struct file_list *filiste = malloc(sizeof(struct file_list));
    filiste->file_nbr = 0;
    filiste->first_file = NULL;
    return filiste;
}

int file_list_assigne_id(struct file_list *filiste, char *name){

    static unsigned int id_ref = 0;

    struct file *f = malloc(sizeof(struct file));
    
    f->id = id_ref++;
    f->transfer_status = PROPOSED;
    strcpy(f->name, name);

    f->next = filiste->first_file;
    filiste->first_file = f;
    filiste->file_nbr++;

    return f->id;
}

// Free

void file_free(struct file *f){
    free(f);
}

void file_recursif_free(struct file *f){
    if(f != NULL){
        file_recursif_free(f->next);
        file_free(f);
    }
}

void list_file_free(struct file_list *filiste){
    file_recursif_free(filiste->first_file);
    free(filiste);
}
