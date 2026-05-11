#include "sistema.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    srand(time(NULL));

    SistemaSimulado sistema = {0};  /* zera todos os campos */

    if (ler_config("config_teste.txt", &sistema) != 0) {
        return 1;
    }

    printf("Escalonador carregando: %s\n", sistema.algoritmo);
    printf("Ponteiro valido: %s\n",sistema.escalonador != NULL ? "sim" : "nao");

    return 0;
}