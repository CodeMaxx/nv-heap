#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <libpmem.h>
#include <unistd.h>
#include <sys/types.h>
#include <cinttypes>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>

#define HEADER_LEGTH			1024
#define BITMAP_LENGTH			512
#define ALLOC_RATIO_BYTE_BYTE	64
#define ALLOC_RATIO_BYTE_BIT	8

struct pmem_length {
	uint64_t length;
};

void print_err (const char * function_name, const char * message);
void * pmem_init (const char * file, const char * pmem_name);
uint64_t hash64 (const char * str);
void u64itostr (uint64_t val, char * str);
void * pmem_create(const char * file, const char * pmem_name);
void * pmem_open(const char *file);
char *strrev(char *str);
int set_bit_range (uint64_t *base, int start, int end, int bit);
int test_bit (uint64_t * base, int position);
void * pmem_dptr (uint64_t offset);
void * pmem_get_root ();
int pmem_set_root (void *address);
uint64_t pmem_pptr (void * address);
void *pmem_malloc (int size);
int pmem_free (void * address, int size);