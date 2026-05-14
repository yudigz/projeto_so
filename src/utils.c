/*
 * utils.c — funções utilitárias gerais
 *
 * Contém helpers usados em diferentes partes do projeto. Por enquanto
 * só tem a conversão de string para minúsculas, usada na leitura do
 * arquivo de configuração para tratar nomes de algoritmo sem distinção
 * de maiúsculas/minúsculas.
 */

#include "utils.h"
#include <string.h>

void str_to_lower(char* s){
    for(int i = 0; s[i] != '\0'; i++){
        s[i] = tolower((unsigned char)s[i]);
    }
}
