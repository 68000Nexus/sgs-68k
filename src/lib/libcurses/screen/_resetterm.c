/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) _resetterm.c: version 25.1 created on 12/2/91 at 19:15:37	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)_resetterm.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"curses:screen/_resetterm.c	1.2"	*/
	
#ident	"@(#)lib/libcurses:_resetterm.c	25.1"


#define		NOMACROS
#include	"curses_inc.h"

resetterm()
{
    return (reset_shell_mode());
}
