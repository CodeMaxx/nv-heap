#include "pmem.h"
using namespace std;

void * pmem_base_addr = NULL;	// Base Virtual address returned after mapping

// Copied from "https://stackoverflow.com/questions/8534274/is-the-strrev-function-not-available-in-linux"
char *strrev(char *str) {
    char *p1, *p2;
    if (! str || ! *str)
        return str;
    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
		*p1 ^= *p2;
		*p2 ^= *p1;
		*p1 ^= *p2;
	}
    return str;
}

void print_err (const char * function_name, const char * message) {
	fprintf(stderr, "%s: %s.\n", function_name, message);
}

uint64_t hash64 (const char * str) {
	uint64_t prime = 31;
	uint64_t hash = 7;
	while(*str) {
		hash = hash * prime + *(str++);
	}
	return hash;
}

void u64itostr (uint64_t val, char * str) {
	int i;
	for (i = 0 ; val; i++) {
		str[i] = val % 10 + '0';
		val /= 10;
	}
	str[i] = '\0';
	strrev(str);
}

void * pmem_init (const char * file, const char * pmem_name) {
	int fd;
	uint64_t pname_h;			// Hash of pmem_name will be read to here from fd
	uint64_t pname_hh;			// Hash to hash of pmem_name will be read in this from fd	
	char pname_h_str[32];		// string correspond to pname_h

	if (!pmem_name || !strlen(pmem_name)) {
		print_err("pmem_init", "Blank pmem_name given");
		return NULL;
	}

	if (!file || !strlen(file)) {
		print_err("pmem_init", "Blank file name given");
		return NULL;
	}

	if ((fd = open(file, O_RDONLY)) < 0) {
		print_err ("pmem_init", "Invalid file name given");		// Opaining file failed
		return NULL;
	}

	if ((read(fd, &pname_h, sizeof(uint64_t)) <=0) || (read (fd, &pname_hh, sizeof(uint64_t)) <= 0)) {
		// Empty file. Let's call pmem_create;
		print_err("pmem_init", "Empty file. Calling pmem_create"); 	// DEBUG, REMOVE
		close(fd);
		return pmem_create(file, pmem_name);
	}

	u64itostr(pname_h, pname_h_str); 	// Converting pmem_name hash int to string
	if (pname_h == hash64(pmem_name) && pname_hh == hash64(pname_h_str)) {
		print_err("pmem_init", "Hash matched. Calling pmem_open");
		close(fd);
		return pmem_open (file);
	}

	// cerr << pname_hh << " : " << hash64(pname_h_str) << endl;	// DEBUG, REMOVE

	if (pname_hh != hash64(pname_h_str)) {
		print_err("pmem_init", "Hash not matched. Calling pmem_create");
		close(fd);
		return pmem_create (file, pmem_name);
	}

	// Default case
	// cerr << "Default\n";		// DEBUG, REMOVE
	return NULL;
}

//********-----convert it to int with 0 and -1 output, program don't need pmem base
void * pmem_create(const char *file, const char * pmem_name) {
	// cerr << "In pmem_create\n";	// DEBUG, REMOVE
	size_t len = 32768;				// Length of one unit of pmem in byte
	int flags = 0;					// Flag for pmem_map_file, not in use, REMOVE
	mode_t mode = 0644;				// File access or creation permissions
	size_t mapped_len;				// Actual mapped length will be stored here
	int is_pmem;					// pmem or not will be stored here
	char pname_h_str[32];			// A string to store pmem hash after int to str conv
	struct pmem_length pmem_len;	// A 8-byte structure to hold pmem length

	// pmem_base_addr is global defined and initialised to NULL in pmem.h
	pmem_base_addr = (char *)pmem_map_file(file, len, PMEM_FILE_CREATE, mode, &mapped_len, &is_pmem);
	
	// perror("pmem_map_file");
	if (!pmem_base_addr) {
		print_err("pmem_create", "pmem_map_file returned NULL");
		return pmem_base_addr;
	}
	*(uint64_t *)pmem_base_addr = hash64(pmem_name);			// Hash of pmem name 8-byte
	u64itostr(hash64(pmem_name), pname_h_str);
	*((uint64_t *)pmem_base_addr + 1) = hash64(pname_h_str);	// Hash-hash of pmem name 8-byte
	pmem_len.length = (uint64_t)len;
	*((uint64_t *)pmem_base_addr + 2) = *(uint64_t *)(&pmem_len);	// Current length 8-byte
	
	// Marking header as occupied Start = 24 for hash, hash_h, length
	// Header 1024 Byte = 0-127 bit in bitmap. 128th bit for root pointer
	set_bit_range(((uint64_t *)pmem_base_addr), 24 * sizeof(uint64_t), 24 * sizeof(uint64_t) + 128 , 1);	
	*((int64_t *)pmem_base_addr + 128) = -1;										
	// Marking root pointer as null

	return pmem_base_addr;
}

