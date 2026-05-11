#include "escalonador.h"

int priop(SistemaSimulado* sistema, int cpu_id) {
    int melhor_idx  = -1;
    int melhor_prio = -1;

    for (int i = 0; i < sistema->qtd_tarefas; i++) {
        Estado e = sistema->tarefas[i].estado;
        int eh_candidata = (e == PRONTA) ||
                           (e == EXECUTANDO && sistema->tarefas[i].cpu_atual == cpu_id);
        if (!eh_candidata) continue;

        int prio = sistema->tarefas[i].prioridade;

        if (melhor_idx == -1 || prio > melhor_prio) {
            melhor_idx  = i;
            melhor_prio = prio;
        }
        /* empate: resolvido na 3.4 */
    }

    return melhor_idx;
}