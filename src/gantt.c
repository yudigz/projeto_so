#include "gantt.h"
#include <stdio.h>

#define ANSI_RESET "\x1b[0m"
#define ANSI_BG(r,g,b)  /* macro pra montar a string de cor*/

void gantt_imprimir(const SistemaSimulado* sistema){
    int n_ticks = sistema->qtd_snapshots;
    int n_tarefas = sistema->qtd_tarefas;

    if(n_ticks == 0){
        printf("Nenhum tick executado ainda.\n");
        return;
    }

    for(int ti = n_tarefas-1; ti>=0; ti--){
        int id = sistema->tarefas[ti].id;
        printf("T%d |", id);

        for (int tick = 0; tick < n_ticks; tick++) {
            const Snapshot* s = &sistema->historico[tick];
            const Snapshot* s_ant = tick > 0 ? &sistema->historico[tick-1] : NULL;
            const Tcb* t = &s->tarefas[ti];
            const Tcb* t_ant = s_ant ? &s_ant->tarefas[ti] : NULL;

            /* chegada: era NOVA (ou inexistente) e agora é PRONTA ou EXECUTANDO */
            int chegou = t_ant == NULL 
                        ? (t->estado == PRONTA || t->estado == EXECUTANDO)
                        : (t_ant->estado == NOVA && t->estado != NOVA);

            /* terminou: era qualquer coisa e agora é FINALIZADA */
            int terminou = t_ant && t_ant->estado != FINALIZADA && t->estado == FINALIZADA;

            if (chegou) {
                printf(" ▼ ");
            } else if (terminou) {
                printf(" ■ ");
            } else if (t->estado == EXECUTANDO) {
                printf("\x1b[48;2;%d;%d;%dm %d " ANSI_RESET, sistema->tarefas[ti].cor.r, sistema->tarefas[ti].cor.g, sistema->tarefas[ti].cor.b, t->cpu_atual);
            } else if (t->estado == SUSPENSA) {
                printf("\x1b[48;2;30;30;30m   " ANSI_RESET);
            } else {
                printf("   ");
            }
        }
        printf("\n");
    }

    printf("   |");
    for (int tick = 0; tick < n_ticks; tick++) {
        const Snapshot* s = &sistema->historico[tick];
        if (s->houve_sorteio)
            printf(" ⚀ ");
        else
            printf("   ");
    }
    printf("\n");

    /* eixo X com numeração dos ticks */
    printf("    ");
    for (int tick = 0; tick < n_ticks; tick++) {
        printf("%2d ", tick + 1);
    }
    printf("\n");
}

void gantt_legenda(const SistemaSimulado* sistema) {
    printf("--- Legenda ---\n");
    printf(" ▼  chegada da tarefa\n");
    printf(" ■  termino da tarefa\n");
    printf(" ⚀  desempate por sorteio\n");
    printf("\x1b[48;2;30;30;30m   " ANSI_RESET " tarefa suspensa\n");
    printf("    ausencia de cor = tarefa pronta (aguardando CPU)\n\n");

    printf("Cores das tarefas:\n");
    for (int i = 0; i < sistema->qtd_tarefas; i++) {
        Tcb* t = &sistema->tarefas[i];
        printf("\x1b[48;2;%d;%d;%dm   " ANSI_RESET " T%d\n", t->cor.r, t->cor.g, t->cor.b, t->id);
    }
    printf("\n");
}