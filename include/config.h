/*
config.h — interface de leitura do arquivo de configuração

Expõe ler_config(), que popula um SistemaSimulado a partir de um
arquivo de texto
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "sistema.h"

int ler_config(const char* caminho, SistemaSimulado* sistema);

#endif