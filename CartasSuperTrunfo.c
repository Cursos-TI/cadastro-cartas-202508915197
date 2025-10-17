// Super Trunfo C - Jogo das Cidades

// Regras:
// 1. O jogo é jogado entre dois jogadores ou contra o computador.
// 2. Cada jogador começa com 5 cartas.
// 3. Em cada rodada, um jogador escolhe uma característica para comparar.
// 4. O jogador com a maior (ou menor) característica ganha a rodada e leva a carta do oponente.
// 5. O jogo termina quando um jogador fica sem cartas.
// 6. O jogador com mais cartas no final é o vencedor.
// 7. Estatísticas de vitórias, empates e jogos são mantidas entre partidas.
// 8. O jogo suporta salvar e carregar cartas de um arquivo binário.

// Bibliotecas necessarias
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
// Bibliotecas necessarias para as cores (#ifdef _WIN32 - #include <windows.h>)
// Variáveis globais, constantes e tipos declarados: 
// Constantes do programa para escabilidade
#define MAX_CARTAS 100
#define MAX_JOGADORES 2
typedef struct Jogador Jogador;
typedef struct Carta Carta;
typedef struct Estatisticas Estatisticas;

#define CARTAS_POR_JOGADOR 5
#define ARQUIVO_CARTAS "cartas.bin"

// Estrutura que representa uma carta do jogo.
// Cada carta contém atributos originais e campos derivados
// (densidade, PIB per capita e super_poder).
typedef struct Carta {
    char estado;                // Letra do estado (A-Z)
    char codigo[5];             // Código da carta (ex: A01)
    char nome_cidade[50];       // Nome da cidade
    int populacao;              // População da cidade
    float area;                 // Área em km²
    float pib;                  // PIB em bilhões de reais
    int num_pontos_turisticos;  // Número de pontos turísticos
    float densidade_populacional; // População / área (derivado)
    float pib_per_capita;        // (pib * 1e9) / populacao (derivado)
    float super_poder;           // valor normalizado (0-100) (derivado)
} Carta;

// Estrutura genérica para representar o estado de um jogador (humano ou computador).
typedef struct Jogador {
    Carta cartas[CARTAS_POR_JOGADOR]; // cartas em mãos
    int vitorias;                     // vitórias na partida atual
    int empates;                      // empates na partida atual
    int cartas_restantes;             // quantas cartas ainda tem
} Jogador;
typedef struct Estatisticas {
    int jogos_jogados;                // partidas iniciadas
    int vitorias[MAX_JOGADORES];      // vitórias por jogador
    int empates;                      // empates entre partidas
} Estatisticas;

// Estrutura para armazenar estatísticas gerais entre partidas contra computador.
typedef struct {
    int jogos_jogados;                // partidas iniciadas
    int vitorias[2];                  // vitórias por jogador (0=humano, 1=computador)
    int empates;                      // empates entre partidas
} EstatisticasComputador;

// Funções de cor no terminal (compatível Windows / Unix)
// Implementação específica por plataforma:

#ifdef _WIN32
#include <windows.h>
// set_color: define cor no console Windows
void set_color(int color) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color); }
// reset_color: restaura cor padrão no Windows
void reset_color() { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); }
#else
// set_color: envia código ANSI para terminal Unix-like
void set_color(int color) { printf("\033[%dm", color); }
// reset_color: reseta cor ANSI
void reset_color() { printf("\033[0m"); }
#endif

// Function prototypes
void embaralhar_cartas(Carta *cartas, int n);
void distribuir_cartas(Carta *baralho, int n_cartas, Jogador *jogadores, int modo_computador);
void exibe_menu_batalha(void);
void exibir_cartas_jogador(const Jogador *j, int jogador_id);
void exibir_cartas_jogador_computador(const Jogador *j, int jogador_id, int eh_computador);
void limpar_buffer_stdin(void);
void remover_carta(Jogador *j, int idx);
void exibir_resultado_turno(float sp1, float sp2, int *v1, int *v2, int *empates);
void exibir_resultado_turno_computador(float sp1, float sp2, int *v1, int *v2, int *empates);
void exibir_cartas_resumido(const Carta *cartas, int n);
void exibir_carta(const Carta *c);
void apagar_carta(Carta *cartas, int *n_cartas);
static int escolher_carta_comandos(Jogador *j, int jogador_id, int *cmd);

// implementação das funções
// apos decisao de qual tipo de partida em menu antes da batalha quue toma decisao para qual caminho seguir


