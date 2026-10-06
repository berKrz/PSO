#include "pso.h"
#include "utils.h"
#include <stdlib.h>

Config g_cfg;

Config config_default(void) {
  return (Config){
    .swarm_size  = 30,
    .n_dims      = 2,
    .iterations  = 100,
    .w           = 1.0,
    .c1          = 2.0,
    .c2          = 2.0,
    .domain_min  = -5.0,
    .domain_max  =  5.0,
    .direction   = MINIMIZE,
    .pbest_init  = PBEST_POSITION,
    .interactive = 0,
    .fitness_fn  = fitness_sphere,
  };
}

double fitness_sphere(double *pos) {
  double sum = 0.0;
  for (int i = 0; i < g_cfg.n_dims; i++)
    sum += pos[i] * pos[i];
  return sum;
}

double fitness_rosenbrock(double *pos) {
  if (g_cfg.n_dims < 2) die("Para Rosenbrock, n_dims precisa ser maior que 1");
  double sum = 0.0;
  for (int i = 0; i < g_cfg.n_dims-1; i++) {
    double t1 = pos[i+1] - pos[i] * pos[i];
    double t2 = 1 - pos[i];
    sum += 100.0 * t1 * t1 + t2 * t2;
  }
  return sum;
}

static int is_better(double a, double b) {
  return g_cfg.direction == MINIMIZE ? a < b : a > b;
}

static double rand_double(double lo, double hi) {
  return lo + ((double)rand() / RAND_MAX) * (hi - lo);
}

static void copy_pos(double *dst, double *src) {
  for (int d = 0; d < g_cfg.n_dims; d++)
    dst[d] = src[d];
}

void init_swarm(Particle *swarm, GBest *gbest) {
  gbest->pos     = malloc(g_cfg.n_dims * sizeof(double));
  gbest->fitness = g_cfg.direction == MINIMIZE ? 1e300 : -1e300;

  for (int i = 0; i < g_cfg.swarm_size; i++) {
    swarm[i].pos           = malloc(g_cfg.n_dims * sizeof(double));
    swarm[i].vel           = malloc(g_cfg.n_dims * sizeof(double));
    swarm[i].pbest         = malloc(g_cfg.n_dims * sizeof(double));
    swarm[i].pbest_updated = 0;

    for (int d = 0; d < g_cfg.n_dims; d++) {
      swarm[i].pos[d] = rand_double(g_cfg.domain_min, g_cfg.domain_max);
      swarm[i].vel[d] = 0.0;
    }

    swarm[i].fitness = g_cfg.fitness_fn(swarm[i].pos);

    if (g_cfg.pbest_init == PBEST_POSITION) {
      copy_pos(swarm[i].pbest, swarm[i].pos);
      swarm[i].pbest_fitness = swarm[i].fitness;
    } else {
      for (int d = 0; d < g_cfg.n_dims; d++)
        swarm[i].pbest[d] = rand_double(g_cfg.domain_min, g_cfg.domain_max);
      swarm[i].pbest_fitness = g_cfg.fitness_fn(swarm[i].pbest);
    }

    if (is_better(swarm[i].pbest_fitness, gbest->fitness)) {
      gbest->fitness = swarm[i].pbest_fitness;
      copy_pos(gbest->pos, swarm[i].pbest);
    }
  }
}

void update_swarm(Particle *swarm, GBest *gbest) {
  for (int i = 0; i < g_cfg.swarm_size; i++) {
    swarm[i].pbest_updated = 0;

    for (int d = 0; d < g_cfg.n_dims; d++) {
      double r1 = (double)rand() / RAND_MAX;
      double r2 = (double)rand() / RAND_MAX;

      swarm[i].vel[d] = g_cfg.w  * swarm[i].vel[d]
                        + g_cfg.c1 * r1 * (swarm[i].pbest[d] - swarm[i].pos[d])
                        + g_cfg.c2 * r2 * (gbest->pos[d]     - swarm[i].pos[d]);

      swarm[i].pos[d] += swarm[i].vel[d];

      if (swarm[i].pos[d] < g_cfg.domain_min) {
        swarm[i].pos[d] = g_cfg.domain_min;
        swarm[i].vel[d] = 0.0;
      } else if (swarm[i].pos[d] > g_cfg.domain_max) {
        swarm[i].pos[d] = g_cfg.domain_max;
        swarm[i].vel[d] = 0.0;
      }
    }

    swarm[i].fitness = g_cfg.fitness_fn(swarm[i].pos);

    if (is_better(swarm[i].fitness, swarm[i].pbest_fitness)) {
      swarm[i].pbest_fitness = swarm[i].fitness;
      copy_pos(swarm[i].pbest, swarm[i].pos);
      swarm[i].pbest_updated = 1;
    }

    // Async gbest update: gbest is updated as soon as any particle improves it
    if (is_better(swarm[i].pbest_fitness, gbest->fitness)) {
      gbest->fitness = swarm[i].pbest_fitness;
      copy_pos(gbest->pos, swarm[i].pbest);
    }
  }
}

void free_swarm(Particle *swarm) {
  for (int i = 0; i < g_cfg.swarm_size; i++) {
    free(swarm[i].pos);
    free(swarm[i].vel);
    free(swarm[i].pbest);
  }
}

void free_gbest(GBest *gbest) {
  free(gbest->pos);
}
