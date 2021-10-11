#include <stdlib.h>
#include <string.h>

#include "salon_list.h"

// Salon

struct salon *salon_new(char *name, struct client *creator){

    struct salon *sal = malloc(sizeof(struct salon));

    sal->name = malloc(NICK_LEN * sizeof(char));
    strcpy(sal->name, name);

    sal->members = client_list_new();
    sal->members->first_client = creator;

    return sal;
}

void salon_free(struct salon *sal){
    free(sal->name);
    client_list_free(sal->members);
}

void salon_recursive_free(struct salon *sal){
    if(sal != NULL){
        salon_recursive_free(sal->next);
        salon_free(sal);
    }
}

// Salon list

struct salon_list *salon_list_new(){
    struct salon_list *salist = malloc(sizeof(struct salon_list));
    salist->first_salon = NULL;

    return salist;
}

void salon_list_free(struct salon_list *salist){
    salon_recursive_free(salist->first_salon);
    free(salist);
}