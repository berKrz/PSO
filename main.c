#include "pso.h"
#include "utils.h"
#include "args.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv) {
    g_cfg = config_default();
    parse_args(argc, argv);
    srand((unsigned)time(NULL) ^ (unsigned)getpid());
    clear_screen();

    Particle *swarm = malloc(g_cfg.swarm_size * sizeof(Particle));
    GBest     gbest;

    init_swarm(swarm, &gbest);

    printf("Enxame Inicial\n\n");
    print_swarm(swarm, &gbest);
    wait_for_enter();
    print_separator();

    for (int i = 0; i < g_cfg.iterations; i++) {
        update_swarm(swarm, &gbest);

        if (g_cfg.interactive) {
            print_iteration_header(i);
            print_swarm(swarm, &gbest);
            wait_for_enter();
            print_separator();
        } else {
            print_iteration_line(i, &gbest);
        }
    }

    print_result(&gbest);

    free_swarm(swarm);
    free(swarm);
    free_gbest(&gbest);
    return EXIT_SUCCESS;
}
