#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sistema.h"
#include "config.h"
#include "simulacao.h"
#include "gantt.h"

int main(void) {
    srand(time(NULL));
    SistemaSimulado sistema = {0};
    if (ler_config("config_teste.txt", &sistema) != 0) return 1;

    executar_completo(&sistema);
    printf("\n");
    gantt_imprimir(&sistema);
    return 0;
}