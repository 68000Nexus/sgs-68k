/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) print.h: version 25.1 created on 12/2/91 at 19:23:40	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)print.h	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"curses:screen/print.h	1.2"	*/
	
#ident	"@(#)libcurses/screen:print.h	25.1"


/* externs from iexpand.c, cexpand.c */
extern void tpr();
extern int cpr();
extern char *cexpand(), *iexpand(), *cconvert(), *rmpadding();

/* externs from print.c */
enum printtypes
    {
    pr_none,
    pr_terminfo,		/* print terminfo listing */
    pr_cap,			/* print termcap listing */
    pr_longnames		/* print C variable name listing */
    };

extern void pr_onecolumn();
extern void pr_caprestrict();
extern void pr_width();
extern void pr_init();
extern void pr_heading();
extern void pr_bheading();
extern void pr_boolean();
extern void pr_bfooting();
extern void pr_nheading();
extern void pr_number();
extern void pr_nfooting();
extern void pr_sheading();
extern void pr_string();
extern void pr_sfooting();
