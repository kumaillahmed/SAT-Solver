#include "list.h"

#include <assert.h>
#include <stdlib.h>

#include "err.h"
#include "util.h"

/**
 * Struct for encapsulating a single list element.
 */
typedef struct ListItem {
    struct ListItem* next;  // pointer to the next element (NULL if last)
    void* data;             // pointer to the data
} ListItem;

List mkList(void) {
    List res;
    res.head = NULL;
    return res;
}

void clearList(List* s) {
    ListItem* current_element = s->head;
    ListItem* next;

    while (current_element != NULL) {
        // save next pointer of current element
        next = current_element->next;

        // frees current element
        free(current_element);

        // update current element pointer
        current_element = next;
    }

    s->head = NULL;
}

void push(List* s, void* data) {
    // creates memory for new item
    ListItem* new_list_item = malloc(sizeof(ListItem));

    // assigns data pointer
    new_list_item->data = data;

    // pointer to current head of list
    new_list_item->next = s->head;

    // makes new item new head of list
    s->head = new_list_item;
}

void* peek(List* s) {
    if (s->head != NULL) {
        return s->head->data;
    } else {
        return NULL;
    }
}

void pop(List* s) {
    // checks if s->head is not NULL
    if (s->head != NULL) {
        // assigns temp_element current head element of list
        ListItem* temp_element = s->head;
        ListItem* next;

        // assigns next with next element after top element
        next = temp_element->next;
        s->head = next;

        // frees temp_element
        free(temp_element);
    }
}

char isEmpty(List* s) { return (s->head == NULL); }

ListIterator mkIterator(List* list) {
    ListIterator res;
    res.list = list;
    res.prev = NULL;
    res.current = list->head;

    return res;
}

void* getCurr(ListIterator* it) {
    assert(it->current != NULL);
    return it->current->data;
}

void next(ListIterator* it) {
    assert(isValid(it));
    it->prev = it->current;
    it->current = it->current->next;
}

char isValid(ListIterator* it) { return it->current != NULL; }
