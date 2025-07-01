#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TAMANHO 10
#define NAVIOS 5
#define BOMBAS 10
#define MAX_TENTATIVAS 10
#define MAX_VIDAS 5

void limparBuffer() {
    char c;
    c = getchar();
    while (c != '\n') {
        c = getchar();
    }
}

void preencherTabuleiro(int tabuleiro[TAMANHO][TAMANHO]) {
    int i;
    int j;
    int countNavios = 0;
    int countBombas = 0;

    for (i = 0; i < TAMANHO; i++) {
        for (j = 0; j < TAMANHO; j++) {
            tabuleiro[i][j] = 0;
        }
    }

    while (countNavios < NAVIOS) {
        int x = rand() % TAMANHO;
        int y = rand() % TAMANHO;

        if (tabuleiro[x][y] == 0) {
            tabuleiro[x][y] = 2;
            countNavios++;
        }
    }

    while (countBombas < BOMBAS) {
        int x = rand() % TAMANHO;
        int y = rand() % TAMANHO;

        if (tabuleiro[x][y] == 0) {
            tabuleiro[x][y] = 1;
            countBombas++;
        }
    }
}

void salvarJogo(char* nome, int tabuleiro[TAMANHO][TAMANHO], int tentativas, int naviosEncontrados, int bombasEncontradas) {
    FILE* arquivo;

    arquivo = fopen(nome, "wb");
    if (arquivo == NULL) {
        printf("Erro ao salvar o jogo.\n");
        return;
    }

    fwrite(tabuleiro, sizeof(int), TAMANHO * TAMANHO, arquivo);
    fwrite(&tentativas, sizeof(int), 1, arquivo);
    fwrite(&naviosEncontrados, sizeof(int), 1, arquivo);
    fwrite(&bombasEncontradas, sizeof(int), 1, arquivo);

    fclose(arquivo);
}

int carregarJogo(char* nome, int tabuleiro[TAMANHO][TAMANHO], int* tentativas, int* naviosEncontrados, int* bombasEncontradas) {
    FILE* arquivo;

    arquivo = fopen(nome, "rb");
    if (arquivo == NULL) {
        return 0;
    }

    fread(tabuleiro, sizeof(int), TAMANHO * TAMANHO, arquivo);
    fread(tentativas, sizeof(int), 1, arquivo);
    fread(naviosEncontrados, sizeof(int), 1, arquivo);
    fread(bombasEncontradas, sizeof(int), 1, arquivo);

    fclose(arquivo);

    return 1;
}

void salvarRanking(char* nome, int pontos) {
    FILE* ranking = fopen("ranking.bin", "ab");

    if (ranking == NULL) {
        printf("Erro ao salvar o ranking.\n");
        return;
    }

    int nomeLength = strlen(nome);
    fwrite(&nomeLength, sizeof(int), 1, ranking);
    fwrite(nome, sizeof(char), nomeLength, ranking);
    fwrite(&pontos, sizeof(int), 1, ranking);

    fclose(ranking);
}

void mostrarRanking() {
    FILE* ranking = fopen("ranking.bin", "rb");
    char nome[50];
    int nomeLength;
    int pontos;

    if (ranking == NULL) {
        printf("Nenhuma pontuação registrada.\n");
        return;
    }

    printf("Ranking:\n");
    while (fread(&nomeLength, sizeof(int), 1, ranking) == 1) {
        fread(nome, sizeof(char), nomeLength, ranking);
        nome[nomeLength] = '\0';
        fread(&pontos, sizeof(int), 1, ranking);
        printf("%s - %d pontos\n", nome, pontos);
    }
    fclose(ranking);

    printf("Digite V para voltar à tela inicial: ");
    char opcao;
    do {
        scanf(" %c", &opcao); limparBuffer();
    } while (opcao != 'V' && opcao != 'v');
}

