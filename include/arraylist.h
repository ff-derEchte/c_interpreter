#include<stdlib.h>

#ifndef ARRAYLIST
#define ARRAYLIST

typedef struct {
    void** data;
    size_t capacity;
    size_t len;
} ArrayList;

ArrayList al_create(size_t capacity);

void al_add_item(ArrayList* list, void* item);
void al_remove_item(ArrayList* list, size_t index);
void* al_get_item(ArrayList* list, size_t index);
void al_delete(ArrayList* list);

#endif