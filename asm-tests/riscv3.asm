	li	a0, 200
	li	a7, 9
	ecall
	add	s0, zero, a0
	addi t1,zero,100
	
LOOP: 
	lw s1,0(s0)
	add s2,s2,s1
	addi s0,s0,4
	addi t1,t1,-1
	bne t1,zero,LOOP
	
	li	a0, 0
	li	a7, 93
	ecall