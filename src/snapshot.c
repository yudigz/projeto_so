#include "snapshot.h"
#include "sistema.h"

#include <stdlib.h>
#include <string.h>

Snapshot snapshot_capturar(const SistemaSimulado* sistema){
    Snapshot s;
    s.relogio = sistema->relogio_global;
    s.qtd_tarefas = sistema->qtd_tarefas;
    s.qtd_cpus = sistema->qtd_cpus;

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