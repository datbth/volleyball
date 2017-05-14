//
// Created by bit on 5/8/17.
//

//#include <malloc.h>
#include "objectList.h"
#include "object.h"

#define SIZE 10
#define RESIZE_RATIO 1.5

struct objectList *objCreate(){
    struct objectList * objectList = malloc(sizeof(struct objectList));
    if(objectList == NULL) {
        return NULL;
    }
    objectList->capacity = SIZE;
    objectList->size = 0;
    objectList->numPlayer = 0;
    objectList->data = malloc(sizeof(Object)*objectList->capacity);
    if (objectList->data == NULL){
        free(objectList);
        return NULL;
    }
    //initialize
    for (int i = 0; i < objectList->capacity; ++i) {
        objectList->data[i] = NULL;
    }
    return objectList;
}

bool objAppend(struct objectList *list, Object *newObj){
    // resize
    if(list->size == list->capacity){
        Object **newdata = malloc((size_t) (sizeof(Object*) * list->capacity * RESIZE_RATIO));
        if (newdata == NULL) {
//            list->size;
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
//    printf("list->size %i\n", list->size);
    list->size++;
    return true;
}

bool al_remove(struct objectList *list, int position){
//    printf("removing");
    //validate
    if(position < 0 || position >= list->size){
//        printf("wrong position\n");
        return false;
    }

    //shift all value to the left starting at this position
//    al_print(list);
    for (int i = position; i < list->size - 1; ++i) {
//        printf("<%p>", list->data[i]);
        list->data[i] = list->data[i+1];
        list->data[i]->id -= 1;
    }
    list->size--;
//    al_print(list);
    return true;
}

void al_free(struct objectList *list){
    if(list != NULL) {
        if(list->data != NULL){
            free(list->data);
        }
        free(list);
    }
}

void al_print(struct objectList *list){
    printf("size %i/%icapacity \n", list->size, list->capacity);
    for (int i = 0; i < list->capacity; ++i) {
        Object * currentObj = list->data[i];
        if (currentObj){
            printf("object pointer %p id%i type%i\n", currentObj, currentObj->id, currentObj->type);
//        printf("object pointer %p \n", list->data[i]);
        }
    }
    printf("\n");
}