// Implementação: partida humano x computador
void jogar_partida_1xComputador(Carta *baralho, int n_cartas, Estatisticas *estat) {
    if (n_cartas < CARTAS_POR_JOGADOR * MAX_JOGADORES) {
        printf("Não há cartas suficientes para iniciar a partida.\n");
        return;
    }

    // Inicializa jogadores (0 = humano, 1 = computador)
    Jogador jogadores[MAX_JOGADORES];
    for (int i = 0; i < MAX_JOGADORES; ++i) {
        jogadores[i].cartas_restantes = 0;
        jogadores[i].vitorias = 0;
        jogadores[i].empates = 0;
    }

    // Embaralha e distribui
    embaralhar_cartas(baralho, n_cartas);
    distribuir_cartas(baralho, n_cartas, jogadores, 1); // modo computador

    int vitorias_turno[2] = {0, 0};
    int empates_turno = 0;

    // Loop dos turnos
    for (int turno = 0; turno < CARTAS_POR_JOGADOR; ++turno) {
        exibe_menu_batalha();
        // Mostra apenas cartas do humano; computador só mostra contagem
        exibir_cartas_jogador_computador(&jogadores[0], 0, 0);
        set_color(31); // Cor vermelha para o computador
        printf("Computador possui %d cartas.\n", jogadores[1].cartas_restantes);
        reset_color();

        // Humano escolhe (comandos suportados)
        int cmd_h = 0;
        int escolha_h = escolher_carta_comandos(&jogadores[0], 0, &cmd_h);
        if (cmd_h == 2) {
            limpar_buffer_stdin();
            memset(estat, 0, sizeof(*estat));
            printf("Retornando ao menu principal. Estatísticas da partida atual descartadas.\n");
            return;
        }
        if (cmd_h == 1) {
            // Humano desistiu
            printf("Humano desistiu do turno! Computador vence este turno.\n");
            vitorias_turno[1]++;
            if (jogadores[0].cartas_restantes > 0) remover_carta(&jogadores[0], 0);
            if (jogadores[1].cartas_restantes > 0) remover_carta(&jogadores[1], 0);
            continue;
        }

        // Computador escolhe (estratégia simples: carta aleatória)
        int escolha_c;
        if (jogadores[1].cartas_restantes > 0) {
            escolha_c = rand() % jogadores[1].cartas_restantes;
            // Informa escolha do computador (nome da cidade) ao jogador
            printf("Computador jogou: %s (carta %d)\n", jogadores[1].cartas[escolha_c].nome_cidade, escolha_c + 1);
        } else {
            printf("Computador não tem cartas restantes!\n");
            break; // Sai do loop se não há cartas
        }

        // Compara super poderes
        float sp_h = jogadores[0].cartas[escolha_h].super_poder;
        float sp_c = jogadores[1].cartas[escolha_c].super_poder;
        exibir_resultado_turno_computador(sp_h, sp_c, &vitorias_turno[0], &vitorias_turno[1], &empates_turno);

        // Remove cartas jogadas (remover maior índice primeiro para evitar deslocamento incorreto)
        if (escolha_h > escolha_c) {
            remover_carta(&jogadores[0], escolha_h);
            remover_carta(&jogadores[1], escolha_c);
        } else {
            remover_carta(&jogadores[1], escolha_c);
            remover_carta(&jogadores[0], escolha_h);
        }
    }

    // Atualiza estatísticas gerais
    estat->jogos_jogados++;
    if (vitorias_turno[0] > vitorias_turno[1]) estat->vitorias[0]++;
    else if (vitorias_turno[1] > vitorias_turno[0]) estat->vitorias[1]++;
    else estat->empates++;

    // Exibe resultado final
    set_color(31);
    printf("Resultado final da partida (Humano x Computador):\n");
    reset_color();

    if (vitorias_turno[0] > vitorias_turno[1]) {
        set_color(32);
        printf("Humano venceu a partida!\n");
        reset_color();
    } else if (vitorias_turno[1] > vitorias_turno[0]) {
        set_color(34);
        printf("Computador venceu a partida!\n");
        reset_color();
    } else {
        set_color(36);
        printf("A partida terminou empatada!\n");
        reset_color();
    }
}

// Utilitários de entrada e validação


//  limpar_buffer_stdin: Consome tudo até o fim da linha para evitar lixo no stdin.

void limpar_buffer_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// ler_inteiro_prompt: Lê uma linha do stdin e converte para inteiro com validação. 
// Retorna 0 em caso de falha na leitura.

int ler_inteiro_prompt(const char *prompt) {
    char buf[64];
    long v;
    char *end;
    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) return 0;
        buf[strcspn(buf, "\n")] = '\0';
        v = strtol(buf, &end, 10);
        if (end != buf && *end == '\0') return (int)v;
        printf("Entrada inválida. Tente novamente.\n");
    }
}

// Funções de validação simples para os campos das cartas:

// Verifica e valida se caractere corresponde a A..Z (nao aceita minúsculas).

int valida_estado(char c) {
    c = toupper((unsigned char)c);
    return (c >= 'A' && c <= 'Z');
}

// valida_codigo:
// Verifica se a string tem formato: letra + digits (tamanho 2..4), loop para digitar novamente..

int valida_codigo(const char *s) {
    size_t L = strlen(s);
    if (L < 2 || L > 4) return 0;
    if (!isalpha((unsigned char)s[0])) return 0;
    for (size_t i = 1; i < L; ++i) if (!isdigit((unsigned char)s[i])) return 0;
    return 1;
}

