#ifndef __LIBOPT_OPT_H__
#define __LIBOPT_OPT_H__

#include <stdbool.h>
#include <stdint.h>

/* The design of this package is greatly inspired by Go's standard library
 * package 'flag'. It doesn't support complex argument parsing, but it's simple
 * to use. */

struct opt_config {
    struct opt_flag **flags;
    int32_t length;
    bool parsed;
    void (*usage)(struct opt_config *conf);
};

struct opt_args {
    int nargs;
    char **args;
};

struct opt_config *
opt_config_init();

void
opt_config_free(struct opt_config *conf, struct opt_args *args);

struct opt_args *
opt_config_parse(struct opt_config *conf, int argc, char **argv);

void
opt_config_print_usage(struct opt_config *conf);

void
opt_config_print_defaults(struct opt_config *conf);

enum opt_flag_types {
    OPT_BOOL,
    OPT_INT,
    OPT_DOUBLE,
    OPT_STRING
};

struct opt_flag {
    bool parsed;
    enum opt_flag_types type;
    char *name;
    char *usage;
    union {
        bool *b;
        int32_t *i;
        double *d;
        char **s;
    } storage;
    union {
        bool b;
        int32_t i;
        double d;
        char *s;
    } defval;
};

void
opt_flag_bool(struct opt_config *conf,
              bool *storage, char *name, char *usage);

void
opt_flag_int(struct opt_config *conf,
             int32_t *storage, char *name, int32_t defval, char *usage);

void
opt_flag_double(struct opt_config *conf,
                double *storage, char *name, double defval, char *usage);

void
opt_flag_string(struct opt_config *conf,
                char **storage, char *name, char *defval, char *usage);

#endif
