#ifndef NVHTX_H
#define NVHTX_H

#include "nvptr.h"
#include <sys/types.h>

#define MAX_TX_DATA_SIZE 256

// For tx_add flags
#define ONLY_IN_TX 0
#define ALLOW_NO_TX 1 // Allows tx_add to be ignored when used outside a transaction

// Types of tx_obj objects
#define USER_WRITE 0
#define MALLOC_CALL 1
#define FREE_CALL 2

class tx_status {
public:
    uint32_t running;
    uint32_t count;

    tx_status();
    tx_status(uint32_t running, uint32_t count);
    void retrieve_cur_status();
    void set_cur_status();
};

class tx_obj
{
    uint type;
    int64_t offset;
    uint size;
    char buf[MAX_TX_DATA_SIZE];

public:
    tx_obj();
    tx_obj(uint type, int64_t offset, uint size);
    tx_obj(uint type, void* address, uint size);
    void undo(); // Sets the address to old value
    void write_to_heap(); // Write the tx_obj in the NVheap
};

void tx_begin();
void tx_add(NVPtr ptr, uint size, uint flags=ONLY_IN_TX);
void tx_add_direct(void* address, uint size, uint flags=ONLY_IN_TX);
void tx_commit();

void tx_malloc(void* address, uint size);
void tx_root();

// Called in nvh_open to undo broken transactions during previous runs
void tx_fix();

void _tx_malloc_free(void* address, uint size, uint type);

#endif // NVHTX_H
