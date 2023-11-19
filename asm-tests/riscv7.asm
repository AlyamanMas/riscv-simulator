main:
	li		a0, 28
	li		a1, 70
	jal		ra, euclid_gcd
	
	li		a7, 1
	ecall
.exit_main:
	li		a0, 0
	li		a7, 93
	ecall
	
# (int a0, int a1) -> int a0
#gcd(int a, int b) {
#	if (a == b) return a;
#	else if (a > b) return gcd(a - b, b);
#	return gcd(a, b - a);
#}
euclid_gcd:
	addi	sp, sp, -4				# push ra to the stack
	sw		ra, 0(sp)
	
	beq		a0, a1, .exit_gcd		# a == b
	
	bgt		a0, a1, .a0_greater		# a > b
	sub		a1, a1, a0				# if (a < b), b -= a
	jal		ra, euclid_gcd			# euclid_gcd(a, b - a)
	
	beq		zero, zero, .exit_gcd
.a0_greater:
	sub		a0, a0, a1				# a -= b
	jal		ra, euclid_gcd			# euclid_gcd(a - b, b)
.exit_gcd:
	lw		ra, 0(sp)				# pop ra from the stack
	addi	sp, sp, 4
	jalr	zero, 0(ra)				# return from the function
