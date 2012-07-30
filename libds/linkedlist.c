#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "linkedlist.h"

/* private helper functions */
static void
ds_list_unlink(struct DSLinkedList *lst, struct DSListNode *remove);

static struct DSListNode * 
ds_list_mergesort(struct DSListNode *head, int32_t (compare)(void*, void*));

static struct DSListNode *
ds_list_merge(struct DSListNode *head1, struct DSListNode *head2,
              int32_t (compare)(void*, void*));

struct DSLinkedList *
ds_list_create()
{
    struct DSLinkedList *lst;

    lst = malloc(sizeof(*lst));
    assert(lst);

    lst->length = 0;
    lst->first = NULL;
    lst->last = NULL;

    return lst;
}

void
ds_list_free(struct DSLinkedList *lst)
{
    struct DSListNode *node, *temp;

    node = lst->first;
    while (node != NULL) {
        temp = node->next;
        free(node->data);
        free(node);
        node = temp;
    }

    free(lst);
}

void
ds_list_free_no_data(struct DSLinkedList *lst)
{
    struct DSListNode *node, *temp;

    node = lst->first;
    while (node != NULL) {
        temp = node->next;
        free(node);
        node = temp;
    }

    free(lst);
}

void
ds_list_prepend(struct DSLinkedList *lst, void *data)
{
    ds_list_insert(lst, NULL, data);
}

void
ds_list_append(struct DSLinkedList *lst, void *data)
{
    ds_list_insert(lst, lst->last, data);
}

void
ds_list_insert(struct DSLinkedList *lst, struct DSListNode *after, void *data)
{
    struct DSListNode *newnode;

    newnode = malloc(sizeof(*newnode));
    assert(newnode);

    newnode->data = data;
    newnode->next = NULL;
    newnode->prev = NULL;

    /* make sure the list obeys invariants */
    assert((lst->first == NULL && lst->last == NULL) ||
           (lst->first != NULL && lst->last != NULL));

    /* If the list is empty, make this node the first and last always */
    if (lst->first == NULL && lst->last == NULL) {
        lst->first = newnode;
        lst->last = newnode;
    }

    /* if after is NULL, make it the first node */
    else if (after == NULL) {
        lst->first->prev = newnode;
        newnode->next = lst->first;
        lst->first = newnode;

        /* if last was also the first, last is still the same */
    }

    /* shortcut: check to see if 'after' is the last node */
    else if (after == lst->last) {
        lst->last->next = newnode;
        newnode->prev = lst->last;
        lst->last = newnode;

        /* if first was also the last, first is still the same */
    }

    /* Find the 'after' node and insert 'newnode' after it in the list. */
    else {
        newnode->next = after->next;
        newnode->prev = after;
        after->next = newnode;

        /* we know after cannot be NULL and cannot be the last node */
        /* if it was first, then this works fine */
    }

    ++lst->length;
}

struct DSLinkedList *
ds_list_copy(struct DSLinkedList *lst)
{
    struct DSLinkedList *copy;
    struct DSListNode *node, *lastnode;

    copy = ds_list_create();

    lastnode = NULL;
    for (node = lst->last; node; node = node->prev) {
        struct DSListNode *newnode;

        newnode = malloc(sizeof(*newnode));
        assert(newnode);

        newnode->data = node->data;
        newnode->next = lastnode;
        newnode->prev = NULL;

        if (node == lst->first)
            copy->first = newnode;
        else if (node == lst->last)
            copy->last = newnode;

        lastnode = newnode;
    }

    /* now fix 'prev' in each of our new nodes */
    lastnode = NULL;
    for (node = copy->first; node; node = node->next) {
        node->prev = lastnode;
        lastnode = node;
    }

    copy->length = lst->length;

    return copy;
}

static void
ds_list_unlink(struct DSLinkedList *lst, struct DSListNode *remove)
{
    if (remove == lst->first && remove == lst->last) {
        lst->first = NULL;
        lst->last = NULL;
    }

    else if (remove == lst->first) {
        lst->first = lst->first->next;
        lst->first->prev = NULL;
    }

    else if (remove == lst->last) {
        lst->last = lst->last->prev;
        lst->last->next = NULL;
    }

    else {
        remove->prev->next = remove->next;
        remove->next->prev = remove->prev;
    }

    --lst->length;
}

void
ds_list_remove(struct DSLinkedList *lst, struct DSListNode *remove)
{
    ds_list_unlink(lst, remove);
    free(remove);
}

void
ds_list_destroy(struct DSLinkedList *lst, struct DSListNode *remove)
{
    ds_list_unlink(lst, remove);

    free(remove->data);
    free(remove);
}

void
ds_list_map(struct DSLinkedList *lst, void (func)(void*))
{
    struct DSListNode *node;

    for (node = lst->first; node; node = node->next)
        func(node->data);
}

/**
 * This implementation of mergesort in place on a linked list is based on
 * http://www.c.happycodings.com/Sorting_Searching/code10.html
 *
 * It is not efficient as possible, namely because I fix the doubly linked
 * attributes (the tail and prev attributes) after the list is sorted.
 */
void
ds_list_sort(struct DSLinkedList *lst, int32_t (compare)(void*, void*))
{
    struct DSListNode *head;

    head = ds_list_mergesort(lst->first, compare);

    /* set the first node of the list */
    lst->first = head;
    lst->first->prev = NULL;

    /* now find the last node and set that */
    /* while we're at it, fix 'prev' in each node */
    while (head->next) {
        head->next->prev = head;
        head = head->next;
    }

    lst->last = head;
    lst->last->next = NULL;
}

struct DSListNode * 
ds_list_mergesort(struct DSListNode *head, int32_t (compare)(void*, void*)) 
{
    struct DSListNode *head1, *head2;

    if (head == NULL || head->next == NULL)
        return head;

    /* cut the list in half */
    head1 = head;
    head2 = head->next;
    while(head2 != NULL && head2->next != NULL) {
        head = head->next;
        head2 = head->next->next;
    }

    head2 = head->next;
    head->next = NULL;

    return ds_list_merge(ds_list_mergesort(head1, compare),
                         ds_list_mergesort(head2, compare),
                         compare);
}

struct DSListNode *
ds_list_merge(struct DSListNode *head1, struct DSListNode *head2,
              int32_t (compare)(void*, void*))
{
    struct DSListNode *head3;

    if (head1 == NULL)
        return head2;

    if (head2 == NULL)
        return head1;

    if (compare(head1->data, head2->data) < 0) {
        head3 = head1;
        head3->next = ds_list_merge(head1->next, head2, compare);
    }

    else {
        head3 = head2;
        head3->next = ds_list_merge(head1, head2->next, compare);
    }

    return head3;
}

