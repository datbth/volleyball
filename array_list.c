//
// Created by bit on 5/8/17.
//

//TODO: create a file named array_list.c, and implement all the following functions in it.
#include <malloc.h>
#include "array_list.h"
#include "object.h"

#define SIZE 10
#define RESIZE_RATIO 1.5

/**
 * Create an array list by allocating a struct array_list and init its contents to an empty list.
 * @return a pointer to a newly allocated struct array_list, or NULL if it cannot be allocated
 */
struct array_list *al_create(){
    struct array_list * arrayList = malloc(sizeof(struct array_list));
    if(arrayList == NULL) {
        return NULL;
    }
    arrayList->capacity = SIZE;
    arrayList->size = 0;
    arrayList->data = malloc(sizeof(Object)*arrayList->capacity);
    if (arrayList->data == NULL){
        free(arrayList);
        return NULL;
    }
    //initialize
    for (int i = 0; i < arrayList->capacity; ++i) {
        arrayList->data[i] = NULL;
    }
    return arrayList;
}

bool al_append(struct array_list *list, Object *newObj){
    // resize
    if(list->size == list->capacity){
        Object **newdata = malloc((size_t) (sizeof(Object*) * list->capacity * RESIZE_RATIO));
        if (newdata == NULL) {
            list->size;
            return false;
        }
        //copy
        for (int i = 0; i < list->capacity; ++i) {
            newdata[i] = list->data[i];
        }
        for (int j = list->capacity; j < list->capacity*RESIZE_RATIO; ++j) {
            newdata[j] = NULL;
        }
        // free the old array and assign new array
        free(list->data);
        list->data = newdata;
        list->capacity *= RESIZE_RATIO;
    }
    //insert
    list->data[list->size] = newObj;
    printf("list->size %i\n", list->size);
    list->size++;
    return true;
}

bool al_remove(struct array_list *list, int position){
    printf("removing");
    //validate
    if(position < 0 || position >= list->size){
        printf("wrong position\n");
        return false;
    }
    // remove value

    //shift all value to the left starting at this position
//    al_print(list);
    for (int i = position; i < list->size - 1; ++i) {
        printf("<%p>", list->data[i]);
        list->data[i] = list->data[i+1];
        list->data[i]->id -= 1;
    }
    list->size--;
//    al_print(list);
    return true;
}

void al_free(struct array_list *list){
    if(list != NULL) {
        if(list->data != NULL){
            free(list->data);
        }
        free(list);
    }
}

void al_print(struct array_list *list){
    for (int i = 0; i < list->size; ++i) {
        printf("object pointer %p id%i \n", list->data[i], list->data[i]->id);
//        printf("object pointer %p \n", list->data[i]);
    }
    printf("\n");
}