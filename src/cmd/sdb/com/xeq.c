/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) xeq.c: version 23.2 created on 5/8/91 at 10:37:18	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)xeq.c	23.2	5/8/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/

#ident	"@(#)sdb/com:xeq.c	1.1"

/*		Copyright (c) 1985 AT&T		*/
/*		All Rights Reserved		*/
	/*	OLD MOT:xeq.c	6.1		*/

/*
 *		MACHINE and OPERATING SYSTEM DEPENDENT
 */

#include "head.h"
#include "coff.h"
#include <fcntl.h>

static struct filet *cfp = (struct filet *)0;	/* current file pointer */
static struct proct *cpp = (struct proct *)0;	/* current proc pointer */

extern MSG		NOPCS;
extern L_INT		entrypt;

/* service routines for sub process control */

/*
 * Single step until loc with descriptor format d is modified;
 * return its new value.	made it work!
 */
/*	sl_class must be set before calling monex.  It calls getlocv,
 *	which uses sl_class to determine whether loc is an image of
 *	a register (important on the 3B, where register chars and
 *	shorts are right justified, while nonregister data is left
 *	justified).
 */
monex(loc, d, cnt)
ADDR loc; char *d; int cnt;
{
	register int oldval;
	register int newval;
	int count, nc;
	struct proct *procp, *procn;
	BOOL inlocal, outsub = FALSE;
	char savclass;			/* save sl_class */	
#if DEBUG > 1
	if(debugflag) {
		fprintf(FPRT2, "monex(loc=%#x, d='%s', cnt=%d);\n",
					loc, d, cnt);
		fprintf(FPRT2, "  : b2=%#x; e2=%#x; USRSTACK=%#x;\n",
				txtmap.b2, txtmap.e2, USRSTACK);
	}
#endif
	newval = oldval = 0;
	nc = 0;
	cfp = (struct filet *)0;
	cpp = (struct proct *)0;
	savclass = sl_class;
	if(loc != -1) {

		/* code is attempting to see if loc is on the stack.. 68k has no physical
 * location for top of stack; future change might be to use ptrace to find
 * current sp and then test for loc between current sp and bottom of stack
*/
		inlocal = (loc <= (USRSTACK - 1)) || ISREGN(loc);
		newval = oldval = getlocv(loc, d, DSP);
		procp = adrtoprocp(dot);
	}
	count = (cnt == 0 ? 1 : cnt);
	do {
		singstep(1,'s');	/* was subpcs('s'); */
		sl_class = savclass;	/* in case bkpt had command */
		if (signo) {
			printf(" at\n");
			break;
		}
		if(loc == -1)	/* just single step through prog */
			continue;
		/* if a variable name and on the stack, must be in same proc */
		procn = adrtoprocp(dot);
		if(inlocal && cnt == 0 && procn != procp) {
			outsub = TRUE;
			continue;
		}
		if(outsub && procn == procp) {	/* re-entering subr; restart */
			outsub = FALSE;
			newval = oldval = getlocv(loc, d, DSP);
		}
		newval = getlocv(loc, d, DSP);
#if DEBUG > 1
		if(debugflag)
			fprintf(FPRT2,
				"monex: loc=%#x; old=%#x; new=%#x; dot=%#x;\n",
					loc, oldval, newval, dot);
#endif
	} while (oldval == newval && ++nc && (cnt == 0 || --count > 0));
	cntval = nc;
	return(newval);
}

