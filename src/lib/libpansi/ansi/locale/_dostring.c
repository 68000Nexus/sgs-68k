#ident	"@(#)_dostring.c	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

extern void _dostringread(char *start, char **mine)
{
	char *ptr;

	ptr = start ;
	while (**mine != '\0') { *ptr++ = **mine++; };
	*ptr++ = '\0'; 
	return;
}
