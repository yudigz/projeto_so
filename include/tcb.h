/*
 * tcb.h — definição do Task Control Block e do ciclo de vida das tarefas
 *
 * O TCB é a estrutura central de cada tarefa: guarda identificador, cor,
 * tempos de ingresso e duração, prioridade, estado atual e CPU em uso.
 * O enum Estado representa o ciclo de vida completo dentro do simulador.
 */

#ifndef TCB_H
#define TCB_H

typedef enum{
    NOVA,
    PRONTA,
    EXECUTANDO,
    SUSPENSA,
    FINALIZADA
}Estado;

typedef struct{
    unsigned char r, g, b;
}Cor;

typedef struct{
    int id;
    Cor cor;
    int ingresso;
    int duracao;
    int duracao_restante;
    int prioridade;
    Estado estado;
    int cpu_atual;
}Tcb;

const char* estado_para_string(Estado e);

#endif