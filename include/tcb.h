#ifndef TCB_H
#define TCB_H

typedef enum{
    NOVA,
    PRONTA,
    EXECUTANDO,
    SUSPENSA,
    FINALIZADA
}Estado;

const char* estado_para_string(Estado e);

#endif