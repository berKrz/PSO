#pragma once

#include "pso.h"

#define LINE_WIDTH 50

void die    (const char *msg);
void die_at (const char *source, const char *key, const char *msg);

void clear_screen           (void);
void wait_for_enter         (void);
void print_separator        (void);
void print_iteration_header (int iter);
void print_particle         (Particle *p, int index, GBest *gbest);
void print_swarm            (Particle *swarm, GBest *gbest);
void print_iteration_line   (int iter, GBest *gbest);
void print_result           (GBest *gbest);
