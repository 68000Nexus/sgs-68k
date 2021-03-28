#ident	"@(#)scanf.c	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"libc-port:stdio/scanf.c	1.8"	*/
	

/*LINTLIBRARY*/
#include <stdio.h>
#include <varargs.h>

extern int _doscan();

/*VARARGS1*/
int
scanf(fmt, va_alist)
char *fmt;
va_dcl
{
	va_list ap;

	va_start(ap);
	return(_doscan(stdin, fmt, ap));
}

/*VARARGS2*/
int
fscanf(iop, fmt, va_alist)
FILE *iop;
char *fmt;
va_dcl
{
	va_list ap;

	va_start(ap);
	return(_doscan(iop, fmt, ap));
}

/*VARARGS2*/
int
sscanf(str, fmt, va_alist)
register char *str;
char *fmt;
va_dcl
{
	va_list ap;
	FILE strbuf;

	va_start(ap);
#ifndef DYNAMIC
	strbuf._flag = _IOREAD;
#else /* DYNAMIC */
	strbuf._flag = _IOREAD | _IOSTRING;
#endif /* DYNAMIC */
	strbuf._ptr = strbuf._base = (unsigned char*)str;
	strbuf._cnt = strlen(str);
#ifndef DYNAMIC
	strbuf._file = _NFILE;
#else /* DYNAMIC */
	strbuf._file = -1;
#endif /* DYNAMIC */
	return(_doscan(&strbuf, fmt, ap));
}
