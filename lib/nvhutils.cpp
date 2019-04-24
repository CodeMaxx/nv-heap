#include "nvhutils.h"

void print_err (const char * function_name, const char * message) {
    fprintf(stderr, "%s: %s.\n", function_name, message);
}

uint64_t hash64 (const char * str) {
    uint64_t prime = 31;
    uint64_t hash = 7;
    while(*str) {
        hash = hash * prime + *(str++);
    }
    return hash;
}

void u64itostr (uint64_t val, char * str) {
    int i;
    for (i = 0 ; val; i++) {
        str[i] = val % 10 + '0';
        val /= 10;
    }
    str[i] = '\0';
    strrev(str);
}
