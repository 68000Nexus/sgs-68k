#
#	get_op.sa 3.5 4/26/91
#
#  Description: This routine is called by the unsupported format/data
# type exception handler ('unsupp' - vector 55) and the unimplemented
# instruction exception handler ('unimp' - vector 11).  'get_op'
# determines the opclass (0, 2, or 3) and branches to the
# opclass handler routine.  See 68881/2 User's Manual table 4-11
# for a description of the opclasses.
#
# For UNSUPPORTED data/format (exception vector 55) and for
# UNIMPLEMENTED instructions (exception vector 11) the following
# applies:
#
# - For unnormormalized numbers (opclass 0, 2, or 3) the
# number(s) is normalized and the operand type tag is updated.
#		
# - For a packed number (opclass 2) the number is unpacked and the
# operand type tag is updated.
#
# - For denormalized numbers (opclass 0 or 2) the number(s) is not
# changed but passed to the next module.  The next module for
# unimp is do_func, the next module for unsupp is res_func.
#
# For UNSUPPORTED data/format (exception vector 55) only the
# following applies:
#
# - If there is a move out with a packed number (opclass 3) the
# number is packed and written to user memory.  For the other
# opclasses the number(s) are written back to the fsave stack
# and the instruction is then restored back into the '040.  The
# '040 is then able to complete the instruction.
#
# For example:
# fadd.x fpm,fpn where the fpm contains an unnormalized number.
# The '040 takes an unsupported data trap and gets to this
# routine.  The number is normalized, put back on the stack and
# then an frestore is done to restore the instruction back into
# the '040.  The '040 then re-executes the fadd.x fpm,fpn with
# a normalized number in the source and the instruction is
# successful.
#		
# Next consider if in the process of normalizing the un-
# normalized number it becomes a denormalized number.  The
# routine which converts the unnorm to a norm (called mk_norm)
# detects this and tags the number as a denorm.  The routine
# res_func sees the denorm tag and converts the denorm to a
# norm.  The instruction is then restored back into the '040
# which re_executess the instruction.
#
#
#		Copyright (C) Motorola, Inc. 1990
#			All Rights Reserved
#
#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF MOTOROLA 
#	The copyright notice above does not evidence any  
#	actual or intended publication of such source code.

#	idnt	2,1	Motorola 040 Floating Point Software Package

	text

#	include	fpsp.h	

	global		PIRN,PIRZRM,PIRP
	global		SMALRN,SMALRZRM,SMALRP
	global		BIGRN,BIGRZRM,BIGRP

PIRN:
	long		0x40000000,0xc90fdaa2,0x2168c235	# pi
PIRZRM:
	long		0x40000000,0xc90fdaa2,0x2168c234	# pi
PIRP:
	long		0x40000000,0xc90fdaa2,0x2168c235	# pi

#round to nearest
SMALRN:
	long		0x3ffd0000,0x9a209a84,0xfbcff798	# log10(2)
	long		0x40000000,0xadf85458,0xa2bb4a9a	# e
	long		0x3fff0000,0xb8aa3b29,0x5c17f0bc	# log2(e)
	long		0x3ffd0000,0xde5bd8a9,0x37287195	# log10(e)
	long		0x00000000,0x00000000,0x00000000	# 0.0
# round to zero;round to negative infinity
SMALRZRM:
	long		0x3ffd0000,0x9a209a84,0xfbcff798	# log10(2)
	long		0x40000000,0xadf85458,0xa2bb4a9a	# e
	long		0x3fff0000,0xb8aa3b29,0x5c17f0bb	# log2(e)
	long		0x3ffd0000,0xde5bd8a9,0x37287195	# log10(e)
	long		0x00000000,0x00000000,0x00000000	# 0.0
# round to positive infinity
SMALRP:
	long		0x3ffd0000,0x9a209a84,0xfbcff799	# log10(2)
	long		0x40000000,0xadf85458,0xa2bb4a9b	# e
	long		0x3fff0000,0xb8aa3b29,0x5c17f0bc	# log2(e)
	long		0x3ffd0000,0xde5bd8a9,0x37287195	# log10(e)
	long		0x00000000,0x00000000,0x00000000	# 0.0

