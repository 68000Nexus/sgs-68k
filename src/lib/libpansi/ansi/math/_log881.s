#ident	"@(#)_log881.s	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
	ident	"@(#)_log881.s	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
#		Copyright (c) 1985 AT&T		#
#		All Rights Reserved		#
#	/OLD MOT:_log881.s	7.2	
	

#	/OLD MOT:_log881.s	7.1	
	



	file	"_log881.s"

# unsigned int
# _log881(x,y)
# double x;
# double *y;
# {
	set	F%1,-4
_m4_ifdef_(`__STRICT_STDC__',
	global	_log881
_log881:,
	global	log881
log881:)
	link.l	%fp,&F%1
# unsigned register int exc;		/* imagine exc as being %d0 */
# exc = flog(x,*y);
_m4_ifdef_(`EMU',
`       mov.d (8,%fp),-(%sp)
	jsr flog10d
	add.w   &8,%sp',
`       flog10.d (8,%fp),%fp0')
	fmovm.l	%status,%d0		# contains any exception or
					# condition code info
	fmov.d	%fp0,([16.w,%fp])
	fmovm.l	%status,%d1		# may contain overflow info
	or.l	%d1,%d0
# return (exc);				/* it is already in %d0 */
# }
	unlk	%fp
	rts


# unsigned int
# _logn881(x,y)
# double x;
# double *y;
# {
	set	F%2,-4
_m4_ifdef_(`__STRICT_STDC__',
	global	_logn881
_logn881:,
	global	logn881
logn881:)
	link.l	%fp,&F%2
# unsigned register int exc;		/* imagine exc as being %d0 */
# exc = flogn(x,*y);
	fmov.d	(8,%fp),%fp1
	fcmp.d	%fp1,&0x3fe0000000000000
	fblt	L%13	# true that x < 0.5
	fsub.w	&1,%fp1
_m4_ifdef_(`EMU',
`       fmov.x %fp1,-(%sp)
	jsr flognp1x
	add.w   &12,%sp',
`       flognp1.x %fp1,%fp0')
back:
	fmovm.l	%status,%d0		# contains any exception or
					# condition code info
	fmov.d	%fp0,([16.w,%fp])
	fmovm.l	%status,%d1		# may contain overflow info
	or.l	%d1,%d0
# return (exc);				/* it is already in %d0 */
# }
	unlk	%fp
	rts
L%13:
_m4_ifdef_(`EMU',
`       fmov.x %fp1,-(%sp)
	jsr flognx
	add.w   &12,%sp',
`       flogn.x %fp1,%fp0')
	bra.w	back
