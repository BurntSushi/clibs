#include <stdio.h>

#include "opt.h"

int main(int argc, char **argv)
{
    struct opt_config *conf;
    struct opt_args *args;
    int flag_num, i;

    conf = opt_config_init();
    opt_flag_int(conf, &flag_num, "num", 0, "Usage info for flag 'num'.");
    args = opt_config_parse(conf, argc, argv);

    printf("flag num is: %d\n", flag_num);
    printf("\n");
    printf("number of non-flag arguments: %d\n", args->nargs);
    for (i = 0; i < args->nargs; i++)
        printf("\targ %d: %s\n", i, args->args[i]);

    opt_config_free(conf);
    opt_args_free(args);

    return 0;
}
