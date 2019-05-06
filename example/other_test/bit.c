#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

// This checks whether the positionth bit is 0 or 1 from base.
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

int main () {
	uint64_t base[500];
	int test_or_set, set_or_reset, start, end;
	set_bit_range(base, 0, 400*8, 0);
	while (1) {
		printf("%s\n", "1 = test, 2 = set");
		scanf("%d", &test_or_set);
		if (test_or_set == 1) {
			int i;
			scanf("%d %d", &start, &end);
			for (i = start; i <= end; i++)
				printf("The %dth bit is %d\n", i, test_bit(base, i));
		}
		else if (test_or_set == 2) {
			scanf ("%d %d %d", &start, &end, &set_or_reset);
			if (set_bit_range(base, start, end, set_or_reset) < 0) 
				printf("Error while setting bit.\n");
			else {
				printf("Bit set\n");
			}
		}
		else{
			printf("test_or_set = 1 or 2. Try again\n");
		}
	}
	return 0;
}