#round to nearest
BIGRN:
	long		0x3ffe0000,0xb17217f7,0xd1cf79ac	# ln(2)
	long		0x40000000,0x935d8ddd,0xaaa8ac17	# ln(10)
	long		0x3fff0000,0x80000000,0x00000000	# 10 ^ 0

	global		PTENRN
PTENRN:
	long		0x40020000,0xA0000000,0x00000000	# 10 ^ 1
	long		0x40050000,0xC8000000,0x00000000	# 10 ^ 2
	long		0x400C0000,0x9C400000,0x00000000	# 10 ^ 4
	long		0x40190000,0xBEBC2000,0x00000000	# 10 ^ 8
	long		0x40340000,0x8E1BC9BF,0x04000000	# 10 ^ 16
	long		0x40690000,0x9DC5ADA8,0x2B70B59E	# 10 ^ 32
	long		0x40D30000,0xC2781F49,0xFFCFA6D5	# 10 ^ 64
	long		0x41A80000,0x93BA47C9,0x80E98CE0	# 10 ^ 128
	long		0x43510000,0xAA7EEBFB,0x9DF9DE8E	# 10 ^ 256
	long		0x46A30000,0xE319A0AE,0xA60E91C7	# 10 ^ 512
	long		0x4D480000,0xC9767586,0x81750C17	# 10 ^ 1024
	long		0x5A920000,0x9E8B3B5D,0xC53D5DE5	# 10 ^ 2048
	long		0x75250000,0xC4605202,0x8A20979B	# 10 ^ 4096
#round to minus infinity
BIGRZRM:
	long		0x3ffe0000,0xb17217f7,0xd1cf79ab	# ln(2)
	long		0x40000000,0x935d8ddd,0xaaa8ac16	# ln(10)
	long		0x3fff0000,0x80000000,0x00000000	# 10 ^ 0

	global		PTENRM
PTENRM:
	long		0x40020000,0xA0000000,0x00000000	# 10 ^ 1
	long		0x40050000,0xC8000000,0x00000000	# 10 ^ 2
	long		0x400C0000,0x9C400000,0x00000000	# 10 ^ 4
	long		0x40190000,0xBEBC2000,0x00000000	# 10 ^ 8
	long		0x40340000,0x8E1BC9BF,0x04000000	# 10 ^ 16
	long		0x40690000,0x9DC5ADA8,0x2B70B59D	# 10 ^ 32
	long		0x40D30000,0xC2781F49,0xFFCFA6D5	# 10 ^ 64
	long		0x41A80000,0x93BA47C9,0x80E98CDF	# 10 ^ 128
	long		0x43510000,0xAA7EEBFB,0x9DF9DE8D	# 10 ^ 256
	long		0x46A30000,0xE319A0AE,0xA60E91C6	# 10 ^ 512
	long		0x4D480000,0xC9767586,0x81750C17	# 10 ^ 1024
	long		0x5A920000,0x9E8B3B5D,0xC53D5DE5	# 10 ^ 2048
	long		0x75250000,0xC4605202,0x8A20979A	# 10 ^ 4096
#round to positive infinity
BIGRP:
	long		0x3ffe0000,0xb17217f7,0xd1cf79ac	# ln(2)
	long		0x40000000,0x935d8ddd,0xaaa8ac17	# ln(10)
	long		0x3fff0000,0x80000000,0x00000000	# 10 ^ 0

	global		PTENRP
PTENRP:
	long		0x40020000,0xA0000000,0x00000000	# 10 ^ 1
	long		0x40050000,0xC8000000,0x00000000	# 10 ^ 2
	long		0x400C0000,0x9C400000,0x00000000	# 10 ^ 4
	long		0x40190000,0xBEBC2000,0x00000000	# 10 ^ 8
	long		0x40340000,0x8E1BC9BF,0x04000000	# 10 ^ 16
	long		0x40690000,0x9DC5ADA8,0x2B70B59E	# 10 ^ 32
	long		0x40D30000,0xC2781F49,0xFFCFA6D6	# 10 ^ 64
	long		0x41A80000,0x93BA47C9,0x80E98CE0	# 10 ^ 128
	long		0x43510000,0xAA7EEBFB,0x9DF9DE8E	# 10 ^ 256
	long		0x46A30000,0xE319A0AE,0xA60E91C7	# 10 ^ 512
	long		0x4D480000,0xC9767586,0x81750C18	# 10 ^ 1024
	long		0x5A920000,0x9E8B3B5D,0xC53D5DE6	# 10 ^ 2048
	long		0x75250000,0xC4605202,0x8A20979B	# 10 ^ 4096

