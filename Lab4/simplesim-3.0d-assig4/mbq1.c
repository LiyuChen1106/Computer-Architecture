#include <stdio.h>

int main() {
	int array[1000000];
	int i;
	//prefetches the next cache block
	for(i = 0; i < 1000000; i = i+16){
		array[i] = 1;
	}
	return 0;
}