// valida_nome: Garante que o nome não é vazio, loop para digitar novamente.

int valida_nome(const char *nome) {
    return nome != NULL && strlen(nome) > 0;
}

// valida_populacao / valida_area / valida_pib / valida_pontos:
// Checagens simples de intervalo, loop para digitar novamente..

int valida_populacao(int p) { return p > 0; }
int valida_area(float a) { return a > 0.0f; }
int valida_pib(float p) { return p > 0.0f; }
int valida_pontos(int n) { return n >= 0; }

// Cálculos derivados e normalização

// calcula_campos_derivados:

// A partir dos atributos básicos calcula:
// - densidade_populacional
// - pib_per_capita
// - super_poder (valor bruto)

void calcula_campos_derivados(struct Carta *c) {
    if (c->area > 0.0f)
        c->densidade_populacional = (float)c->populacao / c->area;
    else
        c->densidade_populacional = 0.0f;
    if (c->populacao > 0)
        c->pib_per_capita = (c->pib * 1e9f) / c->populacao; // reais por habitante
    else
        c->pib_per_capita = 0.0f;

    // super_poder inicial (soma simples dos campos)
    c->super_poder = (float)c->populacao + c->area + c->pib + (float)c->num_pontos_turisticos + c->densidade_populacional + c->pib_per_capita;
}

// Calcula o Super Poder sem normalização:
//  soma direta dos componentes numéricos (população, área, PIB, número de pontos turísticos,
//  PIB per capita e inverso da densidade). Mantém proteção contra divisão por zero.

void calcular_super_poder_normalizado(Carta *cartas, int n) {
     if (n <= 0) return;
     for (int i = 0; i < n; ++i) {
          float dens = cartas[i].densidade_populacional;
          // evita divisão por zero; densidades muito pequenas geram inverso grande,
          // Aqui mantemos o comportamento de usar o inverso
          float inv_dens = 0.0f;
          if (dens > 1e-9f) inv_dens = 1.0f / dens;

          float soma = 0.0f;
          soma += (float)cartas[i].populacao;                    // população (valor bruto)
          soma += cartas[i].area;                                // área em km²
          soma += cartas[i].pib;                                 // PIB em bilhões
          soma += (float)cartas[i].num_pontos_turisticos;        // pontos turísticos
          soma += cartas[i].pib_per_capita;                      // PIB per capita em reais
          soma += inv_dens;                                      // inverso da densidade

          cartas[i].super_poder = soma;
     }
}

// Funções de arquivo:
// salvar_cartas:
// Grava o número de cartas seguido do array de cartas em modo binário.
void salvar_cartas(const Carta *cartas, int n) {
    FILE *f = fopen(ARQUIVO_CARTAS, "wb");
    if (!f) { printf("Erro ao salvar cartas!\n"); return; }
    fwrite(&n, sizeof(int), 1, f);
    fwrite(cartas, sizeof(Carta), n, f);
    fclose(f);
    set_color(32);
    printf("Cartas salvas com sucesso!\n");
    reset_color();
}

// carregar_cartas:
// Lê o número de cartas e depois as cartas do arquivo binário.
// Retorna a quantidade de cartas lidas (0 se falha ou não existir).

int carregar_cartas(Carta *cartas) {
    FILE *f = fopen(ARQUIVO_CARTAS, "rb");
    if (!f) return 0;
    int n = 0;
    if (fread(&n, sizeof(int), 1, f) != 1) { fclose(f); return 0; }
    if (n < 0 || n > MAX_CARTAS) { fclose(f); return 0; }
    if (fread(cartas, sizeof(Carta), n, f) != (size_t)n) { fclose(f); return 0; }
    fclose(f);
    return n;
}

// Cadastro, exibição e remoção de cartas:
// cadastrar_carta:
// - Interage com o usuário para preencher os campos de uma nova carta.
// - Permite digitar 'voltar' em qualquer passo para cancelar.

