#include <iostream>
#include <unistd.h>
#include <sys/mamn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class linkedList {
private:
	struct node {
		int val;
		struct node * next;
	};
	struct node * head;

public:
	linkedList(void * base, int present) {
		if (present) {
			head = (struct node *)base;
		}
	}
	insertFront(int val) {
		if 
	}
};

int main(int argc, char *argv[])
{
	int fd;
	void *mbuff;
	if (argc != 2) {
		cout << "Usage: ./exe file name.\n";
		exit(1);
	}

	if ((fd = open(argv[1], O_RDWR)) == -1) {
		cout << "Failed to open the file\n";
		exit(1);
	}
	if ((mbuff = mmap(0, 4096,  PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
		perror(mmap);
		exit(1);
	}
	return 0;
}