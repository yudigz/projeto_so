/*
 * main.c — ponto de entrada do simulador
 *
 * Lê o arquivo de configuração, pergunta o modo de execução e dispara a
 * simulação. No modo completo imprime o Gantt no terminal e exporta o SVG.
 * No modo passo-a-passo entrega o controle para o TUI interativo.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sistema.h"
#include "config.h"
#include "simulacao.h"
#include "gantt.h"
#include "tui.h"

int main(void) {
    char caminho[256];
    int modo;

    srand((unsigned int)time(NULL));

    printf("=== Simulador de SO ===\n");
    printf("Caminho do arquivo de configuracao: ");
    if (fgets(caminho, sizeof(caminho), stdin) == NULL) return 1;
    caminho[strcspn(caminho, "\n")] = '\0';

    printf("Modo: 1=passo-a-passo  2=execucao completa\nOpcao: ");
    if (scanf("%d", &modo) != 1) return 1;
    while (getchar() != '\n');

    SistemaSimulado sistema = {0};
    if (ler_config(caminho, &sistema) != 0) return 1;

    if (modo == 2) {
        executar_completo(&sistema);
        printf("\n");
        gantt_legenda(&sistema);
        gantt_imprimir(&sistema);
        gantt_exportar_svg(&sistema, "gantt.svg");
        printf("SVG exportado para gantt.svg\n");
    } else {
        tui_loop(&sistema);
    }

    return 0;
}
