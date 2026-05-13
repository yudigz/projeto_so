#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include "sistema.h"

Escalonador get_escalonador(const char* nome);

int srtf(SistemaSimulado* sistema, int cpu_id);

int priop(SistemaSimulado* sistema, int cpu_id);

#endif