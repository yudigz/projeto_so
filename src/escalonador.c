#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include "escalonador.h"

/* handle da biblioteca dinamica atualmente carregada.
   Guardado para que o .so nao seja descarregado enquanto a simulacao usa
   o ponteiro de funcao que veio de dentro dele. */
static void* handle_dl = NULL;

static int desempatar(SistemaSimulado* sistema, int idx_a, int idx_b, int cpu_id){
    Tcb* a = &sistema->tarefas[idx_a];
    Tcb* b = &sistema->tarefas[idx_b];

    //tarefa que esta executando permanece
    if(a->estado == EXECUTANDO && a->cpu_atual == cpu_id) return idx_a;
    if(b->estado == EXECUTANDO && b->cpu_atual == cpu_id) return idx_b;

    //menor ingresso
    if(a->ingresso != b->ingresso)
        return (a->ingresso < b->ingresso) ? idx_a : idx_b;

    //menor duracao original
    if(a->duracao != b->duracao)
        return (a->duracao < b->duracao) ? idx_a : idx_b;

    //sorteio
    int vencedor = (rand() % 2 == 0) ? idx_a : idx_b;
    sistema->houve_sorteio = 1;
    sistema->sorteio_tarefa_idx = vencedor;
    return vencedor;
}

Escalonador get_escalonador(const char* nome){
    /* --- algoritmos embutidos: retorna direto sem precisar de arquivo --- */
    if(strcmp(nome, "srtf") == 0)  return srtf;
    if(strcmp(nome, "priop") == 0) return priop;

    /* --- plugin externo: tenta carregar ./escalonadores/lib{nome}.so --- */
    char caminho[256];
    snprintf(caminho, sizeof(caminho), "./escalonadores/lib%s.so", nome);

    /* fecha handle anterior caso get_escalonador seja chamado mais de uma vez */
    if(handle_dl != NULL){
        dlclose(handle_dl);
        handle_dl = NULL;
    }

    handle_dl = dlopen(caminho, RTLD_LAZY);
    if(handle_dl == NULL){
        /* dlerror() devolve a mensagem do sistema explicando o motivo da falha */
        fprintf(stderr, "Erro ao carregar plugin '%s': %s\n", caminho, dlerror());
        return NULL;
    }

    /* limpa estado de erro antes de chamar dlsym (boa pratica: dlsym pode
       retornar NULL legitimamente se o simbolo valer NULL, entao a unica
       forma confiavel de detectar erro eh checar dlerror() apos a chamada) */
    dlerror();
    Escalonador fn = (Escalonador) dlsym(handle_dl, nome);
    const char* erro = dlerror();
    if(erro != NULL){
        fprintf(stderr, "Erro ao buscar funcao '%s' em '%s': %s\n", nome, caminho, erro);
        dlclose(handle_dl);
        handle_dl = NULL;
        return NULL;
    }

    printf("Plugin carregado: funcao '%s' de '%s'\n", nome, caminho);
    return fn;
}

int srtf(SistemaSimulado* sistema, int cpu_id) {
    int melhor_idx = -1;
    int melhor_dur = -1;  /* -1 = ainda não achei nenhuma */

    for (int i = 0; i < sistema->qtd_tarefas; i++) {
        Estado e = sistema->tarefas[i].estado;
        int eh_candidata = (e == PRONTA) ||
                           (e == EXECUTANDO && sistema->tarefas[i].cpu_atual == cpu_id);
        if (!eh_candidata) continue;

        int dur = sistema->tarefas[i].duracao_restante;

        if (melhor_idx == -1 || dur < melhor_dur) {
            melhor_idx = i;
            melhor_dur = dur;
        } else if (dur == melhor_dur){
            melhor_idx = desempatar(sistema, melhor_idx, i, cpu_id);
        }
    }

    return melhor_idx;
}

int priop(SistemaSimulado* sistema, int cpu_id){
    int melhor_idx = -1;
    int melhor_prio = -1;

    for (int i = 0; i < sistema->qtd_tarefas; i++) {
        Estado e = sistema->tarefas[i].estado;
        int eh_candidata = (e == PRONTA) ||
                           (e == EXECUTANDO && sistema->tarefas[i].cpu_atual == cpu_id);
        if (!eh_candidata) continue;

        int prio = sistema->tarefas[i].prioridade;

        if (melhor_idx == -1 || prio > melhor_prio) {
            melhor_idx = i;
            melhor_prio = prio;
        } else if (prio == melhor_prio){
            melhor_idx = desempatar(sistema, melhor_idx, i, cpu_id);
        }
    }

    return melhor_idx;
}
