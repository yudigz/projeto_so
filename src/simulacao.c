#include "simulacao.h"
#include "snapshot.h"
#include <stdlib.h>
#include <stdio.h>

void tick(SistemaSimulado* sistema){

    for(int i =0; i < sistema->qtd_tarefas; i++){
        if(sistema->tarefas[i].estado == NOVA && sistema->tarefas[i].ingresso == sistema->relogio_global){
            sistema->tarefas[i].estado = PRONTA;
        }
    }

    /* verifica finalizada*/
    for(int i =0; i < sistema->qtd_tarefas; i++){
        if(sistema->tarefas[i].estado != EXECUTANDO) continue;
        
        sistema->tarefas[i].duracao_restante--;

        if(sistema->tarefas[i].duracao_restante == 0){
            int cpu_id = sistema->tarefas[i].cpu_atual;

            /* atualiza o TCB*/
            sistema->tarefas[i].estado = FINALIZADA;
            sistema->tarefas[i].cpu_atual = -1;

            /* libera a cpu*/
            sistema->cpus[cpu_id].tarefa_atual = NULL;
            sistema->cpus[cpu_id].quantum_restante = 0;
        }
    }

    /* verifica quantum */
    for(int i =0; i < sistema->qtd_cpus; i++){
        if(sistema->cpus[i].tarefa_atual == NULL) continue;

        sistema->cpus[i].quantum_restante--;

        if(sistema->cpus[i].quantum_restante <= 0){
            Tcb* t = sistema->cpus[i].tarefa_atual;
            
            /* tarefa volta pra fila de prontos*/
            t->estado = PRONTA;
            t->cpu_atual = -1;

            /* cpu fica libre para escalonamento*/
            sistema->cpus[i].tarefa_atual = NULL;
            sistema->cpus[i].quantum_restante = 0;
        }
    }

    sistema->houve_sorteio = 0;
    sistema->sorteio_tarefa_idx = -1;

    for(int i =0; i < sistema->qtd_cpus; i++){
        /*so reescalona CPUs sem tarefa rodando*/
        if(sistema->cpus[i].tarefa_atual != NULL) continue;

        int idx = sistema->escalonador(sistema, i);

        if(idx == -1){
            /* nenhuma tarefa disponivel, desliga CPU*/
            sistema->cpus[i].ligado = 0;
            sistema->cpus[i].ticks_ociosos_acumulados++;
        } else{
            /* atrinui a tarefa escolhida a esta CPU*/
            sistema->cpus[i].ligado = 1;
            sistema->cpus[i].tarefa_atual = &sistema->tarefas[idx];
            sistema->cpus[i].quantum_restante = sistema->quantum;

            sistema->tarefas[idx].estado = EXECUTANDO;
            sistema->tarefas[idx].cpu_atual = i;
        }
    }

    sistema->relogio_global++;
    
    if(sistema->qtd_snapshots >= sistema->cap_historico){
        sistema->cap_historico = sistema->cap_historico == 0 ? 16 : sistema->cap_historico * 2;
        sistema->historico = realloc(sistema->historico, sistema->cap_historico * sizeof(Snapshot));
    }
    sistema->historico[sistema->qtd_snapshots++] = snapshot_capturar(sistema);
}

int simulacao_finalizada(const SistemaSimulado* sistema){
    for(int i =0; i < sistema->qtd_tarefas; i++){
        if(sistema->tarefas[i].estado != FINALIZADA) return 0;
    }
    return 1;
}

int avancar(SistemaSimulado* sistema){
    if(simulacao_finalizada(sistema)){
        printf("Simulacao ja finalizada no tick: %d.\n", sistema->relogio_global);
        return 0;
    }
    sistema->qtd_snapshots = sistema->idx_snapshot_atual + 1;
    tick(sistema);
    sistema->idx_snapshot_atual = sistema->qtd_snapshots - 1;
    return 1;
}

int retroceder(SistemaSimulado* sistema){
    if(sistema->idx_snapshot_atual <=0){
        printf("Nao ha snapshot anterior para retroceder.\n");
        return 0;
    }
    sistema->idx_snapshot_atual--;
    snapshot_restaurar(sistema, &sistema->historico[sistema->idx_snapshot_atual]);
    return 1;
}

void modificar_tarefa(SistemaSimulado* sistema, int tarefa_id, int novo_estado, int nova_prioridade, int nova_duracao_restante){
    Tcb* t = NULL;
    /* pga a tarefa pelo id*/
    for(int i =0; i < sistema->qtd_tarefas; i++){
        if(sistema->tarefas[i].id == tarefa_id){
            t = &sistema->tarefas[i];
            break;
        }
    } 

    if(t == NULL){
        printf("Erro: tarefa com id %d nao encontrada", tarefa_id);
        return;
    }

    if (novo_estado != -1) {
        Estado e_anterior = t->estado;
        t->estado = (Estado)novo_estado;

        /* se a tarefa saiu de EXECUTANDO, libera a CPU */
        if (e_anterior == EXECUTANDO && t->estado != EXECUTANDO) {
            int cpu_id = t->cpu_atual;
            if (cpu_id >= 0) {
                sistema->cpus[cpu_id].tarefa_atual    = NULL;
                sistema->cpus[cpu_id].quantum_restante = 0;
            }
            t->cpu_atual = -1;
        }
    }
    if(nova_prioridade != -1){
        t->prioridade = nova_prioridade;
    }
    if(nova_duracao_restante != -1){
        t->duracao_restante = nova_duracao_restante;
    }

}

void executar_completo(SistemaSimulado* sistema) {
    while (!simulacao_finalizada(sistema)){
        tick(sistema);
    }
    sistema->idx_snapshot_atual = sistema->qtd_snapshots - 1;
}

void inspecionar_sistema(const SistemaSimulado* sistema){
    printf("=== Tick %d ===\n", sistema->relogio_global);

    printf("--- Tarefas ---\n");
    for(int i=0; i < sistema->qtd_tarefas; i++){
        Tcb* t = &sistema->tarefas[i];
        printf("T%d | estado=%-12s | prio=%d | dur_rest=%d cpu=%d\n",
            t->id, estado_para_string(t->estado), t->prioridade, t->duracao_restante, t->cpu_atual);
    }

    printf("\n-- CPUs --\n");
    for (int i = 0; i < sistema->qtd_cpus; i++) {
        Cpu* c = &sistema->cpus[i];
        if (c->tarefa_atual != NULL)
            printf("CPU%d | ligada | executando T%d | quantum_rest=%d\n",
                   c->id, c->tarefa_atual->id, c->quantum_restante);
        else
            printf("CPU%d | ociosa | ticks_ociosos=%d\n",
                   c->id, c->ticks_ociosos_acumulados);
    }
    printf("\n");
}
