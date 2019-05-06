#include <stdio.h>
#include <string.h>
#include <libpmemobj.h>

#define LAYOUT_NAME "intro_0" /* will use this in create and open */
#define MAX_BUF_LEN 10 /* maximum length of our buffer */

struct my_root {
	size_t len; /* = strlen(buf) */
	char buf[MAX_BUF_LEN];
};

int main(int argc, char *argv[])
{
	PMEMobjpool *pop = pmemobj_open(argv[1], LAYOUT_NAME);
	printf("The pop is: %ld\n", (unsigned long)pop);				//Abhik
	if (pop == NULL) {
		perror("pmemobj_open");
		return 1;
	}

	PMEMoid root = pmemobj_root(pop, sizeof (struct my_root));
	printf("PMEMoid root is %ld\n", *((unsigned int *)&root));		//Abhik
	struct my_root *rootp = pmemobj_direct(root);

	if (rootp->len == strlen(rootp->buf))
		printf("%s\n", rootp->buf);


	pmemobj_close(pop);

	return 0;
}