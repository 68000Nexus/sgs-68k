#ident	"@(#)opt_data.c	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"libc-port:gen/opt_data.c	1.3"	*/


/*
 * Global variables
 * used in getopt
 */

int	opterr = 1;
int	optind = 1;
int	optopt = 0;
char	*optarg = 0;
