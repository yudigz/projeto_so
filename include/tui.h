/*
 * tui.h — interface de usuário em modo texto (TUI)
 *
 * tui_desenhar() renderiza o painel completo do simulador no terminal:
 * estado das CPUs, lista de tarefas, diagrama de Gantt e legenda.
 * tui_loop() entra no modo interativo onde o usuário avança, retrocede
 * e inspeciona a simulação com comandos de teclado.
 */

#ifndef TUI_H
#define TUI_H

#include "sistema.h"

void tui_desenhar(const SistemaSimulado* sistema);
void tui_loop(SistemaSimulado* sistema);

#endif
