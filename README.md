# Particle Swarm Optimization em C

Implementação de Particle Swarm Optimization (PSO) em C, com suporte a domínios N-dimensionais e múltiplas funções de fitness. Totalmente configurável via argumentos de linha de comando ou arquivo INI.

---

## Funcionalidades

- Atualização de velocidade clássica: `v ← w·v + c₁·r₁·(pbest − x) + c₂·r₂·(gbest − x)`
- Atualização assíncrona do melhor global (gbest atualizado imediatamente ao surgir uma melhora)
- Inicialização do pbest pela posição inicial ou por posição aleatória independente (`--pbest-init`)
- Domínio N-dimensional configurável (`--n-dims`)
- Minimização ou maximização da função de fitness
- Funções de fitness disponíveis: `sphere`, `rosenbrock`
- Configuração por arquivo INI, argumentos CLI, ou ambos (CLI tem precedência)
- Execução interativa passo a passo (`--interactive`)
- Exibição do enxame inicial e do melhor global ao fim da execução

---

## Compilação

```bash
make
```

---

## Uso

```
Uso: ./pso [OPCOES]

Opções:
  -F, --config        ARQ    Caminho para arquivo de configuração INI
  -s, --swarm-size    INT    Tamanho do enxame                          [padrão: 30]
  -D, --n-dims        INT    Número de dimensões por partícula          [padrão: 2]
  -n, --iterations    INT    Número de iterações                        [padrão: 100]
  -m, --domain-min    FLOAT  Valor mínimo do domínio                    [padrão: -5.0]
  -M, --domain-max    FLOAT  Valor máximo do domínio                    [padrão: 5.0]
  -d, --direction     STR    minimize | maximize                        [padrão: minimize]
  -f, --fitness       STR    sphere | rosenbrock                        [padrão: sphere]
  -c, --c1            FLOAT  Coeficiente cognitivo                      [padrão: 2.0]
  -C, --c2            FLOAT  Coeficiente social                         [padrão: 2.0]
  -p, --pbest-init    STR    position | random                          [padrão: position]
  -I, --interactive          Executa passo a passo com pausas
  -h, --help                 Exibe esta mensagem e encerra
```

**Exemplo:** minimizar a função de Rosenbrock em 5 dimensões com enxame de 50 partículas:

```bash
./pso --fitness rosenbrock --n-dims 5 --swarm-size 50 --iterations 500
```

---

## Arquivo de configuração

Todos os parâmetros podem ser definidos em um arquivo INI e carregados com `-F`. Argumentos passados diretamente na linha de comando sempre sobrescrevem os valores do arquivo.

```ini
# Exemplo de configuração
swarm_size  = 30
n_dims      = 5
iterations  = 200
direction   = minimize
fitness     = sphere
c1          = 2.0
c2          = 2.0
domain_min  = -5.0
domain_max  = 5.0
pbest_init  = position
```

```bash
./pso --config run.ini
```

Chaves podem ser escritas com hífen ou sublinhado (`swarm-size` e `swarm_size` são equivalentes). Linhas iniciadas com `#` são ignoradas.

---

## Ambiente de desenvolvimento

O repositório inclui um flake Nix com a toolchain C do projeto. Para entrar no ambiente:

```bash
nix develop
```

Se [direnv](https://direnv.net) estiver instalado, o ambiente é ativado automaticamente ao entrar no diretório:

```bash
direnv allow
```

O uso de Nix e direnv é opcional. Para compilar e executar o binário basta ter `gcc` e `make` disponíveis.
