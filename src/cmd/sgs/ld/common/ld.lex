%{
#ident	"@(#)ld.lex	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."

#include "system.h"
#include "extrns.h"		/* external function definitions */
#include <signal.h>
#include "bool.h"
#include "paths.h"
#include "sgs.h"
#include "y.tab.h"		/* tokens generated by yacc      */

#if TRVEC
#include "tv.h"
#include "ldtv.h"
#endif

#include "instr.h"

extern char version[];		/* ld version information */

/*
 *	There are four start states for the lex parse:
 *
 *	A	: look for initial keyword of a ld directive
 *	MEMoREG	: scan a ld directive
 *	COMA	: skip a stand-alone comment
 *	COMoMR	: skip a comment at the end of a directive
 */

#define RET1(a)   {yylval.ivalue=a;moveit(1);return(a);}
#define RET2(a,b) {yylval.sptr=a;moveit(2);return(b);}
#define RET3(a,b) {yylval.lptr=a;moveit(3);return(b);}



#define LSAVSIZE 6
long lsavarea[LSAVSIZE];	/* save area for integer tokens */
int lsavndx = 0;
extern long *savelng();
extern enum scope scope_directive;

#define CSAVSIZE 1024
char csavarea[CSAVSIZE];	/* save area for string tokens */
char *csavptr = &csavarea[0];
extern char *savestr();

char inline[256];		/* global save area for the current input line */
char *inptr = &inline[0];

char **argptr;		/* main procedure argument list */
int argcnt;		/*	and count		*/


extern int in_y_exp;	/*	in_y_exp :
			 *		TRUE  : yacc is parsing assignment
			 *		FALSE : yacc is not in assignment
			 *
			 *	See explanation is ld.yac.
			 */

int	assigncnt;	/* number of assignment slots		*/


#if FLEXNAMES
#	define STRBASE 6144
#else
#	define STRBASE 3072
#endif
/*eject*/
%}
%start A INaNAME MEMoREG COMA COMoMR INaCOM
%p 3400
%a 3600
%o 6000
%%
%{
		long longint;	/* local variables used in processing */
		char *p;	/* numbers and strings		      */
 		int  symbols_seen; /* For hide/export processing */
 		char *badscope = "No symbols specified in hide/export statement!";
%}

<A>"/*"		BEGIN COMA;
<MEMoREG>"/*"	BEGIN COMoMR;
<COMA>"*/"	BEGIN A;
<COMoMR>"*/"	BEGIN MEMoREG;
<INaNAME>"/*"	BEGIN INaCOM;
<INaCOM>"*/"	BEGIN INaNAME;
<INaCOM,COMA,COMoMR>.	;

<A>MEMORY	{ BEGIN MEMoREG; RET1(MEMORY); } 
<A>REGIONS	{ BEGIN MEMoREG; RET1(REGIONS); } 
<A>TV		{ BEGIN MEMoREG; RET1(TV); }

<A>SECTIONS	RET1(SECTIONS);
<A>DSECT	RET1(DSECT);
<A>NOLOAD	RET1(NOLOAD);
<A>COPY		RET1(COPY);
<A>INFO		RET1(INFO);
<A>OVERLAY	RET1(OVERLAY);
<A>COMMON	RET1(COMMN);

<A>HIDE	{ BEGIN INaNAME; RET1(HIDE); }
<A>EXPORT	{ BEGIN INaNAME; RET1(EXPORT); }

<MEMoREG>"}"		{ BEGIN A; RET1(RBRACE); }
<MEMoREG>ORIGIN|origin|org|o   RET1(ORG);
<MEMoREG>LENGTH|length|len|l   RET1(LEN);
<MEMoREG>SPARE|spare|s	RET1(SPARE);
<MEMoREG>RANGE|range	RET1(RANGE);
<MEMoREG>ASSIGN|assign	RET1(ASSIGN);

<INaNAME>[\!\\a-zA-Z0-9$._\*?\[\]\%\/-]+	{
			if (scope_directive == __undefined) {
 				REJECT;
 				}
 			sym_scope(yytext, scope_directive);
 			symbols_seen++;
 			}
<INaNAME>"{"		{
 			symbols_seen = 0;
 			RET1(LBRACE);
 			}
<INaNAME>"}"		{
 			if (symbols_seen == 0)
 				lderror(2, 0, NULL, badscope);
 			BEGIN A;
 			RET1(RBRACE);
 			}
