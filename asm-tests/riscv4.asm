.data
	.c_var: .word 0x12345678, 2
	
.text 
setup:
	li	s0, 0xFFFF9796
	li	s1, 0xABABCDCD
	la	s2, .c_var
main:
	beq	s0, zero, .false
	lw	s0, 0(s2)
	beq	zero, zero, .return
.false:
	add s0, zero, s1
	beq zero, zero, .return
.return:
	# Any exit code
	li	a0, 0
	li	a7, 93
	ecall