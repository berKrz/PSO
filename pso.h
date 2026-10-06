#pragma once

typedef enum { MINIMIZE, MAXIMIZE } OptDirection;
typedef enum { PBEST_POSITION, PBEST_RANDOM } PbestInit;

typedef struct {
  double *pos;
  double *vel;
  double *pbest;
  double  pbest_fitness;
  double  fitness;
  int     pbest_updated;
} Particle;

typedef struct {
  double *pos;
  double  fitness;
} GBest;

typedef struct {
  int          swarm_size;
  int          n_dims;
  int          iterations;
  double       w;
  double       c1;
  double       c2;
  double       domain_min;
  double       domain_max;
  OptDirection direction;
  PbestInit    pbest_init;
  int          interactive;

  double (*fitness_fn)(double *pos);
} Config;

extern Config g_cfg;

Config config_default     (void);
double fitness_sphere     (double *pos);
double fitness_rosenbrock (double *pos);
double fitness_rastrigin  (double *pos);
void   init_swarm         (Particle *swarm, GBest *gbest);
void   update_swarm       (Particle *swarm, GBest *gbest);
void   free_swarm         (Particle *swarm);
void   free_gbest         (GBest *gbest);
