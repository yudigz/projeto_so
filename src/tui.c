/*
tui.c — interface interativa de terminal (modo passo-a-passo)

Desenha o estado do simulador numa caixa de texto com quatro seções:
cabeçalho, Gantt, tabela de tarefas/CPUs e legenda. O loop principal
lê comandos do usuário (n, b, m, i, q) para avançar, retroceder,
modificar tarefas, inspecionar e sair.

  printf("\x1b[48;2;180;30;30mOFF" ANSI_RESET);
           ↑    ↑  ↑            ↑
          ESC  48=fundo  2=modo RGB  R;G;B

48 = cor de fundo (foreground seria 38)
2 = modo RGB verdadeiro (24-bit)
180;30;30 = vermelho escuro (R=180, G=30, B=30)
ANSI_RESET = \x1b[0m → apaga toda formatação
 */

#include "tui.h"
#include "simulacao.h"
#include "gantt.h"
#include <stdio.h>
#include <string.h>

#define ANSI_RESET "\x1b[0m"

/* caracteres de desenho de caixa em UTF-8 */
#define TL "\xe2\x94\x8c"   /* ┌   */
#define TR "\xe2\x94\x90"   /* ┐   */
#define BL "\xe2\x94\x94"   /* └   */
#define BR "\xe2\x94\x98"   /* ┘   */
#define HL "\xe2\x94\x80"   /* ─   */
#define VL "\xe2\x94\x82"   /* │   */
#define ML "\xe2\x94\x9c"   /* ├   */
#define MR "\xe2\x94\xa4"   /* ┤   */

#define W   70      /* largura visual total */
#define IW  68      /* largura interna */
#define LC  34      /* largura da coluna esquerda (secoes de duas colunas) */
#define RC  33      /* largura da coluna direita  (secoes de duas colunas) */

/* ── helpers de desenho de caixa ─────────────────────── */

static void linha_topo(void) {
    int i;
    printf(TL);
    for (i = 0; i < IW; i++) printf(HL);
    printf(TR "\n");
}

static void linha_sep(void) {
    int i;
    printf(ML);
    for (i = 0; i < IW; i++) printf(HL);
    printf(MR "\n");
}

static void linha_base(void) {
    int i;
    printf(BL);
    for (i = 0; i < IW; i++) printf(HL);
    printf(BR "\n");
}

static void linha_plain(const char* s) {
    int len = (int)strlen(s);
    int pad = IW - 1 - len;
    int i;
    if (pad < 0) pad = 0;
    printf(VL " %s", s);
    for (i = 0; i < pad; i++) putchar(' ');
    printf(VL "\n");
}

/* igual a linha_plain mas recebe largura visual explícita — necessário
   quando a string tem caracteres UTF-8 multibyte (símbolo = 1 coluna, 3 bytes) */
static void linha_vis(const char* s, int vis) {
    int pad = IW - 1 - vis;
    int i;
    if (pad < 0) pad = 0;
    printf(VL " %s", s);
    for (i = 0; i < pad; i++) putchar(' ');
    printf(VL "\n");
}

static void linha_centrada(const char* s) {
    int len = (int)strlen(s);
    int total = IW - len;
    int pl, pr, i;
    if (total < 0) total = 0;
    pl = total / 2;
    pr = total - pl;
    printf(VL);
    for (i = 0; i < pl; i++) putchar(' ');
    printf("%s", s);
    for (i = 0; i < pr; i++) putchar(' ');
    printf(VL "\n");
}

/* ── auxiliares ──────────────────────────────────────── */

static int max_id_digs(const SistemaSimulado* s) {
    int i, max = 0;
    for (i = 0; i < s->qtd_tarefas; i++)
        if (s->tarefas[i].id > max) max = s->tarefas[i].id;
    return max >= 100 ? 3 : max >= 10 ? 2 : 1;
}

/* ── secao 1: cabecalho ──────────────────────────────── */

