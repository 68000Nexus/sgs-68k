#ident	"@(#)tolower.c	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#include <ctype.h>
#include "_ctype.h"

extern int tolower(int c)
{
	if (isupper(c))
		c = ((_ctype + 258)[c]);
	return(c);
}
