#include <stdio.h>

int main() {
	int i = 0;
	asm(
		"addi $1, $0, 15\n\t"   // init reg1
		"addi $10, $0, 25\n\t"   // init reg2
	
	);
	while (i < 1000000){


		asm(
		"add $3, $1, $10\n\t"
		"addi $4, $3, 40\n\t"	        // two cycle
		
		"add $5, $1, $10\n\t"
		"addi $6, $0, 40\n\t"
		"addi $7, $5, 50\n\t"		// one cycle
		);
		i++;
		asm(
		"add $8, $1, $10\n\t"
		"sw $8, 0($3)\n\t"		// store with one cycle
		
		"add $9, $1, $10\n\t"
		"addi $6, $0, 40\n\t"
		"sw $9, 0($3)\n\t"		// store with no stall
		

        	);

	}
	
	return 0;
}
