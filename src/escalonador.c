#include <stdlib.h>
#include "escalonador.h"

int srtf(SistemaSimulado* sistema, int cpu_id) {
    int melhor_idx = -1;
    int melhor_dur = -1;  /* -1 = ainda não achei nenhuma */

    for (int i = 0; i < sistema->qtd_tarefas; i++) {
        Estado e = sistema->tarefas[i].estado;
        int eh_candidata = (e == PRONTA) ||
                           (e == EXECUTANDO && sistema->tarefas[i].cpu_atual == cpu_id);
        if (!eh_candidata) continue;

        int dur = sistema->tarefas[i].duracao_restante;

        if (melhor_idx == -1 || dur < melhor_dur) {
            melhor_idx = i;
            melhor_dur = dur;
        } else if (dur == melhor_dur){
            melhor_idx = desempatar(sistema, melhor_idx, i, cpu_id);
        }
    }

    return melhor_idx;
}

int priop(SistemaSimulado* sistema, int cpu_id){
    int melhor_idx = -1;
    int melhor_prio = -1;

    for (int i = 0; i < sistema->qtd_tarefas; i++) {
        Estado e = sistema->tarefas[i].estado;
        int eh_candidata = (e == PRONTA) ||
                           (e == EXECUTANDO && sistema->tarefas[i].cpu_atual == cpu_id);
        if (!eh_candidata) continue;

        int prio = sistema->tarefas[i].prioridade;

        if (melhor_idx == -1 || prio > melhor_prio) {
            melhor_idx = i;
            melhor_prio = prio;
        } else if (dur == melhor_prio){
            melhor_idx = desempatar(sistema, melhor_idx, i, cpu_id);
        }
    }

    return melhor_idx;
}

static int desempatar(SistemaSimulado* sistema, int idx_a, int idx_b, int cpu_id){
    Tcb* a = &sistema->tarefas[idx_a];
    Tcb* b = &sistema->tarefas[idx_b];

    //tarefa que esta executando permanece
    if(a->estado == EXECUTANDO && a->cpu_atual = cpu_id) return idx_a;
    if(b->estado == EXECUTANDO && b->cpu_atual = cpu_id) return idx_b;

    //menor ingresso
    if(a->ingresso != b->ingresso)
        return (a->ingresso < b->ingresso) ? idx_a : idx_b;

    //menor duracao original
    if(a->duracao != b->duracao)
        return (a->duracao < b->duracao) ? idx_a : idx_b;

    //sorteio
    sistema->houve_sorteio = 1;
    return (rand() % 2 == 0) ? idx_a : idx_b;
}