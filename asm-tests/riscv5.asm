.data
	.a_arr: .word 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29
	.b_arr: .word 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29

.text
setup:
	la	s0, .a_arr
	la	s1, .b_arr
main:
	add t0, s0, zero		# t0 = a = 4000
	addi t1, zero, 9		# t1 = 9
	slli t1, t1, 2			# t1 *= 4 => t1 = 9 * 4 = 36
	add t1, t1, s1			# t1 += b => t1 = b + 36 = 8036
.L1:
	lw t2, 0(t0)			# t2 = Mem[a] = Mem[4000] = 10
	sw t2, 0(t1)			# Mem[8036] = b[9] = 10
	addi t0, t0, 4			# t0 += 4 = 4004
	addi t1, t1, -4			# t1 -= 4 = 8032
	slt t3, t1, s1			# t3 = t1 < s1 = 8032 < 8000 = false
	beq t3, zero, .L1		# if !t3, goto .L1
.exit:
	li	a0, 0
	li	a7, 93
	ecall