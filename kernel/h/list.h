#ifndef LIST_H
#define LIST_H

#include <types.h>

typedef struct element element_t;
struct element
{
    void*    data;
    element_t* next;
};

typedef struct {
    element_t* begin;
    element_t* current;
} ring_t;

 

#endif
