#include <stddef.h>
#include "sistema.h"

/*
 * Algoritmo Round Robin (RR)
 *
 * Ideia: cada tarefa recebe a CPU por no maximo um quantum. Quando o quantum
 * expira, a tarefa volta para a fila de prontas e a proxima na ordem circular
 * e escolhida. O controle do quantum em si ja e feito por simulacao.c — o RR
 * so precisa decidir QUEM e o proximo.
 *
 * Problema de estado: quando o escalonador e chamado, a tarefa que acabou de
 * ter seu quantum expirado ja esta de volta como PRONTA (cpu_atual == -1).
 * Nao e possivel saber quem acabou de rodar olhando so para o estado atual.
 *
 * Solucao: o snapshot do tick anterior ainda registra qual tarefa estava
 * executando em cada CPU. Usamos esse historico para descobrir o "ultimo que
 * rodou aqui" e iniciar a busca circular logo apos ele.
 */
int rr(SistemaSimulado* sistema, int cpu_id) {
    int start = 0;

    /* Descobre o indice da tarefa que rodou por ultimo nesta CPU */
    if (sistema->qtd_snapshots > 0) {
        const Snapshot* ant = &sistema->historico[sistema->qtd_snapshots - 1];

        if (ant->cpus[cpu_id].tarefa_atual != NULL) {
            int ultimo_id = ant->cpus[cpu_id].tarefa_atual->id;

            for (int i = 0; i < sistema->qtd_tarefas; i++) {
                if (sistema->tarefas[i].id == ultimo_id) {
                    /* começa a busca na posicao SEGUINTE a quem acabou de rodar */
                    start = (i + 1) % sistema->qtd_tarefas;
                    break;
                }
            }
        }
    }

    /* Percorre o array em ordem circular ate achar a proxima PRONTA */
    for (int j = 0; j < sistema->qtd_tarefas; j++) {
        int idx = (start + j) % sistema->qtd_tarefas;
        if (sistema->tarefas[idx].estado == PRONTA)
            return idx;
    }

    return -1; /* nenhuma tarefa disponivel: CPU sera desligada */
}
