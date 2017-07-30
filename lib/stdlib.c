#ifndef STD_LIB_C
#define STD_LIB_C

#include "stdlib.h"

void memcpy(char * destination, char * source, short size) {
    for(short i = 0; i < size; ++i) {
        destination[i] = source[i];
    }
}

int int_to_str(char *mem, int max_len, int number) {
    
    int pos = 0;
    char temp[max_len];
    while((number > 0) && (pos < max_len)) {
        short digit = number % 10;
        number /= 10;
        temp[pos++] = (char)(digit + 48);
    }

    for(int i = (pos - 1); i > -1; --i) {//reverse it
        mem[(pos - i) - 1] = temp[i];
    }

    return pos;
}

#endif /*STD_LIB_C*/

