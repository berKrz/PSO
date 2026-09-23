#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void die(const char *msg) {
    fprintf(stderr, "Erro: %s\n", msg);
    fprintf(stderr, "Execute com --help para ver as opcoes.\n");
    exit(EXIT_FAILURE);
}

void die_at(const char *source, const char *key, const char *msg) {
    if (key)
        fprintf(stderr, "Erro em '%s' [%s]: %s\n", source, key, msg);
    else
        fprintf(stderr, "Erro em '%s': %s\n", source, msg);
    fprintf(stderr, "Execute com --help para ver as opcoes.\n");
    exit(EXIT_FAILURE);
}

void clear_screen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void wait_for_enter(void) {
    if (!g_cfg.interactive) return;
    printf("\n  [Enter para continuar...]\n");
    while (getchar() != '\n');
}

void print_separator(void) {
    printf("\n");
    for (int i = 0; i < LINE_WIDTH / 2; i++) printf("- ");
    printf("\n\n");
}

void print_iteration_header(int iter) {
    char buf[32];
    int  n = snprintf(buf, sizeof(buf), "=== Iteracao %d ", iter);
    printf("%s", buf);
    for (int i = n; i < LINE_WIDTH; i++) printf("=");
    printf("\n\n");
}

static void print_vec(double *v) {
    printf("[ ");
    for (int d = 0; d < g_cfg.n_dims; d++)
        printf("%7.3f ", v[d]);
    printf("]");
}

void print_particle(Particle *p, int index, GBest *gbest) {
    int is_gbest_holder = (p->pbest_fitness == gbest->fitness);

    if (is_gbest_holder)
        printf(">> Particula %2d", index);
    else
        printf("   Particula %2d", index);

    if (p->pbest_updated)
        printf("   (*)\n");
    else
        printf("\n");

    printf("     pos: ");
    print_vec(p->pos);
    printf("\n");

    printf("     vel: ");
    print_vec(p->vel);
    printf("\n");

    printf("     fitness = %g   pbest = %g\n\n", p->fitness, p->pbest_fitness);
}

void print_swarm(Particle *swarm, GBest *gbest) {
    for (int i = 0; i < g_cfg.swarm_size; i++)
        print_particle(&swarm[i], i, gbest);
    printf("gbest = %g\n", gbest->fitness);
}

void print_iteration_line(int iter, GBest *gbest) {
    printf("Iteracao %3d   gbest = %g\n", iter, gbest->fitness);
}

void print_result(GBest *gbest) {
    for (int i = 0; i < LINE_WIDTH; i++) printf("=");
    printf("\n");

    char rbuf[32];
    int  rn = snprintf(rbuf, sizeof(rbuf), "=== Resultado Final ");
    printf("%s", rbuf);
    for (int i = rn; i < LINE_WIDTH; i++) printf("=");
    printf("\n\n");

    printf("Melhor global:\n");
    printf("   pos: [ ");
    for (int d = 0; d < g_cfg.n_dims; d++)
        printf("%7.3f ", gbest->pos[d]);
    printf("]\n");
    printf("   fitness = %g\n\n", gbest->fitness);

    for (int i = 0; i < LINE_WIDTH; i++) printf("=");
    printf("\n");
}
