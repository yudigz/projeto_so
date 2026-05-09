#ifndef TCB_H
#define TCH_B

typedef enum{
    NOVA,
    PRONTA,
    EXECUTANDO,
    SUSPENSA,
    FINALIZADA
}Estado;

const char* estado_para_string(Estado e);

#endif