void * pmem_open(const char *file) {
	pmem_base_addr = (char *)pmem_map_file(file, 32768, PMEM_FILE_CREATE, 0644, NULL, NULL);
	return NULL;
}

// Return offset of root pointer
void * pmem_get_root () {
	if ((*((int64_t *)pmem_base_addr + 128)) == -1)
		return NULL;
	return pmem_dptr((*((uint64_t *)pmem_base_addr + HEADER_LEGTH / ALLOC_RATIO_BYTE_BIT)));
}

// WILL Return 0 on success, -1 on error. Now 1 always
int pmem_set_root (void *address) {
	if (!address){
		// print_err ("pmem_set_root", "NULL");
		(*((int64_t *)pmem_base_addr + 128)) = -1;
		return 0;
	}
	else {
		// print_err("pmem_set_root", "Valid address");
		// cout << "pmem_set_root got " << pmem_pptr(address) << endl;
		(*((int64_t *)pmem_base_addr + 128)) = pmem_pptr(address);
		return 0;
	}
}

uint64_t pmem_pptr (void * address) {
	if (!address)
		return -1;
	// cout << "pmem_pptr: returning " << (uint64_t)((char *)address - (char *)pmem_base_addr) << endl;
	return (uint64_t)((char *)address - (char *)pmem_base_addr);
}

void * pmem_dptr (uint64_t offset) {
	if (offset == -1)
		return NULL;
	// cout << "pmem_dptr got offset = " << endl;
	return (void *)((char *)pmem_base_addr + offset);
}
// This checks whether the positionth BIT is 0 or 1 from base.
int test_bit (uint64_t * base, int position) {
	if (base[position/(sizeof(uint64_t) * 8)] & ((uint64_t)1) << position % (sizeof(uint64_t) * 8))
		return 1;
	else return 0;
}

// This write bit from start to end of base. If bit != 1 or 0 return -1, else 0
int set_bit_range (uint64_t *base, int start, int end, int bit) {
	int i;
	if (bit != 0 && bit != 1)
		return -1;
	if (bit) {
		for (i = start; i <= end; i++) {
			base[i / (sizeof(uint64_t) * 8)] |= (((uint64_t)1) << (i % (sizeof(uint64_t) * 8)));
		}
	}
	if (!bit) {
		for (i = start; i <= end; i++) {
			base[i / (sizeof(uint64_t) * 8)] &= ~(((uint64_t)1) << (i % (sizeof(uint64_t) * 8)));
		}
	}
	return 0;
}

void *pmem_malloc (int size) {
	void * bm_base;			// base address of bitmap
	void *bm_start;			// Start address of working bitmap
	int header_allocation;	// Already allocated size of header in BYTE
	int max_i;				// Maximum offset in BIT
	int start_i, end_i;		// Star and end offset of selected range in BIT
	int round = (sizeof(uint64_t));		// Smallest allocation size
	int alloc_size = (round * (size / round)) + round *(round && (size % round)); // Actual allocn in BYTE
	
	// cout << "pmem_malloc: alloc_size = " << alloc_size <<endl;

	bm_base = (char *)pmem_base_addr + 3 * sizeof(uint64_t);
	header_allocation = HEADER_LEGTH / ALLOC_RATIO_BYTE_BYTE;
	bm_start = (char *)bm_base + header_allocation;
	max_i = 4096 - 128 - 1;  	// -1 bcz last offset = 4097 and 128 filled

	for (start_i = end_i = 0 ; start_i <= max_i && end_i <= max_i;) {
		if (test_bit((uint64_t *)bm_start, end_i) == 0) {
			end_i ++;
			if ((end_i - start_i)*8 >= alloc_size)	// NOTE: 1bit per byte
			{
				set_bit_range((uint64_t *)bm_start, start_i, end_i - 1, 1);
				return (void *)((char *)pmem_base_addr + HEADER_LEGTH + start_i * sizeof(uint64_t));
			}
		}
		else {
			if (start_i == end_i){
				start_i ++;
				end_i ++;
			}
			else{
				start_i = end_i;
			}
		}
	}
	print_err("pmem_malloc","returning null");
	return NULL; 
}

// Currently always returning 0, Latter it'll check some error etc and return -1 for error
int pmem_free (void * address, int size) {
	int alloc_size, round;
	round = sizeof(uint64_t);
	alloc_size = (round * (size / round)) + round *(round && (size % round));
	set_bit_range ((uint64_t *)address, 0, alloc_size / 8 - 1, 0);
	return 0;
}
