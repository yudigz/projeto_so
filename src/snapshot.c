/*
snapshot.c — captura e restauração do estado do sistema

A cada tick é tirada uma "foto" completa do sistema (tarefas + CPUs).
Isso permite retroceder a simulação restaurando qualquer estado anterior.
Os ponteiros de tarefa_atual dentro das CPUs precisam ser reajustados
depois de copiar, pois apontam para arrays diferentes (snapshot vs sistema).
 */

#include "snapshot.h"
#include "sistema.h"

#include <stdlib.h>
#include <string.h>

Snapshot snapshot_capturar(const SistemaSimulado* sistema){
    Snapshot s;
    s.relogio = sistema->relogio_global;
    s.qtd_tarefas = sistema->qtd_tarefas;
    s.qtd_cpus = sistema->qtd_cpus;
    s.houve_sorteio = sistema->houve_sorteio;
    s.sorteio_tarefa_idx = sistema->sorteio_tarefa_idx;

    /* copia profunda das tarefas*/
    s.tarefas = malloc(sistema->qtd_tarefas * sizeof(Tcb));
    memcpy(s.tarefas, sistema->tarefas, sistema->qtd_tarefas * sizeof(Tcb));

    /* copia profunda das CPUs*/
    s.cpus = malloc(sistema->qtd_cpus * sizeof(Cpu));
    memcpy(s.cpus, sistema->cpus, sistema->qtd_cpus * sizeof(Cpu));

    /* muda as tarefas da CPU do snapshot p/ as tarefas da snapshot*/
    for(int i =0; i< s.qtd_cpus; i++){
        if(s.cpus[i].tarefa_atual != NULL){
            int idx = sistema->cpus[i].tarefa_atual - sistema->tarefas;
            s.cpus[i].tarefa_atual = &s.tarefas[idx];
        }
    }
    return s;
}

void snapshot_restaurar(SistemaSimulado* sistema, const Snapshot* s){
    sistema->relogio_global = s->relogio;

    memcpy(sistema->tarefas, s->tarefas, s->qtd_tarefas * sizeof(Tcb));
    memcpy(sistema->cpus, s->cpus, s->qtd_cpus * sizeof(Cpu));

    /* recoloca a tarefa_atual p/ apontar ao array do sistema, nao pro de snapshot(copia separada) */
    for(int i =0; i < s->qtd_cpus; i++){
        if(s->cpus[i].tarefa_atual != NULL){
            int idx = s->cpus[i].tarefa_atual - s->tarefas;
            sistema->cpus[i].tarefa_atual = &sistema->tarefas[idx];
        }else{
            sistema->cpus[i].tarefa_atual = NULL;
        }
    }
}
