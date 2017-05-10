#ifndef ARRAY_LIST
#define ARRAY_LIST

#include <stdbool.h>
#include "object.h"

//TODO: implement an array list type (also called a dynamic array) according to the following API.
/* Refer to the dynamic array PDF file for more details about what is an array list.
 * The elements inserted in the array list should be of type double.
 *
 * You need to validate the arguments passed to the functions to avoid crashes
 * (such as segmentation faults). If there is no other choice, then you can use
 * assertions.
 *
 * Do NOT modify this file, EXCEPT for completing the struct array_list definition.
 *
 * Marking:
 * struct array_list: 3 marks
 * al_create: 3 marks
 * al_append: 6 marks
 * al_remove: 6 marks
 * al_free: 3 marks
 * al_print: 3 marks
 * test_array_list: 6 marks
 */

typedef struct Object Object;

struct array_list {
    //TODO: put the fields you need here
    int size;
    int capacity;
    Object **data;
};

//TODO: create a file named array_list.c, and implement all the following functions in it.
/**
 * Create an array list by allocating a struct array_list and init its contents to an empty list.
 * @return a pointer to a newly allocated struct array_list, or NULL if it cannot be allocated
 */
struct array_list *al_create();

/**
 * Insert a value at a given position in the array list.
 * @param list insert a value in this list
 * @param position insert a value at this position (it can insert at the end, 1 position after
 *          the last element, and it can insert before the end by moving elements to the right
 *          to make space)
 * @param newObj the value to insert
 * @return true on successful insert, or false if the value cannot be inserted at this position
 */
bool al_append(struct array_list *list, Object *newObj);

/**
 * Remove a value at a given position from the array list.
 * @param list remove a value from this list
 * @param position remove a value from this position (it cannot leave any position empty, so
 *          it might need to move elements to the left to avoid leaving an empty position)
 * @param value the value removed from the list is saved here
 * @return true on successful remove, or false if the value cannot be removed from this position
 */
bool al_remove(struct array_list *list, int position);

/**
 * Free all the memory used by an array list.
 * @param list the list to free
 */
void al_free(struct array_list *list);

void al_print(struct array_list *list);

#endif
