// Super Trunfo C

//Bibliotecas necessárias
#include <stdio.h>
#include <string.h>


// Estrutura para armazenar os dados de uma carta
typedef struct {
    char estado;                // Letra do estado (A-H)
    char codigo[5];             // Código da carta (ex: A01)
    char nome_cidade[50];       // Nome da cidade
    int populacao;              // População da cidade
    float area;                 // Área em km²
    float pib;                  // PIB em bilhões de reais
    int num_pontos_turisticos;  // Número de pontos turísticos
} Carta;

// Função para exibir o nome do jogo
void exibe_nome_jogo() {
    printf("------------------------------------------------------------------\n");
    printf("-                        SUPER TRUNFO C                          -\n");
    printf("-                       JOGO DAS CIDADES                         -\n");
    printf("------------------------------------------------------------------\n\n");
}

// Função principal
int main() {

    // Exibe o nome do jogo
    exibe_nome_jogo();

    // Array para armazenar duas cartas
    Carta cartas[2];

    // Cadastro da carta 1
    printf("Cadastro da Carta 1\n");
    printf("Informe a letra do Estado (A-H): ");
    scanf(" %c", &cartas[0].estado);

    printf("Informe o Código da Carta (ex: A01): ");
    scanf("%4s", cartas[0].codigo);
    getchar(); // Limpa o buffer

    printf("Informe o Nome da Cidade: ");
    fgets(cartas[0].nome_cidade, sizeof(cartas[0].nome_cidade), stdin);
    cartas[0].nome_cidade[strcspn(cartas[0].nome_cidade, "\n")] = '\0'; // Remove o \n

    printf("Informe a População: ");
    scanf("%d", &cartas[0].populacao);

    printf("Informe a Área (em km²): ");
    scanf("%f", &cartas[0].area);

    printf("Informe o PIB (em bilhões de reais): ");
    scanf("%f", &cartas[0].pib);

    printf("Informe o Número de Pontos Turísticos: ");
    scanf("%d", &cartas[0].num_pontos_turisticos);

    getchar(); // Limpa o buffer antes do próximo fgets

        // Exibição dos dados das cartas para verificação

    printf("\nExibição dos dados das cartas para verificação\n");

    printf("\n-----------------------------------\n");
    printf("Dados da Primeira Carta:\n");
    printf("Estado: %c\n", cartas[0].estado);
    printf("Código: %s\n", cartas[0].codigo);
    printf("Nome da Cidade: %s\n", cartas[0].nome_cidade);
    printf("População: %d\n", cartas[0].populacao);
    printf("Área: %.2f km²\n", cartas[0].area);
    printf("PIB: %.2f bilhões de reais\n", cartas[0].pib);
    printf("Número de Pontos Turísticos: %d\n", cartas[0].num_pontos_turisticos);
    printf("-----------------------------------\n");

    // Cadastro da carta 2
    printf("\nCadastro da Carta 2\n");
    printf("Informe a letra do Estado (A-H): ");
    scanf(" %c", &cartas[1].estado);

    printf("Informe o Código da Carta (ex: A01): ");
    scanf("%4s", cartas[1].codigo);
    getchar();

    printf("Informe o Nome da Cidade: ");
    fgets(cartas[1].nome_cidade, sizeof(cartas[1].nome_cidade), stdin);
    cartas[1].nome_cidade[strcspn(cartas[1].nome_cidade, "\n")] = '\0';

    printf("Informe a População: ");
    scanf("%d", &cartas[1].populacao);

    printf("Informe a Área (em km²): ");
    scanf("%f", &cartas[1].area);

    printf("Informe o PIB (em bilhões de reais): ");
    scanf("%f", &cartas[1].pib);

    printf("Informe o Número de Pontos Turísticos: ");
    scanf("%d", &cartas[1].num_pontos_turisticos);

    printf("\n-----------------------------------\n");

    // Exibição dos dados das cartas para verificação

    printf("\nExibição dos dados das cartas para verificação\n");

    printf("\n-----------------------------------\n");
    printf("Dados da Segunda Carta:\n");
    printf("Estado: %c\n", cartas[1].estado);
    printf("Código: %s\n", cartas[1].codigo);
    printf("Nome da Cidade: %s\n", cartas[1].nome_cidade);
    printf("População: %d\n", cartas[1].populacao);
    printf("Área: %.2f km²\n", cartas[1].area);
    printf("PIB: %.2f bilhões de reais\n", cartas[1].pib);
    printf("Número de Pontos Turísticos: %d\n", cartas[1].num_pontos_turisticos);
    printf("-----------------------------------\n");

    return 0;
}
