#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "tcb.h"
#include "cpu.h"

typedef struct{
    int relogio;
    Tcb* tarefas;
    int qtd_tarefas;
    Cpu* cpus;
    int qtd_cpus;
}Snapshot;

#endif