/*
snapshot.h — definição do Snapshot e assinaturas de captura/restauração

Um Snapshot é uma cópia completa do estado do sistema em um dado tick.
Usado pelo mecanismo de retrocesso para restaurar qualquer ponto anterior
da simulação sem precisar reexecutar desde o início.
 */

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