#ifndef SIMULACAO_H
#define SIMULACAO_H

#include "sistema.h"

void tick(SistemaSimulado* sistema);
int simulacao_finalizada(const SistemaSimulado* sistema);
int avancar(SistemaSimulado* sistema);

#endif