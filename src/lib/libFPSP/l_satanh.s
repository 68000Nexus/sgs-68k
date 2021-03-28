#
#	satanh.sa 3.3 12/19/90
#
#	The entry point satanh computes the inverse
#	hyperbolic tangent of
#	an input argument; satanhd does the same except for denormalized
#	input.
#
#	Input: Double-extended number X in location pointed to
#		by address register a0.
#
#	Output: The value arctanh(X) returned in floating-point register Fp0.
#
#	Accuracy and Monotonicity: The returned result is within 3 ulps in
#		64 significant bit, i.e. within 0.5001 ulp to 53 bits if the
#		result is subsequently rounded to double precision. The 
#		result is provably monotonic in double precision.
#
#	Speed: The program satanh takes approximately 270 cycles.
#
#	Algorithm:
#
#	ATANH
#	1. If |X| >= 1, go to 3.
#
#	2. (|X| < 1) Calculate atanh(X) by
#		sgn := sign(X)
#		y := |X|
#		z := 2y/(1-y)
#		atanh(X) := sgn * (1/2) * logp1(z)
#		Exit.
#
#	3. If |X| > 1, go to 5.
#
#	4. (|X| = 1) Generate infinity with an appropriate sign and
#		divide-by-zero by	
#		sgn := sign(X)
#		atan(X) := sgn / (+0).
#		Exit.
#
#	5. (|X| > 1) Generate an invalid operation by 0 * infinity.
#		Exit.
#

#		Copyright (C) Motorola, Inc. 1990
#			All Rights Reserved
#
#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF MOTOROLA 
#	The copyright notice above does not evidence any  
#	actual or intended publication of such source code.

#	idnt	2,1	Motorola 040 Floating Point Software Package

	text

#	xref	t_dz	
#	xref	t_operr	
#	xref	t_frcinx	
#	xref	t_extdnrm	
#	xref	slognp1	

	global		satanhd
satanhd:
#--ATANH(X) = X FOR DENORMALIZED X

	bra		t_extdnrm

	global		satanh
satanh:
	mov.l		(%a0),%d0
	mov.w		4(%a0),%d0
	and.l		&0x7FFFFFFF,%d0
	cmp.l		%d0,&0x3FFF8000
	bge.b		ATANHBIG

#--THIS IS THE USUAL CASE, |X| < 1
#--Y = |X|, Z = 2Y/(1-Y), ATANH(X) = SIGN(X) * (1/2) * LOG1P(Z).

	fabs.x		(%a0),%fp0	# Y = |X|
	fmov.x		%fp0,%fp1
	fneg.x		%fp1		# -Y
	fadd.x		%fp0,%fp0	# 2Y
	fadd.s		&0x3F800000,%fp1	# 1-Y
	fdiv.x		%fp1,%fp0	# 2Y/(1-Y)
	mov.l		(%a0),%d0
	and.l		&0x80000000,%d0
	or.l		&0x3F000000,%d0	# SIGN(X)*HALF
	mov.l		%d0,-(%sp)

	fmovm.x		&0x80,(%a0)	# overwrite input {%fp0}
	mov.l		%d1,-(%sp)
	clr.l		%d1
	bsr		slognp1		# LOG1P(Z)
	fmov.l		(%sp)+,%control
	fmul.s		(%sp)+,%fp0
	bra		t_frcinx

ATANHBIG:
	fabs.x		(%a0),%fp0	# |X|
	fcmp.s		%fp0,&0x3F800000
	fbgt		t_operr
	bra		t_dz

#	end		
