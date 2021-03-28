#ident	"@(#)_sinh881.s	23.4	8/27/91 Copyright (c) 1991 by Arix Corp."
	ident	"@(#)_sinh881.s	23.4	8/27/91 Copyright (c) 1991 by Arix Corp."
#		Copyright (c) 1985 AT&T		#
#		All Rights Reserved		#
#	/OLD MOT:_sinh881.s	6.1	
	



	file	"_sinh881.s"

# unsigned int
# _sinh881(x,y)
# double x;
# double *y;
# {
	set	F%1,-4
_m4_ifdef_(`__STRICT_STDC__',
	global	_sinh881
_sinh881:,
	global	sinh881
sinh881:)
	link.l	%fp,&F%1
# unsigned register int exc;		/* imagine exc as being %d0 */
# exc = fsinh(x,&y);
	move.d	(8,%fp),-(%sp)
	jsr fsinhd
	add.w	&8,%sp
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
# _cosh881(x,y)
# double x;
# double *y;
# {
	set	F%2,-4
_m4_ifdef_(`__STRICT_STDC__',
	global	_cosh881
_cosh881:,
	global	cosh881
cosh881:)
	link.l	%fp,&F%2
# unsigned register int exc;		/* imagine exc as being %d0 */
# exc = fcosh(x,&y);
	move.d	(8,%fp),-(%sp)
	jsr fcoshd
	add.w	&8,%sp
	fmovm.l	%status,%d0		# contains any exception or
					# condition code info
	fmov.d	%fp0,([16.w,%fp])
	fmovm.l	%status,%d1		# may contain overflow info
	or.l	%d1,%d0
# return (exc);				/* it is already in %d0 */
# }
	unlk	%fp
	rts
