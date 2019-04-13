#include "../lib/pmem.h"
using namespace std;

struct ll {
	int64_t val;
	int64_t next;
};

struct ll * insert (struct ll * head, int64_t val) {
	struct ll * temp, *temp1;
	// cout << "Insert: strted\n";
	if (!head) {
		// cout << "Insert: null head\n";
		head = (struct ll *)pmem_malloc (sizeof(struct ll));
		head -> val = val;
		head -> next = -1;
	}
	else {
		// cout << "Insert: valid head\n";
		temp = head;
		while (pmem_dptr(temp -> next)) {
			temp = (struct ll *)pmem_dptr(temp -> next);
		}
		temp1 = (struct ll *)pmem_malloc (sizeof(struct ll));
		temp1 -> val = val;
		temp1 -> next = -1;
		temp -> next = pmem_pptr ((void *)temp1);
	}
	pmem_set_root(head);
	// cout << pmem_get_root() << endl;
	// cout << "Insert: done\n";
	return head;
}

struct ll * remove (struct ll * head, int val) {
	struct ll *temp, *curr, *next;
	if (!head)
		return head;
	if (head -> val == val) {
		temp = (struct ll *) pmem_dptr(head -> next);
		pmem_free (head, sizeof(struct ll));
		head = temp;
	}
	else {
		curr = head;
		next = (struct ll *)pmem_dptr(curr ->next);
		// while (next) {
		// 	if (next -> val == val){
		// 		temp = next;
		//		next = next -> next;
		// 		curr -> next = next -> next;
		// 		free(temp);
		// 	}
		// }
		while (next) {
			if (next -> val == val) {
				temp = next;
				next = (struct ll *)pmem_dptr(next ->next);
				curr -> next = pmem_pptr(next);
				pmem_free(temp, sizeof(struct ll *));
				break;
			}
			curr = next;
			next = (struct ll *)pmem_dptr(next -> next);
		}
	}
	pmem_set_root(head);
	return head;
} 

void print (struct ll * head) {
	struct ll * temp = head;
	// cout << "In print \n";
	while (temp) {
		// cout << "print: In while";
		cout << temp -> val << "->";
		temp = (struct ll *)pmem_dptr(temp -> next);
	}
	cout << "NULL" << endl;
}

int main (int argc, char * argv[]) {
	ios_base::sync_with_stdio(false);
	pmem_init(argv[1], argv[2]);
	struct ll * head;
	head = (struct ll *) pmem_get_root();
	int option, val, run = 1;
	while (run) {
		cin >> option;
		switch (option)
		{
			case (0):
			run = 0;
			break;
			case (1): 	//Insert
			cin >> val;
			head = insert (head, val);
			break;
			case (2):
			print (head);
			break;
			case (3):
			cin >> val;
			head = remove (head, val);
			break;
		}
	}
	return 0;
}
