#ident	"@(#)getitimer.s	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
	ident	"@(#)getitimer.s	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
#	Copyright (c) 1984 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#	Copyright (c) 1988 by Arix Corporation
#	All Rights Reserved


# 	trap vectors
	_m4_include_(SYSTRAP_H)
	
	file	"getitimer.s"
	global	getitimer
	global	cerror%

getitimer:
	MCOUNT
	mov.l	&getitimer%,%d0
	trap	&0
	bcc.b 	noerror
	jmp 	cerror%
noerror:
	rts
