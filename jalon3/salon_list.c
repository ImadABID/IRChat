#include <stdlib.h>
#include <string.h>

#include "salon_list.h"

// Salon

struct salon *salon_new(char *name, struct client *creator){

    struct salon *sal = malloc(sizeof(struct salon));

    sal->name = malloc(NICK_LEN * sizeof(char));
    strcpy(sal->name, name);

    sal->members = client_list_new();
    client_list_insert(sal->members, creator);

    return sal;
}

void salon_free(struct salon *sal){
    free(sal->name);
    free(sal->members);
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
    salist->salon_nbr = 0;
    return salist;
}

void salon_list_free(struct salon_list *salist){
    salon_recursive_free(salist->first_salon);
    free(salist);
}


// insert
void salon_list_insert(struct salon_list *salist, struct salon *sal){
    sal->next = salist->first_salon;
    salist->first_salon = sal;
    salist->salon_nbr++;
}

// drop salon
int salon_list_drop_salon(struct salon_list *salist, struct salon *sal){

    struct salon *sal_i = salist->first_salon;
    struct salon *sal_i_prev = NULL;

    // Finding prev sal_i
    while(sal_i != NULL){

        if(sal_i == sal){
            break;
        }

        sal_i_prev = sal_i;
        sal_i = sal_i->next;
    }

    if(sal_i == NULL){
        return -1;
    }

    // deleting salon
    if(sal_i_prev == NULL){
        salist->first_salon = sal_i->next;
    }else{
        sal_i_prev->next = sal_i->next;
    }

    salon_free(sal);

    salist->salon_nbr--;

    return 0;
}

// drop client
int salon_detache_client_by_fd(struct salon *sal, int c_fd){

    return client_list_detache_client_by_fd(sal->members, c_fd);
}

/*
    Drop client from all salons.
    returns :
        - struct salon * salon_from_where_the_client_was_deleted
        - NULL if the client doesn't belong to any salon
*/
struct salon *salon_list_detache_client_by_fd(struct salon_list *salist, int c_fd){
    struct salon *sal = salist->first_salon;
    while(sal != NULL){
        if(salon_detache_client_by_fd(sal, c_fd) != -1){
            return sal;
        }
        sal = sal->next;
    }

    return NULL;
}

// get salon
struct salon *salon_list_get_salon_by_name(struct salon_list *salist, char *name){
    
    struct salon *sal = salist->first_salon;

    while(sal != NULL){
        if(strcmp(sal->name, name) == 0){
            return sal;
        }
        sal = sal->next;
    }

    // No such salon with name = name
    return NULL;
}

//name check
char salon_list_name_already_used(struct salon_list *salist, char *name){

    struct salon *sal = salist->first_salon;

    while(sal != NULL){
        if(strcmp(sal->name, name) == 0){
            return 1;
        }
        sal = sal->next;
    }

    return 0;
}

// Get nicknames in an array
void salon_list_names_as_array(struct salon_list *salist, char *buff){
    int i = 0;
    struct salon *sal = salist->first_salon;
    while(sal != NULL){
        strcpy(buff+i*NICK_LEN, sal->name);
        sal = sal->next;
        i++;
    }
}