/* Single step count source stmts (cm is 's' or 'S') */
singstep(count, cm)
char cm; {
	struct stmt thisstmt, curstmt;
	register struct proct *thisproc, *curproc;
	struct filet *fp;
	
#if DEBUG > 1
	if(debugflag)
		fprintf(FPRT2, "singstep(count=%d, cm='%c');\n", count, cm);
#endif
	usrtty();	/* restore user tty modes */
	if (count == 0) count = 1;
	for( ; count; count--) {
		setcur(0);	/*  sets dot */
		thisproc = adrtoprocp(dot-(signo ? NOBACKUP : 0));
		thisstmt = adtostmt(dot-(signo ? NOBACKUP : 0), thisproc);
		do {
			if(vmode) {	/* +1 in docommand() */
			    if( (fp = thisproc->sfptr) != cfp && fp != badfile)
				printf("[%.14s]\n", (cfp=fp)->sfilename);
			    if( thisproc != cpp && thisproc != badproc) {
				cpp = thisproc;
				if(vmode == 1) {
					printf("[%s()]\n", cpp->pname);
				}
			    }
			    if(vmode > 1)
			    	if(exactaddr == -1) {	/* at begin of a line */
					printf("%s:", thisproc->pname);
					fprint();
			    	}
			    if(vmode > 2) {
				printf("    ");
				printpc();
			    }
			}
			if(singsub(cm) == 1)
			    return;		/* at a breakpoint */
			if( pid == 0 && PC < 0 )
			{
				return( -1 );
			}
			else
			{
				dot = USERPC;
			}
			curstmt = adtostmt(dot-(signo ? NOBACKUP : 0),
				(curproc=adrtoprocp(dot-(signo?NOBACKUP:0))) );
#if DEBUG > 1
			if(debugflag)
			    fprintf(FPRT2,
				"singstep(): dot=%#x; lnno=%d; stno=%d\n",
					dot, curstmt.lnno, curstmt.stno);
#endif
		} while (!signo && 
			((thisproc == curproc && thisstmt.lnno == curstmt.lnno
				&& thisstmt.stno == curstmt.stno) ||
				curstmt.lnno <= -1 ||
				curstmt.stno <= 0));
		if (signo) break;
	}
	sdbtty();	/* save user tty modes, restore sdb tty modes */
}

/* added new subroutine singsub()	[only singstep() uses]
 *	Tests if at a subroutine call and if 'S' 
 *	or if 's' and no lineno stuff, "single step"
 *	thru the entire subroutine; else just do a single step
 */
singsub(cm)
char cm;
{
	ADDR retaddr;
	struct proct *procp;
				/* MACHINE DEPENDENT */
	if( pid == 0 && PC < 0 )
	{
		error( NOPCS );
		return( -1 );
	}
	else
	{
		dot = USERPC;
/* hanna */
		if( userpc == adsubx){
			printf("Procedure completed normally\n");
 			userpc = dot = SDBREG(PC) = lastknownadr ;
			return(1);
			}
/* hanna */
	}
	if (SUBCALL) {		/* SUBCALL depends on dot */
		subpcs('s');
		retaddr = SUBRET;
#if DEBUG > 1
		if(debugflag)
			fprintf(FPRT2, "SUBRET=%#x;\n", retaddr);
#endif
		/* if subr doesn't have lineno stuff, treat 's' as 'S' */
		if( pid == 0 && PC < 0 )
		{
			error( NOPCS );
			return( -1 );
		}
		else
		{
			dot = USERPC;
		}
		procp = adrtoprocp(dot);
#if DEBUG > 1
		if(debugflag)
			fprintf(FPRT2, "  : pname=%s; notstab=%d; lineno=%d;\n",
				procp->pname, procp->notstab, procp->lineno);
#endif
		if(cm == 's' && (procp != badproc && procp->lineno > 0))
			return(0);	/* single step thru subroutine */
		dot = retaddr;		/* do entire subroutine at once */
		subpcs('b');
		subpcs('c');
		dot = retaddr;
		subpcs('d');
		if( pid == 0 && PC < 0 )
		{
			error( NOPCS );
			return( -1 );
		}
		else
		{
			dot = USERPC;
		}
#if DEBUG > 1
		if(debugflag)
			fprintf(FPRT2, "USERPC=%#x;\n",dot);
#endif
		if (retaddr != dot && signo == 0) {
			/* save user, restore sdb tty */
			sdbtty();
			printf("Breakpoint at \n");
			return(1);
		}
	} else
		subpcs('s');
	return(0);
}