void cadastrar_carta(Carta *c) {
    char buf[128];

    // Estado
    while (1) {
        printf("Informe a letra do Estado (A-Z) ou 'sair': ");
        if (!fgets(buf, sizeof(buf), stdin)) return;
        buf[strcspn(buf, "\n")] = '\0';
        if (strcmp(buf, "sair") == 0) return;
        if (strlen(buf) == 1 && valida_estado(buf[0])) {
            c->estado = toupper((unsigned char)buf[0]);
            break;
        }
        printf("Estado inválido.\n");
    }

    // Código
    while (1) {
        printf("Informe o Código da Carta (ex: A01) ou 'sair': ");
        if (!fgets(buf, sizeof(buf), stdin)) return;
        buf[strcspn(buf, "\n")] = '\0';
        if (strcmp(buf, "sair") == 0) return;
        if (valida_codigo(buf)) {
            snprintf(c->codigo, sizeof(c->codigo), "%s", buf);
            break;
        }
        printf("Código inválido.\n");
    }

    // Nome da cidade
    while (1) {
        printf("Informe o Nome da Cidade ou 'sair': ");
        if (!fgets(buf, sizeof(buf), stdin)) return;
        buf[strcspn(buf, "\n")] = '\0';
        if (strcmp(buf, "sair") == 0) return;
        if (valida_nome(buf)) {
            strncpy(c->nome_cidade, buf, sizeof(c->nome_cidade));
            snprintf(c->nome_cidade, sizeof(c->nome_cidade), "%s", buf);
            break;
        }
        printf("Nome inválido.\n");
    }

    // População
    while (1) {
        printf("Informe a População ou 'sair': ");
        if (!fgets(buf, sizeof(buf), stdin)) return;
        buf[strcspn(buf, "\n")] = '\0';
        if (strcmp(buf, "sair") == 0) return;
        char *end;
        long v = strtol(buf, &end, 10);
        if (end != buf && *end == '\0' && valida_populacao((int)v)) {
            c->populacao = (int)v;
            break;
        }
        printf("População inválida.\n");
    }

    // Área
    while (1) {
        printf("Informe a Área (km²) ou 'sair': ");
        if (!fgets(buf, sizeof(buf), stdin)) return;
        buf[strcspn(buf, "\n")] = '\0';
        if (strcmp(buf, "sair") == 0) return;
        char *end;
        float v = strtof(buf, &end);
        if (end != buf && *end == '\0' && valida_area(v)) {
            c->area = v;
            break;
        }
        printf("Área inválida.\n");
    }

    // PIB
    while (1) {
        printf("Informe o PIB (em bilhões de reais) ou 'sair': ");
        if (!fgets(buf, sizeof(buf), stdin)) return;
        buf[strcspn(buf, "\n")] = '\0';
        if (strcmp(buf, "sair") == 0) return;
        char *end;
        float v = strtof(buf, &end);
        if (end != buf && *end == '\0' && valida_pib(v)) {
            c->pib = v;
            break;
        }
        printf("PIB inválido.\n");
    }

    // Pontos turísticos
    while (1) {
        printf("Informe o Número de Pontos Turísticos ou 'sair': ");
        if (!fgets(buf, sizeof(buf), stdin)) return;
        buf[strcspn(buf, "\n")] = '\0';
        if (strcmp(buf, "sair") == 0) return;
        char *end;
        long v = strtol(buf, &end, 10);
        if (end != buf && *end == '\0' && valida_pontos((int)v)) {
            c->num_pontos_turisticos = (int)v;
            break;
        }
        printf("Número inválido.\n");
    }

    // Recalcula campos derivados ao final do cadastro
    calcula_campos_derivados(c);
    printf("Carta cadastrada: %s (%s)\n", c->nome_cidade, c->codigo);
}

// exibir_carta:
// - Mostra todos os atributos (originais e derivados) de uma carta.

void exibir_carta(const Carta *c) {
    printf("-----------------------------------\n");
    printf("Estado: %c\n", c->estado);
    printf("Código: %s\n", c->codigo);
    printf("Nome da Cidade: %s\n", c->nome_cidade);
    printf("População: %d\n", c->populacao);
    printf("Área: %.2f km²\n", c->area);
    printf("PIB: %.2f bilhões de reais\n", c->pib);
    printf("Número de Pontos Turísticos: %d\n", c->num_pontos_turisticos);
    printf("Densidade Populacional: %.2f hab/km²\n", c->densidade_populacional);
    printf("PIB per capita: R$ %.2f\n", c->pib_per_capita);
    printf("Super poder: %.2f\n", c->super_poder);
    printf("-----------------------------------\n");
}

// exibir_cartas_resumido:
// - Lista as cartas cadastradas em forma resumida com índice e super_poder.

void exibir_cartas_resumido(const Carta *cartas, int n) {
    if (n == 0) {
        printf("Nenhuma carta cadastrada.\n");
        return;
    }
    printf("Cartas cadastradas:\n");
    for (int i = 0; i < n; ++i) {
        printf("%d - %s | Super poder: %.2f\n", i+1, cartas[i].nome_cidade, cartas[i].super_poder);
    }
}

// apagar_carta:
// - Permite ao usuário escolher uma carta por índice e a remove do array,
// - compactando o vetor e decrementando o contador.

void apagar_carta(Carta *cartas, int *n_cartas) {
    if (*n_cartas == 0) {
        printf("Nenhuma carta para apagar.\n");
        return;
    }
    while (1) {
        printf("Escolha a carta para apagar (1-%d) ou 0 para voltar: ", *n_cartas);
        int opt = ler_inteiro_prompt("");
        if (opt == 0) return;
        if (opt >= 1 && opt <= *n_cartas) {
            for (int i = opt-1; i < *n_cartas - 1; ++i) cartas[i] = cartas[i+1];
            (*n_cartas)--;
            printf("Carta apagada.\n");
            return;
        }
        printf("Opção inválida.\n");
    }
}

