/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) V3.winsch.c: version 25.1 created on 12/2/91 at 19:11:09	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)V3.winsch.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"curses:screen/V3.winsch.c	1.1"	*/
	
#ident	"@(#)lib/libcurses:V3.winsch.c	25.1"


#include	"curses_inc.h"
extern	int	_outchar();

#ifdef	_VR3_COMPAT_CODE
#undef	winsch
winsch(win, c)
WINDOW		*win;
_ochtype	c;
{
    return (w32insch(win, _FROM_OCHTYPE(c)));
}
#endif	/* _VR3_COMPAT_CODE */
