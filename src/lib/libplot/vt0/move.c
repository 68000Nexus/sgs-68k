/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) move.c: version 25.1 created on 12/2/91 at 20:13:34	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)move.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT/vt0:#ident	"libplot:vt0/move.c	1.2"	*/
	
#ident	"@(#)libplot/vt0:move.c	25.1"

extern vti;
extern xnow,ynow;
move(xi,yi){
	struct {char pad,c; int x,y;} p;
	p.c = 9;
	p.x = xnow = xsc(xi);
	p.y = ynow = ysc(yi);
	write(vti,&p.c,5);
}
