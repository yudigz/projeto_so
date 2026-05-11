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
        }
        /* empate: resolvido na 3.4 — por ora pega a primeira que encontrar */
    }

    return melhor_idx;
}

int priop(SistemaSimulado* sistema, int cpu_id){
    int melhor_idx = -1;
    int melhor_dur = -1;

    for (int i = 0; i < sistema->qtd_tarefas; i++) {
        Estado e = sistema->tarefas[i].estado;
        int eh_candidata = (e == PRONTA) ||
                           (e == EXECUTANDO && sistema->tarefas[i].cpu_atual == cpu_id);
        if (!eh_candidata) continue;

        int prio = sistema->tarefas[i].prioridade;

        if (melhor_idx == -1 || prio > melhor_prio) {
            melhor_idx = i;
            melhor_prio = prio;
        }
    }

    return melhor_idx;
}