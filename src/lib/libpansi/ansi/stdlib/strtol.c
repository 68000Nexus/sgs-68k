#ident	"@(#)strtol.c	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"libc-port:gen/strtol.c	2.6"	*/
	

/*LINTLIBRARY*/
#include "shlib.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#define _DIGIT(x)	(isdigit(x) ? (x) - '0' : \
			islower(x) ? (x) + 10 - 'a' : (x) + 10 - 'A')
#define _MBASE	('z' - 'a' + 1 + 10)

long strtol(register char *str, char **ptr, register int base)
{
	register long val;
	register int c;
	int xx, neg = 0;
	long smaller = 0;

	if (ptr != (char **)0)
		*ptr = str; /* in case no number is formed */

	if (base < 0 || base > _MBASE)
		return (0); /* base is invalid -- should be a fatal error */

	if (!isalnum(c = *str)) {
		while (isspace(c))
			c = *++str;
		switch (c) {
		case '-':
			neg++;
		case '+': /* fall-through */
			c = *++str;
		}
	}

	if (base == 0)
		if (c != '0')
			base = 10;
		else if (str[1] == 'x' || str[1] == 'X')
			base = 16;
		else
			base = 8;

	/*
	 * for any base > 10, the digits incrementally following
	 *	9 are assumed to be "abc...z" or "ABC...Z"
	 */
	if (!isalnum(c) || (xx = _DIGIT(c)) >= base)
		return (0); /* no number formed */

	if (base == 16 && c == '0' && isxdigit(str[2]) &&
	    (str[1] == 'x' || str[1] == 'X'))
		c = *(str += 2); /* skip over leading "0x" or "0X" */

	for (val = _DIGIT(c); isalnum(c = *++str) && (xx = _DIGIT(c)) < base; ) {
		val = base * val + xx;
		if (val < smaller ) {	/* overflow or underflow */
			errno = ERANGE;
			return ( neg ? LONG_MIN : LONG_MAX );
		}
		else smaller = val ;	/* for next time */
	}

	if (ptr != (char **)0)
		*ptr = str;

	return (neg ? -val : val);
}
