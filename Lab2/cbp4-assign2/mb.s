	.file	1 "mb.c"

 # GNU C 2.7.2.3 [AL 1.1, MM 40, tma 0.1] SimpleScalar running sstrix compiled by GNU C

 # Cc1 defaults:
 # -mgas -mgpOPT

 # Cc1 arguments (-G value = 8, Cpu = default, ISA = 1):
 # -quiet -dumpbase -o

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