// Embaralhar, distribuir e "animação":
// - Embaralha o array de cartas usando Fisher-Yates.

void embaralhar_cartas(Carta *cartas, int n) {
    if (n <= 1) return;
    for (int i = n - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        Carta tmp = cartas[i];
        cartas[i] = cartas[j];
        cartas[j] = tmp;
    }
}

// animacao de entrega cartas:
// - Pequena mensagem / pausa para simular entrega de carta.
// Exibe uma mensagem animada simulando a entrega de uma carta a um jogador.

// Parâmetros:
// jogador       - índice do jogador (0 = Jogador 1, 1 = segundo jogador)
// carta_idx     - índice da carta sendo entregue (0-based)
// modo_computador - 1 se for modo computador, 0 se for modo 1x1

// Comportamento:
// Mostra mensagem colorida indicando entrega da carta e pausa brevemente.

void animacao_entrega_cartas(int jogador, int carta_idx, int modo_computador) {
    // Validação do índice do jogador
    if (jogador < 0 || jogador >= MAX_JOGADORES) {
        set_color(33); // amarelo
        printf("Erro: índice de jogador inválido (%d). Nenhuma carta entregue.\n", jogador);
        reset_color();
        return;
    }

    // Define cor por jogador: 0 -> verde, 1 -> vermelho/azul dependendo do modo
    if (jogador == 0) {
        set_color(32); // verde para Jogador 1
        printf("Entregando carta %d ao Jogador 1...\n", carta_idx + 1);
    } else if (jogador == 1) {
        if (modo_computador) {
            set_color(31); // vermelho para Computador
            printf("Entregando carta %d ao Computador...\n", carta_idx + 1);
        } else {
            set_color(34); // azul para Jogador 2
            printf("Entregando carta %d ao Jogador 2...\n", carta_idx + 1);
        }
    } else {
        set_color(33);
        printf("Entregando carta %d ao Jogador %d...\n", carta_idx + 1, jogador + 1);
    }
    reset_color();

#ifdef _WIN32
    Sleep(200);
#else
    struct timespec delay = {0, 200000000}; // 200 ms
    nanosleep(&delay, NULL);
#endif
}

// void para batalha jogador x jogador:
// - Inicia uma partida entre dois jogadores humanos.
void jogar_partida_1x1(Carta *baralho, int n_cartas, Estatisticas *estat) {
    if (n_cartas < CARTAS_POR_JOGADOR * MAX_JOGADORES) {
        printf("Não há cartas suficientes para iniciar a partida.\n");
        return;
    }

    // Inicializa jogadores
    Jogador jogadores[MAX_JOGADORES];
    for (int i = 0; i < MAX_JOGADORES; ++i) {
        jogadores[i].cartas_restantes = 0;
        jogadores[i].vitorias = 0;
        jogadores[i].empates = 0;
    }

    // Embaralha e distribui cartas
    embaralhar_cartas(baralho, n_cartas);
    distribuir_cartas(baralho, n_cartas, jogadores, 0); // modo 1x1

    // Estatísticas do turno
    int vitorias_turno[2] = {0, 0};
    int empates_turno = 0;

    // Loop dos turnos
    for (int turno = 0; turno < CARTAS_POR_JOGADOR; ++turno) {
        // Exibe estado atual
        exibe_menu_batalha();
        for (int j = 0; j < MAX_JOGADORES; ++j) exibir_cartas_jogador(&jogadores[j], j);

        // Jogador 1 escolhe uma carta
        int cmd0 = 0;
        int escolha1 = escolher_carta_comandos(&jogadores[0], 0, &cmd0);
        if (cmd0 == 2) { // voltar -> abortar partida
            limpar_buffer_stdin();
            memset(estat, 0, sizeof(*estat));
            printf("Retornando ao menu principal. Estatísticas da partida atual descartadas.\n");
            return;
        }
        if (cmd0 == 1) {
            // Jogador 1 desistiu do turno
            printf("Jogador 1 desistiu do turno! Jogador 2 vence este turno.\n");
            vitorias_turno[1]++;
            if (jogadores[0].cartas_restantes > 0) remover_carta(&jogadores[0], 0);
            if (jogadores[1].cartas_restantes > 0) remover_carta(&jogadores[1], 0);
            continue;
        }

        // Jogador 2 escolhe uma carta
        int cmd1 = 0;
        int escolha2 = escolher_carta_comandos(&jogadores[1], 1, &cmd1);
        if (cmd1 == 2) { // voltar -> abortar partida
            limpar_buffer_stdin();
            memset(estat, 0, sizeof(*estat));
            printf("Retornando ao menu principal. Estatísticas da partida atual descartadas.\n");
            return;
        }
        if (cmd1 == 1) {
            // Jogador 2 desistiu do turno
            printf("Jogador 2 desistiu do turno! Jogador 1 vence este turno.\n");
            vitorias_turno[0]++;
            if (jogadores[0].cartas_restantes > 0) remover_carta(&jogadores[0], 0);
            if (jogadores[1].cartas_restantes > 0) remover_carta(&jogadores[1], 0);
            continue;
        }

        // Ambos escolheram normalmente -> compara escolhas
        float sp1 = jogadores[0].cartas[escolha1].super_poder;
        float sp2 = jogadores[1].cartas[escolha2].super_poder;

        // Exibe resultado do turno
        exibir_resultado_turno(sp1, sp2, &vitorias_turno[0], &vitorias_turno[1], &empates_turno);

        // Remove cartas jogadas (remover maior índice primeiro)
        if (escolha1 > escolha2) {
            remover_carta(&jogadores[0], escolha1);
            remover_carta(&jogadores[1], escolha2);
        } else {
            remover_carta(&jogadores[1], escolha2);
            remover_carta(&jogadores[0], escolha1);
        }
    }

    // Atualiza estatísticas gerais
    estat->jogos_jogados++;
    if (vitorias_turno[0] > vitorias_turno[1]) estat->vitorias[0]++;
    else if (vitorias_turno[1] > vitorias_turno[0]) estat->vitorias[1]++;
    else estat->empates++;

    // Exibe resultado final da partida
    set_color(31);
    printf("Resultado final da partida:\n");
    reset_color();

    if (vitorias_turno[0] > vitorias_turno[1]) {
        set_color(32);
        printf("Jogador 1 venceu a partida!\n");
        reset_color();
    } else if (vitorias_turno[1] > vitorias_turno[0]) {
        set_color(34);
        printf("Jogador 2 venceu a partida!\n");
        reset_color();
    } else {
        set_color(36);
        printf("A partida terminou empatada!\n");
        reset_color();
    }
}

