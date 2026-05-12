#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "tcb.h"
#include "cpu.h"

struct SistemaSimulado;

typedef struct{
    int relogio;
    Tcb* tarefas;
    int qtd_tarefas;
    Cpu* cpus;
    int qtd_cpus;
    int houve_sorteio;
    int sorteio_tarefa_idx;
}Snapshot;

Snapshot snapshot_capturar(const struct SistemaSimulado* sistema);

void snapshot_restaurar(struct SistemaSimulado* sistema, const Snapshot* s);

#endif