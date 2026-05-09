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
    //struct ListaEventos* eventos 
}Tcb;

const char* estado_para_string(Estado e);

#endif