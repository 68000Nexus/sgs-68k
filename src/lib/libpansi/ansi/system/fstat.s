#ident	"@(#)fstat.s	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
	ident	"@(#)fstat.s	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.
#	MOT:fstat.s	2.1 
# C library -- fstat


# error = fstat(file, statbuf);

# char statbuf[34]

# 	trap vectors
	_m4_include_(SYSTRAP_H)

	file	"fstat.s"
	global	fstat
	global	cerror%

fstat:
	MCOUNT
	mov.l	&fstat%,%d0
	trap	&0
	bcc.b 	noerror
	jmp 	cerror%
noerror:
	mov.l	&0,%d0
	rts
