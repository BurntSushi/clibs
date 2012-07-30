#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "opt.h"

struct {
    bool cool;
    int32_t num1;
    int32_t num2;
    double db;
    char *msg;
} flags;

struct opt_config * do_options();

int main(int argc, char **argv)
{
    struct opt_config *conf;
    struct opt_args *args;
    int i;

    conf = do_options();

    args = opt_config_parse(conf, argc, argv);

    printf("flags.cool: %d\n", flags.cool);
    printf("flags.num1: %d\n", flags.num1);
    printf("flags.num2: %d\n", flags.num2);
    printf("flags.db: %f\n", flags.db);
    printf("flags.msg: %s\n", flags.msg);

    printf("\n");
    printf("number of args: %d\n", args->nargs);
    for (i = 0; i < args->nargs; i++)
        printf("arg %d: %s\n", i, args->args[i]);

    opt_config_free(conf, args);

    return 0;
}

struct opt_config *
do_options()
{
    struct opt_config *conf;

    conf = opt_config_init();

    opt_flag_bool(conf, &flags.cool, "cool", "Whether I'm cool or not.");
    opt_flag_int(conf, &flags.num1, "num1", 1, "The first integer to add.");
    opt_flag_int(conf, &flags.num2, "num2", 2, "The second integer to add.");
    opt_flag_double(conf, &flags.db, "db", 1.5, "The double to multiply by.");
    opt_flag_string(conf, &flags.msg, "msg", "hello", "A word to print.");

    return conf;
}
