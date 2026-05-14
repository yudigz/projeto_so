/*
 * gantt.h — visualização do diagrama de Gantt
 *
 * gantt_imprimir() exibe o histórico de execução no terminal com
 * cores ANSI. gantt_legenda() imprime a legenda de cores das tarefas.
 * gantt_exportar_svg() gera um arquivo SVG completo com linhas de
 * tarefas, CPUs, linha de sorteio e legenda.
 */

#ifndef GANTT_H
#define GANTT_H

#include "sistema.h"
#include "snapshot.h"

void gantt_imprimir(const SistemaSimulado* sistema);

void gantt_legenda(const SistemaSimulado* sistema);

void gantt_exportar_svg(const SistemaSimulado* sistema, const char* caminho);

#endif