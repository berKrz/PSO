#include "args.h"
#include "config.h"
#include "ini.h"
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
    "  -f, --fitness <s>         Funcao de aptidao: sphere|rosenbrock|rastrigin|griewank|ackley"
                                 "|eggholder|schwefel|schaffer_f6 (padrao: sphere)\n"
    "  -c, --c1 <f>              Coeficiente cognitivo (padrao: 2.0)\n"
    "  -C, --c2 <f>              Coeficiente social (padrao: 2.0)\n"
    "  -p, --pbest-init <s>      Inicio do pbest: position|random (padrao: position)\n"
    "  -I, --interactive         Modo interativo (pausa a cada iteracao)\n"
    "  -h, --help                Exibe esta mensagem\n"
  );
}

void parse_args(int argc, char **argv) {
  static const struct option long_options[] = {
    { "config",      required_argument, NULL, 'F' },
    { "swarm-size",  required_argument, NULL, 's' },
    { "n-dims",      required_argument, NULL, 'D' },
    { "iterations",  required_argument, NULL, 'n' },
    { "domain-min",  required_argument, NULL, 'm' },
    { "domain-max",  required_argument, NULL, 'M' },
    { "direction",   required_argument, NULL, 'd' },
    { "fitness",     required_argument, NULL, 'f' },
    { "c1",          required_argument, NULL, 'c' },
    { "c2",          required_argument, NULL, 'C' },
    { "pbest-init",  required_argument, NULL, 'p' },
    { "interactive", no_argument,       NULL, 'I' },
    { "help",        no_argument,       NULL, 'h' },
    { NULL,          0,                 NULL,  0  }
  };

  Config      aux_cfg     = {0};
  const char *config_path = NULL;

  struct {
    int swarm_size, n_dims, iterations;
    int domain_min, domain_max;
    int direction, fitness;
    int c1, c2, pbest_init;
  } set = {0};

  opterr = 0;

  int c;
  while ((c = getopt_long(argc, argv, "F:hIs:n:d:D:f:m:M:c:C:p:", long_options, NULL)) != -1) {
    switch (c) {
      case 'F': config_path = optarg;                                                         break;
      case 's': apply_field("cli", "swarm_size",  optarg, &aux_cfg); set.swarm_size  = 1;     break;
      case 'D': apply_field("cli", "n_dims",      optarg, &aux_cfg); set.n_dims      = 1;     break;
      case 'n': apply_field("cli", "iterations",  optarg, &aux_cfg); set.iterations  = 1;     break;
      case 'm': apply_field("cli", "domain_min",  optarg, &aux_cfg); set.domain_min  = 1;     break;
      case 'M': apply_field("cli", "domain_max",  optarg, &aux_cfg); set.domain_max  = 1;     break;
      case 'd': apply_field("cli", "direction",   optarg, &aux_cfg); set.direction   = 1;     break;
      case 'f': apply_field("cli", "fitness",     optarg, &aux_cfg); set.fitness     = 1;     break;
      case 'c': apply_field("cli", "c1",          optarg, &aux_cfg); set.c1          = 1;     break;
      case 'C': apply_field("cli", "c2",          optarg, &aux_cfg); set.c2          = 1;     break;
      case 'p': apply_field("cli", "pbest_init",  optarg, &aux_cfg); set.pbest_init  = 1;     break;
      case 'I': g_cfg.interactive = 1;                                                        break;
      case 'h': print_help(); exit(EXIT_SUCCESS);
      default:  die("opcao invalida");
    }
  }

  if (optind < argc)
    die("argumento inesperado");

  // Load INI first, then overlay CLI values on top
  if (config_path) parse_ini(config_path, &g_cfg);

  if (set.swarm_size)  g_cfg.swarm_size  = aux_cfg.swarm_size;
  if (set.n_dims)      g_cfg.n_dims      = aux_cfg.n_dims;
  if (set.iterations)  g_cfg.iterations  = aux_cfg.iterations;
  if (set.domain_min)  g_cfg.domain_min  = aux_cfg.domain_min;
  if (set.domain_max)  g_cfg.domain_max  = aux_cfg.domain_max;
  if (set.direction)   g_cfg.direction   = aux_cfg.direction;
  if (set.fitness)     g_cfg.fitness_fn  = aux_cfg.fitness_fn;
  if (set.c1)          g_cfg.c1          = aux_cfg.c1;
  if (set.c2)          g_cfg.c2          = aux_cfg.c2;
  if (set.pbest_init)  g_cfg.pbest_init  = aux_cfg.pbest_init;

  if (g_cfg.domain_min >= g_cfg.domain_max)
    die("domain-min deve ser estritamente menor que domain-max");
}
