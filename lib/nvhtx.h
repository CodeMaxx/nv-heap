#include "nvptr.h"

#define MAX_TX_DATA_SIZE 256
#define ONLY_IN_TX 0
#define ALLOW_NO_TX 1 // Allows tx_add to be ignored when used outside a transaction

void* nvh_tx_address;

class tx_status {
    uint32_t running;
    uint32_t count;

public:
    tx_status();
    tx_status(uint32_t running, uint32_t count);
    void retrieve_cur_status();
    void set_cur_status();
};

class tx_obj
{
    int64_t offset;
    uint size;
    char buf[MAX_TX_DATA_SIZE];

public:
    tx_obj();
    tx_obj(int64_t offset, uint size, char* buf);
    tx_obj(void* address, uint size, char* buf);
    void undo(); // Sets the address to old value
    void write_to_heap(); // Write the tx_obj in the NVheap
};

void tx_begin();
void tx_add(NVptr ptr, uint size, uint flags=0);
void tx_add_direct(void* address, uint size, uint flags=0);
void tx_commit();
