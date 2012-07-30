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

char* name_tostring(void* vname)
{
    return (char*) vname;
}

int32_t namecmp(void* vk1, void* vk2)
{
    char *val1, *val2;

    val1 = ds_hashmap_get_key((struct DSHashKey *) vk1);
    val2 = ds_hashmap_get_key((struct DSHashKey *) vk2);

    return strcmp((char*) val1, (char*) val2);
}

int
main()
{
    struct DSHashMap *hash;

    hash = ds_hashmap_create();

    ds_puti(hash, 8213, names[10]);
    ds_puts(hash, "snot", names[0]);
    ds_puti(hash, 921, names[5]);
    ds_puts(hash, "booger", names[1]);
    ds_puti(hash, 1234, names[20]);

    print_name(ds_gets(hash, "snot"));
    print_name(ds_gets(hash, "booger"));
    print_name(ds_geti(hash, 1234));

    printf("\nKEYS--------\n");
    ds_hashmap_print_keyvals(hash, name_tostring);

    ds_hashmap_sort_keys(hash);

    printf("\nKEYS--------\n");
    ds_hashmap_print_keyvals(hash, name_tostring);

    ds_hashmap_sort_by(hash, namecmp);

    printf("\nKEYS--------\n");
    ds_hashmap_print_keyvals(hash, name_tostring);

    ds_hashmap_remove_str(hash, "booger", false, false);
    ds_hashmap_remove_int(hash, 12344, false);

    printf("\nKEYS--------\n");
    ds_hashmap_print_keyvals(hash, name_tostring);

    ds_hashmap_free(hash, false, false);

    return 0;
}

