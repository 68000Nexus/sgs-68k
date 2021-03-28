#ident	"@(#)xdr_float.c	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
/*
 *	System V NFS - Release 3.2/V3
 *
 *	Copyright 1986, 1987, 1988 Lachman Associates, Incorporated (LAI)
 *
 *	All Rights Reserved.
 *
 *	The copyright above and this notice must be preserved in all
 *	copies of this source code.  The copyright above does not
 *	evidence any actual or intended publication of this source
 *	code.
 *
 *	This is unpublished proprietary trade secret source code of
 *	Lachman Associates.  This source code may not be copied,
 *	disclosed, distributed, demonstrated or licensed except as
 *	expressly authorized by Lachman Associates.
 */
#ident	"@(#)xdr_float.c	3.3 LAI System V NFS Release 3.2/V3	source"
/*
 * xdr_float.c, Generic XDR routines impelmentation.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 *
 * These are the "floating point" xdr routines used to (de)serialize
 * most common data items.  See xdr.h for more info on the interface to
 * xdr.
 */
#ifndef EXOS
#include <sys/types.h>
#endif

#include <rpc/types.h>
#include <rpc/xdr.h>
#include <stdio.h>

/*
 * NB: Not portable.
 * This routine works on Suns (Sky / 68000's) and Vaxen.
 */
   
/* What IEEE single precision floating point looks like on a Vax */
struct  ieee_single {
	unsigned int	mantissa: 23;
	unsigned int	exp     : 8;
	unsigned int	sign    : 1;
};

/* Vax single precision floating point */
struct  vax_single {
	unsigned int	mantissa1 : 7;
	unsigned int	exp       : 8;
	unsigned int	sign      : 1;
	unsigned int	mantissa2 : 16;
		
};

#define VAX_SNG_BIAS    0x81
#define IEEE_SNG_BIAS   0x7f
 
static struct sgl_limits {
	struct vax_single s;
	struct ieee_single ieee;
} sgl_limits[2] = {
	{{ 0x3f, 0xff, 0x0, 0xffff },	/* Max Vax */
	{ 0x0, 0xff, 0x0 }},		/* Max IEEE */
	{{ 0x0, 0x0, 0x0, 0x0 },	/* Min Vax */
	{ 0x0, 0x0, 0x0 }}		/* Min IEEE */
};

bool_t 
xdr_float(xdrs, fp)
	register XDR *xdrs;
	register float *fp;
{
	struct ieee_single is;
	struct vax_single vs, *vsp;
	struct sgl_limits *lim;
	int i;

	switch (xdrs->x_op) {

	case XDR_ENCODE:
#ifdef sun
		return (XDR_PUTLONG(xdrs, (long *)fp));
#endif
#ifdef vax
		vs = *((struct vax_single *)fp);
		for (i = 0, lim = sgl_limits;
			i < sizeof(sgl_limits)/sizeof(struct sgl_limits);
			i++, lim++) {
			if ((vs.mantissa2 == lim->s.mantissa2) &&
				(vs.exp == lim->s.exp) &&
				(vs.mantissa1 == lim->s.mantissa1)) {
				is = lim->ieee;
				goto shipit;
			}
		}
		is.exp = vs.exp - VAX_SNG_BIAS + IEEE_SNG_BIAS;
		is.mantissa = (vs.mantissa1 << 16) | vs.mantissa2;
	shipit:
		is.sign = vs.sign;
		return (XDR_PUTLONG(xdrs, (long *)&is));
#endif
#ifdef u3b2
		/* don't know floating point format */
		return(FALSE);
#endif
#ifdef i386
		/* don't know floating point format */
		return(FALSE);
#endif

	case XDR_DECODE:
#ifdef sun
		return (XDR_GETLONG(xdrs, (long *)fp));
#endif
#ifdef vax
		vsp = (struct vax_single *)fp;
		if (!XDR_GETLONG(xdrs, (long *)&is))
			return (FALSE);
		for (i = 0, lim = sgl_limits;
			i < sizeof(sgl_limits)/sizeof(struct sgl_limits);
			i++, lim++) {
			if ((is.exp == lim->ieee.exp) &&
				(is.mantissa = lim->ieee.mantissa)) {
				*vsp = lim->s;
				goto doneit;
			}
		}
		vsp->exp = is.exp - IEEE_SNG_BIAS + VAX_SNG_BIAS;
		vsp->mantissa2 = is.mantissa;
		vsp->mantissa1 = (is.mantissa >> 16);
	doneit:
		vsp->sign = is.sign;
		return (TRUE);
#endif
#ifdef u3b2
		/* don't know floating point format */
		return(FALSE);
#endif
#ifdef i386
		/* don't know floating point format */
		return(FALSE);
#endif

	case XDR_FREE:
		return (TRUE);
	}
	return (FALSE);
}

