/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) untouchwin.c: version 25.1 created on 12/2/91 at 19:28:07	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)untouchwin.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"curses:screen/untouchwin.c	1.3"	*/
	
#ident	"@(#)lib/libcurses:untouchwin.c	25.1"

#include	"curses_inc.h"

untouchwin(win)
WINDOW	*win;
{
    return (wtouchln(win,0,win->_maxy,FALSE));
}
