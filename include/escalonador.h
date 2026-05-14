/*
 * escalonador.h — interface de carregamento de escalonadores
 *
 * get_escalonador() retorna o ponteiro de função correspondente ao
 * nome informado: tenta os algoritmos embutidos (srtf, priop) e,
 * se não encontrar, tenta carregar dinamicamente um plugin
 * ./escalonadores/lib{nome}.so via dlopen/dlsym.
 */

#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include "sistema.h"

Escalonador get_escalonador(const char* nome);

int srtf(SistemaSimulado* sistema, int cpu_id);

int priop(SistemaSimulado* sistema, int cpu_id);

#endif