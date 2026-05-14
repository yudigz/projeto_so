/*
utils.h — funções utilitárias genéricas

Expõe str_to_lower(), que converte uma string para minúsculas
in-place. Usada na leitura do config para normalizar o nome
do algoritmo antes de passá-lo ao get_escalonador().
 */

#ifndef UTILS_H
#define UTILS_H

#include <ctype.h>

void str_to_lower(char* s);

#endif