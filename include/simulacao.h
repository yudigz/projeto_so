#ifndef SIMULACAO_H
#define SIMULACAO_H

#include "sistema.h"

void tick(SistemaSimulado* sistema);

int simulacao_finalizada(const SistemaSimulado* sistema);

int avancar(SistemaSimulado* sistema);

int retroceder(SistemaSimulado* sistema);

void modificar_tarefa(SistemaSimulado* sistema, int tarefa_id, int novo_estado, int nova_prioridade, int nova_duracao_restante);

void executar_completo(SistemaSimulado* sistema);

void inspecionar_sistema(const SistemaSimulado* sistema);

#endif