#ifndef SISTEMA_H
#define SISTEMA_H

#include "tcb.h"
#include "cpu.h"

struct SistemaSimulado;
typedef int (*Escalonador)(struct SistemaSimulado* sistema, int cpu_id);

typedef struct{
    int relogio_global;
    int quantum;
    char algoritmo[16];
    Tcb* tarefas;
    int qtd_tarefas;
    Cpu* cpus;
    int qtd_cpus;
    Escalonador escalonador;
}SistemaSimulado;

#endif