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

    printf("\n--- Iniciando simulacao ---\n");
    while (avancar(&sistema)) {
        printf("Tick %d | ", sistema.relogio_global);
        for (int i = 0; i < sistema.qtd_cpus; i++) {
            if (sistema.cpus[i].tarefa_atual != NULL)
                printf("CPU%d=T%d ", i, sistema.cpus[i].tarefa_atual->id);
            else
                printf("CPU%d=idle ", i);
        }
        printf("\n");
    }
    printf("--- Simulacao finalizada no tick %d ---\n", sistema.relogio_global);

    return 0;
}