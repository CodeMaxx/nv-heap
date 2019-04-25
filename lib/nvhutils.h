#ifndef NVHUTILS_H
#define NVHUTILS_H

// TODO: Add comments to all files
#include <cinttypes>

char *strrev(char *str);
void print_err (const char * function_name, const char * message);
uint64_t hash64 (const char * str);
void u64itostr (uint64_t val, char * str);

#endif // NVHUTILS_H
