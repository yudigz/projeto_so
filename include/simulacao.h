/*
 * simulacao.h — motor de simulação e controle de avanço/retrocesso
 *
 * tick() avança o relógio um passo: transiciona estados, chama o
 * escalonador e salva um snapshot. avancar()/retroceder() navegam
 * pelo histórico sem recalcular. modificar_tarefa() permite alterar
 * um TCB em tempo de execução. executar_completo() roda até o fim.
 * inspecionar_sistema() despeja o estado atual no stdout.
 */

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