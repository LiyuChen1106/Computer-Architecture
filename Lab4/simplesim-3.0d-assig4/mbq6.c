#include <stdio.h>

int main() {
	int array[1000000];
	int i;
	int j;
	int k;
	
	for(i = 0; i < 1000000; i = i+20){
		array[i] = 1;
		for (j = i; j<1000000 && j< i+60; j = j+30)
			array[j] = 2;
			for (k=j; k<1000000 && k< j+80; k = k+40)
				array[k] = 3;
	}
	
	return 0;
}
