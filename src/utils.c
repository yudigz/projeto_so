#include "utils.h"
#include <string.h>

void str_to_lower(char* s){
    for(int i = 0; s[i] != '\0'; i++){
        s[i] = tolower((unsigned char)s[i]);
    }
}