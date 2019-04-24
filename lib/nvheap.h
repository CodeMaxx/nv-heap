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

#define NVH_LENGTH              32768
#define HEADER_LEGTH            1024
#define BITMAP_LENGTH           512 // TODO: Define these in terms of each other
#define ALLOC_RATIO_BYTE_BYTE   64
#define ALLOC_RATIO_BYTE_BIT    8

#define TX_BUFFER_SIZE          32768

extern void* nvh_base_addr;

struct nvh_length {
    uint64_t length;
};

/*-----Following functions are for use by User written program-----*/
int nvh_init (const char * file, const char * nvh_name);// Initialize a NV-Heap
void * nvh_get_root ();                                 // Return existing root of the NV-Heap
int nvh_set_root (void *address);                       // Set address as root address of NV-Heap
void *nvh_malloc (int size);                            // Allocate size byte in NV-Heap
int nvh_free (void * address, int size);                // Free size byte from *address
void nvh_persist ();                                    // Persisting write
int nvh_close ();                                       // Closing the NV-Heap

/*-----Following Functions are ment for internal use by Library-----
-------User Written program should not use them---------------------
-------If used undesirable result may be produced-----------------*/
void print_err (const char * function_name, const char * message);
uint64_t hash64 (const char * str);
void u64itostr (uint64_t val, char * str);
int nvh_create(const char * file, const char * nvh_name);
int nvh_open(const char *file);
char *strrev(char *str);
int set_bit_range (uint64_t *base, int start, int end, int bit);
int test_bit (uint64_t * base, int position);
