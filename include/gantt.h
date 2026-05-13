#ifndef GANTT_H
#define GANTT_H

#include "sistema.h"
#include "snapshot.h"

void gantt_imprimir(const SistemaSimulado* sistema);

void gantt_legenda(const SistemaSimulado* sistema);

void gantt_exportar_svg(const SistemaSimulado* sistema, const char* caminho);

#endif