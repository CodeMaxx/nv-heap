#include "nvptr.h"
#include "nvheap.h"
#include <iostream>

using namespace std;

NVPtr::NVPtr() {
    offset = -1;
}

NVPtr::NVPtr(int64_t offset) {
    this->offset = offset;
}

void NVPtr::operator = (const void* address) {
    if (!address) {
        offset = -1;
        return;
    }
    if ((char *)address < (char *)nvh_base_addr) {
        cout << "Address given is not part of (lower) NV-Heap\n";
        cout << "Exiting.. \n";
        exit(1);
    }

    if ((char *)address > ((char *)nvh_base_addr + NVH_LENGTH)) {
        cout << "Address given is not part of (upper) NV-Heap\n";
        cout << "Exiting.. \n";
        exit(1);
    }
    offset = (uint64_t)((char *)address - (char *)nvh_base_addr);
}

void NVPtr::operator = (const NVPtr &nvp) {
    offset = nvp.offset;
}

void* NVPtr::dptr() {
    if (offset > NVH_LENGTH || offset < -1){
        print_err ("nvh_dptr", "Given NVptr is out of this NV-Heap. Exiting.");
        exit(1);
    }
    if (offset == -1)
        return NULL;
    // cout << "nvh_dptr got offset = " << endl;
    return (void *)((char *)nvh_base_addr + offset);
}

int64_t NVPtr::get_offset() {
    return offset;
}
