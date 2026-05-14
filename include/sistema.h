/*
 * sistema.h — estrutura principal do sistema simulado
 *
 * Agrega todas as informações do sistema em execução: relógio global,
 * arrays de tarefas e CPUs, ponteiro para o escalonador ativo, histórico
 * de snapshots para avanço/retrocesso e flags de sorteio.
 */

#ifndef SISTEMA_H
#define SISTEMA_H
 
#include "tcb.h"
#include "cpu.h"
#include "snapshot.h"
 
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
    Snapshot* historico;
    int qtd_snapshots;
    int cap_historico;
    int idx_snapshot_atual; 
} SistemaSimulado;

typedef int (*Escalonador)(SistemaSimulado* sistema, int cpu_id);
 
#endif