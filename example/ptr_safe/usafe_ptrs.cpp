#include <iostream>
#include <nvheap.h>
using namespace std;

struct ll {
    int64_t val;
    NVPtr next;
};

int main (int argc, char **argv) {
	ios_base::sync_with_stdio(false);
    if (nvh_init(argv[1], argv[2]) < 0) {
        cerr << "Failed to initialize NV-heap\n";
        exit(1);
    }
    struct ll * head;
    head = (struct ll *) nvh_get_root();
    cout << "Inserting wrong pointer\n";
    head -> next = (void *)-1;
    return(0);
}