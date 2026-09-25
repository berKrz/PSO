#include "config.h"
#include "pso.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

// Individual Handlers
static void set_swarm_size(const char *source, const char *key, const char *val) {
    int n = atoi(val);
    if (n <= 0) die_at(source, key, "deve ser um inteiro positivo");
    g_cfg.swarm_size = n;
}

static void set_n_dims(const char *source, const char *key, const char *val) {
    int n = atoi(val);
    if (n <= 0) die_at(source, key, "deve ser um inteiro positivo");
    g_cfg.n_dims = n;
}

static void set_iterations(const char *source, const char *key, const char *val) {
    int n = atoi(val);
    if (n <= 0) die_at(source, key, "deve ser um inteiro positivo");
    g_cfg.iterations = n;
}

static void set_domain_min(const char *source, const char *key, const char *val) {
    (void)source; (void)key;
    g_cfg.domain_min = atof(val);
}

static void set_domain_max(const char *source, const char *key, const char *val) {
    (void)source; (void)key;
    g_cfg.domain_max = atof(val);
}

static void set_direction(const char *source, const char *key, const char *val) {
    if      (strcmp(val, "minimize") == 0) g_cfg.direction = MINIMIZE;
    else if (strcmp(val, "maximize") == 0) g_cfg.direction = MAXIMIZE;
    else die_at(source, key, "deve ser 'minimize' ou 'maximize'");
}

static void set_fitness(const char *source, const char *key, const char *val) {
    static const struct {
        const char *name;
        double (*fn)(double *, int);
    } table[] = {
        { "sphere", fitness_sphere },
        { NULL,     NULL           }
    };
    for (int i = 0; table[i].name; i++) {
        if (strcmp(val, table[i].name) == 0) {
            g_cfg.fitness_fn = table[i].fn;
            return;
        }
    }
    die_at(source, key, "funcao de aptidao desconhecida");
}

static void set_c1(const char *source, const char *key, const char *val) {
    double v = atof(val);
    if (v < 0.0) die_at(source, key, "deve ser nao-negativo");
    g_cfg.c1 = v;
}

static void set_c2(const char *source, const char *key, const char *val) {
    double v = atof(val);
    if (v < 0.0) die_at(source, key, "deve ser nao-negativo");
    g_cfg.c2 = v;
}

static void set_pbest_init(const char *source, const char *key, const char *val) {
    if      (strcmp(val, "position") == 0) g_cfg.pbest_init = PBEST_POSITION;
    else if (strcmp(val, "random")   == 0) g_cfg.pbest_init = PBEST_RANDOM;
    else die_at(source, key, "deve ser 'position' ou 'random'");
}

// Dispatch Table

typedef void (*FieldHandler)(const char *source, const char *key, const char *val);

typedef struct {
    const char  *key;
    FieldHandler handler;
} FieldEntry;

static const FieldEntry fields[] = {
    { "swarm_size",    set_swarm_size    },
    { "n_dims",        set_n_dims        },
    { "iterations",    set_iterations    },
    { "domain_min",    set_domain_min    },
    { "domain_max",    set_domain_max    },
    { "direction",     set_direction     },
    { "fitness",       set_fitness       },
    { "c1",            set_c1            },
    { "c2",            set_c2            },
    { "pbest_init",    set_pbest_init    },
    { NULL,            NULL              }
};

void apply_field(const char *source, const char *key, const char *val) {
    for (int i = 0; fields[i].key; i++) {
        if (strcmp(key, fields[i].key) == 0) {
            fields[i].handler(source, key, val);
            return;
        }
    }
    die_at(source, key, "chave desconhecida");
}
