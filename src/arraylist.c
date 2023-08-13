#include "arraylist.h"
#include<stdlib.h>

void inc_capacity(ArrayList* list) {
    list->capacity = (list->capacity*3)/2;
    void** new_data = (void**)realloc(list->data, list-> capacity);
    list->data = new_data;
}

ArrayList al_create(size_t capacity) {
    ArrayList list;
    list.capacity = capacity;
    list.data = (void**) malloc(capacity);
    return list;
}

void al_delete(ArrayList* list) {
    //free each item
    for (size_t i = 0; i < list->len; i++) {
        free(list->data[i]);
    }
    //free the pointer array
    free(list->data);
}

void al_add_item(ArrayList* list, void* item) {
    if (list->capacity <= list->len) {
        inc_capacity(list);
    }

    list->data[list->len] = item;
    list->len++;
}

void al_remove_item(ArrayList* list, size_t index) {
    if (index >= list->len) {
        return; 
    }

    size_t bytes_to_copy = (list->len - index - 1) * sizeof(void*);

    if (bytes_to_copy > 0) {
        memmove(&(list->data[index]), &(list->data[index + 1]), bytes_to_copy);
    }
    list->len--;
}

void* al_get_item(ArrayList* list, size_t index) {
    if (index >= list->len) {
        return NULL;
    }

    return list->data[index];
}

