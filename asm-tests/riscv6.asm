main:
	li		a0, 6
	li		a1, 12
	jal		ra, euclid_gcd
	
	li		a7, 1
	ecall
	
	li		a0, 0
	li		a7, 93
	ecall

# int a0, int a1, returns a0
euclid_gcd:
	beq		a0, a1, .return_gcd
	blt		a0, a1, .a0_smaller
	sub		a0, a0, a1
	beq		zero, zero, euclid_gcd
.a0_smaller:
	sub		a1, a1, a0
	beq		zero, zero, euclid_gcd
.return_gcd:
	jalr	zero, 0(ra)
