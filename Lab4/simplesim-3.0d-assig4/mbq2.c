#include <stdio.h>

int main() {
	int array[1000000];
	int i;
	int j;
	//same stride for every memory access
	for(i = 0; i < 1000000; i = i+18){
		array[i] = 1;
	}
	
	//If we change the stride size to be larger than 32, 
	//then the next line prefetch miss rate will be very close to baseline miss rate with no prefetch mechanism.
	//for(j=0; j<1000000; j = j+32){
	//	array[j] = 2;
	//}
	return 0;
}
