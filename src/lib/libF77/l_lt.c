/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) l_lt.c: version 25.1 created on 12/2/91 at 18:34:04	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)l_lt.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"libF77:l_lt.c	1.3"	*/
	
#ident	"@(#)libF77:l_lt.c	25.1"

long int l_lt(a,b,la,lb)
char *a, *b;
long int la, lb;
{
return(s_cmp(a,b,la,lb) < 0);
}
