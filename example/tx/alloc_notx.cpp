#include <iostream>
#include <unistd.h>
#include <nvheap.h>
using namespace std;

int main (int argc, char * argv[]) {
    ios_base::sync_with_stdio(false);
    if (nvh_init(argv[1], argv[2]) < 0) {
        cerr << "Failed to initialize NV-heap\n";
        exit(1);
    }
    int malloc_count = 0, i;
    cout << "Initially used bytes " << nvh_meminfo() << endl;
    cout << "How many nvh_malloc do you want? ";
    cin >> malloc_count;
    // tx_begin();
    for (i = 0; i < malloc_count; i++) {
    	sleep(1);
    	nvh_malloc(sizeof(uint64_t));
    	cout << i + 1 << "nvh_malloc done\n";
    	cout << "Currently used bytes " << nvh_meminfo() << endl;
    }
    // tx_commit();
    return 0;
}