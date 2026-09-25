#include "args.h"
#include "config.h"
#include "pso.h"
#include "utils.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_help(void) {
    printf(
        "Uso: pso [opcoes]\n"
        "\n"
        "Opcoes:\n"
        "  -F, --config <arquivo>    Carrega configuracoes de um arquivo INI\n"
        "  -s, --swarm-size <n>      Tamanho do enxame (padrao: 30)\n"
        "  -D, --n-dims <n>          Numero de dimensoes por particula (padrao: 2)\n"
        "  -n, --iterations <n>      Numero de iteracoes (padrao: 100)\n"
        "  -m, --domain-min <f>      Limite inferior do dominio (padrao: -5.0)\n"
        "  -M, --domain-max <f>      Limite superior do dominio (padrao: 5.0)\n"
        "  -d, --direction <s>       Sentido: minimize|maximize (padrao: minimize)\n"
        "  -f, --fitness <s>         Funcao de aptidao: sphere (padrao: sphere)\n"
        "      --c1 <f>              Coeficiente cognitivo (padrao: 2.0)\n"
        "      --c2 <f>              Coeficiente social (padrao: 2.0)\n"
        "      --pbest-init <s>      Inicio do pbest: position|random (padrao: position)\n"
        "  -I, --interactive         Modo interativo (pausa a cada iteracao)\n"
        "  -h, --help                Exibe esta mensagem\n"
    );
}

static void hyphen_to_underscore(char *s) {
    for (; *s; s++)
        if (*s == '-') *s = '_';
}

void parse_args(int argc, char **argv) {
    static const struct option long_options[] = {
        { "swarm-size",  required_argument, NULL, 's' },
        { "n-dims",      required_argument, NULL, 'D' },
        { "iterations",  required_argument, NULL, 'n' },
        { "domain-min",  required_argument, NULL, 'm' },
        { "domain-max",  required_argument, NULL, 'M' },
        { "direction",   required_argument, NULL, 'd' },
        { "fitness",     required_argument, NULL, 'f' },
        { "c1",          required_argument, NULL, 0   },
        { "c2",          required_argument, NULL, 0   },
        { "pbest-init",  required_argument, NULL, 0   },
        { "interactive", no_argument,       NULL, 'I' },
        { "help",        no_argument,       NULL, 'h' },
        { NULL,          0,                 NULL, 0   }
    };

    opterr = 0;

    int c, option_index;
    while ((c = getopt_long(argc, argv, "hIs:n:d:D:f:m:M:", long_options, &option_index)) != -1) {
        switch (c) {
            case 0: {
                char key[64];
                strncpy(key, long_options[option_index].name, sizeof(key) - 1);
                key[sizeof(key) - 1] = '\0';
                hyphen_to_underscore(key);
                apply_field("cli", key, optarg);
                break;
            }
            case 's': apply_field("cli", "swarm_size",  optarg); break;
            case 'D': apply_field("cli", "n_dims",      optarg); break;
            case 'n': apply_field("cli", "iterations",  optarg); break;
            case 'm': apply_field("cli", "domain_min",  optarg); break;
            case 'M': apply_field("cli", "domain_max",  optarg); break;
            case 'd': apply_field("cli", "direction",   optarg); break;
            case 'f': apply_field("cli", "fitness",     optarg); break;
            case 'I': g_cfg.interactive = 1;                     break;
            case 'h': print_help(); exit(EXIT_SUCCESS);
            default:  die("opcao invalida");
        }
    }

    if (optind < argc)
        die("argumento inesperado");
}
