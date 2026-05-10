#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"


static void strip_newline(char* s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
    if (len > 1 && s[len-2] == '\r') s[len-2] = '\0';
}

int ler_config(const char* caminho, SistemaSimulado* sistema) {
    FILE* arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo '%s'\n", caminho);
        return -1;
    }

    char linha[512];

    /* Linha 1: parametros do sistema (algoritmo;quantum;qtd_cpus) */
    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        fprintf(stderr, "Erro: arquivo vazio ou invalido\n");
        fclose(arquivo);
        return -1;
    }
    strip_newline(linha);
    if(separa_linha_sistema(linha, sistema) != 0) {
        fclose(arquivo);
        return -1;
    }
    printf("algoritmo='%s' quantum=%d cpus=%d\n",
       sistema->algoritmo,
       sistema->quantum,
       sistema->qtd_cpus);

    /* Linhas seguintes: uma tarefa por linha */
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        strip_newline(linha);

        /* Ignora linhas vazias */
        if (strlen(linha) == 0) continue;

        printf("Linha tarefa: %s\n", linha);
    }

    fclose(arquivo);
    return 0;
}

static int separa_linha_sistema(char* linha, SistemaSimulado* sistema) {
    char* token = strtok(linha, ";");
    if (token == NULL) { /* linha mal formada */ return -1; }
    strncpy(sistema->algoritmo, token, sizeof(sistema->algoritmo) - 1);
    sistema->algoritmo[sizeof(sistema->algoritmo) - 1] = '\0';  /* garante terminador */

    token = strtok(NULL, ";");
    if (token == NULL) return -1;
    sistema->quantum = atoi(token);

    token = strtok(NULL, ";");
    if (token == NULL) return -1;
    sistema->qtd_cpus = atoi(token);

    return 0;
}