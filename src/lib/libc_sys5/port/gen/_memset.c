#ident	"@(#)_memset.c	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"libc-port:gen/memset.c	1.3"	*/
	

/*LINTLIBRARY*/
/*
 * Set an array of n chars starting at sp to the character c.
 * Return sp.
 */
char *
memset(sp, c, n)
register char *sp, c;
register int n;
{
	register char *sp0 = sp;

	while (--n >= 0)
		*sp++ = c;
	return (sp0);
}
