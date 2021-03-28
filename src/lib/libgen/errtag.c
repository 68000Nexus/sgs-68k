/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) errtag.c: version 25.1 created on 12/2/91 at 19:32:27	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)errtag.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"liberr:errtag.c	1.1"	*/
	
#ident	"@(#)libgen:errtag.c	25.1"


/*	Set tag to file name and line number;
	Used by errmsg() macro.
*/

#include	"errmsg.h"


void
errtag( str, num )
char	*str;
int	num;
{
	Err.tagstr = str;
	Err.tagnum = num;
}