static void secao_cabecalho(const SistemaSimulado* s) {
    char buf[128];
    linha_centrada("SIMULADOR DE SO MULTITAREFA PREEMPTIVO");
    snprintf(buf, sizeof(buf), "Algoritmo: %-6s  Quantum: %d  Tick: %d",
             s->algoritmo, s->quantum, s->relogio_global);
    linha_centrada(buf);
}

/* ── secao 2: gantt ──────────────────────────────────── */

static void secao_gantt(const SistemaSimulado* s) {
    int n_ticks   = s->idx_snapshot_atual + 1;
    int n_tarefas = s->qtd_tarefas;
    int n_cpus    = s->qtd_cpus;
    int ti, tick, p, digs, pfx, avail, max_t, start, visible, row_vis, rpad;

    if (n_ticks <= 0) {
        linha_plain("Gantt: nenhum tick executado ainda.");
        return;
    }

    digs  = max_id_digs(s);
    pfx   = digs + 4;
    avail = IW - pfx;
    max_t = avail / 3;
    if (max_t < 1) max_t = 1;

    start   = (n_ticks > max_t) ? n_ticks - max_t : 0;
    visible = n_ticks - start;
    row_vis = pfx + visible * 3;
    rpad    = IW - row_vis;
    if (rpad < 0) rpad = 0;

    linha_plain("Gantt:");

    /* tarefas: maior ID no topo */
    for (ti = n_tarefas - 1; ti >= 0; ti--) {
        int id = s->tarefas[ti].id;
        printf(VL " T%-*d |", digs, id);

        for (tick = start; tick < n_ticks; tick++) {
            const Snapshot* sv = &s->historico[tick];
            const Snapshot* sa = tick > 0 ? &s->historico[tick - 1] : NULL;
            const Tcb* t  = &sv->tarefas[ti];
            const Tcb* ta = sa ? &sa->tarefas[ti] : NULL;

            int chegou   = ta == NULL
                           ? (t->estado == PRONTA || t->estado == EXECUTANDO)
                           : (ta->estado == NOVA && t->estado != NOVA);
            int terminou = ta != NULL
                           && ta->estado != FINALIZADA
                           && t->estado == FINALIZADA;

            if (chegou) {
                printf(" \xe2\x96\xbc ");              /* ▼ */
            } else if (terminou) {
                printf(" \xe2\x96\xa0 ");              /* ■ */
            } else if (t->estado == EXECUTANDO) {
                printf("\x1b[48;2;%d;%d;%dm %d " ANSI_RESET,
                       s->tarefas[ti].cor.r,
                       s->tarefas[ti].cor.g,
                       s->tarefas[ti].cor.b,
                       t->cpu_atual);
            } else if (t->estado == SUSPENSA) {
                printf("\x1b[48;2;0;0;0m   " ANSI_RESET);
            } else {
                printf("   ");
            }
        }
        for (p = 0; p < rpad; p++) putchar(' ');
        printf(VL "\n");
    }

    /* linhas das CPUs: bloco colorido quando executando, OFF vermelho quando ociosa */
    for (int ci = 0; ci < n_cpus; ci++) {
        printf(VL " C%-*d |", digs, ci);
        for (tick = start; tick < n_ticks; tick++) {
            const Snapshot* sv = &s->historico[tick];
            const Cpu* c = &sv->cpus[ci];
            if (c->tarefa_atual != NULL) {
                printf("\x1b[48;2;%d;%d;%dm   " ANSI_RESET,
                       c->tarefa_atual->cor.r,
                       c->tarefa_atual->cor.g,
                       c->tarefa_atual->cor.b);
            } else {
                printf("\x1b[48;2;180;30;30mOFF" ANSI_RESET);
            }
        }
        for (p = 0; p < rpad; p++) putchar(' ');
        printf(VL "\n");
    }

    /* linha de sorteio */
    printf(VL " %*s|", digs + 2, "");
    for (tick = start; tick < n_ticks; tick++) {
        if (s->historico[tick].houve_sorteio)
            printf(" \xe2\x9a\x80 ");                  /* ⚀ */
        else
            printf("   ");
    }
    for (p = 0; p < rpad; p++) putchar(' ');
    printf(VL "\n");

    /* eixo X */
    printf(VL " %*s  ", digs + 1, "");
    for (tick = start; tick < n_ticks; tick++)
        printf("%2d ", tick + 1);
    for (p = 0; p < rpad; p++) putchar(' ');
    printf(VL "\n");
}

