#ifndef ARRAY_LIST
#define ARRAY_LIST

#include <stdbool.h>
#include "object.h"

typedef struct Object Object;

struct objectList {
    int numPlayer;
    int size;
    int capacity;
    Object **data;
};

struct objectList *objCreate();

bool objAppend(struct objectList *list, Object *newObj);

bool al_remove(struct objectList *list, int position);

void al_free(struct objectList *list);

void al_print(struct objectList *list);

#endif
