#ifndef CONFIG_H
#define CONFIG_H

#include "sistema.h"

static void strip_newline(char* s);

int ler_config(const char* caminho, SistemaSimulado* sistema);

static int separa_linha_sistema(char* linha, SistemaSimulado* sistema);

#endif