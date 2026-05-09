#include "tcb.h"

const char* strings_estados[]={
    "Nova",
    "Pronta",
    "Executando",
    "Suspensa",
    "Finalizada"
};

const char* estado_para_string(Estado e){
    switch(e) {
        case NOVA:
            return strings_estados[0];
        case PRONTA:
            return strings_estados[1];
        case EXECUTANDO:
            return strings_estados[2];
        case SUSPENSA:
            return strings_estados[3];
        case FINALIZADA:
            return strings_estados[4];
        default:
            return "Estado inexistente\n";
    }
}