// distribuir_cartas:
// - Distribui CARTAS_POR_JOGADOR cartas para cada jogador a partir do baralho
// - Assinatura: void distribuir_cartas(Carta *baralho, int n_cartas, Jogador *jogadores, int modo_computador)
// - Faz distribuição round-robin e chama animacao_entrega_cartas para feedback.
void distribuir_cartas(Carta *baralho, int n_cartas, Jogador *jogadores, int modo_computador) {
    int needed = CARTAS_POR_JOGADOR * MAX_JOGADORES;
    if (n_cartas < needed) {
        // não há cartas suficientes; inicializa estados vazios e retorna
        for (int p = 0; p < MAX_JOGADORES; ++p) {
            jogadores[p].cartas_restantes = 0;
            jogadores[p].vitorias = 0;
            jogadores[p].empates = 0;
        }
        return;
    }

    // Inicializa jogadores
    for (int p = 0; p < MAX_JOGADORES; ++p) {
        jogadores[p].cartas_restantes = 0;
        jogadores[p].vitorias = 0;
        jogadores[p].empates = 0;
    }

    // Distribuição round-robin (uma carta por jogador em cada rodada)
    int idx = 0;
    for (int c = 0; c < CARTAS_POR_JOGADOR; ++c) {
        for (int p = 0; p < MAX_JOGADORES; ++p) {
            jogadores[p].cartas[jogadores[p].cartas_restantes] = baralho[idx++];
            jogadores[p].cartas_restantes++;
            animacao_entrega_cartas(p, c, modo_computador);
        }
    }
}

//  exibir_cartas_jogador:
// - Mostra as cartas atualmente na mão do jogador (resumido).

void exibir_cartas_jogador(const Jogador *j, int jogador_id) {
    set_color(jogador_id == 0 ? 32 : 34);
    printf("Cartas do Jogador %d:\n", jogador_id + 1);
    for (int i = 0; i < j->cartas_restantes; ++i) {
        printf("%d - %s | Super poder: %.2f\n", i + 1, j->cartas[i].nome_cidade, j->cartas[i].super_poder);
    }
    reset_color();
}

// exibir_cartas_jogador_computador:
// - Versão especial para modo computador que mostra "Computador" em vez de "Jogador 2"

void exibir_cartas_jogador_computador(const Jogador *j, int jogador_id, int eh_computador) {
    set_color(jogador_id == 0 ? 32 : 31); // Verde para humano, vermelho para computador
    if (eh_computador && jogador_id == 1) {
        printf("Cartas do Computador:\n");
    } else {
        printf("Cartas do Jogador %d:\n", jogador_id + 1);
    }
    for (int i = 0; i < j->cartas_restantes; ++i) {
        printf("%d - %s | Super poder: %.2f\n", i + 1, j->cartas[i].nome_cidade, j->cartas[i].super_poder);
    }
    reset_color();
}

// escolher_carta:
// - Solicita ao jogador um índice de carta válido e retorna o índice 0-based.

