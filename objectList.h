#ifndef ARRAY_LIST
#define ARRAY_LIST

#include <stdbool.h>
#include "object.h"

struct objectList {
    int numPlayer;
    int size;
    int capacity;
    Object **data;
};

/**
 * Create object with a list of attributes
 * @return objectList if create success / NULL if not create successfully
 */
struct objectList *objCreate();

/**
 * Add newObject to objectList
 * @param list Choose a list to add
 * @param newObj An object to add in the list
 * @return true if add successfully / false if not add successfully
 */
bool objAppend(struct objectList *list, Object *newObj);

/**
 * Remove an object at the position in the list
 * @param list Choose a list to remove an object
 * @param position The position of object to remove
 * @return true if remove successfully / false if not remove successfully
 */
bool al_remove(struct objectList *list, int position);

/**
 *  Remove item from the object list
 * @param item Choose item to remove
 * @param objects pointer to object need to be removed from object list
 */
void item_removeFromList(Item *item, struct objectList *objects);

/**
 * free object
 * @param list pointer to object list
 */
void al_free(struct objectList *list);

/**
 *
 * @param list
 */
void al_print(struct objectList *list);

#endif
