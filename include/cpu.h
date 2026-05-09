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