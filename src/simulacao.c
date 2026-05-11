#include "simulacao.h"
#include <stddef.h>

void tick(SistemaSimulado* sistema){

    for(int i =0; i < sistema->qtd_tarefas; i++){
        if(sistema->tarefas[i].estado == NOVA && sistema->tarefas[i].ingresso == sistema->relogio_global){
            sistema->tarefas[i].estado = PRONTA;
        }
    }

    /* verifica finalizada*/
    for(int i =0; i < sistema->qtd_tarefas; i++){
        if(sistema->tarefas[i].estado != EXECUTANDO) continue;
        
        sistema->tarefas[i].duracao_restante--;

        if(sistema->tarefas[i].duracao_restante == 0){
            int cpu_id = sistema->tarefas[i].cpu_atual;

            /* atualiza o TCB*/
            sistema->tarefas[i].estado = FINALIZADA;
            sistema->tarefas[i].cpu_atual = -1;

            /* libera a cpu*/
            sistema->cpus[cpu_id].tarefa_atual = NULL;
            sistema->cpus[cpu_id].quantum_restante = 0;
        }
    }

    /* verifica quantum */
    for(int i =0; i < sistema->qtd_cpus; i++){
        if(sistema->cpus[i].tarefa_atual == NULL) continue;

        sistema->cpus[i].quantum_restante--;

        if(sistema->cpus[i].quantum_restante <= 0){
            Tcb* t = sistema->cpus[i].tarefa_atual;
            
            /* tarefa volta pra fila de prontos*/
            t->estado = PRONTA;
            t->cpu_atual = -1;

            /* cpu fica libre para escalonamento*/
            sistema->cpus[i].tarefa_atual = NULL;
            sistema->cpus[i].quantum_restante = 0;
        }
    }

    sistema->houve_sorteio = 0;
    sistema->sorteio_tarefa_idx = -1;

    for(int i =0; i < sistema->qtd_cpus; i++){
        /*so reescalona CPUs sem tarefa rodando*/
        if(sistema->cpus[i].tarefa_atual != NULL) continue;

        int idx = sistema->escalonador(sistema, i);

        if(idx == -1){
            /* nenhuma tarefa disponivel, desliga CPU*/
            sistema->cpus[i].ligado = 0;
            sistema->cpus[i].ticks_ociosos_acumulados++;
        } else{
            /* atrinui a tarefa escolhida a esta CPU*/
            sistema->cpus[i].ligado = 1;
            sistema->cpus[i].tarefa_atual = &sistema->tarefas[idx];
            sistema->cpus[i].quantum_restante = sistema->quantum;

            sistema->tarefas[idx].estado = EXECUTANDO;
            sistema->tarefas[idx].cpu_atual = i;
        }
    }

    sistema->relogio_global++;
}