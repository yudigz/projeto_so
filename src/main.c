#include "sistema.h"
#include "config.h"

#include <time.h>
srand(time(NULL));

int main(void) {
    SistemaSimulado sistema = {0};  /* zera todos os campos */

    if (ler_config("config_teste.txt", &sistema) != 0) {
        return 1;
    }

    return 0;
}