#ident	"@(#)_strcspn.c	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"libc-port:gen/strcspn.c	1.4"	*/
	

/*LINTLIBRARY*/
/*
 * Return the number of characters in the maximum leading segment
 * of string which consists solely of characters NOT from charset.
 */
int
strcspn(string, charset)
char	*string;
register char	*charset;
{
	register char *p, *q;

	for(q=string; *q != '\0'; ++q) {
		for(p=charset; *p != '\0' && *p != *q; ++p)
			;
		if(*p != '\0')
			break;
	}
	return(q-string);
}