#	xref	nrm_zero	
#	xref	decbin	
#	xref	round	

	global		get_op
	global		uns_getop
	global		uni_getop
get_op:
	clr.b		DY_MO_FLG(%a6)
	tst.b		UFLG_TMP(%a6)	# test flag for unsupp/unimp state
	beq.b		uni_getop

uns_getop:
	btst		&direction_bit,CMDREG1B(%a6)
	bne.w		opclass3	# branch if a fmove out (any kind)
	btst		&6,CMDREG1B(%a6)
	beq.b		uns_notpacked

	bfextu		CMDREG1B(%a6){&3:&3},%d0
	cmp.b		%d0,&3
	beq.w		pack_source	# check for a packed src op, branch if so
uns_notpacked:
	bsr		chk_dy_mo	# set the dyadic/monadic flag
	tst.b		DY_MO_FLG(%a6)
	beq.b		src_op_ck	# if monadic, go check src op
#				;else, check dst op (fall through)

	btst		&7,DTAG(%a6)
	beq.b		src_op_ck	# if dst op is norm, check src op
	bra.b		dst_ex_dnrm	# else, handle destination unnorm/dnrm

uni_getop:
	bfextu		CMDREG1B(%a6){&0:&6},%d0	# get opclass and src fields
	cmp.l		%d0,&0x17	# if op class and size fields are $17, 
#				;it is FMOVECR; if not, continue
#
# If the instruction is fmovecr, exit get_op.  It is handled
# in do_func and smovecr.sa.
#
	bne.w		not_fmovecr	# handle fmovecr as an unimplemented inst
	rts

not_fmovecr:
	btst		&E1,E_BYTE(%a6)	# if set, there is a packed operand
	bne.w		pack_source	# check for packed src op, branch if so

# The following lines of are coded to optimize on normalized operands
	mov.b		STAG(%a6),%d0
	or.b		DTAG(%a6),%d0	# check if either of STAG/DTAG msb set
	bmi.b		dest_op_ck	# if so, some op needs to be fixed
	rts

dest_op_ck:
	btst		&7,DTAG(%a6)	# check for unsupported data types in
	beq.b		src_op_ck	# the destination, if not, check src op
	bsr		chk_dy_mo	# set dyadic/monadic flag
	tst.b		DY_MO_FLG(%a6)
	beq.b		src_op_ck	# if monadic, check src op
#
# At this point, destination has an extended denorm or unnorm.
#
dst_ex_dnrm:
	mov.w		FPTEMP_EX(%a6),%d0	# get destination exponent
	and.w		&0x7fff,%d0	# mask sign, check if exp = 0000
	beq.b		src_op_ck	# if denorm then check source op.
#				;denorms are taken care of in res_func 
#				;(unsupp) or do_func (unimp)
#				;else unnorm fall through
	lea.l		FPTEMP(%a6),%a0	# point a0 to dop - used in mk_norm
	bsr		mk_norm		# go normalize - mk_norm returns:
#				;L_SCR1{7:5} = operand tag 
#				;	(000 = norm, 100 = denorm)
#				;L_SCR1{4} = fpte15 or ete15 
#				;	0 = exp >  $3fff
#				;	1 = exp <= $3fff
#				;and puts the normalized num back 
#				;on the fsave stack
#
	mov.b		L_SCR1(%a6),DTAG(%a6)	# write the new tag & fpte15 
#				;to the fsave stack and fall 
#				;through to check source operand
#
src_op_ck:
	btst		&7,STAG(%a6)
	beq.w		end_getop	# check for unsupported data types on the
#				;source operand
	btst		&5,STAG(%a6)
	bne.b		src_sd_dnrm	# if bit 5 set, handle sgl/dbl denorms
#
# At this point only unnorms or extended denorms are possible.
#
src_ex_dnrm:
	mov.w		ETEMP_EX(%a6),%d0	# get source exponent
	and.w		&0x7fff,%d0	# mask sign, check if exp = 0000
	beq.w		end_getop	# if denorm then exit, denorms are 
#				;handled in do_func
	lea.l		ETEMP(%a6),%a0	# point a0 to sop - used in mk_norm
	bsr		mk_norm		# go normalize - mk_norm returns:
#				;L_SCR1{7:5} = operand tag 
#				;	(000 = norm, 100 = denorm)
#				;L_SCR1{4} = fpte15 or ete15 
#				;	0 = exp >  $3fff
#				;	1 = exp <= $3fff
#				;and puts the normalized num back 
#				;on the fsave stack
#
	mov.b		L_SCR1(%a6),STAG(%a6)	# write the new tag & ete15 
	rts				# end_getop

#
# At this point, only single or double denorms are possible.
# If the inst is not fmove, normalize the source.  If it is,
# do nothing to the input.
#
src_sd_dnrm:
	btst		&4,CMDREG1B(%a6)	# differentiate between sgl/dbl denorm
	bne.b		is_double
is_single:
	mov.w		&0x3f81,%d1	# write bias for sgl denorm
	bra.b		common		# goto the common code
is_double:
	mov.w		&0x3c01,%d1	# write the bias for a dbl denorm
common:
	btst		&sign_bit,ETEMP_EX(%a6)	# grab sign bit of mantissa
	beq.b		pos
	bset		&15,%d1		# set sign bit because it is negative
pos:
	mov.w		%d1,ETEMP_EX(%a6)
#				;put exponent on stack

	mov.w		CMDREG1B(%a6),%d1
	and.w		&0xe3ff,%d1	# clear out source specifier
	or.w		&0x0800,%d1	# set source specifier to extended prec
	mov.w		%d1,CMDREG1B(%a6)	# write back to the command word in stack
#				;this is needed to fix unsupp data stack
	lea.l		ETEMP(%a6),%a0	# point a0 to sop
	bsr		mk_norm		# convert sgl/dbl denorm to norm
	mov.b		L_SCR1(%a6),STAG(%a6)	# put tag into source tag reg - d0
	rts				# end_getop
#
# At this point, the source is definitely packed, whether
# instruction is dyadic or monadic is still unknown
#
pack_source:
	mov.l		FPTEMP_LO(%a6),ETEMP(%a6)	# write ms part of packed 
#				;number to etemp slot
	bsr		chk_dy_mo	# set dyadic/monadic flag
	bsr		unpack

	tst.b		DY_MO_FLG(%a6)
	beq.b		end_getop	# if monadic, exit
#				;else, fix FPTEMP
pack_dya:
	bfextu		CMDREG1B(%a6){&6:&3},%d0	# extract dest fp reg
	mov.l		&7,%d1
	sub.l		%d0,%d1
	clr.l		%d0
	bset		%d1,%d0		# set up d0 as a dynamic register mask
	fmovm.x		%d0,FPTEMP(%a6)	# write to FPTEMP

	btst		&7,DTAG(%a6)	# check dest tag for unnorm or denorm
	bne.w		dst_ex_dnrm	# else, handle the unnorm or ext denorm
#
# Dest is not denormalized.  Check for norm, and set fpte15 
# accordingly.
#
	mov.b		DTAG(%a6),%d0
	and.b		&0xf0,%d0	# strip to only dtag:fpte15
	tst.b		%d0		# check for normalized value
	bne.b		end_getop	# if inf/nan/zero leave get_op
	mov.w		FPTEMP_EX(%a6),%d0
	and.w		&0x7fff,%d0
	cmp.w		%d0,&0x3fff	# check if fpte15 needs setting
	bge.b		end_getop	# if >= $3fff, leave fpte15=0
	or.b		&0x10,DTAG(%a6)
	bra.b		end_getop

#
# At this point, it is either an fmoveout packed, unnorm or denorm
#
opclass3:
	clr.b		DY_MO_FLG(%a6)	# set dyadic/monadic flag to monadic
	bfextu		CMDREG1B(%a6){&4:&2},%d0
	cmp.b		%d0,&3
	bne.w		src_ex_dnrm	# if not equal, must be unnorm or denorm
#				;else it is a packed move out
#				;exit
end_getop:
	rts

#
# Sets the DY_MO_FLG correctly. This is used only on if it is an
# unuspported data type exception.  Set if dyadic.
#
chk_dy_mo:
	mov.w		CMDREG1B(%a6),%d0
	btst		&5,%d0		# testing extension command word
	beq.b		set_mon		# if bit 5 = 0 then monadic
	btst		&4,%d0		# know that bit 5 = 1
	beq.b		set_dya		# if bit 4 = 0 then dyadic
	and.w		&0x007f,%d0	# get rid of all but extension bits {6:0}
	cmp.w		%d0,&0x0038	# if extension = $38 then fcmp (dyadic)
	bne.b		set_mon