/* ── secao 3: legenda ────────────────────────────────── */

static void secao_legenda(const SistemaSimulado* s) {
    int i, pad;

    linha_plain("Legenda:");

    /* cada símbolo UTF-8 ocupa 1 coluna visual mas 3 bytes no strlen,
       por isso usamos linha_vis com a largura visual correta */
    linha_vis("  \xE2\x96\xBC  chegada da tarefa",   22);
    linha_vis("  \xE2\x96\xA0  termino da tarefa",   22);
    linha_vis("  \xE2\x9A\x80  desempate por sorteio", 26);

    printf(VL " \x1b[48;2;0;0;0m   " ANSI_RESET "  tarefa suspensa");
    pad = IW - (1 + 3 + 2 + 16);
    for (i = 0; i < pad; i++) putchar(' ');
    printf(VL "\n");

    printf(VL " \x1b[48;2;180;30;30mOFF" ANSI_RESET "  processador desligado");
    pad = IW - (1 + 3 + 2 + 22);
    for (i = 0; i < pad; i++) putchar(' ');
    printf(VL "\n");

    linha_plain("     ausencia de cor = tarefa pronta (aguardando CPU)");

    /* amostras de cor de cada tarefa, cabe quantas couberem na linha */
    printf(VL " Cores: ");
    int pos = 8;
    for (i = 0; i < s->qtd_tarefas; i++) {
        const Tcb* t = &s->tarefas[i];
        int id_digs = t->id >= 100 ? 3 : t->id >= 10 ? 2 : 1;
        int needed = 3 + 1 + 1 + id_digs + 1;
        if (pos + needed > IW - 1) break;
        printf("\x1b[48;2;%d;%d;%dm   " ANSI_RESET " T%d ",
               t->cor.r, t->cor.g, t->cor.b, t->id);
        pos += needed;
    }
    pad = IW - pos;
    for (i = 0; i < pad; i++) putchar(' ');
    printf(VL "\n");
}

/* ── secao 4: tarefas + CPUs ─────────────────────────── */

static void secao_tarefas_cpus(const SistemaSimulado* s) {
    int n_t  = s->qtd_tarefas;
    int n_c  = s->qtd_cpus;
    int rows = n_t > n_c ? n_t : n_c;
    int digs = max_id_digs(s);
    int row, p;

    printf(VL " Tarefas");
    for (p = 8; p < LC; p++) putchar(' ');
    printf(VL " CPUs");
    for (p = 5; p < RC; p++) putchar(' ');
    printf(VL "\n");

    for (row = 0; row < rows; row++) {
        printf(VL);

        /* coluna esquerda: tarefa */
        if (row < n_t) {
            const Tcb* t = &s->tarefas[row];
            char plain[64];
            int plen, colored_vis, left_vis, lpad;
            plen = snprintf(plain, sizeof(plain), " %-10s p=%-3d d=%-5d",
                            estado_para_string(t->estado),
                            t->prioridade,
                            t->duracao_restante);
            colored_vis = digs + 3;
            left_vis    = colored_vis + plen;
            lpad        = LC - left_vis;
            if (lpad < 0) lpad = 0;

            if (t->estado == SUSPENSA)
                printf("\x1b[48;2;0;0;0m T%-*d " ANSI_RESET, digs, t->id);
            else
                printf("\x1b[48;2;%d;%d;%dm T%-*d " ANSI_RESET,
                       t->cor.r, t->cor.g, t->cor.b, digs, t->id);
            printf("%s", plain);
            for (p = 0; p < lpad; p++) putchar(' ');
        } else {
            for (p = 0; p < LC; p++) putchar(' ');
        }

        printf(VL);

        /* coluna direita: CPU */
        if (row < n_c) {
            const Cpu* c = &s->cpus[row];
            char cbuf[64];
            int clen, rpad;
            if (c->tarefa_atual != NULL) {
                clen = snprintf(cbuf, sizeof(cbuf), " CPU%d T%d q=%-3d",
                                c->id, c->tarefa_atual->id, c->quantum_restante);
            } else {
                clen = snprintf(cbuf, sizeof(cbuf), " CPU%d ocioso idle=%-4d",
                                c->id, c->ticks_ociosos_acumulados);
            }
            printf("%s", cbuf);
            rpad = RC - clen;
            if (rpad < 0) rpad = 0;
            for (p = 0; p < rpad; p++) putchar(' ');
        } else {
            for (p = 0; p < RC; p++) putchar(' ');
        }

        printf(VL "\n");
    }
}

