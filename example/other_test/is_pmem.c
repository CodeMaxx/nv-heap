#include <stdio.h>
#include <libpmem.h>

int main () {
	size_t mapped_len;
    int is_pmem = 1;
	void * nvh_base_addr = (char *)pmem_map_file("abc.txt", 50, PMEM_FILE_CREATE, 0644, &mapped_len, &is_pmem);
	printf("Is this a pmem? %d\n", is_pmem);
	return 0;
}