<A,MEMoREG>"."		RET1(DOT);
<A,MEMoREG>"}"		RET1(RBRACE);
<A,MEMoREG>"{"		RET1(LBRACE);
<A,MEMoREG>")"		RET1(RPAREN);
<A,MEMoREG>"("		RET1(LPAREN);
<A,MEMoREG>"]"		RET1(RSQBRACK);
<A,MEMoREG>"["		RET1(LSQBRACK);
<A,MEMoREG>":"		RET1(COLON);
<A,MEMoREG>";"		RET1(SEMICOL);
<A,MEMoREG>"-"		RET1(MINUS);
<A,MEMoREG>"="		RET1(EQ);
<A,MEMoREG>">"		RET1(GT);
<A,MEMoREG>","		RET1(COMMA);
<A,MEMoREG>"&"		RET1(AND);
<A,MEMoREG>"&&"		RET1(ANDAND);
<A,MEMoREG>"|"		RET1(OR);
<A,MEMoREG>"||"		RET1(OROR);
<A,MEMoREG>"=="		RET1(EQEQ);
<A,MEMoREG>"!="		RET1(NE);
<A,MEMoREG>"~"		RET1(BNOT);
<A,MEMoREG>"!"		RET1(NOT);
<A,MEMoREG>"/"		RET1(DIV);
<A,MEMoREG>"*"		RET1(MULT);
<A,MEMoREG>"%"		RET1(PC);
<A,MEMoREG>"<"		RET1(LT);
<A,MEMoREG>">="		RET1(GE);
<A,MEMoREG>"<="		RET1(LE);
<A,MEMoREG>"<<"		RET1(LSHIFT);
<A,MEMoREG>">>"		RET1(RSHIFT);
<A,MEMoREG>"+"		RET1(PLUS);
<A,MEMoREG>"+="		RET1(PLUSEQ);
<A,MEMoREG>"-="		RET1(MINUSEQ);
<A,MEMoREG>"*="		RET1(MULTEQ);
<A,MEMoREG>"/="		RET1(DIVEQ);
<A,MEMoREG>ALIGN|align	RET1(ALIGN);
<A,MEMoREG>BLOCK|block	RET1(BLOCK);
<A,MEMoREG>GROUP|group	RET1(GROUP);
<A,MEMoREG>PHY|phy	RET1(PHY);
<A,MEMoREG>SIZEOF|sizeof	RET1(SIZEOF);
<A,MEMoREG>ADDR|addr	RET1(ADDR);
<A,MEMoREG>NEXT|next	RET1(NEXT);
<A,MEMoREG>BIND|bind	RET1(BIND);


"\n"	{
	lineno++;
	inptr = &inline[0];
	}
[ \t]+	{
	*inptr++ = ' ';
	*inptr   = '\0';
	}

<INaNAME>.	{
		lderror(0, 0, NULL, "Discarding '%c' in scope/hide directive",
			yytext[0]);
		}

<A,MEMoREG>[0][0-7]*	{
		longint = 0L;
		for(p=yytext+1; *p != '\0'; p++)  {
			longint = longint * 8 + (*p-'0');
			}
		RET3(savelng(longint), LONGINT);
		}
<A,MEMoREG>[0-9]+	{
		longint = 0L;
		for(p=yytext; *p != '\0'; p++)  {
			longint = longint * 10 + (*p-'0');
			}
		RET3(savelng(longint), LONGINT);
		}
<A,MEMoREG>0[Xx][0-9a-fA-F]+  {
		longint = 0L;
		for(p=yytext+2; *p != '\0'; p++)  {
			longint = longint << 4;
			if (*p >= '0' && *p <= '9')
				longint += *p-'0';
			else if (*p >= 'a' && *p <= 'f')
				longint += *p-'a'+10;
			else longint += *p-'A'+10;
			}
		RET3(savelng(longint), LONGINT);
		}

<A,MEMoREG>[a-zA-Z0-9%$._]+"/"("*"|"=")	{
			yyless(yyleng-2);
			RET2(savestr(yytext), NAME);
		}

<A,MEMoREG>[a-zA-Z0-9$._/]+[a-zA-Z0-9$._/+-]*"/"("*"|"=")	{
			if ( in_y_exp )
				REJECT;
			yyless(yyleng-2);
			RET2(savestr(yytext), FILENAME);
		}

<A,MEMoREG>[a-zA-Z0-9%$._]+		{
			RET2(savestr(yytext), NAME);
		}

<A,MEMoREG>[a-zA-Z0-9$._/]+[a-zA-Z0-9$._/+-]*		{
			if ( in_y_exp )
				REJECT;
			RET2(savestr(yytext), FILENAME);
		}
