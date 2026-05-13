#include "gantt.h"
#include <stdio.h>

#define ANSI_RESET "\x1b[0m"

void gantt_imprimir(const SistemaSimulado* sistema) {
    int n_ticks   = sistema->idx_snapshot_atual + 1;
    int n_tarefas = sistema->qtd_tarefas;

    if (n_ticks <= 0) {
        printf("Nenhum tick executado ainda.\n");
        return;
    }

    /* maior ID no topo, menor ID mais proximo do eixo X */
    for (int ti = n_tarefas - 1; ti >= 0; ti--) {
        int id = sistema->tarefas[ti].id;
        printf("T%d |", id);

        for (int tick = 0; tick < n_ticks; tick++) {
            const Snapshot* s     = &sistema->historico[tick];
            const Snapshot* s_ant = tick > 0 ? &sistema->historico[tick-1] : NULL;
            const Tcb* t     = &s->tarefas[ti];
            const Tcb* t_ant = s_ant ? &s_ant->tarefas[ti] : NULL;

            int chegou   = t_ant == NULL
                           ? (t->estado == PRONTA || t->estado == EXECUTANDO)
                           : (t_ant->estado == NOVA && t->estado != NOVA);
            int terminou = t_ant && t_ant->estado != FINALIZADA && t->estado == FINALIZADA;

            if (chegou) {
                printf(" ▼ ");
            } else if (terminou) {
                printf(" ■ ");
            } else if (t->estado == EXECUTANDO) {
                printf("\x1b[48;2;%d;%d;%dm %d " ANSI_RESET,
                       sistema->tarefas[ti].cor.r,
                       sistema->tarefas[ti].cor.g,
                       sistema->tarefas[ti].cor.b,
                       t->cpu_atual);
            } else if (t->estado == SUSPENSA) {
                printf("\x1b[48;2;30;30;30m   " ANSI_RESET);
            } else {
                printf("   ");
            }
        }
        printf("\n");
    }

    /* linha de eventos globais (sorteio) */
    printf("   |");
    for (int tick = 0; tick < n_ticks; tick++) {
        const Snapshot* s = &sistema->historico[tick];
        if (s->houve_sorteio)
            printf(" ⚀ ");
        else
            printf("   ");
    }
    printf("\n");

    /* eixo X */
    printf("    ");
    for (int tick = 0; tick < n_ticks; tick++)
        printf("%2d ", tick + 1);
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
        printf("\x1b[48;2;%d;%d;%dm   " ANSI_RESET " T%d\n",
               t->cor.r, t->cor.g, t->cor.b, t->id);
    }
    printf("\n");
}

void gantt_exportar_svg(const SistemaSimulado* sistema, const char* caminho) {
    FILE* f = fopen(caminho, "w");
    if (f == NULL) {
        fprintf(stderr, "Erro: nao foi possivel criar '%s'\n", caminho);
        return;
    }

    int n_ticks   = sistema->qtd_snapshots;
    int n_tarefas = sistema->qtd_tarefas;
    int cw = 30, ch = 30;
    int ml = 40, mt = 20, mb = 40;
    int largura = ml + n_ticks * cw;
    int altura  = mt + n_tarefas * ch + mb;

    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\">\n", largura, altura);
    fprintf(f, "<rect width=\"%d\" height=\"%d\" fill=\"white\"/>\n", largura, altura);

    for (int ti = n_tarefas - 1; ti >= 0; ti--) {
        int linha_visual = (n_tarefas - 1 - ti);
        int y = mt + linha_visual * ch;
        int id = sistema->tarefas[ti].id;

        fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"12\" text-anchor=\"end\">T%d</text>\n",
                ml - 4, y + ch/2 + 4, id);

        for (int tick = 0; tick < n_ticks; tick++) {
            const Snapshot* s      = &sistema->historico[tick];
            const Snapshot* s_ant  = tick > 0       ? &sistema->historico[tick-1] : NULL;
            const Snapshot* s_prox = tick < n_ticks-1 ? &sistema->historico[tick+1] : NULL;
            const Tcb* t     = &s->tarefas[ti];
            const Tcb* t_ant = s_ant ? &s_ant->tarefas[ti] : NULL;
            int x = ml + tick * cw;

            int chegou = t_ant == NULL
                         ? (t->estado == PRONTA || t->estado == EXECUTANDO)
                         : (t_ant->estado == NOVA && t->estado != NOVA);

            /* ultimo tick EXECUTANDO: proximo ja e FINALIZADA ou nao existe */
            int terminou = (t->estado == EXECUTANDO) &&
                           (s_prox == NULL || s_prox->tarefas[ti].estado == FINALIZADA);

            if (t->estado == EXECUTANDO) {
                /* fundo colorido */
                fprintf(f, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"#%02X%02X%02X\"/>\n",
                        x, y, cw, ch,
                        sistema->tarefas[ti].cor.r,
                        sistema->tarefas[ti].cor.g,
                        sistema->tarefas[ti].cor.b);

                /* icone ou numero da CPU por cima */
                if (chegou)
                    fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"14\" fill=\"white\" text-anchor=\"middle\">&#9660;</text>\n",
                            x + cw/2, y + ch/2 + 5);
                else if (terminou)
                    fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"14\" fill=\"white\" text-anchor=\"middle\">&#9632;</text>\n",
                            x + cw/2, y + ch/2 + 5);
                else
                    fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"11\" fill=\"white\" text-anchor=\"middle\">%d</text>\n",
                            x + cw/2, y + ch/2 + 4, t->cpu_atual);

            } else if (t->estado == SUSPENSA) {
                fprintf(f, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"#1e1e1e\"/>\n",
                        x, y, cw, ch);
            }
        }
    }

    /* eixo X */
    for (int tick = 0; tick < n_ticks; tick++) {
        int x = ml + tick * cw;
        fprintf(f, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"#ccc\" stroke-width=\"1\"/>\n",
                x, mt, x, mt + n_tarefas * ch);
        fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"10\" text-anchor=\"middle\">%d</text>\n",
                x + cw/2, mt + n_tarefas * ch + 14, tick + 1);
    }

    fprintf(f, "</svg>\n");
    fclose(f);
    printf("SVG exportado para '%s'\n", caminho);
}