doscall()
{
/*    Needs assembly language interface, libg.s (libg.a) in USER prog */
	int subargs[NUMARGS];	 
		/* subargs[0]    = address, 
		 * subargs[1]    = number of arguments
		 * subargs[2:NUMARGS] = actual arguments
		 */
	union {
		int w[128-NUMARGS];
		char c[4*(128-NUMARGS)];
	}substr;
	
	register int i, numchars, *subargp;
	register char ch;
	ADDR adr, praddr;
	ADDR j;
	union word word;
	
	praddr = extaddr(proc);
	if (praddr == -1) {
		fprintf(FPRT1, "Cannot find %s\n", proc);
		return;
	}
	if (adargs == -1) {
		error("Program not loaded with -lg");
		return;
	}
	
	numchars = 0;
	subargp = subargs;
	do
	{
		ch = *++argsp;	/* argsp --> first real char after '(' */
	} while( ch == ' ' || ch == '(' );
#if DEBUG > 1
	if(debugflag)
		fprintf(FPRT2, "argsp=%s;\n", argsp);
#endif
	*subargp++ = praddr;
	subargp++;
#if DEBUG > 1
	if(debugflag)
		fprintf(FPRT2, "sizeof substr.c = %d; sizeof subargs = %d;\n",
				sizeof substr.c, sizeof subargs);
#endif
	
	for (i=0; i<NUMARGS - 2; i++) {  /* process an argument */
		ch = *argsp;
		if (ch == '\'') {
			*subargp++ = *(argsp+1);
			argsp += 2;
		} else if (ch == '"') {
			*subargp++ = adargs + sizeof subargs + numchars;
			argsp++;
			for (;;) {
				ch = *argsp++;
				if (ch == '\\')
					ch = bsconv(*argsp++);
				substr.c[numchars++] = ch;
				if (ch == '"') {
					substr.c[numchars-1] = '\0';
					break;
				}
				if (ch == '\0') {
					error("Unterminated string constant");
					return;
				}
				if (numchars > sizeof substr.c) {
					error("Too many string constants");
					return;
				}
			}
		} else if ((ch >= '0' && ch <= '9') || ch == '-') {
			*subargp++ = readint(&argsp);
		} else if ((ch >= 'a' && ch <= 'z') ||
				(ch >= 'A' && ch <= 'Z') || ch == '_') {
			char *desc;
			cpname(var, argsp);
			j = varaddr("", var); /* curproc()->pname --> "" */
			if (j == -1)
				return;
			if (ISARY(sl_stype))
				*subargp++ = j;
			else if (ISTRTYP(sl_stype)) {
				int numwords = (sl_size+WORDSIZE-1)/WORDSIZE;
				while (numwords-- > 0) {
					*subargp++ = getval(j, "d", DSP);
					j += WORDSIZE;
				}
			}

			else {
				/* pushing a character pointer means getting
				 * the value at the address of the pointer
				 *
				 * Note that this is different than a character
				 * array (handled above), where it is the
				 * address of the array which must be pushed.
				 */
				if ((desc = typetodesc(sl_stype))[0] == 's')
					desc = "x";

				if (ISREGV(sl_class)) {
				    if (*desc == 'g') {
					*subargp++ = getreg(j, "d");
					if (ISREGN(j))
					    *subargp++ = getreg(j+1, "d");
					else *subargp++ = getreg(j+REGSIZE,"d");
				    }
				    else  *subargp = getreg(j, desc);
				}
				else {
				    if (*desc == 'g') {
					*subargp++ = getval(j, "d", DSP);
					*subargp++ = getval(j, "d", DSP);
				    } else *subargp++ = getval(j, desc, DSP);
				}
			}
			do {
				argsp++;
			} while (varchar(*argsp) || number(*argsp));
		} else if (ch != ')') {
			fprintf(FPRT1, "Unexpected character %c\n", ch);
			return;
		}
	
		do {
			ch = *argsp++;
		} while(ch == ' ');
		if (ch == ')') {
			if (scallx == 0) {
				scallx = 1;
				if (pid) {
					pcs = SDBREG(PC);
 					fps = SDBREG(FP);
 					aps = SDBREG(AP);
				}
				else {
					pcs = entrypt; 
					fps = aps = -1;
				}
				if (bkpts = scanbkpt(userpc)) {
					if (flagss = bkpts->flag) {
						bkpts->flag = BKPTSET;
					}
				}
			}
			/* set dot below, instead of here */
			adrflg = 1;
			cntval = 1;
			if (pid == 0 || signo) subpcs('r');
			dot = SDBREG(PC) = adsubc;
			if (dot == -1) {	/* moved from above */
				error("Internal error - cannot find _dbsubc");
				return;
			}
			subargs[1] = (subargp - subargs) - 2;	/* num args */
			adr = adargs;
			for (j=0; j<(subargp-subargs); j++) {	/* fix */
				put(adr, DSP, subargs[j]);
				adr += WORDSIZE;
			}
			adr = adargs + sizeof subargs;
			for (j=0; j<(numchars+WORDSIZE-1)/WORDSIZE; j++) {
				put(adr, DSP, substr.w[j]);
				adr += WORDSIZE;
			}

#if DEBUG > 1
			if(debugflag)
				fprintf(FPRT2, "word.w=0x%x; adr=0x%lx\n",
					word.w, adr);
#endif
			dschar = *argsp++;
			errflg = 0;
			dopcs('c');
			if (!signo) printf("Breakpoint");
			printf(" at\n");
			return;
		}
		while (*argsp == ' ' || *argsp == ',')
			argsp++;
	}
	
	error ("Too many arguments");
}


