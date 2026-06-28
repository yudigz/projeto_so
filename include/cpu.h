/*
cpu.h — definição da estrutura de CPU

Representa um processador do sistema simulado. Guarda a tarefa em
execução no momento, o estado ligado/desligado, o quantum restante
e a contagem acumulada de ticks em que ficou ocioso.
 */

#ifndef CPU_H
#define CPU_H

#include "tcb.h"

typedef struct{
    int id;
    Tcb* tarefa_atual;
    int ligado;
    int ticks_ociosos_acumulados;
    int quantum_restante;
}Cpu;


#endif