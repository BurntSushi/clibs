#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ds.h"

const int NUM_NAMES = 5;
char* names[] = { "springsteen", "andrew", "plato", "kaitlyn", "cauchy" };

void print_name(void* vname)
{
    char* name;

    name = (char*) vname;
    printf("%s (%d)\n", name, (uint32_t)strlen(name));
}

int32_t mycmp(void *data1, void *data2)
{
    return strcmp((char*) data1, (char*) data2);
}

int
main()
{
    struct DSLinkedList *lst, *lstcopy;
    struct DSListNode *node;
    int i;

    lst = ds_list_create();

    for (i = 0; i < NUM_NAMES; ++i)
        ds_list_append(lst, names[i]);

    printf("length: %d\n", lst->length);
    ds_list_map(lst, print_name);

    for (node = lst->first; node; node = node->next) {
        char* name;

        name = (char*) node->data;

        if (strcmp(name, "cauchy") == 0) {
            ds_list_remove(lst, node);
            ds_list_insert(lst, node->next, name);
            break;
        }
    }

    printf("--------\n");

    ds_list_map(lst, print_name);

    printf("--------\n");

    lstcopy = ds_list_copy(lst);
    ds_list_sort(lst, mycmp);

    ds_list_map(lst, print_name);
    printf("########\n");
    ds_list_map(lstcopy, print_name);

    ds_list_free_no_data(lstcopy);
    ds_list_free_no_data(lst);

    return 0;
}

