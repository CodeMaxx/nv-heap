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