/* ── secao 5: rodape ─────────────────────────────────── */

static void secao_rodape(void) {
    linha_plain("n=avancar  b=retroceder  m=modificar  i=inspecionar  q=sair");
}

/* ── funcoes publicas ────────────────────────────────── */

void tui_desenhar(const SistemaSimulado* s) {
    printf("\x1b[2J\x1b[H");   /* limpa tela e posiciona cursor no topo */

    linha_topo();
    secao_cabecalho(s);
    linha_sep();
    secao_gantt(s);
    linha_sep();
    secao_tarefas_cpus(s);
    linha_sep();
    secao_legenda(s);
    linha_sep();
    secao_rodape();
    linha_base();
}

/* sub-menu inline (nao limpa tela) */
static void menu_modificar(SistemaSimulado* s) {
    int id, campo, valor;
    int e = -1, p = -1, d = -1;
    int i;

    printf("IDs disponiveis:");
    for (i = 0; i < s->qtd_tarefas; i++) printf(" %d", s->tarefas[i].id);
    printf("\nID da tarefa: ");
    if (scanf("%d", &id) != 1) { while (getchar() != '\n'); return; }

    printf("Campo: 1=estado  2=prioridade  3=duracao_restante\nOpcao: ");
    if (scanf("%d", &campo) != 1) { while (getchar() != '\n'); return; }

    if (campo == 1)
        printf("  0=Nova  1=Pronta  2=Executando  3=Suspensa  4=Finalizada\n");
    printf("Novo valor: ");
    if (scanf("%d", &valor) != 1) { while (getchar() != '\n'); return; }
    while (getchar() != '\n');

    if      (campo == 1) e = valor;
    else if (campo == 2) p = valor;
    else if (campo == 3) d = valor;
    else { printf("Campo invalido.\n"); return; }

    modificar_tarefa(s, id, e, p, d);
}

void tui_loop(SistemaSimulado* s) {
    char cmd[8];

    printf("\x1b[?1049h");   /* entra no alternate screen buffer (sem scrollback) */
    fflush(stdout);
    tui_desenhar(s);

    for (;;) {
        printf("[tick %d] > ", s->relogio_global);
        fflush(stdout);
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) break;
        cmd[strcspn(cmd, "\n")] = '\0';

        if (strcmp(cmd, "n") == 0) {
            if (avancar(s)) tui_desenhar(s);
        } else if (strcmp(cmd, "b") == 0) {
            if (retroceder(s)) tui_desenhar(s);
        } else if (strcmp(cmd, "m") == 0) {
            menu_modificar(s);
            tui_desenhar(s);
        } else if (strcmp(cmd, "i") == 0) {
            inspecionar_sistema(s);
        } else if (strcmp(cmd, "q") == 0) {
            break;
        } else {
            printf("Comando invalido. Use: n b m i q\n");
        }
    }

    printf("\x1b[?1049l");   /* sai do alternate screen, restaura terminal normal */
    fflush(stdout);
    gantt_exportar_svg(s, "gantt.svg");
    printf("SVG exportado para gantt.svg\n");
}
