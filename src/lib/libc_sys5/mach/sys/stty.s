#ident	"@(#)stty.s	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
	ident	"@(#)stty.s	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.
#	MOT:stty.s	2.1 

# C library -- stty

	file	"stty.s"
	set	stty%,31
	global	stty
	global	cerror%

stty:
	MCOUNT
	mov.l	&stty%,%d0
	trap	&0
	bcc.b	noerror
	jmp	cerror%
noerror:
	rts
