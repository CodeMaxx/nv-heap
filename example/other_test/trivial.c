#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	int fd;
	char *pmaddr;
	char ch;
	int i = 0;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		exit(1);
	}

	if ((fd = open(argv[1], O_RDWR)) < 0) {
		perror(argv[1]);
		exit(1);
	}

	/*
	 * Map 4k from the file into our memory for read & write.
	 * Use MAP_SHARED for Persistent Memory so that stores go
	 * directly to the PM and are globally visible.
	 */
	if ((pmaddr = mmap(NULL, 100, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_32BIT,
					fd, (off_t)4096)) == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	/* don't need the fd anymore, the mapping stays around */
	close(fd);

	/* store a string to the Persistent Memory */
	strcpy(pmaddr, "Hello, Persistent Memory!");

	/*
	 * The above stores may or may not be sitting in cache at
	 * this point, depending on other system activity causing
	 * cache pressure.  Force the change to be durable (flushed
	 * all the say to the Persistent Memory) using msync().
	 */
	while((ch = pmaddr[i++]) != 0) {
		printf("%c",ch);
	}
	if (msync((void *)pmaddr, 4096, MS_SYNC) < 0) {
		perror("msync");
		exit(1);
	}

	printf("Done.\n");
	exit(0);
}