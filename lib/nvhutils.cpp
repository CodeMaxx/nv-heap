#include "nvhutils.h"
#include <iostream>
#include <cstdio>
#include <cstring>

// Copied from "https://stackoverflow.com/questions/8534274/is-the-strrev-function-not-available-in-linux"
char *strrev(char *str) {
    char *p1, *p2;
    if (! str || ! *str)
        return str;
    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return str;
}

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
