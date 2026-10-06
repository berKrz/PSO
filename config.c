#include "config.h"
#include "pso.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

// Individual Handlers
static void set_swarm_size(const char *source, const char *key, const char *val, Config *dst) {
  int n = atoi(val);
  if (n <= 0) die_at(source, key, "deve ser um inteiro positivo");
  dst->swarm_size = n;
}

static void set_n_dims(const char *source, const char *key, const char *val, Config *dst) {
  int n = atoi(val);
  if (n <= 0) die_at(source, key, "deve ser um inteiro positivo");
  dst->n_dims = n;
}

static void set_iterations(const char *source, const char *key, const char *val, Config *dst) {
  int n = atoi(val);
  if (n <= 0) die_at(source, key, "deve ser um inteiro positivo");
  dst->iterations = n;
}

static void set_domain_min(const char *source, const char *key, const char *val, Config *dst) {
  (void)source; (void)key;
  dst->domain_min = atof(val);
}

static void set_domain_max(const char *source, const char *key, const char *val, Config *dst) {
  (void)source; (void)key;
  dst->domain_max = atof(val);
}

static void set_direction(const char *source, const char *key, const char *val, Config *dst) {
  if      (strcmp(val, "minimize") == 0) dst->direction = MINIMIZE;
  else if (strcmp(val, "maximize") == 0) dst->direction = MAXIMIZE;
  else die_at(source, key, "deve ser 'minimize' ou 'maximize'");
}

static void set_fitness(const char *source, const char *key, const char *val, Config *dst) {
  static const struct {
    const char *name;
    double (*fn)(double *);
  } table[] = {
    { "sphere",         fitness_sphere },
    { "rosenbrock", fitness_rosenbrock },
    { NULL,     NULL           }
  };
  for (int i = 0; table[i].name; i++) {
    if (strcmp(val, table[i].name) == 0) {
      dst->fitness_fn = table[i].fn;
      return;
    }
  }
  die_at(source, key, "funcao de aptidao desconhecida");
}

static void set_c1(const char *source, const char *key, const char *val, Config *dst) {
  double v = atof(val);
  if (v < 0.0) die_at(source, key, "deve ser nao-negativo");
  dst->c1 = v;
}

static void set_c2(const char *source, const char *key, const char *val, Config *dst) {
  double v = atof(val);
  if (v < 0.0) die_at(source, key, "deve ser nao-negativo");
  dst->c2 = v;
}

static void set_pbest_init(const char *source, const char *key, const char *val, Config *dst) {
  if      (strcmp(val, "position") == 0) dst->pbest_init = PBEST_POSITION;
  else if (strcmp(val, "random")   == 0) dst->pbest_init = PBEST_RANDOM;
  else die_at(source, key, "deve ser 'position' ou 'random'");
}

// Dispatch Table
typedef void (*FieldHandler)(const char *source, const char *key, const char *val, Config *dst);

typedef struct {
  const char  *key;
  FieldHandler handler;
} FieldEntry;

static const FieldEntry fields[] = {
  { "swarm_size",  set_swarm_size  },
  { "n_dims",      set_n_dims      },
  { "iterations",  set_iterations  },
  { "domain_min",  set_domain_min  },
  { "domain_max",  set_domain_max  },
  { "direction",   set_direction   },
  { "fitness",     set_fitness     },
  { "c1",          set_c1          },
  { "c2",          set_c2          },
  { "pbest_init",  set_pbest_init  },
  { NULL,          NULL            }
};

void apply_field(const char *source, const char *key, const char *val, Config *dst) {
  for (int i = 0; fields[i].key; i++) {
    if (strcmp(key, fields[i].key) == 0) {
      fields[i].handler(source, key, val, dst);
      return;
    }
  }
  die_at(source, key, "chave desconhecida");
}
