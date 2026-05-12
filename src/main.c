#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sistema.h"
#include "config.h"
#include "simulacao.h"

int main(void) {
    srand(time(NULL));
    SistemaSimulado sistema = {0};
    if (ler_config("config_teste.txt", &sistema) != 0) return 1;

    printf("\n--- Avancando 5 ticks ---\n");
    for (int i = 0; i < 5; i++) avancar(&sistema);
    printf("Tick atual: %d\n", sistema.relogio_global);

    printf("\n--- Retrocedendo 2 ticks ---\n");
    retroceder(&sistema);
    retroceder(&sistema);
    printf("Tick atual: %d\n", sistema.relogio_global);

    printf("\n--- Estado das CPUs apos retroceder ---\n");
    for (int i = 0; i < sistema.qtd_cpus; i++) {
        if (sistema.cpus[i].tarefa_atual != NULL)
            printf("CPU%d=T%d\n", i, sistema.cpus[i].tarefa_atual->id);
        else
            printf("CPU%d=idle\n", i);
    }
    return 0;
}