set_dya:
	st.b		DY_MO_FLG(%a6)	# set the inst flag type to dyadic
	rts
set_mon:
	clr.b		DY_MO_FLG(%a6)	# set the inst flag type to monadic
	rts
#
#	MK_NORM
#
# Normalizes unnormalized numbers, sets tag to norm or denorm, sets unfl
# exception if denorm.
#
# CASE opclass 0x0 unsupp
#	mk_norm till msb set
#	set tag = norm
#
# CASE opclass 0x0 unimp
#	mk_norm till msb set or exp = 0
#	if integer bit = 0
#	   tag = denorm
#	else
#	   tag = norm
#
# CASE opclass 011 unsupp
#	mk_norm till msb set or exp = 0
#	if integer bit = 0
#	   tag = denorm
#	   set unfl_nmcexe = 1
#	else
#	   tag = norm
#
# if exp <= $3fff
#   set ete15 or fpte15 = 1
# else set ete15 or fpte15 = 0

# input:
#	a0 = points to operand to be normalized
# output:
#	L_SCR1{7:5} = operand tag (000 = norm, 100 = denorm)
#	L_SCR1{4}   = fpte15 or ete15 (0 = exp > $3fff, 1 = exp <=$3fff)
#	the normalized operand is placed back on the fsave stack
mk_norm:
	clr.l		L_SCR1(%a6)
	bclr		&sign_bit,LOCAL_EX(%a0)
	sne		LOCAL_SGN(%a0)	# transform into internal extended format

	cmp.b		1+EXC_VEC(%a6),&0x2c	# check if unimp
	bne.b		uns_data	# branch if unsupp
	bsr		uni_inst	# call if unimp (opclass 0x0)
	bra.b		reload
uns_data:
	btst		&direction_bit,CMDREG1B(%a6)	# check transfer direction
	bne.b		bit_set		# branch if set (opclass 011)
	bsr		uns_opx		# call if opclass 0x0
	bra.b		reload
bit_set:
	bsr		uns_op3		# opclass 011
reload:
	cmp.w		LOCAL_EX(%a0),&0x3fff	# if exp > $3fff
	bgt.b		end_mk		# fpte15/ete15 already set to 0
	bset		&4,L_SCR1(%a6)	# else set fpte15/ete15 to 1
#				;calling routine actually sets the 
#				;value on the stack (along with the 
#				;tag), since this routine doesn't 
#				;know if it should set ete15 or fpte15
#				;ie, it doesn't know if this is the 
#				;src op or dest op.
end_mk:
	bfclr		LOCAL_SGN(%a0){&0:&8}
	beq.b		end_mk_pos
	bset		&sign_bit,LOCAL_EX(%a0)	# convert back to IEEE format
end_mk_pos:
	rts
#
#     CASE opclass 011 unsupp
#
uns_op3:
	bsr		nrm_zero	# normalize till msb = 1 or exp = zero
	btst		&7,LOCAL_HI(%a0)	# if msb = 1
	bne.b		no_unfl		# then branch
set_unfl:
	or.w		&dnrm_tag,L_SCR1(%a6)	# set denorm tag
	bset		&unfl_bit,FPSR_EXCEPT(%a6)	# set unfl exception bit
no_unfl:
	rts
#
#     CASE opclass 0x0 unsupp
#
uns_opx:
	bsr		nrm_zero	# normalize the number
	btst		&7,LOCAL_HI(%a0)	# check if integer bit (j-bit) is set 
	beq.b		uns_den		# if clear then now have a denorm
uns_nrm:
	or.b		&norm_tag,L_SCR1(%a6)	# set tag to norm
	rts
uns_den:
	or.b		&dnrm_tag,L_SCR1(%a6)	# set tag to denorm
	rts
#
#     CASE opclass 0x0 unimp
#
uni_inst:
	bsr		nrm_zero
	btst		&7,LOCAL_HI(%a0)	# check if integer bit (j-bit) is set 
	beq.b		uni_den		# if clear then now have a denorm
uni_nrm:
	or.b		&norm_tag,L_SCR1(%a6)	# set tag to norm
	rts
