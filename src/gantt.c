/*
gantt.c — visualização do gráfico de Gantt

Três funções públicas:
  gantt_imprimir() — imprime o Gantt no terminal com cores ANSI
  gantt_legenda() — imprime a legenda dos símbolos no terminal
  gantt_exportar_svg() — gera o arquivo gantt.svg com o gráfico completo,
                         linhas de CPU, linha de sorteio e legenda embutida
 */

#include "gantt.h"
#include <stdio.h>

#define ANSI_RESET "\x1b[0m"

void gantt_imprimir(const SistemaSimulado* sistema) {
    int n_ticks = sistema->idx_snapshot_atual + 1;
    int n_tarefas = sistema->qtd_tarefas;
    int n_cpus = sistema->qtd_cpus;

    if (n_ticks <= 0) {
        printf("Nenhum tick executado ainda.\n");
        return;
    }

    /* maior ID no topo, menor ID mais proximo do eixo X */
    for (int ti = n_tarefas - 1; ti >= 0; ti--) {
        int id = sistema->tarefas[ti].id;
        printf("T%d |", id);

        for (int tick = 0; tick < n_ticks; tick++) {
            const Snapshot* s = &sistema->historico[tick];
            const Snapshot* s_ant = tick > 0 ? &sistema->historico[tick-1] : NULL;
            const Tcb* t = &s->tarefas[ti];
            const Tcb* t_ant = s_ant ? &s_ant->tarefas[ti] : NULL;

            int chegou = t_ant == NULL
                           ? (t->estado == PRONTA || t->estado == EXECUTANDO)
                           : (t_ant->estado == NOVA && t->estado != NOVA);
            int terminou = t_ant && t_ant->estado != FINALIZADA && t->estado == FINALIZADA;

            if (chegou) {
                printf(" \xE2\x96\xBC ");
            } else if (terminou) {
                printf(" \xE2\x96\xA0 ");
            } else if (t->estado == EXECUTANDO) {
                printf("\x1b[48;2;%d;%d;%dm %d " ANSI_RESET,
                       t->cor.r, t->cor.g, t->cor.b, t->cpu_atual);
            } else if (t->estado == SUSPENSA) {
                printf("\x1b[48;2;0;0;0m   " ANSI_RESET);
            } else {
                printf("   ");
            }
        }
        printf("\n");
    }

    for (int ci = 0; ci < n_cpus; ci++) {
        printf("C%d |", ci);
        for (int tick = 0; tick < n_ticks; tick++) {
            const Snapshot* s = &sistema->historico[tick];
            const Cpu* c = &s->cpus[ci];
            if (c->tarefa_atual != NULL) {
                printf("\x1b[48;2;%d;%d;%dm   " ANSI_RESET,
                       c->tarefa_atual->cor.r,
                       c->tarefa_atual->cor.g,
                       c->tarefa_atual->cor.b);
            } else {
                printf("\x1b[48;2;180;30;30mOFF" ANSI_RESET);
            }
        }
        printf("\n");
    }

    /* linha de eventos globais (sorteio) */
    printf("   |");
    for (int tick = 0; tick < n_ticks; tick++) {
        const Snapshot* s = &sistema->historico[tick];
        if (s->houve_sorteio)
            printf(" \xE2\x9A\x80 ");
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
    printf(" \xE2\x96\xBC  chegada da tarefa\n");
    printf(" \xE2\x96\xA0  termino da tarefa\n");
    printf(" \xE2\x9A\x80  desempate por sorteio\n");
    printf("\x1b[48;2;0;0;0m   " ANSI_RESET " tarefa suspensa\n");
    printf("    ausencia de cor = tarefa pronta (aguardando CPU)\n");
    printf("\x1b[48;2;180;30;30mOFF" ANSI_RESET " processador desligado\n\n");

    printf("Cores das tarefas:\n");
    for (int i = 0; i < sistema->qtd_tarefas; i++) {
        const Tcb* t = &sistema->tarefas[i];
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

    int n_ticks = sistema->qtd_snapshots;
    int n_tarefas = sistema->qtd_tarefas;
    int n_cpus = sistema->qtd_cpus;
    int cw = 30, ch = 30;
    int ml = 60, mt = 20;

    int y_tarefas = mt;
    int y_cpus = y_tarefas + n_tarefas * ch + 10;
    int y_loteria = y_cpus + n_cpus * ch + 10;
    int y_xaxis = y_loteria + ch + 10;

    int leg_item_h = 22;
    int y_legenda = y_xaxis + 20;
    int leg_h = 20 + (6 + n_tarefas) * leg_item_h + 10;

    int largura_gantt = ml + n_ticks * cw;
    int largura = largura_gantt > 420 ? largura_gantt : 420;
    int altura = y_legenda + leg_h;

    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\">\n", largura, altura);
    fprintf(f, "<rect width=\"%d\" height=\"%d\" fill=\"white\"/>\n", largura, altura);

    /* linhas das tarefas */
    for (int ti = n_tarefas - 1; ti >= 0; ti--) {
        int linha_visual = (n_tarefas - 1 - ti);
        int y = y_tarefas + linha_visual * ch;
        int id = sistema->tarefas[ti].id;

        fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"12\" text-anchor=\"end\">T%d</text>\n",
                ml - 4, y + ch/2 + 4, id);

        for (int tick = 0; tick < n_ticks; tick++) {
            const Snapshot* s = &sistema->historico[tick];
            const Snapshot* s_ant = tick > 0 ? &sistema->historico[tick-1] : NULL;
            const Tcb* t = &s->tarefas[ti];
            const Tcb* t_ant = s_ant ? &s_ant->tarefas[ti] : NULL;
            int x = ml + tick * cw;

            int chegou = t_ant == NULL
                           ? (t->estado == PRONTA || t->estado == EXECUTANDO)
                           : (t_ant->estado == NOVA && t->estado != NOVA);
            int terminou = t_ant && t_ant->estado != FINALIZADA
                           && t->estado == FINALIZADA;

            if (t->estado == EXECUTANDO) {
                fprintf(f, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\""
                           " fill=\"#%02X%02X%02X\"/>\n",
                        x, y, cw, ch, t->cor.r, t->cor.g, t->cor.b);
            } else if (t->estado == SUSPENSA) {
                fprintf(f, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\""
                           " fill=\"#000000\"/>\n",
                        x, y, cw, ch);
            }

            if (chegou) {
                const char* cor_sym = (t->estado == EXECUTANDO) ? "white" : "black";
                fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"14\" fill=\"%s\""
                           " text-anchor=\"middle\">&#9660;</text>\n",
                        x + cw/2, y + ch/2 + 5, cor_sym);
            } else if (terminou) {
                fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"14\" fill=\"black\""
                           " text-anchor=\"middle\">&#9632;</text>\n",
                        x + cw/2, y + ch/2 + 5);
            } else if (t->estado == EXECUTANDO) {
                fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"11\" fill=\"white\""
                           " text-anchor=\"middle\">%d</text>\n",
                        x + cw/2, y + ch/2 + 4, t->cpu_atual);
            }
        }
    }

    /* linhas das CPUs */
    for (int ci = 0; ci < n_cpus; ci++) {
        int y = y_cpus + ci * ch;
        fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"12\" text-anchor=\"end\">C%d</text>\n",
                ml - 4, y + ch/2 + 4, ci);

        for (int tick = 0; tick < n_ticks; tick++) {
            int x = ml + tick * cw;
            const Snapshot* s = &sistema->historico[tick];
            const Cpu* c = &s->cpus[ci];

            if (c->tarefa_atual != NULL) {
                const Tcb* t = c->tarefa_atual;
                fprintf(f, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\""
                           " fill=\"#%02X%02X%02X\"/>\n",
                        x, y, cw, ch, t->cor.r, t->cor.g, t->cor.b);
                fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"10\" fill=\"white\""
                           " text-anchor=\"middle\">T%d</text>\n",
                        x + cw/2, y + ch/2 + 4, t->id);
            } else {
                fprintf(f, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\""
                           " fill=\"#b41e1e\"/>\n",
                        x, y, cw, ch);
                fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"9\" fill=\"white\""
                           " text-anchor=\"middle\">OFF</text>\n",
                        x + cw/2, y + ch/2 + 3);
            }
        }
    }

    /* linha de sorteio */
    fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"11\" text-anchor=\"end\">sort</text>\n",
            ml - 4, y_loteria + ch/2 + 4);
    for (int tick = 0; tick < n_ticks; tick++) {
        int x = ml + tick * cw;
        if (sistema->historico[tick].houve_sorteio) {
            fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"16\""
                       " text-anchor=\"middle\">&#9856;</text>\n",
                    x + cw/2, y_loteria + ch/2 + 6);
        }
    }

    /* linhas de grade verticais e numeracao do eixo X */
    for (int tick = 0; tick < n_ticks; tick++) {
        int x = ml + tick * cw;
        fprintf(f, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\""
                   " stroke=\"#ccc\" stroke-width=\"1\"/>\n",
                x, mt, x, y_xaxis - 10);
        fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"10\" text-anchor=\"middle\">%d</text>\n",
                x + cw/2, y_xaxis, tick + 1);
    }

    /* linhas separadoras entre secoes */
    fprintf(f, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\""
               " stroke=\"#999\" stroke-width=\"1\"/>\n",
            ml - 55, y_cpus - 5, largura, y_cpus - 5);
    fprintf(f, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\""
               " stroke=\"#999\" stroke-width=\"1\"/>\n",
            ml - 55, y_loteria - 5, largura, y_loteria - 5);

    /* legenda */
    int lx = ml;
    int ly = y_legenda;

    fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"13\" font-weight=\"bold\">Legenda:</text>\n",
            lx, ly);
    ly += leg_item_h;

    fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"14\">&#9660;</text>\n", lx, ly);
    fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"12\">  chegada da tarefa</text>\n",
            lx + 18, ly);
    ly += leg_item_h;

    fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"14\">&#9632;</text>\n", lx, ly);
    fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"12\">  termino da tarefa</text>\n",
            lx + 18, ly);
    ly += leg_item_h;

    fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"14\">&#9856;</text>\n", lx, ly);
    fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"12\">  desempate por sorteio</text>\n",
            lx + 18, ly);
    ly += leg_item_h;

    fprintf(f, "<rect x=\"%d\" y=\"%d\" width=\"14\" height=\"14\" fill=\"#000000\"/>\n",
            lx, ly - 12);
    fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"12\">  tarefa suspensa</text>\n",
            lx + 18, ly);
    ly += leg_item_h;

    fprintf(f, "<rect x=\"%d\" y=\"%d\" width=\"14\" height=\"14\""
               " fill=\"white\" stroke=\"#999\" stroke-width=\"1\"/>\n",
            lx, ly - 12);
    fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"12\">  tarefa pronta (aguardando CPU)</text>\n",
            lx + 18, ly);
    ly += leg_item_h;

    fprintf(f, "<rect x=\"%d\" y=\"%d\" width=\"14\" height=\"14\" fill=\"#b41e1e\"/>\n",
            lx, ly - 12);
    fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"12\">  processador desligado</text>\n",
            lx + 18, ly);
    ly += leg_item_h;

    for (int i = 0; i < n_tarefas; i++) {
        const Tcb* t = &sistema->tarefas[i];
        fprintf(f, "<rect x=\"%d\" y=\"%d\" width=\"14\" height=\"14\""
                   " fill=\"#%02X%02X%02X\"/>\n",
                lx, ly - 12, t->cor.r, t->cor.g, t->cor.b);
        fprintf(f, "<text x=\"%d\" y=\"%d\" font-size=\"12\">  T%d</text>\n",
                lx + 18, ly, t->id);
        ly += leg_item_h;
    }

    fprintf(f, "</svg>\n");
    fclose(f);
    printf("SVG exportado para '%s'\n", caminho);
}