/* get arguments from core file, place them in args */
/* removed if vaxsdb tests and made (major) changes for u3B */
getargs() {
	register struct proct *procp;
	register char **av;
	register int ac, i;
	register long pa;
	char *argsp = args;
	char *argsp1 = args;
	union {
		char c[WORDSIZE];
		int w;
	} word;

	procp = 0;
	if (pid != 0 || datmap.ufd > 0)
	    for( procp = initframe(); procp != badproc; procp = nextframe() ) {
#if DEBUG > 1
		if(debugflag)
			fprintf(FPRT2, "getargs(): pname=%s;\n", procp->pname);
#endif
		if (eqstr("main", procp->pname))
			break;
	    }
	if (procp == 0 || procp == badproc || errflg )
		cpstr(args, oldargs);
	else if ( (ac = get(argp + WORDSIZE * 2, DSP)) == 0)  /* locate argc; argp=fp at this point */
		cpstr(args, oldargs);
	else {
	av = (char **) get(argp + WORDSIZE * 3, DSP); /* char *argv[]; */
		av++;			/* 0'th is file name -- skip */
		ac--;			/* arg count */
		pa = get(av++, DSP);
		while (ac) {
			i = 0;		/* VAX: no boundary problems */
			word.w = get(pa, DSP);
#if DEBUG > 1
			if(debugflag)
				fprintf(FPRT2,
					"ac=%d; word.w=0x%x; pa=0%o; i=%d;\n",
							ac, word.w, pa, i);
#endif
			pa += WORDSIZE;
			for ( ; i < WORDSIZE; i++) {
#if DEBUG > 1
				if(debugflag)
					fprintf(FPRT2, "word.c%d=%c(0%o);\n",
							i,word.c[i],word.c[i]);
#endif
				if (word.c[i] == '\0') {
					if (argsp == argsp1) {	/*  "" */
						*argsp++ = '"';
						*argsp++ = '"';
					}
					*argsp++ = ' ';
					argsp1 = argsp;	/*  arg begins */
					ac--;
					pa = get(av++, DSP);
					break;
				}
				else {
					if (word.c[i]==' ' || word.c[i]=='\t')
						*argsp++ = '\\';	/* dd0 */
					*argsp++ = word.c[i];
				}
			}
		}
		*argsp = '\0';
        }

	printf("%s %s\n", symfil, args);
	errflg = 0;
	return;
}

