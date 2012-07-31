/*
 * Example 'simple' demonstrates a contrived sample for using the 'opt'
 * library. Command line arguments are specified using the opt_flag_*
 * functions (bool, int, double and string types are currently supported).
 *
 * If an error occurs while parsing arguments, a usage blurb is echoed to
 * stderr with an error message and the usage information of each paramater.
 * 
 * The '--help' switch is automatically added for you, and it will print
 * the usage information.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "opt.h"

/* A collection of all flag variables. Pointers to the following structure
 * members are passed to the opt_flag_* functions, and values are loaded
 * into the pointers upon parsing. */
struct {
    bool cool;
    int32_t num1;
    int32_t num2;
    double db;
    char *msg;
} flags;

int main(int argc, char **argv)
{
    struct opt_config *conf;
    struct opt_args *args;
    int i;

    conf = opt_config_init();

    /* Add all of the flags to be parsed.
     * All flags require an argument except for bool, which always
     * defaults to false. */
    opt_flag_bool(conf, &flags.cool, "cool", "Whether I'm cool or not.");
    opt_flag_int(conf, &flags.num1, "num1", 1, "The first integer to add.");
    opt_flag_int(conf, &flags.num2, "num2", 2, "The second integer to add.");
    opt_flag_double(conf, &flags.db, "db", 1.5, "The double to multiply by.");
    opt_flag_string(conf, &flags.msg, "msg", "default", "A word to print.");

    /* Now parse the command line options. All non-options are returned. */
    args = opt_config_parse(conf, argc, argv);

    /* Echo the values. If a flag wasn't provided, its default value
     * is automatically loaded. */
    printf("flags.cool: %d\n", flags.cool);
    printf("flags.num1: %d\n", flags.num1);
    printf("flags.num2: %d\n", flags.num2);
    printf("flags.db: %f\n", flags.db);
    printf("flags.msg: %s\n", flags.msg);

    /* Echo all of the arguments. Arguments *must* proceed flags. */
    printf("\n");
    printf("number of args: %d\n", args->nargs);
    for (i = 0; i < args->nargs; i++)
        printf("arg %d: %s\n", i, args->args[i]);

    /* Make valgrind happy. */
    opt_config_free(conf);
    opt_args_free(args);

    return 0;
}
