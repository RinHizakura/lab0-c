#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

// For replacing 'strcpy'
#ifndef strlcpy
#define strlcpy(dst, src, sz) snprintf((dst), (sz), "%s", (src))
#endif

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    /* TODO: What if malloc returned NULL? */
    if (q == NULL)
        return NULL;

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (q == NULL)
        return;

    /* TODO: How about freeing the list elements and the strings? */
    while (q->head != NULL) {
        list_ele_t *tmp = q->head;
        q->head = q->head->next;
        // After update the queue header, free the list element
        free(tmp->value);
        free(tmp);
    }
    /* Free queue structure */
    free(q);
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
    list_ele_t *newh;
    /* TODO: What should you do if the q is NULL? */
    newh = malloc(sizeof(list_ele_t));
    if (newh == NULL)
        return false;
    /* Don't forget to allocate space for the string and copy it */
    char *tmp = malloc(sizeof(char) * (strlen(s) + 1));
    if (tmp == NULL) {
        // remember to release newh!!
        free(newh);
        return false;
    }

    newh->value = tmp;
    strlcpy(newh->value, s, (strlen(s) + 1));
    newh->next = q->head;
    q->head = newh;
    // If queue is empty, insert head also should setup tail
    if (q->size == 0)
        q->tail = newh;

    q->size++;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    /* TODO: You need to write the complete code for this function */
    /* Remember: It should operate in O(1) time */
    list_ele_t *newh;
    newh = malloc(sizeof(list_ele_t));
    if (newh == NULL)
        return false;

    char *tmp = malloc(sizeof(char) * (strlen(s) + 1));
    if (tmp == NULL) {
        // remember to release newh!!
        free(newh);
        return false;
    }

    newh->value = tmp;
    strlcpy(newh->value, s, (strlen(s) + 1));

    // Stupid implement, but we can just make it correct first
    if (q->size == 0) {
        newh->next = NULL;
        q->tail = newh;
        q->head = newh;
    } else {
        newh->next = NULL;
        q->tail->next = newh;
        q->tail = newh;
    }

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
    /* TODO: You need to fix up this code. */
    if (sp == NULL || q->head == NULL)
        return false;

    if (bufsize > strlen(q->head->value)) {
        strncpy(sp, q->head->value, strlen(q->head->value));
        sp[strlen(q->head->value)] = '\0';
    } else {
        strncpy(sp, q->head->value, bufsize);
        sp[bufsize] = '\0';
    }

    list_ele_t *tmp = q->head;
    q->head = q->head->next;
    // After update the queue header, free the list element
    free(tmp->value);
    free(tmp);

    q->size--;
    if (q->size == 0)
        q->tail = NULL;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    /* TODO: You need to write the code for this function */
    /* Remember: It should operate in O(1) time */
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
    if ((q == NULL) || (q->size == 0))
        return;

    list_ele_t *cur = q->head;
    list_ele_t *next = q->head->next;
    list_ele_t *tmp;

    q->tail = q->head;

    while (next != NULL) {
        tmp = next->next;
        next->next = cur;
        cur = next;
        next = tmp;
    }

    q->tail->next = NULL;
    q->head = cur;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
list_ele_t *sorted_merge(list_ele_t *a, list_ele_t *b)
{
    if (a == NULL)
        return b;
    else if (b == NULL)
        return a;

    list_ele_t *result = NULL;
    if (a->value[0] <= b->value[0]) {
        result = a;
        result->next = sorted_merge(a->next, b);
    } else {
        result = b;
        result->next = sorted_merge(a, b->next);
    }

    return result;
}

void front_back_split(list_ele_t *head,
                      list_ele_t **front_ref,
                      list_ele_t **back_ref)
{
    // if length is less than 2
    if (head == NULL || head->next == NULL) {
        *front_ref = head;
        *back_ref = NULL;
        return;
    }

    list_ele_t *slow = head;
    list_ele_t *fast = head->next;

    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *front_ref = head;
    *back_ref = slow->next;
    slow->next = NULL;
}
void merge_sort(list_ele_t **head)
{
    if (*head == NULL || (*head)->next == NULL)
        return;

    list_ele_t *a;
    list_ele_t *b;

    front_back_split(*head, &a, &b);

    merge_sort(&a);
    merge_sort(&b);

    *head = sorted_merge(a, b);
}

void q_sort(queue_t *q)
{
    merge_sort(&q->head);

    // O(n) update for tail
    // Not a good way! But just leave this here
    list_ele_t *tmp = q->head;
    while (tmp->next != NULL)
        tmp = tmp->next;

    q->tail = tmp;
}
