#ifndef CONFIG_H
#define CONFIG_H

#include "sistema.h"

static void strip_newline(char* s);

static int separa_linha_sistema(char* linha, SistemaSimulado* sistema);

static int separa_linha_tarefa(char* linha, Tcb* tcb);

int ler_config(const char* caminho, SistemaSimulado* sistema);

#endif