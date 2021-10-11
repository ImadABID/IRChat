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
};

// Salon
struct salon *salon_new(char *name, struct client *creator);
void salon_free(struct salon *sal);
void salon_recursive_free(struct salon *sal);

// Salon list
struct salon_list *salon_list_new();
void salon_list_free(struct salon_list *salist);

#endif