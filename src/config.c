#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "utils.h"

static void strip_newline(char* s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
    if (len > 1 && s[len-2] == '\r') s[len-2] = '\0';
}

static int separa_linha_sistema(char* linha, SistemaSimulado* sistema) {
    char* token = strtok(linha, ";");
    if (token == NULL) { /* linha mal formada */ return -1; }
    strncpy(sistema->algoritmo, token, sizeof(sistema->algoritmo) - 1);
    sistema->algoritmo[sizeof(sistema->algoritmo) - 1] = '\0';  /* garante terminador */
    str_to_lower(sistema->algoritmo);

    token = strtok(NULL, ";");
    if (token == NULL) return -1;
    sistema->quantum = atoi(token);

    token = strtok(NULL, ";");
    if (token == NULL) return -1;
    sistema->qtd_cpus = atoi(token);

    return 0;
}

static int separa_linha_tarefa(char* linha, Tcb* tcb){
    char* token = strtok(linha, ";");
    if (token == NULL) return -1;
    tcb->id = atoi(token);

    token = strtok(NULL, ";");
    if (token == NULL) return -1;
    unsigned long hex = strtol(token, NULL, 16);
    tcb->cor.r = (hex >> 16) & 0xFF;
    tcb->cor.g = (hex >> 8)  & 0xFF;
    tcb->cor.b =  hex        & 0xFF;

    token = strtok(NULL, ";");
    if (token == NULL) return -1;
    tcb->ingresso = atoi(token);

    token = strtok(NULL, ";");
    if (token == NULL) return -1;
    tcb->duracao = atoi(token);
    tcb->duracao_restante = tcb->duracao;  /* começa igual, decrementa depois */

    token = strtok(NULL, ";");
    if (token == NULL) return -1;
    tcb->prioridade = atoi(token);

    token = strtok(NULL, ";");

    tcb->estado    = NOVA;   /* toda tarefa começa como NOVA */
    tcb->cpu_atual = -1;     /* -1 = sem CPU atribuída */

    return 0;
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
    printf("algoritmo='%s' quantum=%d cpus=%d\n", sistema->algoritmo, sistema->quantum, sistema->qtd_cpus);

    sistema->tarefas = NULL;
    sistema->qtd_tarefas = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        strip_newline(linha);
        if (strlen(linha) == 0) continue;

        /* cresce o array em 1 */
        sistema->tarefas = realloc(sistema->tarefas,
                                (sistema->qtd_tarefas + 1) * sizeof(Tcb));

        separa_linha_tarefa(linha, &sistema->tarefas[sistema->qtd_tarefas]);
        sistema->qtd_tarefas++;
    }

    for (int i = 0; i < sistema->qtd_tarefas; i++) {
    Tcb* t = &sistema->tarefas[i];
    printf("Tarefa %d | cor=(%d,%d,%d) | ingresso=%d | duracao=%d | prio=%d\n",
           t->id, t->cor.r, t->cor.g, t->cor.b,
           t->ingresso, t->duracao, t->prioridade);
    }

    fclose(arquivo);
    return 0;
}
