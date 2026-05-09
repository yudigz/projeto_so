#ifndef SISTEMA_H
#define SISTEMA_H

#include "tcb.h"
#include "cpu.h"

typedef struct{
    int relogio_global;
    int quantum;
    char algoritmo[16];
    Tcb* tarefas;
    int qtde_tarefas;
    Cpu* cpu;
    int qtde_cpu;
}SistemaSimulado;

#endif