int escolher_carta(const Jogador *j, int jogador_id) {
    int escolha;
    while (1) {
        printf("Jogador %d, escolha a carta (1-%d): ", jogador_id + 1, j->cartas_restantes);
        escolha = ler_inteiro_prompt("");
        if (escolha >= 1 && escolha <= j->cartas_restantes) return escolha - 1;
        printf("Escolha inválida.\n");
    }
}

// remover_carta:
// - Remove a carta em índice idx do jogador (compactando o vetor).

void remover_carta(Jogador *j, int idx) {
    if (idx < j->cartas_restantes - 1) {
        memmove(&j->cartas[idx], &j->cartas[idx + 1], (j->cartas_restantes - idx - 1) * sizeof(Carta));
    }
    j->cartas_restantes--;
}
// - Compara os super_poderes das duas cartas, exibe resultado e
// - Ajusta contadores de vitórias/empates para o turno.

void exibir_resultado_turno(float sp1, float sp2, int *v1, int *v2, int *empates) {
    printf("Super poder Jogador 1: %.2f | Super poder Jogador 2: %.2f\n", sp1, sp2);
    if (sp1 > sp2) {
        printf("Jogador 1 venceu o turno!\n");
        (*v1)++;
    } else if (sp2 > sp1) {
        printf("Jogador 2 venceu o turno!\n");
        (*v2)++;
    } else {
        printf("Empate no turno!\n");
        (*empates)++;
    }
}

// exibir_resultado_turno_computador:
// - Versão especial para modo computador que mostra "Computador" em vez de "Jogador 2"

void exibir_resultado_turno_computador(float sp1, float sp2, int *v1, int *v2, int *empates) {
    printf("Super poder Jogador 1: %.2f | Super poder Computador: %.2f\n", sp1, sp2);
    if (sp1 > sp2) {
        set_color(32); // Verde para vitória do jogador
        printf("Jogador 1 venceu o turno!\n");
        reset_color();
        (*v1)++;
    } else if (sp2 > sp1) {
        set_color(31); // Vermelho para vitória do computador
        printf("Computador venceu o turno!\n");
        reset_color();
        (*v2)++;
    } else {
        set_color(33); // Amarelo para empate
        printf("Empate no turno!\n");
        reset_color();
        (*empates)++;
    }
}

// exibir_estatisticas:
// - Mostra estatísticas acumuladas entre partidas.

void exibir_estatisticas(const Estatisticas *e) {
    printf("Estatísticas do jogo:\n");
    printf("Jogos jogados: %d\n", e->jogos_jogados);
    printf("Vitórias Jogador 1: %d\n", e->vitorias[0]);
    printf("Vitórias Jogador 2: %d\n", e->vitorias[1]);
    printf("Empates: %d\n", e->empates);
}

// Menus (apenas exibição
// - exibe_nome_jogo: imprime cabeçalho do jogo

