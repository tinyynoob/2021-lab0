#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
        return NULL;
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    while (q->head) {
        list_ele_t *temp = q->head;
        q->head = q->head->next;
        free(temp->value);
        free(temp);
    }
    free(q);
    // we dont need to set q to NULL since it has no effect outside the function
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q)
        return false;
    list_ele_t *newh = (list_ele_t *) malloc(sizeof(list_ele_t));
    if (!newh)
        return false;
    int s_size = strlen(s) + 1;  // strlen() may not be safe if there is no '\0'
    char *s_in_ele = (char *) malloc(sizeof(char) * s_size);
    if (!s_in_ele) {
        free(newh);
        return false;
    }
    for (int i = 0; i < s_size; i++)  // copy the string including '/0'
        s_in_ele[i] = s[i];
    newh->value = s_in_ele;
    newh->next = q->head;
    q->head = newh;
    if (!q->tail)  // if newh is the only element
        q->tail = newh;
    q->size++;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is false or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q)
        return false;
    list_ele_t *newt = (list_ele_t *) malloc(sizeof(list_ele_t));
    if (!newt)
        return false;
    int s_size = strlen(s) + 1;
    char *s_in_ele = (char *) malloc(sizeof(char) * s_size);
    if (!s_in_ele) {
        free(newt);
        return false;
    }
    for (int i = 0; i < s_size; i++)  // copy the string including '/0'
        s_in_ele[i] = s[i];
    newt->value = s_in_ele;
    newt->next = NULL;
    if (q->tail)
        q->tail->next = newt;
    q->tail = newt;
    if (!q->head)  // if newt is the only element
        q->head = newt;
    q->size++;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->size)
        return false;
    char *it = q->head->value;
    size_t sp_size = 0;
    while (!*it && sp_size < bufsize) {  // compute sp_size
        it++;
        sp_size++;
    }
    if (sp) {  // If sp is non-NULL
        sp = (char *) malloc(sizeof(char) * sp_size);
        if (!sp)
            return false;
        for (size_t i = 0; i < sp_size; i++)  // copy the removed string to sp
            sp[i] = q->head->value[i];
    }
    if (sp_size == bufsize)  // if the maximum is achieved
        sp[bufsize - 1] = '\0';
    free(q->head->value);
    list_ele_t *toDelete = q->head;
    q->head = q->head->next;
    free(toDelete);
    if (!q->head)
        q->tail = NULL;
    q->size--;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (!q)
        return 0;
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || q->size == 0 || q->size == 1)
        return;
    q->tail = q->head;
    list_ele_t *prev = q->head;
    list_ele_t *temp = q->head->next->next;
    q->head = q->head->next;
    q->head->next = prev;
    while (temp) {
        prev = q->head;
        q->head = temp;
        temp = temp->next;
        q->head->next = prev;
    }
    q->tail->next = NULL;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q || !q->size)
        return;
    q->head = sortList(q->head, string_compare);
    list_ele_t *it;
    for (it = q->head; it->next; it = it->next)
        ;
    q->tail = it;
}

list_ele_t *sortList(list_ele_t *head, int (*cmp)(char *, char *))
{
    if (!head || !head->next)
        return head;
    char *pivot = head->value;
    list_ele_t *left = NULL, *right = NULL, *it = head->next;
    while (it) {
        list_ele_t *temp = it->next;
        push(cmp(it->value, pivot) > 0 ? &right : &left, it);
        it = temp;
    }
    left = sortList(left, cmp);
    right = sortList(right, cmp);

    head->next = right;
    if (left) {
        for (it = left; it->next; it = it->next)
            ;
        it->next = head;
        return left;
    } else
        return head;
}

/*
 * compare according to ascii value
 * Next goal: try to find a non-branch solution
 */
int string_compare(char *a, char *b)
{
    while (*a && *b) {
        if (*a < *b)
            return -1;
        else if (*a > *b)
            return 1;
        a++;
        b++;
    }
    // if someone terminates
    if (!*a && !*b)
        return 0;
    else if (!*a)
        return -1;
    else
        return 1;
}