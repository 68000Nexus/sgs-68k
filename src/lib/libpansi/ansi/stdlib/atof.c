#ident	"@(#)atof.c	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"libc-port:gen/atof.c	2.16"	*/
/*	ATT:#ident	"libc-port:gen/atof.c	2.14"	*/
	

/*LINTLIBRARY*/
/*
 *	C library - ascii to floating (atof) and string to double (strtod)
 *
 *	This version compiles both atof and strtod depending on the value
 *	of STRTOD, which is set in the file and may be overridden on the
 *	"cc" command line.  The only difference is the storage of a pointer
 *	to the character which terminated the conversion.
 *	Long-integer accumulation is used, except on the PDP11, where
 *	"long" arithmetic is simulated, so floating-point is much faster.
 */
#ifndef STRTOD
#define STRTOD	0
#endif
#include "shlib.h"
#include <ctype.h>
#include <values.h>
#include <math.h>
#include <string.h>
#include "_locale.h"

#if u3b || u3b5 || m68 || m68k || M68020
#	define _POW1_25LEN	10 /* must be numbits(DMAXEXP/2) */
#else
#	define _POW1_25LEN	6
#endif
static double _pow1_25[_POW1_25LEN] = { 0.0 };

#if STRTOD
#define _STORE_PTR	(*ptr = p)
#define _DEC_PTR		(*ptr = p - 1)
#define _GOT_DIGIT	(got_digit++)
#define _RET_ZERO(val)	if (!got_digit) return (0.0)

extern double strtod(register char *p, char **ptr)
#else
#define _STORE_PTR
#define _DEC_PTR
#define _GOT_DIGIT
#define _RET_ZERO(val)	if (!val) return (0.0)

extern double atof( register char *p)
#endif
{
	register int c;
	int exp = 0, neg_val = 0;
	double fl_val;
	char *pp;
#if STRTOD
	int got_digit = 0;
	char *dummy;

	if (ptr == (char **)0)
		ptr = &dummy; /* harmless dumping place */
	_STORE_PTR;
#endif

	while (isspace(c = *p)) /* eat leading white space */
		p++;

	switch (c) { /* process sign */
	case '-':
		neg_val++;
	case '+': /* fall-through */
		p++;
	}

	{	/* accumulate value */
#if pdp11
	/* "long" arithmetic on the PDP-11 is simulated using int's and
	 * is outrageously slow, so the accumulation is done using double's */
		register int decpt = 0;

		fl_val = 0.0;
		while (isdigit(c = *p++) || c == *_monetary_num->decimal_point && 
			   !decpt++) {

			if ( c == *_monetary_num->decimal_point ) { /* must correct p */
				p+=(strlen(_monetary_num->decimal_point) - 1 );
				continue;
			}

			_GOT_DIGIT;
			exp -= decpt; /* decr exponent if dec point seen */
			if (fl_val < 2.0 * MAXPOWTWO)
				fl_val = 10.0 * fl_val + (double)(c - '0');
			else
				exp++;
		}
		_RET_ZERO(fl_val);
#else
		register long high = 0, low = 0, scale = 1;
		register int decpt = 0, nzeroes = 0;

		while (isdigit(c = *p++) || c == *_monetary_num->decimal_point && 
			   !decpt++) {

			if ( c == *_monetary_num->decimal_point ) { /* must correct p */
				p+=(strlen(_monetary_num->decimal_point) - 1 );
				continue;
			}

			_GOT_DIGIT;
			if (decpt) { /* handle trailing zeroes specially */
				if (c == '0') { /* ignore zero for now */
					nzeroes++;
					continue;
				}
				while (nzeroes > 0) { /* put zeroes back in */
					exp--;
					if (high < (long) (MAXLONG/10)) {
						high *= 10;
					} else if (scale < (long) (MAXLONG/10)) {
						scale *= 10;
						low *= 10;
					} else
						exp++;
					nzeroes--;
				}
				exp--; /* decr exponent if decimal pt. seen */
			}
			if (high < (long) (MAXLONG/10)) {
				high *= 10;
				high += c - '0';
			} else if (scale < (long) (MAXLONG/10)) {
				scale *= 10;
				low *= 10;
				low += c - '0';
			} else
				exp++;
		}
		_RET_ZERO(high);
		fl_val = (double)high;
		if (scale > 1)
			fl_val = (double)scale * fl_val + (double)low;
#endif
	}
	_DEC_PTR; /* in case there is no legitimate exponent */
	if (c == 'E' || c == 'e') { /* accumulate exponent */
		register int e_exp = 0, neg_exp = 0;
		_STORE_PTR;

		switch (*p) { /* process sign */
		case '-':
			neg_exp++;
		case '+': /* fall-through */
		case ' ': /* many FORTRAN environments generate this! */
			p++;
		}
		if (isdigit(c = *p)) { /* found a legitimate exponent */
			do {
				/* limit outrageously large exponents */
				if (e_exp < DMAXEXP)
					e_exp = 10 * e_exp + c - '0';
			} while (isdigit(c = *++p));
			if (neg_exp)
				exp -= e_exp;
			else
				exp += e_exp;
			_STORE_PTR;
		}
	}
#if STRTOD
	if (!fl_val) /* atof will already have returned, but strtod had */
		return (fl_val); /* to find the end of the exponent first */
#endif
	/*
	 * The following computation is done in two stages,
	 * first accumulating powers of (10/8), then jamming powers of 8,
	 * to avoid underflow in situations like the following (for
	 * the DEC representation): 1.2345678901234567890e-37,
	 * where exp would be about (-37 + -18) = -55, and the
	 * value 10^(-55) can't be represented, but 1.25^(-55) can
	 * be represented, and then 8^(-55) jammed via ldexp().
	 */
	if (exp != 0) { /* apply exponent */
		register double *powptr = _pow1_25;
		volatile double fl_exp = fl_val; /* hanna: 68881 cant use reg here */

		if (*powptr == 0.0) { /* need to initialize table */
			*powptr = 1.25;
			for (; powptr < &_pow1_25[_POW1_25LEN - 1]; powptr++)
				powptr[1] = *powptr * *powptr;
			powptr = _pow1_25;
		}
		if ((c = exp) < 0) {
			c = -c;
			fl_exp = 1.0;
		}
		if (c >= DMAXEXP/2) /* outrageously large exponents (= is 5.3.1
					update - kd)*/
			c = DMAXEXP/2; /* will be handled by ldexp */
		for ( ; ; powptr++) {
			/* binary representation of ints assumed; otherwise
			 * replace (& 01) by (% 2) and (>>= 1) by (/= 2) */
			if (c & 01)
				fl_exp *= *powptr;
			if ((c >>= 1) == 0)
				break;
		}
		fl_val = ldexp(exp < 0 ? fl_val/fl_exp : fl_exp, 3 * exp);
	}
	return (neg_val ? -fl_val : fl_val); /* apply sign */
}
