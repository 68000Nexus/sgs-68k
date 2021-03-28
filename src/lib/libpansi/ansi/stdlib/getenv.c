#ident	"@(#)getenv.c	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"libc-port:gen/getenv.c	1.6"	*/
	

/*LINTLIBRARY*/
/*
 *	getenv(name)
 *	returns ptr to value associated with name, if any, else NULL
 */
#include "shlib.h"
#include <stddef.h>
#include <_stdlib.h>

static char *nvmatch ( register char *s1, register char *s2 ); /* forward */

char *getenv( const char *name )
{
	register char *v, **p=_environ;

	if(p == NULL)
		return(NULL);
	while(*p != NULL)
		if((v = nvmatch(name, *p++)) != NULL)
			return(v);
	return(NULL);
}

/*
 *	s1 is either name, or name=value
 *	s2 is name=value
 *	if names match, return value of s2, else NULL
 *	used for environment searching: see getenv
 */

static char *nvmatch ( register char *s1, register char *s2 )
{
	while(*s1 == *s2++)
		if(*s1++ == '=')
			return(s2);
	if(*s1 == '\0' && *(s2-1) == '=')
		return(s2);
	return(NULL);
}
