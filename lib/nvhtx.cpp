#include "nvhtx.h"
#include "nvheap.h"
#include "nvhutils.h"
#include <cstdlib>
#include <iostream>

using namespace std;

// TODO: What if these functions don't work atomically? Will it create a problem...check.
// TODO: Add checks to see that the addresses sent are inside the heap

void* nvh_tx_address;

// Transaction Status
tx_status::tx_status() {
    running = 0;
    count = 0;
}

tx_status::tx_status(uint32_t running, uint32_t count) {
    this->running = running;
    this->count = count;
}

void tx_status::retrieve_cur_status() {
    tx_status* cur = (tx_status*) nvh_tx_address;
    running = cur->running;
    count = cur->count;
}

void tx_status::set_cur_status() {
    tx_status* cur = (tx_status*) nvh_tx_address;
    cur->running = running;
    cur->count = count;
}


// Transaction Object
tx_obj::tx_obj() {
    type = USER_WRITE;
    offset = 0;
    size = 0;
}

tx_obj::tx_obj(uint type, int64_t offset, uint size) {
    this->type = type;
    this->offset = offset;
    this->size = size;
    if(type == USER_WRITE) {
        void* address = (void*) ((char*) nvh_base_addr + offset);
        memcpy((void*) &this->buf, address, size);
    }
}

tx_obj::tx_obj(uint type, void* address, uint size) {
    this->type = type;
    this->offset = (int64_t) ((char*) address - (char*) nvh_base_addr);
    this->size = size;
    if(type == USER_WRITE)
        memcpy((void*) &this->buf, address, size);
}

void tx_obj::undo() {
    void* address = (void*) ((char*) nvh_base_addr + offset);
    if(type == USER_WRITE)
        memcpy(address, (void*) buf, size);
    else if(type == MALLOC_CALL)
        nvh_free(address, size);
    else if(type == FREE_CALL) {
        int alloc_size, round;
        round = sizeof(uint64_t);
        alloc_size = (round * (size / round)) + round *(round && (size % round));
        set_bit_range ((uint64_t *)address, 0, alloc_size / 8 - 1, 1);
        nvh_persist ();
    }
}

void tx_obj::write_to_heap() {
    tx_status stat;
    stat.retrieve_cur_status();
    void* address = (void*) ((char*) nvh_tx_address + sizeof(tx_status) + stat.count*sizeof(tx_obj));
    memcpy(address, (void*) this, sizeof(tx_obj));
    stat.count++;
    stat.set_cur_status();
}


// To Begin a transaction
void tx_begin() {
    tx_status stat(1, 0);
    memcpy(nvh_tx_address, &stat, sizeof(tx_status));
}

void tx_add(NVPtr ptr, uint size, uint flags) {
    tx_status stat;
    stat.retrieve_cur_status();
    if(!stat.running) {
        if(flags == ALLOW_NO_TX)
            return;
        else {
            print_err("tx_add_direct", "Transaction Object cannot be added outside a transaction");
            exit(1);
        }
    }
    // Calculate buffer here
    tx_obj to(USER_WRITE, ptr.get_offset(), size);
    to.write_to_heap();
}

void tx_add_direct(void* address, uint size, uint flags) {
    tx_add((int64_t) ((char*) address - (char*) nvh_base_addr), size, flags);
}

void tx_commit() {
    tx_status stat;
    memcpy(nvh_tx_address, (void*) &stat, sizeof(tx_status));
}


void tx_root() {
    tx_add_direct((void*) ((int64_t *)nvh_base_addr + 128), sizeof(int64_t), ALLOW_NO_TX);
}

void _tx_malloc_free(void* address, uint size, uint type) {
    tx_status stat;
    stat.retrieve_cur_status();
    if(!stat.running)
        return;
    tx_obj to(type, address, size);
    to.write_to_heap();
}

void tx_malloc(void* address, uint size) {
    _tx_malloc_free(address, size, MALLOC_CALL);
}

void tx_free(void* address, uint size) {
    _tx_malloc_free(address, size, FREE_CALL);
}

void tx_fix() {
    tx_status stat;
    stat.retrieve_cur_status();
    if(stat.running) {
        cout << "Undo-ing broken transactions..." << endl;
        char* address = (char*) nvh_base_addr + NVH_LENGTH + sizeof(tx_status);
        for(int i = 0; i < stat.count; i++, address += sizeof(tx_obj)) {
            tx_obj* to = (tx_obj*) address;
            to->undo();
        }
        stat.count = 0;
        stat.running = 0;
        stat.set_cur_status();
        cout << "Fixed broken transactions" << endl;
        return;
    }
    cout << "No broken transactions found" << endl;
}