%%
/*eject*/
main(argc,argv)
int argc;
char *argv[];
{

/*
 * This is the MAIN procdure for PASS 1 of ld
 *
 * PASS 1 is the parsing pass:
 *	1. The ld command line, and any specified ifiles, are
 *		scanned for good syntax.
 *	2. Control blocks are constructed, containing the information
 *		extracted from the input.
 *	3. If there are no errors, the control blocks are written out
 *		to a "transfer file", for reading by PASS 2.
 *	4. If there are no errors, PASS 2 is called via a system 
 *		"execl" call.
 */

/*
 * Initialize PASS 1
 */

	initpass1();
	argptr = argv;
	argcnt = argc;
	if ( argcnt <= 1 )
	{
		fprintf( stderr, "Usage: %sld [ options ] file ...\n", SGS );
		exit(1);
	}

/*
 * Process the command-line arguments to ld
 */

	lineno = 0;
	curfilnm = savefn("*command line*");

	undefine();	/* undefine start symbol if necessary */

	while( --argcnt )
		if( **++argptr == '-' )
			/*
			 * The argument is a flag
			 */
			pflags( ++(*argptr), FALSE );
		else {
			/*
			 * The argument is a file name
			 */
			BEGIN A;
			filespec(*argptr);
			}

/*
 * Perform a consistency check among the ld flags:
 *	1. "-h" and "-X" flags
 *	2. "-ild", "-r", and "-a" flags
 *	3. "-r" and "-s" flags
 *	4. "-p" and "-B" flags
 *
 * Assign default values to the ld flags
 *	1. "-h" flag
 *	2. "-a" flag
 *	3. "-p" flag
 */

	if( Xflag )
		if( hflag > 0 ) {
			if( hflag < sizeof(AOUTHDR) )
				lderror(1, 0,NULL, "optional header size (%d bytes) is too small to contain the UNIX a.out header (%d bytes)",
					hflag, sizeof(AOUTHDR) );
			}
		else
			hflag = sizeof(AOUTHDR);
#if ILDOPT
	if (ildflag) {
		rflag = 1;
		if (sflag) {
			lderror (0, 0, NULL, "both -ild and -s flags are set. -s flag turned off");
			sflag = 0;
		}
	}
#endif
	if( rflag  &&  sflag ) {
		lderror(0, 0,NULL, "both -r and -s flags are set. -s flag turned off" );
		sflag = 0;
		}
	if ( Bflag && (pflag > 0) && (pflag < Bflag)) {
		lderror(0, 0, NULL, "pflag(%d) less than Bflag, set to value of Bflag(%d)\n",
			pflag, Bflag);
		pflag = Bflag;
		}
/*
 * The documentation claims that the aflag is assumed if neither -r nor -a
 * is specified as any of the options.  Therefore, the only time that we
 * need to turn OFF the aflag is if ONLY -r was specified AND the -a flag
 * was NOT specified (aflag < 2). The aflag will be greater than 1 if it
 * was explicitly given on the command line.
 */
	if( rflag && (aflag < 2) )
		aflag = 0;

/*
 * Supply version information
 */

	if( Vflag ) {
		fprintf( stderr, "%sld : %s : Version %s",
			SGS, RELEASE, version );
		if( Vflag == 2 ) {
#if AR16WR
 			fprintf( stderr, " : PDP 11/70-45 : " );
#endif
#if AR32WR
			fprintf( stderr, " : VAX 11/780 : " );
#endif
#if ! AR16WR && ! AR32WR
#ifdef u3b
 			fprintf( stderr, " : 3B-20 : " );
#else
			fprintf( stderr, " : non-DEC : ");
#endif
#endif
#if TS
 			fprintf( stderr, "UN*X TS" );
#else
 			fprintf( stderr, "UN*X" );
#endif
			}
		fprintf( stderr, "\n" );
		}

/*
 * Exit if any errors were found
 */

	if(errlev != 0){
		exit(errlev);
		}

/*
 * Call PASS 2
 */

	pass2();
}
long *
savelng(l)
long l;
{

/*
 * Copy a long token into local save area and return a pointer to it.
 *
 * The save area is used as a circular buffer
 */

	lsavndx++;
	lsavndx %= LSAVSIZE;
	lsavarea[lsavndx] = l;

	return( &lsavarea[lsavndx] );
}




char *
savestr(p) 
char *p;
{

/*
 * Copy a string into local save area and return a pointer to it
 *
 * The save area is used as a circular buffer
 */

	register char *saveptr;

	if( csavptr > (csavarea + CSAVSIZE - 256) )
		csavptr = csavarea;
	saveptr = csavptr;

	while( *csavptr++ = *p++ );

	return( saveptr );
}




moveit(type)
int type;
{
	register char *p;

	p = &yytext[0];

	while( *inptr++ = *p++ );
	inptr--;

}
/*eject*/
initpass1()
{
	register int oldmode;
	int ldexit();
	long times();

/*
 * Set the following signals to be caught, unless ld is running in the
 * background. In this case, leave them set at "ignore"
 */

	if( signal(SIGINT, ldexit)  ==  SIG_IGN )
		signal(SIGINT, SIG_IGN);
	if( signal(SIGHUP, ldexit)  ==  SIG_IGN )
		signal(SIGHUP, SIG_IGN);
	if( signal(SIGQUIT, ldexit)  ==  SIG_IGN )
		signal(SIGQUIT, SIG_IGN);


#if PERF_DATA
	ttime = times(&ptimes);
#endif

/*
 * Allocate the global file name save area
 */

	strbase = myalloc(STRBASE);
	strnext = strbase;
	strlimit = strbase + STRBASE;

	/* bk: by default, environ is reserved by implementation */
	make_equate( "environ", "_environ" );
}
