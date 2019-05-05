//Store and retrival
int nvh_init (const char * file, const char * nvh_name);// Initialize a NV-Heap
void * nvh_get_root ();                                 // Return existing root of the NV-Heap
int nvh_set_root (void *address);                       // Set address as root address of NV-Heap
void *nvh_malloc (int size);                            // Allocate size byte in NV-Heap
int nvh_free (void * address, int size);                // Free size byte from *address
void nvh_persist ();                                    // Persisting write
int nvh_close ();                                       // Closing the NV-Heap

// Non volatile Pointer
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

// Transections
#define MAX_TX_DATA_SIZE 256
#define ONLY_IN_TX 0
#define ALLOW_NO_TX 1
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
    void undo(); 									// Sets the address to old value
    void write_to_heap(); 							// Write the tx_obj in the NVheap
};

void tx_begin();
void tx_add(NVPtr ptr, uint size, uint flags=ONLY_IN_TX);
void tx_add_direct(void* address, uint size, uint flags=ONLY_IN_TX);
void tx_commit();

void tx_malloc(void* address, uint size);
void tx_root();
