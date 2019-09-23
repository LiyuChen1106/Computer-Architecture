	.file	1 "mbq1.c"

 # GNU C 2.7.2.3 [AL 1.1, MM 40, tma 0.1] SimpleScalar running sstrix compiled by GNU C

 # Cc1 defaults:
 # -mgas -mgpOPT

 # Cc1 arguments (-G value = 8, Cpu = default, ISA = 1):
 # -quiet -dumpbase -O2 -o

gcc2_compiled.:
__gnu_compiled_c:
	.text
	.align	2
	.globl	main

	.extern	stdin, 4
	.extern	stdout, 4

	.text

	.loc	1 3
	.ent	main
main:
	.frame	$sp,24,$31		# vars= 0, regs= 1/0, args= 16, extra= 0
	.mask	0x80000000,-8
	.fmask	0x00000000,0
	subu	$sp,$sp,24
	sw	$31,16($sp)
	jal	__main
 #APP
	addi $1, $0, 15
	addi $10, $0, 25
	
 #NO_APP
	li	$2,0x0000270f		# 9999
$L16:
 #APP
	add $3, $1, $10
	addi $4, $3, 40
	add $5, $1, $10
	addi $6, $0, 40
	addi $7, $5, 50
	
 #NO_APP
	subu	$2,$2,1
 #APP
	add $8, $1, $10
	sw $8, 0($3)
	add $9, $1, $10
	addi $6, $0, 40
	sw $9, 0($3)
	lw $4, 0($8)
	addi $5, $4, 20
	lw $3, 0($6)
	sw $2, 0($7)
	addi $9, $3, 12
	
 #NO_APP
	bgez	$2,$L16
	move	$2,$0
	lw	$31,16($sp)
	addu	$sp,$sp,24
	j	$31
	.end	main
