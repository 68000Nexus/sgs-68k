/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) errno_def.c: version 25.1 created on 12/2/91 at 20:16:48	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)errno_def.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/


/*	ATT:#ident	"libnsl:nsl/__errno_def.c	1.2"	*/
	
#ident	"@(#)nsl:errno_def.c	25.1"

void (*__errno)() = 0;
