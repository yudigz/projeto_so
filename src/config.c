#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "utils.h"
#include "escalonador.h"

static void aplicar_default(SistemaSimulado* sistema){
    strncpy(sistema->algoritmo,"srtf", sizeof(sistema->algoritmo) - 1);
    sistema->algoritmo[sizeof(sistema->algoritmo) - 1] = '\0';
    sistema->quantum = 3;
    sistema->qtd_cpus = 2;
}

static int validar_config(const SistemaSimulado* sistema){
    if(sistema->qtd_cpus < 2){
        fprintf(stderr,"Erro: o sistema precisa de pelo menos 2 CPUs(configurado: %d)\n",sistema->qtd_cpus);
        return -1;
    }
    if(sistema->qtd_tarefas < 1){
        fprintf(stderr,"Erro: o arquivo de config nao possui nenhuma tarefa\n");
        return -1;
    }
    if(sistema->quantum <= 0){
        fprintf(stderr,"Erro: o quantum deve ser maior que zero(configurado: %d)\n",sistema->quantum);
        return -1;
    }
    for(int i = 0;i < sistema->qtd_tarefas; i++){
        if(sistema->tarefas[i].ingresso < 0){
            fprintf(stderr, "Erro: tarefa %d tem ingresso negativo (%d)\n",sistema->tarefas[i].id,sistema->tarefas[i].ingresso);
            return -1;
        }
    }
    if(strcmp(sistema->algoritmo, "srtf") != 0 && strcmp(sistema->algoritmo, "priop") != 0){
        fprintf(stderr, "Erro: algoritmo desconhecido '%s' \n",sistema->algoritmo);
        return -1;
    }
    return 0;
}

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
    aplicar_default(sistema);
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
        sistema->tarefas = realloc(sistema->tarefas,(sistema->qtd_tarefas + 1) * sizeof(Tcb));

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

    /* aloca e inicializa o array de CPUs */
    sistema->cpus = malloc(sistema->qtd_cpus * sizeof(Cpu));
    for (int i =0; i < sistema->qtd_cpus; i++){
        sistema->cpus[i].id = i;
        sistema->cpus[i].tarefa_atual = NULL;
        sistema->cpus[i].ligado = 1;
        sistema->cpus[i].ticks_ociosos_acumulados = 0;
        sistema->cpus[i].quantum_restante = 0;
    }

    if(validar_config(sistema) != 0){
        free(sistema->tarefas);
        sistema->tarefas = NULL;
        return -1;
    }

    sistema->escalonador = get_escalonador(sistema->algoritmo);
    if(sistema->escalonador == NULL){
        fprintf(stderr, "Erro: escalonador '%s' nao encontrado\n",sistema->algoritmo);
        return -1;
    }

    sistema->historico     = NULL;
    sistema->qtd_snapshots = 0;
    sistema->cap_historico = 0;

    return 0;
}