uni_den:
	or.b		&dnrm_tag,L_SCR1(%a6)	# set tag to denorm
	rts

#
#	Decimal to binary conversion
#
# Special cases of inf and NaNs are completed outside of decbin.  
# If the input is an snan, the snan bit is not set.
# 
# input:
#	ETEMP(a6)	- points to packed decimal string in memory
# output:
#	fp0	- contains packed string converted to extended precision
#	ETEMP	- same as fp0
unpack:
	mov.w		CMDREG1B(%a6),%d0	# examine command word, looking for fmove's
	and.w		&0x3b,%d0
	beq		move_unpack	# special handling for fmove: must set FPSR_CC

	mov.w		ETEMP(%a6),%d0	# get word with inf information
	bfextu		%d0{&20:&12},%d1	# get exponent into d1
	cmp.w		%d1,&0x0fff	# test for inf or NaN
	bne.b		try_zero	# if not equal, it is not special
	bfextu		%d0{&17:&3},%d1	# get SE and y bits into d1
	cmp.w		%d1,&7		# SE and y bits must be on for special
	bne.b		try_zero	# if not on, it is not special
#input is of the special cases of inf and NaN
	tst.l		ETEMP_HI(%a6)	# check ms mantissa
	bne.b		fix_nan		# if non-zero, it is a NaN
	tst.l		ETEMP_LO(%a6)	# check ls mantissa
	bne.b		fix_nan		# if non-zero, it is a NaN
	bra.w		finish		# special already on stack
fix_nan:
	btst		&signan_bit,ETEMP_HI(%a6)	# test for snan
	bne.w		finish
	or.l		&snaniop_mask,USER_FPSR(%a6)	# always set snan if it is so
	bra.w		finish
try_zero:
	mov.w		ETEMP_EX+2(%a6),%d0	# get word 4
	and.w		&0x000f,%d0	# clear all but last ni(y)bble
	tst.w		%d0		# check for zero.
	bne.w		not_spec
	tst.l		ETEMP_HI(%a6)	# check words 3 and 2
	bne.w		not_spec
	tst.l		ETEMP_LO(%a6)	# check words 1 and 0
	bne.w		not_spec
	tst.l		ETEMP(%a6)	# test sign of the zero
	bge.b		pos_zero
	mov.l		&0x80000000,ETEMP(%a6)	# write neg zero to etemp
	clr.l		ETEMP_HI(%a6)
	clr.l		ETEMP_LO(%a6)
	bra.w		finish
pos_zero:
	clr.l		ETEMP(%a6)
	clr.l		ETEMP_HI(%a6)
	clr.l		ETEMP_LO(%a6)
	bra.w		finish

not_spec:
	fmovm.x		&0x1,-(%sp)	# save fp0 - decbin returns in it {%fp0}
	bsr		decbin
	fmov.x		%fp0,ETEMP(%a6)	# put the unpacked sop in the fsave stack
	fmovm.x		(%sp)+,&0x80	#  {%fp0}
	fmov.l		&0,%status	# clr fpsr from decbin
	bra		finish

#
# Special handling for packed move in:  Same results as all other
# packed cases, but we must set the FPSR condition codes properly.
#
move_unpack:
	mov.w		ETEMP(%a6),%d0	# get word with inf information
	bfextu		%d0{&20:&12},%d1	# get exponent into d1
	cmp.w		%d1,&0x0fff	# test for inf or NaN
	bne.b		mtry_zero	# if not equal, it is not special
	bfextu		%d0{&17:&3},%d1	# get SE and y bits into d1
	cmp.w		%d1,&7		# SE and y bits must be on for special
	bne.b		mtry_zero	# if not on, it is not special
#input is of the special cases of inf and NaN
	tst.l		ETEMP_HI(%a6)	# check ms mantissa
	bne.b		mfix_nan	# if non-zero, it is a NaN
	tst.l		ETEMP_LO(%a6)	# check ls mantissa
	bne.b		mfix_nan	# if non-zero, it is a NaN
#input is inf
	or.l		&inf_mask,USER_FPSR(%a6)	# set I bit
	tst.l		ETEMP(%a6)	# check sign
	bge.w		finish
	or.l		&neg_mask,USER_FPSR(%a6)	# set N bit
	bra.w		finish		# special already on stack
