#ifndef NVPTR_H
#define NVPTR_H

#include <cinttypes>

class NVPtr {
    int64_t offset;
public:
    NVPtr();
    NVPtr(int64_t offset);
    void operator = (const void* address);
    void operator = (const NVPtr &nvp);
    void* dptr();
    int64_t get_offset();
};


#endif // NVPTR_H
