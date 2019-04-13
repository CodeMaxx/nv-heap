#include <iostream>
using namespace std;

int main () {
	int size, round, alloc_size;
	round = 8;
	while (1) { 
		cin >> size;
		alloc_size = (round * (size / round)) + round *(round && (size % round)); 
		cout << alloc_size << endl;
	}
}