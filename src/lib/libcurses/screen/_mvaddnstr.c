/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) _mvaddnstr.c: version 25.1 created on 12/2/91 at 19:13:36	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)_mvaddnstr.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"curses:screen/_mvaddnstr.c	1.1"	*/
	
#ident	"@(#)lib/libcurses:_mvaddnstr.c	25.1"


#define		NOMACROS
#include	"curses_inc.h"

mvaddnstr(y, x, s, n)
int	y, x;
char	*s;
int	n;
{
    return (wmove(stdscr, y, x)==ERR?ERR:waddnstr(stdscr, s, n));
}