mfix_nan:
	or.l		&nan_mask,USER_FPSR(%a6)	# set NaN bit
	mov.b		&nan_tag,STAG(%a6)	# set stag to NaN
	btst		&signan_bit,ETEMP_HI(%a6)	# test for snan
	bne.b		mn_snan
	or.l		&snaniop_mask,USER_FPSR(%a6)	# set snan bit
	btst		&snan_bit,FPCR_ENABLE(%a6)	# test for snan enabled
	bne.b		mn_snan
	bset		&signan_bit,ETEMP_HI(%a6)	# force snans to qnans
mn_snan:
	tst.l		ETEMP(%a6)	# check for sign
	bge.w		finish		# if clr, go on
	or.l		&neg_mask,USER_FPSR(%a6)	# set N bit
	bra.w		finish

mtry_zero:
	mov.w		ETEMP_EX+2(%a6),%d0	# get word 4
	and.w		&0x000f,%d0	# clear all but last ni(y)bble
	tst.w		%d0		# check for zero.
	bne.b		mnot_spec
	tst.l		ETEMP_HI(%a6)	# check words 3 and 2
	bne.b		mnot_spec
	tst.l		ETEMP_LO(%a6)	# check words 1 and 0
	bne.b		mnot_spec
	tst.l		ETEMP(%a6)	# test sign of the zero
	bge.b		mpos_zero
	or.l		&neg_mask+z_mask,USER_FPSR(%a6)	# set N and Z
	mov.l		&0x80000000,ETEMP(%a6)	# write neg zero to etemp
	clr.l		ETEMP_HI(%a6)
	clr.l		ETEMP_LO(%a6)
	bra.b		finish
mpos_zero:
	or.l		&z_mask,USER_FPSR(%a6)	# set Z
	clr.l		ETEMP(%a6)
	clr.l		ETEMP_HI(%a6)
	clr.l		ETEMP_LO(%a6)
	bra.b		finish

mnot_spec:
	fmovm.x		&0x1,-(%sp)	# save fp0 - decbin returns in it {%fp0}
	bsr		decbin
	fmov.x		%fp0,ETEMP(%a6)
#				;put the unpacked sop in the fsave stack
	fmovm.x		(%sp)+,&0x80	#  {%fp0}

finish:
	mov.w		CMDREG1B(%a6),%d0	# get the command word
	and.w		&0xfbff,%d0	# change the source specifier field to 
#				;extended (was packed).
	mov.w		%d0,CMDREG1B(%a6)	# write command word back to fsave stack
#				;we need to do this so the 040 will 
#				;re-execute the inst. without taking 
#				;another packed trap.

fix_stag:
#Converted result is now in etemp on fsave stack, now set the source 
#tag (stag) 
#	if (ete =$7fff) then INF or NAN
#		if (etemp = $x.0----0) then
#			stag = INF
#		else
#			stag = NAN
#	else
#		if (ete = $0000) then
#			stag = ZERO
#		else
#			stag = NORM
#
# Note also that the etemp_15 bit (just right of the stag) must
# be set accordingly.  
#
	mov.w		ETEMP_EX(%a6),%d1
	and.w		&0x7fff,%d1	# strip sign
	cmp.w		%d1,&0x7fff
	bne.b		z_or_nrm
	mov.l		ETEMP_HI(%a6),%d1
	bne.b		is_nan
	mov.l		ETEMP_LO(%a6),%d1
	bne.b		is_nan
is_inf:
	mov.b		&0x40,STAG(%a6)
	mov.l		&0x40,%d0
	rts
is_nan:
	mov.b		&0x60,STAG(%a6)
	mov.l		&0x60,%d0
	rts
z_or_nrm:
	tst.w		%d1
	bne.b		is_nrm
is_zro:
# For a zero, set etemp_15
	mov.b		&0x30,STAG(%a6)
	mov.l		&0x20,%d0
	rts
is_nrm:
# For a norm, check if the exp <= $3fff; if so, set etemp_15
	cmp.w		%d1,&0x3fff
	ble.b		set_bit15
	mov.b		&0,STAG(%a6)
	bra.b		end_is_nrm
set_bit15:
	mov.b		&0x10,STAG(%a6)
end_is_nrm:
	mov.l		&0,%d0
end_fix:
	rts
end_get:
	rts
#	end		