/*
 * This routine works on Suns (Sky / 68000's) and Vaxen.
 */

/* What IEEE double precision floating point looks like on a Vax */
struct  ieee_double {
	unsigned int	mantissa1 : 20;
	unsigned int	exp	  : 11;
	unsigned int	sign	  : 1;
	unsigned int	mantissa2 : 32;
};
 
/* Vax double precision floating point */
struct  vax_double {
	unsigned int	mantissa1 : 7;
	unsigned int	exp       : 8;
	unsigned int	sign      : 1;
	unsigned int	mantissa2 : 16;
	unsigned int	mantissa3 : 16;
	unsigned int	mantissa4 : 16;
};

#define VAX_DBL_BIAS    0x81
#define IEEE_DBL_BIAS   0x3ff
#define MASK(nbits)     ((1 << nbits) - 1)

static struct dbl_limits {
	struct  vax_double d;
	struct  ieee_double ieee;
} dbl_limits[2] = {
	{{ 0x7f, 0xff, 0x0, 0xffff, 0xffff, 0xffff },	/* Max Vax */
	{ 0x0, 0x7ff, 0x0, 0x0 }},			/* Max IEEE */
	{{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},		/* Min Vax */
	{ 0x0, 0x0, 0x0, 0x0 }}				/* Min IEEE */
};

bool_t           
xdr_double(xdrs, dp)
	register XDR *xdrs;
	double *dp;
{
	register long *lp;
	struct  ieee_double id;
	struct  vax_double vd;
	register struct dbl_limits *lim;
	int i;

	switch (xdrs->x_op) {

	case XDR_ENCODE:
#ifdef sun
		lp = (long *)dp;
#endif
#ifdef vax
		vd = *((struct  vax_double *)dp);
		for (i = 0, lim = dbl_limits;
			i < sizeof(dbl_limits)/sizeof(struct dbl_limits);
			i++, lim++) {
			if ((vd.mantissa4 == lim->d.mantissa4) &&
				(vd.mantissa3 == lim->d.mantissa3) &&
				(vd.mantissa2 == lim->d.mantissa2) &&
				(vd.mantissa1 == lim->d.mantissa1) &&
				(vd.exp == lim->d.exp)) {
				id = lim->ieee;
				goto shipit;
			}
		}
		id.exp = vd.exp - VAX_DBL_BIAS + IEEE_DBL_BIAS;
		id.mantissa1 = (vd.mantissa1 << 13) | (vd.mantissa2 >> 3);
		id.mantissa2 = ((vd.mantissa2 & MASK(3)) << 29) |
				(vd.mantissa3 << 13) |
				((vd.mantissa4 >> 3) & MASK(13));
	shipit:
		id.sign = vd.sign;
		lp = (long *)&id;
#endif
#ifdef u3b2
		/* don't know floating point format */
		return(FALSE);
#endif
#ifdef i386
		/* don't know floating point format */
		return(FALSE);
#endif
		return (XDR_PUTLONG(xdrs, lp++) && XDR_PUTLONG(xdrs, lp));

	case XDR_DECODE:
#ifdef sun
		lp = (long *)dp;
		return (XDR_GETLONG(xdrs, lp++) && XDR_GETLONG(xdrs, lp));
#endif
#ifdef vax
		lp = (long *)&id;
		if (!XDR_GETLONG(xdrs, lp++) || !XDR_GETLONG(xdrs, lp))
			return (FALSE);
		for (i = 0, lim = dbl_limits;
			i < sizeof(dbl_limits)/sizeof(struct dbl_limits);
			i++, lim++) {
			if ((id.mantissa2 == lim->ieee.mantissa2) &&
				(id.mantissa1 == lim->ieee.mantissa1) &&
				(id.exp == lim->ieee.exp)) {
				vd = lim->d;
				goto doneit;
			}
		}
		vd.exp = id.exp - IEEE_DBL_BIAS + VAX_DBL_BIAS;
		vd.mantissa1 = (id.mantissa1 >> 13);
		vd.mantissa2 = ((id.mantissa1 & MASK(13)) << 3) |
				(id.mantissa2 >> 29);
		vd.mantissa3 = (id.mantissa2 >> 13);
		vd.mantissa4 = (id.mantissa2 << 3);
	doneit:
		vd.sign = id.sign;
		*dp = *((double *)&vd);
		return (TRUE);
#endif
#ifdef u3b2
		/* don't know floating point format */
		return(FALSE);
#endif
#ifdef i386
		/* don't know floating point format */
		return(FALSE);
#endif

	case XDR_FREE:
		return (TRUE);
	}
	return (FALSE);
}