void jogarOJogo(int tabuleiro[TAMANHO][TAMANHO], char* nome, int* tentativas, int* naviosEncontrados, int* bombasEncontradas) {
    int vidas = MAX_VIDAS;
    int linha;
    int coluna;
    char opcao;

    while (*tentativas < MAX_TENTATIVAS && *naviosEncontrados < NAVIOS && vidas > 0) {
        printf("Tentativa %d de %d. Vidas restantes: %d. Pontos: %d\n", *tentativas + 1, MAX_TENTATIVAS, vidas, 10 - (*bombasEncontradas * 2));
        printf("Digite a linha e coluna (0-9) ou 'P' para pausar: ");

        scanf(" %c", &opcao); limparBuffer();

        if (opcao == 'P' || opcao == 'p') {
            salvarJogo(nome, tabuleiro, *tentativas, *naviosEncontrados, *bombasEncontradas);
            printf("Jogo salvo. Saindo...\n");
            return;
        }
        else {
            linha = opcao - '0';
            scanf("%d", &coluna); limparBuffer();

            if (linha < 0 || linha >= TAMANHO || coluna < 0 || coluna >= TAMANHO) {
                printf("Posição inválida. Tente novamente.\n");
                continue;
            }

            if (tabuleiro[linha][coluna] == 1) {
                printf("Bomba! Você perdeu uma vida.\n");
                vidas--;
                (*bombasEncontradas)++;
                tabuleiro[linha][coluna] = -1;
            }
            else if (tabuleiro[linha][coluna] == 2) {
                printf("Navio! Você encontrou um navio.\n");
                (*naviosEncontrados)++;
                tabuleiro[linha][coluna] = -1;
            }
            else if (tabuleiro[linha][coluna] == 0) {
                printf("Água.\n");
                tabuleiro[linha][coluna] = -1;
            }
            else if (tabuleiro[linha][coluna] == -1) {
                printf("Você já tentou essa posição. Tente novamente.\n");
                continue;
            }

            (*tentativas)++;
        }
    }

    if (*naviosEncontrados == NAVIOS) {
        int pontos = 10 - (*bombasEncontradas * 2);
        printf("Parabéns! Você encontrou todos os navios.\n");
        printf("Sua pontuação final é %d pontos.\n", pontos);
        salvarRanking(nome, pontos);
    }
    else {
        printf("Fim de jogo. Você não encontrou todos os navios.\n");
    }
}

int main() {
    int tabuleiro[TAMANHO][TAMANHO];
    int opcao;
    char nome[50];
    int tentativas = 0;
    int naviosEncontrados = 0;
    int bombasEncontradas = 0;

    while (1) {
        printf("Jogo da Batalha Naval\n");
        printf("1 - Jogar\n");
        printf("2 - Ver pontuações\n");
        printf("0 - Sair\n");
        printf("Digite sua opção: ");

        scanf("%d", &opcao); limparBuffer();

        if (opcao == 1) {
            printf("Digite o nome do jogador: ");
            scanf("%s", nome); limparBuffer();

            if (carregarJogo(nome, tabuleiro, &tentativas, &naviosEncontrados, &bombasEncontradas)) {
                printf("Encontramos uma partida em seu nome pausada.\n");
                printf("1 - Recomeçar de onde parou\n");
                printf("2 - Começar uma nova partida\n");
                printf("Digite sua opção: ");
                scanf("%d", &opcao); limparBuffer();

                if (opcao == 2) {
                    remove(nome);
                    preencherTabuleiro(tabuleiro);
                    tentativas = 0;
                    naviosEncontrados = 0;
                    bombasEncontradas = 0;
                }
            }
            else {
                preencherTabuleiro(tabuleiro);
                tentativas = 0;
                naviosEncontrados = 0;
                bombasEncontradas = 0;
            }
            jogarOJogo(tabuleiro, nome, &tentativas, &naviosEncontrados, &bombasEncontradas);
        }
        else if (opcao == 2) {
            mostrarRanking();
        }
        else if (opcao == 0) {
            break;
        }
        else {
            printf("Opção inválida. Tente novamente.\n");
        }
    }

    return 0;
}