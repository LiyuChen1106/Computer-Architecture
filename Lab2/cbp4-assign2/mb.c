
#include <stdio.h>

int main() {
	int a = 0;

	int i;
	int j=0;
	for (i = 0; i < 100000; i++){
		// i mod 8 to generate a pattern of 8 (e.g. NTTTTTTT) for the inner loop
		// tried chaning 8 with different value (6) to see the behavior
		// see reported #misprediction and MPKI for each case in lab report
		if (i % 8 == 0) 
			j = 0;
		
		j += 1;
		
	}
}


/*

gcc2_compiled.:
__gnu_compiled_c:
	.text
	.align	2
	.globl	main

	.text

	.loc	1 4
	.ent	main
main:
	.frame	$fp,40,$31		# vars= 16, regs= 2/0, args= 16, extra= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,40
	sw	$31,36($sp)
	sw	$fp,32($sp)
	move	$fp,$sp
	jal	__main
	sw	$0,16($fp)
	sw	$0,24($fp)
	sw	$0,20($fp)
$L2:
	lw	$2,20($fp)
	li	$3,0x0001869f		# 99999
	slt	$2,$3,$2
	beq	$2,$0,$L5
	j	$L3
$L5:
	lw	$2,20($fp)
	andi	$3,$2,0x0007
	bne	$3,$0,$L6
	sw	$0,24($fp)
$L6:
	lw	$2,24($fp)
	addu	$3,$2,1
	sw	$3,24($fp)
$L4:
	lw	$3,20($fp)
	addu	$2,$3,1
	move	$3,$2
	sw	$3,20($fp)
	j	$L2
$L3:
$L1:
	move	$sp,$fp			# sp not trusted here
	lw	$31,36($sp)
	lw	$fp,32($sp)
	addu	$sp,$sp,40
	j	$31
	.end	main
*/
