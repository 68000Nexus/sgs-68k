#ident	"@(#)acct.s	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
	ident	"@(#)acct.s	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

# C library -- acct
#	MOT:acct.s	2.1 



	file	"acct.s"
	set	acct%,51
	global	acct
	global  cerror%

acct:
	MCOUNT
	mov.l	&acct%,%d0
	trap	&0
	bcc.b 	noerror
	jmp 	cerror%
noerror:
	mov.l	&0,%d0
	rts
