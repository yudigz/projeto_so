#include "tcb.h"

static  const char* strings_estados[]={
    "Nova",
    "Pronta",
    "Executando",
    "Suspensa",
    "Finalizada"
};

const char* estado_para_string(Estado e){
    if(e < NOVA || e > FINALIZADA)
        return "Tarefa inexistente";
    return strings_estados[e];
}