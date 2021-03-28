/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) slk_atroff.c: version 25.1 created on 12/2/91 at 19:25:42	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)slk_atroff.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)curses:slk_atroff.c	1.1"
#include "curses_inc.h"

slk_attroff (a)
chtype a;
{
    WINDOW *win;

    /* currently we change slk attribute only when using software */
    /* slk's.  However, we may introduce a new terminfo variable  */
    /* which would allow manipulating the hardware slk's as well  */

    if ((SP->slk == NULL) || ((win = SP->slk->_win) == NULL))
	return (ERR);

    return (wattroff (win, a));
}
    