void exibe_nome_jogo() {
    set_color(33); // amarelo
    printf("╔════════════════════════════════════════════╗\n");
    printf("║               SUPER TRUNFO C               ║\n");
    printf("║              JOGO DAS CIDADES              ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    reset_color();
}

// exibe_menu_principal: mostra opções do menu principal

void exibe_menu_principal() {
    set_color(91);
    printf("╔════════════════════════════════════════════╗\n");
    printf("║              MENU PRINCIPAL                ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("║ 1 - Iniciar jogo                           ║\n");
    printf("║ 2 - Cadastrar cartas                       ║\n");
    printf("║ 3 - Exibir cartas cadastradas              ║\n");
    printf("║ 4 - Apagar cartas                          ║\n");
    printf("║ 5 - Exibir estatísticas                    ║\n");
    printf("║ 6 - Salvar e sair                          ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    reset_color();
}

// exibe_menu_modo_de_jogo: mostra opções antes de iniciar a batalha

void exibe_menu_antes_do_batalha() {
    set_color(35);
    printf("╔════════════════════════════════════════════╗\n");
    printf("║                MODO DE JOGO                ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("║ 1 - Modo de Jogo: 1x1                      ║\n");
    printf("║ 2 - Modo de Jogo: 1xComputador             ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    reset_color();
}

// exibe_menu_batalha: mostra instruções durante a batalha

void exibe_menu_batalha() {
    set_color(31);
    printf("╔════════════════════════════════════════════╗\n");
    printf("║                MENU DE BATALHA             ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("║ digite \"desistir\" para desistir do turno   ║\n");
    printf("║ digite \"sair\" e aperte Enter 2x para sair  ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    reset_color();
}

// exibe_menu_cadastro: mostra opções do menu de cadastro

void exibe_menu_cadastro() {
    set_color(36);
    printf("╔════════════════════════════════════════════╗\n");
    printf("║          MENU DE CADASTRO DE CARTAS        ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("║ 1 - Cadastrar nova carta                   ║\n");
    printf("║ 2 - Voltar ao menu principal               ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    reset_color();
}

// main principal da partida:

// Função auxiliar: lê escolha de carta permitindo comandos "desistir" e "sair".

// Resultados para desicao do progama:
//  >=0 : índice 0-based escolhido
//  -1  : comando recebido (ver *cmd)
//   0 -> escolha normal
//   1 -> desistir
//   2 -> sair

static int escolher_carta_comandos(Jogador *j, int jogador_id, int *cmd) {
    char buf[256];

    // Loop até receber escolha válida ou comando
    for (;;) {
        set_color(jogador_id == 0 ? 32 : 34);
        printf("Jogador %d, escolha a carta: (1-%d)",
               jogador_id + 1, j->cartas_restantes);
        reset_color();
        if (!fgets(buf, sizeof(buf), stdin)) { *cmd = 2; return -1; }
        buf[strcspn(buf, "\n")] = '\0';
        if (strcmp(buf, "sair") == 0) { *cmd = 2; return -1; }
        if (strcmp(buf, "desistir") == 0) { *cmd = 1; return -1; }
        char *end;
        long v = strtol(buf, &end, 10);
        if (end != buf && *end == '\0' && v >= 1 && v <= j->cartas_restantes) {
            *cmd = 0;
            return (int)(v - 1);
        }
        printf("Escolha inválida.\n");
    }
}

// Iniciar uma partida abre menu de modo de jogo e executa a batalha
void jogar_partida(Carta *baralho, int n_cartas, Estatisticas *estat) {
    exibe_menu_antes_do_batalha();
    int modo;
    while (1) {
        modo = ler_inteiro_prompt("Escolha o modo: ");
        if (modo == 1) {
            jogar_partida_1x1(baralho, n_cartas, estat);
            return;
        }
        if (modo == 2) {
            jogar_partida_1xComputador(baralho, n_cartas, estat);
            return;
        }
        if (modo < 1 || modo > 2) {
            printf("Modo inválido! Escolha 1 (1x1) ou 2 (1xComputador).\n");
        }
    }
}

// main: loop principal do programa
int main(void) {
    srand((unsigned)time(NULL));

    Carta cartas[MAX_CARTAS];
    memset(cartas, 0, sizeof(cartas));
    int n_cartas = 0;
    Estatisticas estat = {0};

    // Tenta carregar cartas salvas
    n_cartas = carregar_cartas(cartas);
    if (n_cartas > 0) {
        for (int i = 0; i < n_cartas; ++i) calcula_campos_derivados(&cartas[i]);
        calcular_super_poder_normalizado(cartas, n_cartas);
        printf("%d cartas carregadas do arquivo.\n", n_cartas);
    }

    // Loop principal
    while (1) {
        exibe_nome_jogo();
        exibe_menu_principal();
        printf("Escolha uma opção: ");
        int opcao = ler_inteiro_prompt("");

        if (opcao == 1) {
            // Iniciar jogo
            if (n_cartas < CARTAS_POR_JOGADOR * MAX_JOGADORES) {
                printf("Cadastre pelo menos %d cartas para jogar!\n", CARTAS_POR_JOGADOR * MAX_JOGADORES);
                continue;
            }
            jogar_partida(cartas, n_cartas, &estat);

        } else if (opcao == 2) {
            // Menu de cadastro de cartas
                for (;;) {
                exibe_menu_cadastro();
                printf("Escolha uma opção: ");
                int op = ler_inteiro_prompt("");
                if (op == 1) {
                if (n_cartas >= MAX_CARTAS) {
                printf("Capacidade máxima de cartas atingida.\n");
                break;
                }
                
                    // garante string vazia antes do cadastro para detectar 'voltar'
                    cartas[n_cartas].codigo[0] = '\0';
                        cadastrar_carta(&cartas[n_cartas]);
                        if (strlen(cartas[n_cartas].codigo) > 0) {
                        calcula_campos_derivados(&cartas[n_cartas]);
                        n_cartas++;
                        calcular_super_poder_normalizado(cartas, n_cartas);
                    }
                        } else if (op == 2) {
                        break;
                        } else {
                        printf("Opção inválida.\n");
                        }
            }

                } else if (opcao == 3) {
                // Exibir cartas cadastradas
                exibir_cartas_resumido(cartas, n_cartas);
                printf("Deseja ver detalhes de alguma carta? (0 para não): ");
                int idx = ler_inteiro_prompt("");
                if (idx >= 1 && idx <= n_cartas) exibir_carta(&cartas[idx - 1]);

                     } else if (opcao == 4) {
                    // Apagar cartas
                    apagar_carta(cartas, &n_cartas);
                    if (n_cartas > 0) calcular_super_poder_normalizado(cartas, n_cartas);

                    } else if (opcao == 5) {
                        exibir_estatisticas(&estat);
                        // Salvar e sair
                        } else if (opcao == 6) {
                            salvar_cartas(cartas, n_cartas);
                            set_color(33);
                            printf("Saindo...\n");
                            reset_color();
                            break;

                        } else {
                            printf("Opção inválida.\n");
                            }
                }

    return 0;
}