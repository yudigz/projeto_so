#ifndef SISTEMA_H
#define SISTEMA_H
 
#include "tcb.h"
#include "cpu.h"
 
typedef struct SistemaSimulado {
    int relogio_global;
    int quantum;
    char algoritmo[16];
    Tcb* tarefas;
    int qtd_tarefas;
    Cpu* cpus;
    int qtd_cpus;
    int (*escalonador)(struct SistemaSimulado* sistema, int cpu_id);
    int houve_sorteio;
    int sorteio_tarefa_idx;
} SistemaSimulado;

typedef int (*Escalonador)(SistemaSimulado* sistema, int cpu_id);
 
#endif