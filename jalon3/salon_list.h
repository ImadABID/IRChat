#ifndef __SALON_LIST_H__
#define __SALON_LIST_H__

#include "common.h"
#include "client_list.h"

struct salon{

    char *name;
    struct client_list *members;

    struct salon *next;
    
};

struct salon_list{
    struct salon *first_salon;

    unsigned int salon_nbr;
};

// Salon
struct salon *salon_new(char *name, struct client *creator);
void salon_free(struct salon *sal);
void salon_recursive_free(struct salon *sal);

// Salon list
struct salon_list *salon_list_new();
void salon_list_free(struct salon_list *salist);

// insert
void salon_list_insert(struct salon_list *salist, struct salon *sal);

//drop client
void salon_drop_client_by_fd(struct salon *sal, int c_fd);

// get salon
struct salon *salon_list_get_salon_by_name(struct salon_list *salist, char *name);

// Get nicknames in an array
void salon_list_names_as_array(struct salon_list *salist, char *buff);

//name check
char salon_list_name_already_used(struct salon_list *salist, char *name);

#endif