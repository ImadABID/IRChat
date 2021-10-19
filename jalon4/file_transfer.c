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

void file_list_add(struct file_list *filiste, char *name){

    struct file *f = malloc(sizeof(struct file));
    

    f->transfer_status = PROPOSED;
    strcpy(f->name, name);

    f->next = filiste->first_file;
    filiste->first_file = f;
    filiste->file_nbr++;
    
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
