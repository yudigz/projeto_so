/*
 * tcb.c — Task Control Block: funções auxiliares de tarefas
 *
 * Por enquanto só contém a conversão do enum Estado para string legível,
 * usada na exibição do TUI e na inspeção do sistema.
 */

#include "tcb.h"

static const char* strings_estados[]={
    "Nova",
    "Pronta",
    "Executando",
    "Suspensa",
    "Finalizada"
};

const char* estado_para_string(Estado e){
    if(e < NOVA || e > FINALIZADA)
        return "Estado inexistente";
    return strings_estados[e];
}
