/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) repl.c: version 25.1 created on 12/2/91 at 18:45:05	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)repl.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"libPW:repl.c	3.4"	*/
	
#ident	"@(#)libPW:repl.c	25.1"

/*
	Replace each occurrence of `old' with `new' in `str'.
	Return `str'.
*/

repl(str,old,new)
char *str;
char old,new;
{
	return(trnslat(str, &old, &new, str));
}
