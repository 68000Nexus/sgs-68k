/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) label.c: version 25.1 created on 12/2/91 at 20:08:55	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)label.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT/plot:#ident	"libplot:plot/label.c	1.2"	*/
	
#ident	"@(#)libplot/plot:label.c	25.1"

#include <stdio.h>
label(s)
char *s;
{
	int i;
	putc('t',stdout);
	for(i=0;s[i];)putc(s[i++],stdout);
	putc('\n',stdout);
}
