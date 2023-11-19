#while (a<7) {
#	D[a] = b+a; a+=1;
#}
	li	a0, 200
	li	a7, 9
	ecall
	add	x20, zero, a0
	li	x10, 0
	li	x11, 10
while_loop:
	slti	x13, x10, 7		# x13 = a < 7
	beq	x13, zero, .while_exit	# if a >= 7, .while_exit()
	slli	x14, x10, 2		# x14 = a * 4
	add	x14, x14, x20		# x14 += D
	add	x15, x10, x11		# x15 = a + b
	sw	x15, 0(x14)
	addi	x10, x10, 1
	beq	zero, zero, while_loop
.while_exit:
	li	a0, 0
	li	a7, 93
	ecall