setup:
	addi	s0, zero, 50
	addi	s1, zero, 30
	addi	s2, zero, 0
main:
	beq	s0, zero, .false
	lw	s0, 0(s2)
	beq	zero, zero, .return
.false:
	add s0, zero, s1
	beq zero, zero, .return
.return:
	# Any exit code