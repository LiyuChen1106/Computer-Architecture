#include <stdio.h>

int main() {
	int i = 0;
	asm(
		"addi $1, $0, 15\n\t"   // init reg1
		"addi $10, $0, 25\n\t"   // init reg2
	
	);
	while (i < 10000){


		asm(
		// This is a common case with two cycle stalls
		// The value in reg$3 will not be ready for addi execution until add instruction finish writeback
		"add $3, $1, $10\n\t"
		"addi $4, $3, 40\n\t"	        // two cycle
		
		// Common one cycle stall
		// There is a unrelated instruction between two dependence
		// -> fill up one cycle, so addi will need to wait for only one cycle
		"add $5, $1, $10\n\t"
		"addi $6, $0, 40\n\t"
		"addi $7, $5, 50\n\t"		// one cycle
		);
		i++;
		asm(
		// Special case of store instruction
		// Since the store instruction needs the address value in memory state
		// -> Only wait for one cycle
		"add $8, $1, $10\n\t"
		"sw $8, 0($3)\n\t"		// store with one cycle
		
		// If there is a unrelated instruction in between,
		// the next store insturction needs no stall
		"add $9, $1, $10\n\t"
		"addi $6, $0, 40\n\t"
		"sw $9, 0($3)\n\t"		// store with no stall
		
		// adding some test case for load-to-use in question two, not effecting the result in question one
		"lw $4, 0($8)\n\t"
		"addi $5, $4, 20\n\t"          // load to use (two cycles)

		"lw $3, 0($6)\n\t"
		"sw $2, 0($7)\n\t"
		"addi $9, $3, 12\n\t"		// load to use (one cycle)
        	);

	}
	
	return 0;
}
