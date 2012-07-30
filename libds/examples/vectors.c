#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ds.h"

#define NUM_NAMES 22
char* names[] = {
    "andrew", "bob", "sally", "billy", "kaitlyn", "springsteen",
    "cauchy", "plato", "darlene", "jenny", "lauren", "barry",
    "brennan", "smalls", "dobes", "pipes", "sarah", "kayla",
    "jack", "bruce", "lorelei", "mickey",
    "SENTINEL"
};

void print_name(void* vname)
{
    printf("%s\n", (char*) vname);
}

uint32_t stricmp(const char *p1, const char *p2)
{
    register unsigned char *s1 = (unsigned char *) p1;
    register unsigned char *s2 = (unsigned char *) p2;
    unsigned char c1, c2;

    do {
        c1 = (unsigned char) toupper((uint32_t) *s1++);
        c2 = (unsigned char) toupper((uint32_t) *s2++);

        if (c1 == '\0')
            return c1 - c2;
    } while (c1 == c2);

    return c1 - c2;
}

int32_t namecmp(void* vname1, void* vname2)
{
    return stricmp((char*) vname1, (char*) vname2);
}

int
main()
{
    struct DSVector *vec, *vec2;
    int32_t i;

    vec = ds_vector_create_capacity(10);

    printf("Size: %d, Capacity: %d\n", vec->size, vec->capacity);

    for (i = 0; strcmp(names[i], "SENTINEL") != 0; ++i)
        ds_vector_append(vec, names[i]);

    printf("Size: %d, Capacity: %d\n", vec->size, vec->capacity);

    ds_vector_insert(vec, "DAVID", vec->size - 10);
    /* ds_vector_remove(vec, ds_vector_find(vec, "DAVID", namecmp)); */

    vec2 = ds_vector_copy(vec);
    ds_vector_sort(vec, namecmp);

    printf("Size: %d, Capacity: %d\n", vec->size, vec->capacity);
    printf("Size: %d, Capacity: %d\n", vec2->size, vec2->capacity);

    ds_vector_map(vec, print_name);
    printf("----------------------\n");
    ds_vector_map(vec2, print_name);

    ds_vector_free_no_data(vec);
    ds_vector_free_no_data(vec2);

    return 0;
}