dopcs(c) 
char c; {
	if (c != 'r' && c != 'R')
		usrtty();
	if (subpcs(c) == -1)
		return(-1);
	sdbtty();
 	
	if (eqany(c, "cCiIsS") && userpc == adsubn) {
		if (dschar == '/') {
			dispf((ADDR) RETREG, *argsp ? argsp : "d", C_REG,
				(short)0, 0);
		}
		else
			printf("Procedure returned normally\n");
 		userpc = dot = SDBREG(PC) = pcs;
		SDBREG(FP) = fps;
		SDBREG(AP) = aps;
		if (bkpts)
			bkpts->flag = flagss;
		scallx = 0;
		longjmp(env, 0);
	}
	return(1);
}

/*
 * execute commands from a breakpoint:
 * 	returns 1 iff executions should continue from breakpoint.
 */
acommand(cmds)
char *cmds; {
	char buf[256];
	register char *p, *q;
	int endflg = 0;

	setcur(0);
	for (p=buf,q=cmds; *q!='\n'; q++)
		if (*q != ' ')		/* squish out spaces */
			*p++ = *q;
	*p = *q;
	q = p = buf;
	do { 		/* process a command */
		for (;;) {
			if (*p == ';') {
				*p = '\n';
				break;
			}
			if (*p == '\n') {
				endflg++;
				break;
			}
			p++;
		}
		if (q[0] == 'k' && eqany(q[1], ";\n"))
			return(0);
		if (decode(q) == 1) {
			fprintf(FPRT1, "Bad command: ");
			do {
				fprintf(FPRT1, "%c", *q);
			} while (*q++ != '\n');
			return(0);
		}
		docommand();
		p = q = p + 1;
	} while (!endflg);
	return(1);
}

/* added sdbtty() and also ifndef SGTTY
 * Save current user tty modes
 * and restore original sdb tty modes
 */

#ifdef	_POSIX_SOURCE
#define	JOBS	1

static	int	sdbpgrp = -1, usrpgrp = -1;
#endif	/* _POSIX_SOURCE */

sdbtty()
{
#ifndef SGTTY
#ifdef	_POSIX_SOURCE
#ifdef	JOBS
	if (sdbpgrp < 0) {
		if ((sdbpgrp = tcgetpgrp(FDTTY)) < 0) {
			sdbpgrp = getpgrp();
		}
	}
	if ((usrpgrp = tcgetpgrp(FDTTY)) < 0) {
		usrpgrp = sdbpgrp;
	}
	tcsetpgrp(FDTTY,sdbpgrp);
	setpgid(0,sdbpgrp);
#endif	/* JOBS	*/
#endif	/* _POSIX_SOURCE */
	usrttyf = fcntl(FDIN, F_GETFL, 0);
	ioctl(FDTTY, TCGETA, &usrttym);
	ioctl(FDTTY, TCSETAW, &sdbttym);
	fcntl(FDIN, F_SETFL, sdbttyf);
#else
	gtty(FDTTY, &usrttym);
	if (sdbttym.sg_flags != usrttym.sg_flags) {
		/* Bug in USG tty driver, put out a DEL as a patch. */
		if (sdbttym.sg_ospeed >= B1200)
			write(FDTTY, "\377", 1);
		stty(FDTTY, &sdbttym);
	}
#endif
}

/* added usrtty() and also ifndef SGTTY
 * Restore user tty modes
 */
usrtty()
{
#ifndef SGTTY
#ifdef	_POSIX_SOURCE
#ifdef	JOBS
	if (usrpgrp < 0)
		usrpgrp = getpgrp();
	tcsetpgrp(FDTTY,usrpgrp);
	setpgid(0,usrpgrp);
#endif	/* JOBS	*/
#endif	/* _POSIX_SOURCE */
	ioctl(FDTTY, TCSETAW, &usrttym);
	fcntl(FDIN, F_SETFL, usrttyf);
#else
	if (sdbttym.sg_flags != usrttym.sg_flags) {
		/* Bug in USG tty driver, put out a DEL as a patch. */
		if (usrttym.sg_ospeed >= B1200)	/* from ex-vi */
			write(FDTTY, "\377", 1);
		stty(FDTTY, &usrttym);
	}
#endif
}
