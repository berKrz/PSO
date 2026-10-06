#include "pso.h"
#include "utils.h"
#include <stdlib.h>
#include <math.h>

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

double fitness_rastrigin(double *pos) {
  if (g_cfg.domain_min < -5.12 || g_cfg.domain_max > 5.12)
    die("Para Rastrigin, cada dimensao deve estar no dominio [-5.12, 5.12]");

  double a = 10.0, sum = a * g_cfg.n_dims;

  for (int i = 0; i < g_cfg.n_dims; i++) {
    sum += pos[i] * pos[i] - a * cos(2.0 * M_PI * pos[i]);
  }

  return sum;
}

double fitness_griewank(double *pos) {
  if (g_cfg.domain_min < -600.0 || g_cfg.domain_max > 600.0)
    die("Para Griewank, cada dimensao deve estar no dominio [-600, 600]");

  double sum = 0.0, product = 1.0;

  for (int i = 0; i < g_cfg.n_dims; i++) {
    sum += (pos[i] * pos[i]);
    product *= cos(pos[i] / sqrt((double)(i + 1)));
  }

  return 1.0 + sum / 4000.0 - product;
}

double fitness_ackley(double *pos) {
  if (g_cfg.domain_min < -5.0 || g_cfg.domain_max > 5.0)
    die("Para Ackley, cada dimensao deve estar no dominio [-5.0, 5.0]");

  const double a = 20.0, b = 0.2, c = 2.0 * M_PI;

  double sum_sq = 0.0, sum_cos = 0.0;

  for (int i = 0; i < g_cfg.n_dims; i++) {
    sum_sq += pos[i] * pos[i];
    sum_cos += cos(c * pos[i]);
  }

  return -a * exp(-b * sqrt(sum_sq / g_cfg.n_dims)) - exp(sum_cos / g_cfg.n_dims) + a + exp(1.0);
}

double fitness_eggholder(double *pos) {
  if (g_cfg.n_dims != 2)
    die("Para Eggholder, n_dims precisa ser igual a 2");

  if (g_cfg.domain_min < -512.0 || g_cfg.domain_max > 512.0)
    die("Para Eggholder, cada dimensao deve estar no dominio [-512, 512]");

  double x = pos[0], y = pos[1];

  return -(y + 47.0) * sin(sqrt(fabs(x / 2.0 + (y + 47.0))))
         - x * sin(sqrt(fabs(x - (y + 47.0))));
}

double fitness_schwefel(double *pos) {
  if (g_cfg.domain_min < -500.0 || g_cfg.domain_max > 500.0)
    die("Para Schwefel, cada dimensao deve estar no dominio [-500, 500]");

  const double a = 418.9828872724338;
  double sum = 0.0;

  for (int i = 0; i < g_cfg.n_dims; i++) {
    sum += -pos[i] * sin(sqrt(fabs(pos[i])));
  }

  return a * g_cfg.n_dims + sum;
}

double fitness_schaffer_f6(double *pos) {
  if (g_cfg.n_dims != 2)
    die("Para Schaffer's F6, n_dims precisa ser igual a 2");

  if (g_cfg.domain_min < -100.0 || g_cfg.domain_max > 100.0)
    die("Para Schaffer's F6, cada dimensao deve estar no dominio [-100, 100]");

  double x = pos[0], y = pos[1], r1 = x * x + y * y, r2 = 1.0 + 0.001 * r1;

  return 0.5 + (sin(sqrt(r1)) * sin(sqrt(r1)) - 0.5) / (r2 * r2);
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
