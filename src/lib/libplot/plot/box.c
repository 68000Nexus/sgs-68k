/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) box.c: version 25.1 created on 12/2/91 at 20:08:37	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)box.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT/plot:#ident	"libplot:plot/box.c	1.2"	*/
	
#ident	"@(#)libplot/plot:box.c	25.1"

box(x0, y0, x1, y1)
{
	move(x0, y0);
	cont(x0, y1);
	cont(x1, y1);
	cont(x1, y0);
	cont(x0, y0);
	move(x1, y1);
}
