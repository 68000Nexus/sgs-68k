#ident	"@(#)gencode.c	25.2	1/20/92 Copyright (c) 1991 by Arix Corp."
/*	OLD MOT:gencode.c	7.1			*/



/*
static	char	sccsid[] = "OLD MOT gencode.c 7.1 5/30/85";
/* static	char	sccsid[] = "OLD MOT gencode.c: 2.11 6/30/83"; */


/* If we are making an 020 compiler we may include the 030 stuff */
#ifdef M68020
#	define M68030 1
#endif

#include	"gendefs.h"
#include	"expand2.h"
#ifdef	M68881
#include	"fpnum.h"		/* floating point constants definations */
extern		 do_fp_num();
#endif	/*M68881*/


extern long	newdot;
extern short	opt;		/* No span-dependent optimization flag.	*/
short	readonly = NO;		/* put .data into .text space */
extern upsymins	*lookup();

#if  (M68010 || M68020 || M68030 || M68040)
int		as10 =  1 ;
#else
int		as10 =  0 ;	/* if '0' -> issue warning if 68010 instr */
#endif /*m68010  m68020*/

#if  (M68020 || M68030 || M68040)
int		as20 =  1 ;
#else
int		as20 =  0 ;	/* if '0' -> issue warning if 68020 instr */
#endif

#ifdef M68040
#define SET_CACHE_VALUE( X )                                            \
	{                                                               \
	  if ( ( arg1->xp->xvalue =                                     \
	    set_cache_value(*(char *)(arg1->xp->xsymptr)) ) < 0 )       \
	  {								\
		    werror( X );                                        \
		    arg1->xp->xvalue = 0;                               \
	  }                                                             \
	}
												  #endif

#ifndef	m68k
long 		object;		/* the instruction op-code+ */
int		numwords;
long		word;
symbol		*symptr;
unsigned short	action;

#else

/*
 *
 *	m68k ADDITIONS:
 *
 *	structure to contain the effective address information
 *	for the m68k addressing modes.
 */
long 		object;		/* the instruction op-code+ */
struct eatype
{
	short	len1;
	unsigned short	action1;
	long	ext1;
	symbol	*sym1;
	short	len2;
	unsigned short	action2;
	long	ext2;
	symbol	*sym2;
	short	len3;
	unsigned short	action3;
	long	ext3;
	symbol	*sym3;
};

struct eatype	ea;	/* all inst use this, "mov" has additional instance */

#define INVALID 0	/* lenx identifiers */
#define EA_WORD	1	/* 1*BITSPOW for a word */
#define EA_LONG	2	/* 2*BITSPOW for a long word */
/*	68020:	Format of full format indexing extension words 	*/ 
#define BASE_SUPPRESS	(1<<7)
#define INDEX_SUPPRESS	(1<<6)

#define NULL_BASE_DISP	(01<<4)
#define LONG_BASE_DISP	(03<<4)		/* displacement off the base, bits 5-4 */
#define WORD_BASE_DISP	(02<<4)		/* NULL_BASE_DISP is "no displacement" */

#define NULL_IND_DISP	01	/* if we have a displacement for mem indirect */
#define WORD_IND_DISP	02	/* if "00", then we have no mem indirection   */
#define LONG_IND_DISP	03

#define FULL_FORM_INDEX	(1<<8)	/* turn on bit 8  indicating full format */
#define BRIEF_FORM_INDEX 0	/* for no mem indir & less than signed byte */

/*
 *
 *	END 68020 additional declarations.
 *
 */

#endif /*m68k*/

#ifdef M68881	
/*
 *	M68881 additions
 */
#	define cp_id	1	/* define default co_processor id */
	/* source format table used by 68881 co-processor
 	 * they are B,W,L,UNSPEC,S,X,P and D corespondently
 	 */
	int SRCFMT[8] = { 6,4,0,7,1,2,3,5 };
/*
 *	END 68881 additional declarations.
 */
#endif /*M68881*/

eff_add(arg,tag,size)
register struct arg	*arg;
register int		tag;
register int		size;
{
	register int	sdiflag;

#ifndef	m68k
	numwords = 0;
	action = 0;
#else
	ea.len1 = ea.len2 = ea.len3 = INVALID;
	ea.action1 = ea.action2 = ea.action3 = 0;
	ea.ext1 = ea.ext2 = ea.ext3 = 0;
#endif


#ifdef MYDEBUG
		printf("\n\teff_add:\n");
		printf("\targ = 0x%x\n", arg);
		if( arg )
		{
			printf("\t\targ->atype = 0%o\n", arg->atype);
			printf("\t\targ->areg1 = 0x%x\n", arg->areg1);
			if( arg->areg1 )
			{
				printf("\t\t\t->name = %s\n", &arg->areg1->name[0]);
				printf("\t\t\t->tag = 0%o\n", arg->areg1->tag);
				printf("\t\t\t->val = %d\n", arg->areg1->val);
				printf("\t\t\t->nbits = %d\n", arg->areg1->nbits);
				printf("\t\t\t->opcode = %d\n", arg->areg1->opcode);
			}
			printf("\t\targ->areg2 = 0x%x\n", arg->areg2);
			if( arg->areg2 )
			{
				printf("\t\t\t->name = %s\n", &arg->areg2->name[0]);
				printf("\t\t\t->tag = 0%o\n", arg->areg2->tag);
				printf("\t\t\t->val = %d\n", arg->areg2->val);
				printf("\t\t\t->nbits = %d\n", arg->areg2->nbits);
				printf("\t\t\t->opcode = %d\n", arg->areg2->opcode);
			}
			printf("\t\targ->asize = 0%o\n", arg->asize);
			printf("\t\targ->ascale = 0%o\n", arg->ascale);
			printf("\t\targ->aflags = 0%o\n", arg->aflags);
			printf("\t\targ->xp1 = 0x%x\n", arg->xp1);
			printf("\t\targ->xp2 = 0x%x\n", arg->xp2);
		}
#endif /*MYDEBUG*/
	switch (arg->atype)
	{
	case ADREG:
		if ((tag & 0x800) == 0)
		{
			yyerror("attempt to use incorrect addressing mode (%dn)");
			return;
		}
		object |= arg->atype;
		object |= arg->areg1->opcode;
		return;

	case AAREG:
		if ((tag & 0x400) == 0)
		{
			yyerror("attempt to use incorrect addressing mode (%an)");
			return;
		}
		object |= arg->atype;
		object |= arg->areg1->opcode;
		return;

	case AIREG:
		object |= arg->atype;
		object |= arg->areg1->opcode;
		return;

#ifdef	M68040
	case AINC:
		if ((tag & 0x100) == 0)
		{
			yyerror("attempt to use incorrect addressing mode ((%an)+)");
			return;
		}
		object |= arg->atype;
		object |= arg->areg1->opcode;
		return;

#else
	case AINC:
		if ((tag & 0x100) == 0)
		{
			yyerror("attempt to use incorrect addressing mode ((%an)+)");
			return;
		}
		object |= arg->atype;
		object |= arg->areg1->opcode;
		return;

#endif
	case ADEC:
		if ((tag & 0x80) == 0)
		{
			yyerror("attempt to use incorrect addressing mode (-(%an))");
			return;
		}
		object |= arg->atype;
		object |= arg->areg1->opcode;
		return;

	case APCOFF:
		if ((tag & 0x4) == 0)
		{
			yyerror("attempt to use incorrect addressing mode (d(%pc))");
			return;
		}

	case AOFF:
#ifndef m68k
		numwords = 1;
		if (arg->atype == AOFF)
			action = (arg->xp->xsymptr == NULL) ? 0 : GENRELOC;
		else /* APCOFF */
			action = (arg->xp->xsymptr == NULL) ? 0 : BRLOC;
#else
		ea.len1 = 1;
		if (arg->atype == AOFF)
			ea.action1 = (arg->xp->xsymptr == NULL) ? 0 : GENRELOC;
		else /* APCOFF */
			ea.action1 = (arg->xp->xsymptr == NULL) ? 0 : BRLOC;
#endif
		object |= arg->atype;
		if (arg->atype == AOFF)
			object |= arg->areg1->opcode;
#ifndef	m68k
		word = arg->xp->xvalue;
		if (word > 32767L || word < -32768L)
			yyerror("displacement exceeds 16 bits");
		symptr = arg->xp->xsymptr;
#else
		ea.ext1 = arg->xp->xvalue;
		if (ea.ext1 > 32767L  || ea.ext1 < -32768L)
			yyerror("displacement exceeds 16 bits signed");
		ea.sym1 = arg->xp->xsymptr;
#endif
		return;

#ifndef m68k
	case APCNDX:
		if ((tag & 0x2) == 0)
		{
			yyerror("attempt to use incorrect addressing mode (d(%pc,%rn.A))");
			return;
		}
	case ANDX:
		numwords = 1;
		action = (arg->xp->xsymptr == NULL) ? 0 : NDXRELOC;
		object |= arg->atype;
		if (arg->atype == ANDX)
			object |= arg->areg1->opcode;
		word = (arg->areg2->tag == AAREG) ? (1 << 15) : 0;
		word |= (arg->areg2->opcode << 12);
		word |= (arg->asize == L) ? (1 << 11) : 0;
		word |= (arg->xp->xvalue & 0xff);
		if (arg->xp->xvalue > 127 || arg->xp->xvalue < -128)
			yyerror("index displacement exceeds 8 bits");
		symptr = arg->xp->xsymptr;
		return;
#else
	case APCNDX:
		if( (tag & 0x2 ) == 0 )
		{
			yyerror("attempt to use incorrect addressing mode (%pc,%rn.A,d)");
			return;
		}	/* else drop thru to ANDX */

	case ANDX:
		/* common codes for brief and full format */
		ea.ext1 = 0;
		ea.action1 = 0;
		ea.len1 = EA_WORD;		/* first extension only 1 wd */
		object  |= arg->atype;
		if( arg->atype == ANDX && arg->areg1 != NULL)
			object  |= arg->areg1->opcode;

		ea.ext1 |= (arg->ascale << 9);
		if( arg->areg2) 	/* if there is an index */
		{
			ea.ext1 |= (arg->areg2->tag == AAREG) ? (1<<15) : 0;
			ea.ext1 |= (arg->areg2->opcode << 12);
			ea.ext1 |= (arg->asize == L) ? (1 << 11) : 0;
		}

		/* generate brief format */
		if ( arg->aflags == BFINDIRECT )
		{
			if(arg->xp1->xsymptr != NULL && arg->atype == APCNDX)
			{
				ea.len1 = BITSPBY;	/* only the low 8 bits need to reloc. */
				ea.action1 = BRLOC;
			}
			else
			{
				ea.action1 = GENRELOC;
				ea.ext1 |= ( arg->xp1->xvalue & 0xff );
			}
			ea.sym1 = arg->xp1->xsymptr;
			return;
		}

		/* generate full format */
		ea.ext1 |= FULL_FORM_INDEX;	/* set full format bit */
		ea.ext1 |= (arg->areg1 ? 0: BASE_SUPPRESS);
		ea.ext1 |= (arg->areg2 ? 0: INDEX_SUPPRESS);
		ea.action2 = 0;
		ea.action3 = 0;

		/*
		 *	Check base displacement, if set - store in ea.
		 *	For the 68020, this displacement could be null,
		 *	in that case set the `NULL' displacement bits.
		 */

		if( arg->xp1 )	/* if there is a base displacement	*/
		{
			if ( arg->xp1->xvalue == 0 && arg->xp1->xsymptr == NULL )
				ea.ext1 |= NULL_BASE_DISP;
			else
			{
				if(arg->xp1->xsize == UNSPEC)
					if(arg->xp1->xsymptr)
						ea.len2 = EA_LONG;
					else
					if(arg->xp1->xvalue < -32768L || 
						arg->xp1->xvalue > 32767L )
						ea.len2 = EA_LONG;
					else
						ea.len2 = EA_WORD;
				else
					ea.len2 = (arg->xp1->xsize == L ? EA_LONG : EA_WORD);
				ea.action2 = ( arg->xp1->xsymptr == NULL ) ? 0 :
					(arg->atype == ANDX?GENRELOC:BRLOC) ;
				ea.ext2 = arg->xp1->xvalue;
				ea.sym2 = arg->xp1->xsymptr;
				ea.ext1 |= (ea.len2 == EA_LONG)? LONG_BASE_DISP: WORD_BASE_DISP;
			}
		}
		else
			ea.ext1 |= NULL_BASE_DISP;

		/*
		 *	are we using memory indirect?
		 *
		 *	if so, we may have a displacement off the 
		 *	memory indirect.  the extension words (1 or 2)
		 *	would follow the first extension word and
		 *	base displacement (again 1 or 2 words, if any).
		 */

		if( arg->aflags == POSTNDXMI || arg->aflags == PRENDXMI )
		{
			/* set memory indirect and indexing options */
			ea.ext1 |= ((arg->aflags == POSTNDXMI && arg->areg2) ? 0x4 : 0 );

			if( arg->xp2 )	/* we have an indirect displacement */
			{
				if ( arg->xp2->xvalue == 0 && arg->xp2->xsymptr == NULL )
					ea.ext1 |= NULL_IND_DISP;
				else
				{
					if(arg->xp2->xsize == UNSPEC)
						if(arg->xp2->xsymptr)
							ea.len3 = EA_LONG;
						else
						if (arg->xp2->xvalue < -32768L || 
							arg->xp2->xvalue > 32767L )
							ea.len3 = EA_LONG;
						else
							ea.len3 = EA_WORD;
					else
						ea.len3 = (arg->xp2->xsize == L ? EA_LONG : EA_WORD);

					ea.action3 = ( arg->xp2->xsymptr == NULL ? 0 : GENRELOC );
					ea.ext3 = arg->xp2->xvalue;
					ea.sym3 = arg->xp2->xsymptr;
					ea.ext1 |= (ea.len3 == EA_LONG) ? LONG_IND_DISP : WORD_IND_DISP;
				}
			}
			else		/* no indirect displacement */
				ea.ext1 |= NULL_IND_DISP;
		}
		return;
#endif

	case ABSW:
		if (tag & 0x1000)
		{
			ea.len1 = EA_WORD;
			ea.action1 = (arg->xp->xsymptr == NULL) ? 0 : GENRELOC;
			object |= ABSW;
			ea.ext1 = arg->xp->xvalue;
			ea.sym1 = arg->xp->xsymptr;
			return;
		}
		if (tag & 0x4000)
			sdiflag = SDI6;
		else if (tag & 0x2000)
			sdiflag = SDI5;
		else
			sdiflag = SDI4;
#ifdef hanna
		if (opt)
			switch (shortsdi(arg->xp->xsymptr,arg->xp->xvalue,sdiflag))
		{
		case S_SDI:	/* abs.w guaranteed to work.		*/
			/* If the tag is CONTROL, then the instructions	*/
			/* is a jmp, jsr, pea or lea.  These will use	*/
			/* the pc-relative capability in the case that	*/
			/* the short form works.			*/
			ea.len1 = EA_WORD;
			ea.action1 = (arg->xp->xsymptr == NULL) ? 0 :
				((tag == CONTROL) ? BRLOC : GENRELOC);
			if (tag == CONTROL)
				object |= APCOFF;
			else
				object |= ABSW;
			ea.ext1 = arg->xp->xvalue;
			ea.sym1 = arg->xp->xsymptr;
			return;
		case U_SDI:	/* Don't know, so generate a short	*/
				/*	for now.			*/
			/* Again, if the tag is CONTROL, then we use	*/
			/* the pc-relative mode for now, and have a	*/
			/* special action routine handle things in the	*/
			/* second pass.					*/
			ea.len1 = EA_WORD;
			if (tag == CONTROL)
			{
				ea.action1 = ABSBR;
				object |= APCOFF;
			}
			else
			{
				ea.action1 = ABSOPT;
				object |= ABSW;
			}
			ea.ext1 = arg->xp->xvalue;
			ea.sym1 = arg->xp->xsymptr;
			return;
		case L_SDI:
			break;
			/* Fall through to no-optimize code.		*/
		}
#endif
		ea.len1 = EA_WORD;
		ea.action1 = (arg->xp->xsymptr == NULL) ? 0 : GENRELOC;
		object |= ABSW;
		ea.ext1 = arg->xp->xvalue;
		ea.sym1 = arg->xp->xsymptr;
		return;
	case ABSL:
		if (tag & 0x1000)
		{
#ifndef	m68k
			numwords = 2;
			action = (arg->xp->xsymptr == NULL) ? 0 : GENRELOC;
#else
			ea.len1 = EA_LONG;
			ea.action1 = (arg->xp->xsymptr == NULL) ? 0 : GENRELOC;
#endif
			object |= ABSL;
#ifndef m68k
			word = arg->xp->xvalue;
			symptr = arg->xp->xsymptr;
#else
			ea.ext1 = arg->xp->xvalue;
			ea.sym1 = arg->xp->xsymptr;
#endif
			return;
		}
		if (tag & 0x4000)
			sdiflag = SDI6;
		else if (tag & 0x2000)
			sdiflag = SDI5;
		else
			sdiflag = SDI4;
		if (opt)
			switch (shortsdi(arg->xp->xsymptr,arg->xp->xvalue,sdiflag))
		{
		case S_SDI:	/* abs.w guaranteed to work.		*/
			/* If the tag is CONTROL, then the instructions	*/
			/* is a jmp, jsr, pea or lea.  These will use	*/
			/* the pc-relative capability in the case that	*/
			/* the short form works.			*/
#ifndef	m68k
			numwords = 1;
			action = (arg->xp->xsymptr == NULL) ? 0 :
				((tag == CONTROL) ? BRLOC : GENRELOC);
#else
			ea.len1 = EA_WORD;
			ea.action1 = (arg->xp->xsymptr == NULL) ? 0 :
				((tag == CONTROL) ? BRLOC : GENRELOC);
#endif
			if (tag == CONTROL)
				object |= APCOFF;
			else
				object |= ABSW;
#ifndef m68k
			word = arg->xp->xvalue;
			symptr = arg->xp->xsymptr;
#else
			ea.ext1 = arg->xp->xvalue;
			ea.sym1 = arg->xp->xsymptr;
#endif
			return;
		case U_SDI:	/* Don't know, so generate a short	*/
				/*	for now.			*/
			/* Again, if the tag is CONTROL, then we use	*/
			/* the pc-relative mode for now, and have a	*/
			/* special action routine handle things in the	*/
			/* second pass.					*/
#ifndef m68k
			numwords = 1;
#else
			ea.len1 = EA_WORD;
#endif
			if (tag == CONTROL)
			{
#ifndef m68k
				action = ABSBR;
#else
				ea.action1 = ABSBR;
#endif
				object |= APCOFF;
			}
			else
			{
#ifndef m68k
				action = ABSOPT;
#else
				ea.action1 = ABSOPT;
#endif
				object |= ABSW;
			}
#ifndef m68k
			word = arg->xp->xvalue;
			symptr = arg->xp->xsymptr;
#else
			ea.ext1 = arg->xp->xvalue;
			ea.sym1 = arg->xp->xsymptr;
#endif
			return;
		case L_SDI:
			break;
			/* Fall through to no-optimize code.		*/
		}
#ifndef m68k
		numwords = 2;
		action = (arg->xp->xsymptr == NULL) ? 0 : GENRELOC;
#else
		ea.len1 = EA_LONG;
		ea.action1 = (arg->xp->xsymptr == NULL) ? 0 : GENRELOC;
#endif
		object |= ABSL;
#ifndef m68k
		word = arg->xp->xvalue;
		symptr = arg->xp->xsymptr;
#else
		ea.ext1 = arg->xp->xvalue;
		ea.sym1 = arg->xp->xsymptr;
#endif
		return;

	case AIMM:
		if ((tag & 0x1) == 0)
		{
			yyerror("attempt to use incorrect addressing mode (immediate)");
			return;
		}
		object |= AIMM;
#ifndef m68k
		symptr = arg->xp->xsymptr;
		action = (symptr == NULL) ? 0 : GENRELOC;
#else
		ea.sym1 = arg->xp->xsymptr;
		ea.action1 = (ea.sym1 == NULL) ? 0 : GENRELOC;
#endif
		switch (size)
		{
		case B:
#ifndef m68k
			numwords = 1;
			word = arg->xp->xvalue & 0xff;
#else
			ea.len1 = EA_WORD;
			ea.ext1 = arg->xp->xvalue & 0xff;
#endif
			return;
		case W:
		case UNSPEC:
#ifndef m68k
			numwords = 1;
			word = arg->xp->xvalue & 0xffff;
#else
			ea.len1 = EA_WORD;
			ea.ext1 = arg->xp->xvalue & 0xffff;
#endif
			return;
		case L:
#ifndef m68k
			numwords = 2;
			word = arg->xp->xvalue;
#else
			ea.len1 = EA_LONG;
			ea.ext1 = arg->xp->xvalue;
#endif
			return;
#ifdef M68881
		case S:	/* single precision 32 bits */
			ea.len1 = EA_LONG;
			if ( arg->xp->xsymptr == NULL  &&
					arg->xp->xvaltype == FP_DEC )
				get_fp_value(size);
			else
				ea.ext1 = arg->xp->xvalue;
			return;

		case D:	/* double precision 64 bits */
			ea.len1 = EA_LONG;
			ea.len2 = EA_LONG;
			if ( arg->xp->xsymptr != NULL )
				werror("please set constant value");
			if ( arg->xp->xsymptr == NULL  &&
					arg->xp->xvaltype == FP_INT )
				werror("size code/extension is invalid");

			if ( arg->xp->xsymptr == NULL  &&
					arg->xp->xvaltype == FP_DEC )
				get_fp_value(size);
			else
			{
				ea.ext1 = arg->xp->xvalue;
				ea.ext2 = arg->xp->xvalue2;
			}
			return;

		case X:	/* extended precision 96 bits */
			ea.len1 = EA_LONG;
			ea.len2 = EA_LONG;
			ea.len3 = EA_LONG;
			if ( arg->xp->xsymptr != NULL )
				werror("please set constant value");
			if ( arg->xp->xsymptr == NULL  &&
					arg->xp->xvaltype == FP_INT )
				werror("size code/extension is invalid");

			if ( arg->xp->xsymptr == NULL  &&
					arg->xp->xvaltype == FP_DEC )
				get_fp_value(size);
			else
			{
				ea.ext1 = arg->xp->xvalue;
				ea.ext2 = arg->xp->xvalue2;
				ea.ext3 = arg->xp->xvalue3;
			}
			return;

		case P:	/* packed BCD 96 bits */
			ea.len1 = EA_LONG;
			ea.len2 = EA_LONG;
			ea.len3 = EA_LONG;
			if ( arg->xp->xsymptr != NULL )
				werror("please set constant value");
			if ( arg->xp->xsymptr == NULL  &&
					arg->xp->xvaltype == FP_INT )
				werror("size code/extension is invalid");

			if ( arg->xp->xsymptr == NULL  &&
					arg->xp->xvaltype == FP_DEC )
				get_fp_value(size);
			else
			{
				ea.ext1 = arg->xp->xvalue;
				ea.ext2 = arg->xp->xvalue2;
				ea.ext3 = arg->xp->xvalue3;
			}
			return;
#endif /*M68881*/
		}
	default:
		yyerror("invalid operand type for instruction");
	}
}	/* eff_add()	*/



gen0op(inst)
register instr		*inst;

{	char instr_buf[NCHPS+1]; /* +1 to deal with pflushan */
	switch ((int)(unsigned char)inst->tag)
	{

#ifdef M68881
	case CPTCC:
		/*	co-processor trap condition without operand	*/
		{
			generate (BITSPOW,0,(0xf07c | cp_id << 9),NULLSYM);
			generate (BITSPOW,0,inst->opcode,NULLSYM);
			return;
		}	/* cpTcc */
	
	case FNOP:
		/* floating point no operation */
		{
			generate (BITSPOW,0,(0xf080 | cp_id << 9),NULLSYM);
			generate (BITSPOW,0,0,NULLSYM);
			return;
		}	/* fnop */

#endif /*M68881*/

#ifdef M68040
	case PFLUSH:	/* pflusha  &  pflushan */
		{
			strcpy(instr_buf,inst->name);
			if (instr_buf[7] == 'n')	/* pflushan */
				generate (BITSPOW,0,(inst->opcode | 
					(cp_id << 4)),NULLSYM);
			else
				generate (BITSPOW,0,(inst->opcode | 
					((cp_id << 4) | (cp_id << 3))),NULLSYM);

			return;
		}
#endif 	/* M68040 */

#ifdef M68030
	case PFLUSH:	/* pflusha */
		{
			if (strlen(inst->name)!=7)
				yyerror("operands expected for pflush");
			generate (BITSPOW,0,inst->opcode,NULLSYM);
			generate(BITSPOW,0,0x2400,NULLSYM);
			return;
		}
#endif 	/* M68030 */

	default:
			generate (BITSPOW,0,inst->opcode,NULLSYM);
	}	/* end switch */
}	/* gen0op */



gen1op(inst,size,arg1)
register instr		*inst;
register int		size;
register struct arg	*arg1;
{	char instr_buf[NCHPS];

	switch ((int)(unsigned char)inst->tag)
	{

#ifdef M68881
	case FOP:
	{	/* fop %fpn */
		register int inst1wd,		/* first word of instruction format */
		             inst2wd;		/* second word of instruction format */

		if (arg1->atype != FPREG)
			yyerror ("floating point register required for first operand");
		if ( size != X && size != UNSPEC)
			/* hoss#0 */
			/* warn the user that the size was changed */
			werror ("instruction requires size X, if source is from a floating point register... changed the size to X");
		size = X;
		inst1wd = 0xf000 | cp_id << 9;
		inst2wd = inst->opcode | arg1->areg1->opcode << 7 | arg1->areg1->opcode << 10;
		generate (BITSPOW, 0, inst1wd, NULLSYM);
		generate (BITSPOW, 0, inst2wd, NULLSYM);
		return;
	}	/* fop */

	case FTST:
	{
		register int inst2wd;		/* second word of instruction format */
		if ( size != UNSPEC && size != B && size != W && size != L &&
			size != S && size != D && size != X && size != P)
			yyerror("instruction has illegal size attribute");
		if (arg1->atype == FPREG && size != X && size != UNSPEC)
			/* hoss#0 */
			/* warn the user that the size was changed */
			werror ("instruction requires size X, if source is a floating point register... changed the size to X");
		object = inst->opcode |  cp_id << 9;
		if (arg1->atype == FPREG)
		{
			size = X;
			inst2wd = (0x003a | arg1->areg1->opcode << 10);
			generate (BITSPOW, 0, object, NULLSYM);
			generate (BITSPOW, 0, inst2wd, NULLSYM);
		}
		else
		{
			if ( size == UNSPEC) size = X;
			if ( size==B || size == W || size == L || size == S)
				eff_add (arg1,DATAA,size);
			else
				eff_add (arg1,MEMORY,size);
			gen_inst_word();
			inst2wd = (0x403a | SRCFMT[size] << 10);  
			generate(BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
		}
		return;
	}	/* ftst */
#endif /*M68881*/


#ifdef	M68881
	case CPBCC:
			/* cpBcc <label> */
			if ( size != W && size != L  && size != UNSPEC)
				yyerror("instruction has illegal size attribute");

			object = inst->opcode  | cp_id << 9  ;

			if ( size == W )
			{
				generate (BITSPOW,0,object,NULLSYM);
				generate(BITSPOW,BRLOC,arg1->xp->xvalue & 0xffff,
					arg1->xp->xsymptr);
				return;
			}
			else
			{
				if (opt)
				switch (shortsdi(arg1->xp->xsymptr,arg1->xp->xvalue,SDI2))
				{
				case S_SDI:	/* cpbcc.w guaranteed to work.	*/
					generate(BITSPOW,0,object,NULLSYM);
					generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
					return;
				case U_SDI:	/* Don't know, so generate a	*/
						/*	short for now.		*/
					generate(BITSPOW,CPBCCOPT,object,NULLSYM);
					generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
					return;
				case L_SDI:
					break;
					/* Fall through to no-optimize code.	*/
				}	/* end switch */
				generate(BITSPOW,0,object | 0x40 ,NULLSYM);	/* turn on bit 6 */
				generate(2*BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
				return;
			}	/* end size = L or UNSPEC */
			/* cpBcc */


	case CPGEN:
			/* cpGEN <co_processor command> */
			if ( size != UNSPEC )
				yyerror("cpGEN instruction is unsized");
			object = inst->opcode |  cp_id << 9;
			/* need more information to generate effective address,
			 * coprocessor command, and extension words
			 */

			return;
			/* cpGEN */

	case CPSCC:
		{	/* cpScc  <ea> */
			if ( size != B && size !=UNSPEC )
				werror("instruction has illegal size attribute, size set to byte");

			size = B;
			object  = 0xf040 | cp_id << 9;
			eff_add (arg1,DATALT,size);

			gen_inst_word();
			generate (BITSPOW,0,inst->opcode,NULLSYM);
			gen_eff_add();
			return;
		}	/* cpScc */
	 
	case CPRESTORE:
	case CPSAVE:
		 	/* cpRESTORE  <ea> */
		 	/* cpSAVE     <ea> */
			if ( size != UNSPEC )
				werror("cpRESTORE/cpSAVE instruction are unsized");
			size = UNSPEC;
			object = inst->opcode |  cp_id << 9;
			if ( (int)(unsigned char)inst->tag == CPSAVE )
				eff_add (arg1,CONPRE,size);
			else
				eff_add (arg1,CONPOST,size);

			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
			return;
		 	/* cpRESTORE, cpSAVE */


	case CPTPCC:
		{	/* cpTPcc &### */
			/* Trap on co_processor condition with no argument is
			 * implemented in gen0op routine.
			 */
			register int inst1wd,	/* first word of instruction format */
			             inst2wd;	/* second word of instruction format */
			if ( size != W && size != L && size != UNSPEC )
				yyerror("cpTPcc has illegal size attribute");
			if (arg1->atype != AIMM)
				yyerror ("co_processor trap instruction required an absolute, immediate operand");
			inst1wd =  cp_id << 9;
			inst2wd = inst->opcode;
			if ( size == W ||
				(size == UNSPEC && 
				(arg1->xp->xvalue >= -(1<<15) && arg1->xp->xvalue < (1<<15)) ) )
			{
				inst1wd |= 0xf07a;
				generate(BITSPOW,0,inst1wd,NULLSYM);
				generate(BITSPOW,0,inst2wd,NULLSYM);
				generate(BITSPOW,(arg1->xp->xsymptr  == NULL)?0:GENRELOC,
						arg1->xp->xvalue &= 0xffff,arg1->xp->xsymptr);
			}
			else
			{
				inst1wd |= 0xf07b;
				generate(BITSPOW,0,inst1wd,NULLSYM);
				generate(BITSPOW,0,inst2wd,NULLSYM);
				generate(2*BITSPOW,(arg1->xp->xsymptr == NULL)?0:GENRELOC,
						arg1->xp->xvalue,arg1->xp->xsymptr);
			}
			return;
		}	/* cpTPcc */
#endif /** M68881 **/

#ifdef M68040
	case PFLUSH:	/* pflush An  &  pflushn An */
		{
			strcpy(instr_buf,inst->name);
			if ( size != UNSPEC )
		       		werror("PFLUSH instruction is unsized");
			if (arg1->atype != AAREG)
				yyerror("address register required as operand of pflush instruction");
			if (instr_buf[6] != 'n')	/* pflushn */
				object = inst->opcode | 
					(cp_id << 3) | arg1->areg1->opcode;
			else 	object = inst->opcode | arg1->areg1->opcode;
			generate (BITSPOW,0,object,NULLSYM);
			return;
		}

	case PTEST:	/* ptestr An   &  ptestw An */
		{
			strcpy(instr_buf,inst->name);
			if ( size != UNSPEC )
		       		werror("PFLUSH instruction is unsized");
			if (arg1->atype != AAREG)
				yyerror("address register required as operand of ptest instruction");

			if (instr_buf[5] == 'r')	/* ptestr */
				 object = inst->opcode | (cp_id << 5) | arg1->areg1->opcode;
			else 	 object = inst->opcode | arg1->areg1->opcode;
			generate (BITSPOW,0,object,NULLSYM);
			return;
		}

	case CINV:	/* cinva caches	*/
		{
			if ( size != UNSPEC )
		       		werror("CINVA instruction is unsized");
			SET_CACHE_VALUE(
			    "CINVA cache specifier must be 'nc | ic | dc | bc'"
			);
			object = inst->opcode | 
				(arg1->xp->xvalue << 6) | (3 << 3);   
			generate (BITSPOW,0,object,NULLSYM);
			return;
		}
	case CPUSH:	/* cpusha caches	*/
		{
			if ( size != UNSPEC )
		       		werror("CPUSHA instruction is unsized");
			SET_CACHE_VALUE(
			   "CPUSHA cache specifier must be 'nc | ic | dc | bc'"
			);
			object = inst->opcode | 
				(arg1->xp->xvalue << 6) | (3 << 3);   
			generate (BITSPOW,0,object,NULLSYM);
			return;
		}
#endif 	/* M68040 */

	case BKPT:
		if (!as20)
			werror("a 68020 instruction");
		if (size != UNSPEC)
			yyerror("bkpt instruction should have no size attribute");
		if (arg1->atype != AIMM || arg1->xp->xtype != ABS)
			yyerror("bkpt vector must be absolute, immediate operand");
#ifndef	M68040
		if (arg1->xp->xvalue < 0 || arg1->xp->xvalue > 7)
		{
			yyerror("bkpt vector must be between 0 and 7");
			arg1->xp->xvalue &= 0x7;
		}
#else
		if (arg1->xp->xvalue != 0)
		{
			/** this according to MC68040 PDS, Rev. 6.2
				(add. to Rev. 6.0)	**/
			yyerror("bkpt vector must be 0 for M68040");
			arg1->xp->xvalue &= 0x0;
		}
#endif /** M68040 **/
		object = inst->opcode | arg1->xp->xvalue;
		generate(BITSPOW,0,object,NULLSYM);
		return;

	case TRAP:
		if (size != UNSPEC)
			werror("trap instruction should have no size attribute");
		if (arg1->atype != AIMM || arg1->xp->xtype != ABS)
			yyerror("trap vector must be absolute, immediate operand");
		if (arg1->xp->xvalue < 0 || arg1->xp->xvalue > 15)
		{
			werror("trap vector must be between 0 and 15");
			arg1->xp->xvalue &= 0xf;
		}
		object = inst->opcode | arg1->xp->xvalue;
		generate(BITSPOW,0,object,NULLSYM);
		return;

	case TPCC:
		/* Trap on condition with no argument is
		 * implemented in gen0op routine.
		 */
		if (!as20)
			werror("a 68020 instruction");
		if (size != W && size != L && size != UNSPEC)
			yyerror("TPcc instruction has incorrect size attribute");

		if (arg1->atype != AIMM)
			yyerror("TPcc instruction requires an immediate operand");

		object = inst->opcode;
		if ( size == W || (size == UNSPEC && (arg1->xp->xvalue >=  -(1<<15) &&  arg1->xp->xvalue < (1<<15) )) )
		{
			generate (BITSPOW,0,object |= 2,NULLSYM);
			generate(BITSPOW,(arg1->xp->xsymptr == NULL)?0:GENRELOC,
				arg1->xp->xvalue & 0xffff, arg1->xp->xsymptr);
		}
		else
		{
			generate (BITSPOW,0,object |= 3,NULLSYM);
			generate(2*BITSPOW,(arg1->xp->xsymptr == NULL)?0:GENRELOC,
				arg1->xp->xvalue, arg1->xp->xsymptr);
		}
		return;


	case RTM:
		if (!as20)
			werror ("a 68020 instruction");
		if (size != UNSPEC)
			yyerror("rtm instruction should have no size attribute");
		object = inst->opcode ;
		if (arg1->atype == AAREG )	/* set bit 3 for an address register */
			object |= 8;
		else
		if (arg1->atype != ADREG )
			yyerror("rtm instruction operand must be an address or data register");
		object |=  arg1->areg1->opcode;
		generate(BITSPOW,0,object,NULLSYM);
		return;

	case STOP:
		if (size != UNSPEC)
			yyerror("stop instruction should have no size attribute");
		if (arg1->atype != AIMM || arg1->xp->xtype != ABS)
			yyerror("stop operand must be absolute, immediate operand");
		if (arg1->xp->xvalue < -(1<<15) || arg1->xp->xvalue >= (1<<15))
		{
			werror("stop operand must fit into a word (masked)");
			arg1->xp->xvalue &= 0xffff;
		}
		generate(BITSPOW,0,inst->opcode,NULLSYM);
		generate(BITSPOW,0,arg1->xp->xvalue,NULLSYM);
		return;

	case SWAP:
		if (size == B || size == L)
			yyerror("only word size attribute allowed for swap instruction");
		if (arg1->atype != ADREG)
			yyerror("swap instruction operand must be a data register");
		object = inst->opcode | arg1->areg1->opcode;
		generate(BITSPOW,0,object,NULLSYM);
		return;

	case EXT:
		/* ext, extb, extw */
		/* for as20
		 * if size is unspecified
		 * extb  is translated as extend byte to word
		 * extw  is translated as extend word to long
		 */
		if (size != UNSPEC && size != W && size != L)
			yyerror("ext instruction only allows word or long size attribute");
		if (arg1->atype != ADREG)
			yyerror("ext instruction operand must be a data register");
		object = inst->opcode | arg1->areg1->opcode;
		object |= (size == L) ? 0xc0 : 0x80;
		object &= ((size == W || size == UNSPEC) ? 0xfeff : 0xffff);
			
		strcpy(instr_buf,inst->name);
		if (instr_buf[3] == 'w')
			object |= 0xc0;	/* word to long */

		generate(BITSPOW,0,object,NULLSYM);
		return;
	
	case UNLK:
		if (size != UNSPEC)
			yyerror("unlk instruction should have no size attribute");
		if (arg1->atype != AAREG)
			yyerror("unlk instruction operand must be an address register");
		object = inst->opcode | arg1->areg1->opcode;
		generate(BITSPOW,0,object,NULLSYM);
		return;

	case BRA:
		if (size == B)
		{
			generate(BITSPBY,BRBYT,inst->opcode >> BITSPBY,NULLSYM);
			generate(BITSPBY,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
			return;
		}
		else
		if (size == W)
		{
			generate(BITSPOW,0,inst->opcode,NULLSYM);
			generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
			return;
		}
		else
		{
			if (opt)
				switch (shortsdi(arg1->xp->xsymptr,arg1->xp->xvalue,SDI1))
			{
			case S_SDI:	/* bra.w guaranteed to work.	*/
				generate(BITSPOW,0,inst->opcode,NULLSYM);
				generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
				return;
			case U_SDI:	/* Don't know, so generate a	*/
					/*	short for now.		*/
				generate(BITSPOW,BRAOPT,inst->opcode,NULLSYM);
				generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
				return;
			case L_SDI:
				break;
				/* Fall through to no-optimize code.	*/
			}
#ifndef m68k
			/* Generate long jump (last resort).	*/
			generate(BITSPOW,0,0x4ef9,NULLSYM);
			generate(2*BITSPOW,GENRELOC,arg1->xp->xvalue,arg1->xp->xsymptr);
#else
			/* 68020 allows size=L */
			generate(BITSPOW,0,inst->opcode | 0xff,NULLSYM);
			generate(2*BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
#endif /*m68k*/
			return;
		}

	case BR:
		if (size == B)
		{
			generate(BITSPBY,BRBYT,inst->opcode >> BITSPBY,NULLSYM);
			generate(BITSPBY,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
			return;
		}
		else
		if (size == W)
		{
			generate(BITSPOW,0,inst->opcode,NULLSYM);
			generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
			return;
		}
		else
		{
			if (opt)
				switch (shortsdi(arg1->xp->xsymptr,arg1->xp->xvalue,SDI2))
			{
			case S_SDI:	/* br.w guaranteed to work.	*/
				generate(BITSPOW,0,inst->opcode,NULLSYM);
				generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
				return;
			case U_SDI:	/* Don't know, so generate a	*/
					/*	short for now.		*/
				generate(BITSPOW,BCCOPT,inst->opcode,NULLSYM);
				generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
				return;
			case L_SDI:
				break;
				/* Fall through to no-optimize code.	*/
			}
#ifndef m68k
			/* Negate the direction of the branch,	*/
			/* then use this to branch around a	*/
			/* jmp.  Direction is reversed by	*/
			/* toggling eighth bit.			*/
			object = inst->opcode ^ 0x100;

			/* Add a displacement which will get	*/
			/* past the jmp.			*/
			object |= 6;
			generate(BITSPOW,0,object,NULLSYM);
			generate(BITSPOW,0,0x4ef9,NULLSYM);
			generate(2*BITSPOW,GENRELOC,arg1->xp->xvalue,arg1->xp->xsymptr);
#else
			/* 68020 allows size=L */
			generate(BITSPOW,0,inst->opcode | 0xff,NULLSYM);
			generate(2*BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
#endif /*m68k*/
			return;
		}

	case BSR:
		if (size == B)
		{
			generate(BITSPBY,0,inst->opcode >> BITSPBY,NULLSYM);
			generate(BITSPBY,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
			return;
		}
		else
		if (size == W)
		{
			generate(BITSPOW,0,inst->opcode,NULLSYM);
			generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
			return;
		}
		else
		{
			if (opt)
				switch (shortsdi(arg1->xp->xsymptr,arg1->xp->xvalue,SDI3))
			{
			case S_SDI:	/* bsr.w guaranteed to work.	*/
				generate(BITSPOW,0,inst->opcode,NULLSYM);
				generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
				return;
			case U_SDI:	/* Don't know, so generate a	*/
					/*	short for now.		*/
				generate(BITSPOW,BSROPT,inst->opcode,NULLSYM);
				generate(BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
				return;
			case L_SDI:
				break;
				/* Fall through to no-optimize code.	*/
			}
#ifndef m68k
			/* Generate long jsr (last resort).	*/
			generate(BITSPOW,0,0x4eb9,NULLSYM);
			generate(2*BITSPOW,GENRELOC,arg1->xp->xvalue,arg1->xp->xsymptr);
#else
			/* 68020 allows size=L */
			generate(BITSPOW,0,inst->opcode | 0xff,NULLSYM);
			generate(2*BITSPOW,BRLOC,arg1->xp->xvalue,arg1->xp->xsymptr);
#endif /*m68k*/
			return;
		}

	case ASHFT:
	case LSHFT:
	case ROT:
	case ROTX:
			/* rotate/shift <ea> */
			if (size != W && size != UNSPEC)
			{
				size = W;
				werror("rotate/shift of memory requires word attribute only");
			}
			object = inst->opcode;
			eff_add(arg1,ALTMEM,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
			return;

	case RTD:
		if ( !as10 )
			werror("a 68010 instruction");
		if (size != UNSPEC)
			yyerror("rtd is unsized");
		if (arg1->xp->xvalue < -(1<<15) || arg1->xp->xvalue >= (1<<15))
		{
			werror("rtd displacement is 16 bit signed (masked)");
			arg1->xp->xvalue &= 0xffff;
		}
		generate(BITSPOW,0,inst->opcode,NULLSYM);
		if (arg1->atype != AIMM)
			yyerror("rtd instruction requires immediate operand");
		else
			generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
				 arg1->xp->xvalue,arg1->xp->xsymptr);
		return;

	case OPTSIZE:		/*	clr, neg, negx, not.	*/
		if (size == UNSPEC)
			size = W;
		object = inst->opcode | (size << 6);
		eff_add(arg1,DATALT,size);
#ifndef	m68k
		generate(BITSPOW,action,object,NULLSYM);
		if (numwords == 1)
			generate(BITSPOW,action,word,symptr);
		else if (numwords == 2)
			generate(2*BITSPOW,action,word,symptr);
#else
		generate(BITSPOW,ea.action1,object,NULLSYM);
		gen_eff_add();
#endif
		return;

	case TST:		
			if (size == UNSPEC)
				size = W;
			object = inst->opcode | (size << 6);

#ifndef m68k
			/* 68000 allows data alterable addressing only */
			eff_add(arg1,DATALT,size);
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			/* 68020 allows data addressing for size = B */
			if (size == B)
				eff_add(arg1,DATAA,size);
			else
				eff_add(arg1,ALL,size);
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif /*m68k*/

		return;

	default:	/* jmp, jsr, nbcd, pea, scc, tas		*/
		if (   size != UNSPEC		/* check for correct size */
		    && size != ((int)(unsigned char)inst->tag - DEFAULT))
			yyerror("incorrect size specification for instruction");
		object = inst->opcode;
		if ((int)(unsigned char)inst->tag == DEFAULT+B)
			eff_add(arg1,DATALT,size);
		else
			eff_add(arg1,CONTROL,size);
#ifndef m68k
		generate(BITSPOW,action,object,NULLSYM);
		if (numwords == 1)
			generate(BITSPOW,action,word,symptr);
		else if (numwords == 2)
			generate(2*BITSPOW,action,word,symptr);
#else
/*	MOTOROLA 4.1
 * 	The following 'if' catches any pea that uses absolute long when
 *      it could use absolute short and changes it to absolute short. 
 */
#ifdef JUNK	/* OOPS.  We really cannot tell what the linker is going
		 * to do with this address!!!!  If we force it to be a short
		 * we may loose some required info.  This came out in the 
		 * code:
		 *
		 *	char cc = 1;
		 *	main()
		 *	{
		 *		asm("pea cc");
		 *	}
		 *
		 * This was bipped to "pea cc.w" as assembly time but
		 * linked into 0x201004.   UGH!!!
		*/
		if ( object == 0x4879 ) /*  pea  (xxx).l  */
		{
/*
 *	Check for the possibility of absolute short effective address
 */
			if (( ea.ext1 >= 0 && ea.ext1 <= 0x7FFF ) ||
			    ( ea.ext1 >= 0xFFFF8000 && ea.ext1 <= 0xFFFFFFFF ))
			{
				ea.len1 = EA_WORD;
				object = 0x4878;    /*  pea  (xxx).w  */
			}
		}
#endif /* JUNK */
		generate(BITSPOW,ea.action1,object,NULLSYM);
		gen_eff_add();
#endif
		return;
	}
}	/* gen1op()	*/



gen2op(inst,size,arg1,arg2)
register instr		*inst;
register int		size;
register struct arg	*arg1,
			*arg2;
{
	char	instr_buf[NCHPS];

	object = inst->opcode;

	switch ((int)(unsigned char)inst->tag)
	{

#ifdef M68881 /* co_processor instructions */
	case CPDBCC:
		{	/* cpDBcc  Dn,label */
			register int inst1wd,		/* first word of instruction format */
			             inst2wd;		/* second word of instruction format */
			if ( size != W && size != UNSPEC)
				yyerror("instruction has illegal size attribute");
			if ( size == UNSPEC ) size = W;
			if (arg1->atype != ADREG)
				yyerror ("cpDBcc instruction requires data register for first operand");
			inst1wd = (0xf048 | cp_id << 9 | arg1->areg1->opcode);
			inst2wd = object;

			generate (BITSPOW, 0, inst1wd, NULLSYM);
			generate (BITSPOW, 0, inst2wd, NULLSYM);

			if ( size == W )
				generate(BITSPOW,BRLOC,arg2->xp->xvalue & 0xffff,
					arg2->xp->xsymptr);
			/*	leave the following, in case of cpDBcc allows size L 
			else
				generate(2*BITSPOW,BRLOC,arg2->xp->xvalue,
					arg2->xp->xsymptr);
			*/
			return;
		}	/* cpDBcc */

	case FCMP:
	{	/* syntactic change in operand ordering, consistent with other cmp
		 * fcmp %fpn,<ea>
		 * fcmp %fpn,%fpm
		 */
		register int inst2wd;		/* second word of instruction format */
		if (size != UNSPEC && size != B && size != W && size != L &&
			size != S && size != D && size != X && size != P)
			yyerror("instruction has illegal size attribute");

		if (arg1->atype != FPREG)
			yyerror ("floating point register required for first operand");
		if (arg2->atype == FPREG && size != X && size != UNSPEC)
			/* hoss#0 */
			werror ("instruction requires size X, if source is from a floating point register... changed the size to X");

		object |= cp_id << 9;
		inst2wd = arg1->areg1->opcode << 7;
		if (arg2->atype == FPREG)
		{	/* source is from a floating point register */
			size = X;
			inst2wd |= (0x0038 |  arg2->areg1->opcode << 10);
			generate (BITSPOW, 0, object , NULLSYM);
			generate (BITSPOW, 0, inst2wd, NULLSYM);
		}
		else
		{
			if (size == UNSPEC) size = X;
			if (size == B || size == W || size == L || size == S)
				eff_add (arg2,DATAA,size);
			else
				eff_add (arg2,MEMORY,size);
			gen_inst_word();
			inst2wd |= (0x4038 | SRCFMT[size] << 10);
			generate(BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
		}
		return;
	}	/* fcmp */


	case FMOV:
	{
		register int inst2wd;		/* second word of instruction format */
		if (size != UNSPEC && size != B && size != W && size != L &&
			size != S && size != D && size != X && size != P)
			yyerror("instruction has illegal size attribute");

		object  |=  cp_id << 9;
		if (arg2->atype == FPREG)
		{	/* fmov %fpm,%fpn
			 * fmov <ea>,%fpn
			 */
			if (arg1->atype == FPREG)
			{
				size = X;
				inst2wd=  arg1->areg1->opcode<<10 | arg2->areg1->opcode<<7;
				generate (BITSPOW, 0, object, NULLSYM);
				generate (BITSPOW, 0, inst2wd, NULLSYM);
				return;
			}
			else
			{	if (arg1->atype == FPCONTROL || arg1->atype == FPIADDR ||
					arg1->atype == FPSTATUS )
					yyerror ("instruction has illegal register for source operand");
				if (size == UNSPEC) size = X;
				if (size == B || size == W || size == L || size == S)
					eff_add (arg1,DATAA,size);
				else
					eff_add (arg1,MEMORY,size);
				gen_inst_word();
				inst2wd = (0x4000 | SRCFMT[size]<<10 | arg2->areg1->opcode <<7);
				generate(BITSPOW,0,inst2wd,NULLSYM);
				gen_eff_add();
				return;
			}
		}
		else
		if (arg1->atype == FPCONTROL || arg1->atype == FPSTATUS)
		{ /* fmov %control,<ea> 
		   * fmov %status,<ea>
		   */
			if (size == UNSPEC) size = L;
			if (size != L )
				yyerror("instruction has illegal size attribute");
			if (arg2->atype == FPREG)
				yyerror ("instruction has illegal register for second operand");
			if(arg1->atype == FPCONTROL)
				inst2wd = 0xb000;
			else
				inst2wd = 0xa800;
			eff_add (arg2,DATALT,size);
			gen_inst_word();
			generate(BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
			return;
		}
		else
		if (arg1->atype == FPIADDR)
		{	/* fmov %iaddr,<ea> */
			if (size == UNSPEC) size = L;
			if (size != L )
				yyerror("instruction has illegal size attribute");
			if (arg2->atype == FPREG)
				yyerror ("instruction has illegal register for second operand");
			inst2wd = 0xa400;
			eff_add (arg2,ALT,size);
			gen_inst_word();
			generate(BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
			return;
		}
		else
		if (arg2->atype == FPCONTROL || arg2->atype == FPSTATUS)
		{ /* fmov <ea>,%control
		   * fmov <ea>,%status
		   */
			if ( size == UNSPEC) size = L;
			if (size != L )
				yyerror("instruction has illegal size attribute");
			if (arg1->atype == FPREG)
				yyerror ("instruction has illegal register for first operand");
			if(arg2->atype == FPCONTROL)
				inst2wd = 0x9000;
			else
				inst2wd = 0x8800;
			eff_add (arg1,DATAA,size);
			gen_inst_word();
			generate(BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
			return;
		}
		else
		if (arg2->atype == FPIADDR)
		{ /* fmov <ea>,%iaddr */
			if ( size == UNSPEC) size = L;
			if (size != L )
				yyerror("instruction has illegal size attribute");
			if (arg1->atype == FPREG)
				yyerror ("instruction has illegal register for first operand");
			inst2wd = 0x8400;
			eff_add (arg1,ALL,size);
			gen_inst_word();
			generate(BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
			return;
		}
		else
		if (arg1->atype == FPREG)
		{	/* fmov %fpn,<ea> */
			if (arg2->atype == FPCONTROL || arg2->atype == FPIADDR ||
				arg2->atype == FPSTATUS)
				yyerror("instruction has illegal register for second operand");
			if (size == UNSPEC) size = X;
			if (size == B || size == W || size == L || size == S)
				eff_add (arg2,DATALT,size);
			else
				eff_add (arg2,ALTMEM,size);
			gen_inst_word();
			inst2wd = 0x6000 | SRCFMT[size] << 10 | arg1->areg1->opcode <<7;
			generate(BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
			return;
		}
		else
		{
			yyerror("FMOV instruction has illegal operands");
			return;
		}
	}	/* fmov */

	case FMOVCR:
	{	/* fmovcr &I,%fpn */
		register int inst2wd;
		if ( size != X && size != UNSPEC)
			werror("instruction has illegal size attribute, size set to X");		
		if ( arg1->atype != AIMM )
			yyerror("instruction required immediate value for first operand");
		if ( arg2->atype != FPREG )
			yyerror("instruction required floating point register for second operand");
		if ( arg1->xp->xvalue != 0 && 
			 ! (arg1->xp->xvalue >= 0xb  && arg1->xp->xvalue <= 0xf) &&
			 ! (arg1->xp->xvalue >= 0x30 && arg1->xp->xvalue <= 0x3f))
			yyerror("instruction has illegal constant value");
		
		size = X;
		object |= cp_id << 9;
		inst2wd = (0x5c00 | arg2->areg1->opcode << 7 | arg1->xp->xvalue & 0x7f);
		generate(BITSPOW,0,object,NULLSYM);
		generate(BITSPOW,(arg1->xp->xsymptr==NULLSYM?0:GENRELOC),
				inst2wd,arg1->xp->xsymptr);
		return;
	}	/* fmovcr */
		
	case FMOVM:
	{	long inst2wd;

		object |= cp_id << 9;

		if ( arg2->atype == FPCONTROL || arg2->atype == FPIADDR ||
			arg2->atype == FPSTATUS )
		{	/* fmovm <ea>,%control
			 * fmovm <ea>,%iaddr
			 * fmovm <ea>,%status
			 */
			if ( size != L && size != UNSPEC )
				werror("instrucion has illegal size attribute, size set to L");
			size = L;
			if (arg2->atype == FPIADDR)
				eff_add (arg1,ALL,size); 
			else
				eff_add (arg1,DATAA,size);
			inst2wd = 0x8000|arg2->areg1->opcode << 10;
			gen_inst_word();
			generate(BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
			return;
		}
		else
		if ( arg1->atype == FPCONTROL || arg1->atype == FPIADDR ||
			arg1->atype == FPSTATUS )
		{	/* fmovm %control,<ea>
			 * fmovm %iaddr,<ea>
			 * fmovm %status,<ea>
			 */
			if ( size != L && size != UNSPEC )
				werror("instrucion has illegal size attribute, size set to L");
			size = L;
			if (arg1->atype == FPIADDR)
				eff_add (arg2,ALT,size); 
			else
				eff_add (arg2,DATALT,size);
			inst2wd = 0xa000|arg1->areg1->opcode << 10;
			gen_inst_word();
			generate(BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
			return;
		}
		else
		if ( arg1->atype == AIMM  || arg1->atype == ADREG)
		{	/* fmovm register list,<ea>	*/
			/* fmovm %dn,<ea>	*/
			if ( size != X && size != UNSPEC)
				werror("instrucion has illegal size attribute, size set to X");
			size = X;
			if (arg1->atype == AIMM)
				inst2wd = (arg2->atype == ADEC?0xe000:0xf000)|
						(arg1->xp->xvalue & 0xff);
			else
				inst2wd = (arg2->atype == ADEC?0xe800:0xf800)| arg1->areg1->opcode << 4;
			eff_add (arg2,CONPRE,size);
			gen_inst_word();
			if ( arg1->xp->xsymptr != NULLSYM && arg1->atype == AIMM)
			{
				/* only relocate the lower 8 bits */
				generate(BITSPBY,0,inst2wd>>BITSPBY,NULLSYM);
				generate(BITSPBY,GENRELOC,arg1->xp->xvalue,arg1->xp->xsymptr);
			}
			else
				generate (BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
			return;
		}
		else
		if ( arg2->atype == AIMM  || arg2->atype == ADREG)
		{	/* fmovm <ea>,register list	*/
			/* fmovm <ea>,%dn	*/
			if ( size != X && size != UNSPEC)
				werror("instrucion has illegal size attribute");
			size = X;
			if (arg2->atype == AIMM)
				inst2wd = 0xd000 | arg2->xp->xvalue & 0xff;
			else
				inst2wd = 0xd800 | arg2->areg1->opcode << 4;
			eff_add (arg1,CONPOST,size);
			gen_inst_word();
			if ( arg2->atype == AIMM && arg2->xp->xsymptr != NULLSYM )
			{
				/* only relocate the lower 8 bits */
				generate(BITSPBY,0,inst2wd>>BITSPBY,NULLSYM);
				generate(BITSPBY,GENRELOC,arg2->xp->xvalue,arg2->xp->xsymptr);
			}
			else
				generate (BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
			return;
		}
		else
		{
			yyerror("instruction has illegal operand combinations");
			return;
		}
	}	/* fmovm */

	case FSGLDIV:
	case FSGLMUL:
		if (size == UNSPEC)
			size = S;
		if (size != B && size != W && size != L && size != S && size != P)
		{
			yyerror("instruction has illegal size attribute");
			return;
		}
		/* fall through to FOP */
	case FOP:
	{
		register int inst1wd,		/* first word of instruction format */
		             inst2wd;		/* second word of instruction format */
		if ( size == UNSPEC )
			size = X;

		if ( size != B && size != W && size != L &&
			size != S && size != D && size != X && size != P)
			yyerror("instruction has illegal size attribute");
		     
		if (arg2->atype != FPREG)
			yyerror ("floating point register required for second operand");

		inst1wd = 0xf000 | cp_id << 9;
		inst2wd = object | arg2->areg1->opcode << 7;
		if (arg1->atype == FPREG)
		{
			if  ( (int)(unsigned char)inst->tag== FSGLDIV ||
				(int)(unsigned char)inst->tag == FSGLMUL) 
			{
				if (size != S)
				{
					/* hoss#0 */
					werror ("instruction requires size S, if source is from a floating point register... changed the size to X");
					size = S;
				}
			}
			else
			if ( size != X )
			{
				/* hoss#0 */
				werror ("instruction requires size X, if source is from a floating point register... changed the size to X");
				size = X;
			}
			inst2wd |= arg1->areg1->opcode << 10;
			generate (BITSPOW, 0, inst1wd, NULLSYM);
			generate (BITSPOW, 0, inst2wd, NULLSYM);
		}
		else
		{
			object = inst1wd;
			if (size == B || size == W || size == L || size == S)
				eff_add (arg1,DATAA,size);
			else
				eff_add (arg1,MEMORY,size);
			gen_inst_word();
			inst2wd |= (0x4000 | (SRCFMT[size] << 10));
			generate(BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
		}
		return;
	}	/* fop */
#endif /*M68881*/

	case CHK:
		/* 68020 allows size = L */
		object = 0x4000;
		if (size != UNSPEC && size != W && !(as20 && size == L))
			yyerror("instruction has incorrect size attribute");
		if (arg2->atype != ADREG)
			yyerror("instruction requires data register for second operand");
		else
			object |= (arg2->areg1->opcode << 9);

		object |= ( size == L )  ?  0x100 :  0x180 ;  
		eff_add(arg1,DATAA,size);

#ifndef m68k
		generate(BITSPOW,action,object,NULLSYM);
		if (numwords == 1)
			generate(BITSPOW,action,word,symptr);
		else if (numwords == 2)
			generate(2*BITSPOW,action,word,symptr);
#else
		generate(BITSPOW,ea.action1,object,NULLSYM);
		gen_eff_add();
#endif
		return;

#ifdef m68k
	case CHK2:			
		{
		register int inst2wd;		/* second word of instruction format */
		if (!as20)
			werror("a 68020 instruction");
		if (size == UNSPEC)
		{
			size = W;
			werror("size needed on CHK2 instruction,size default to word");
		}
		if (  size != B && size != W && size != L )
			yyerror("illegal size attribute");

		object |= size << 9;
		inst2wd = 0x800;		/* turn on bit 11 for instruction CHK2 */
		if (arg2->atype == AAREG)	/* An */
		{
			inst2wd |= arg2->areg1->opcode << 12;
			inst2wd |= 0x8000;	/* turn on bit 15 */
		}
		else
		if (arg2->atype == ADREG)	/* Dn */
			inst2wd |= arg2->areg1->opcode << 12;
		else
			yyerror("second operand must be a general register");

		eff_add(arg1,CONTROL,size);
		gen_inst_word();
		generate(BITSPOW,0,inst2wd,NULLSYM);
		gen_eff_add();
		return;
		}
#ifdef	M68030
	case PFLUSH:
		{     /* pflush <function code>,#<mask> */
		register int inst2wd;	/* second word of instruction format */
		if ( size != UNSPEC )
			werror("PFLUSH instruction is unsized");
		if (strlen(inst->name)!=6)
			yyerror("no operands expected for pflusha");
		inst2wd = 0x3000;
		inst2wd |= function_code(arg1);

		if (arg2->atype != AIMM)
			yyerror("second operand of wrong type -- immediate mask expected");
		inst2wd |= (arg2->xp->xvalue & 0x7) << 5;
		if ((unsigned)arg2->xp->xvalue > 0x7)
			werror("mask too large");

		gen_inst_word();
		generate(BITSPOW,0,inst2wd,NULLSYM);
		return;
		}
	case PLOAD:
		{	/* pload[wr] <function code>,<ea>  */
		register int inst2wd;	/* second word of instruction format */
		if ( size != UNSPEC )
			werror("PLOAD instruction is unsized");
		if ( inst->name[5] == 'r' )
			inst2wd = 0x2200;
		else
			inst2wd = 0x2000;
		inst2wd |= function_code(arg1);
		eff_add(arg2,CONTROL,size);
		gen_inst_word();
		generate(BITSPOW,0,inst2wd,NULLSYM);
		gen_eff_add();
		return;
		}
	case PMOVE:
		{	/* pmove MRn,<ea> or pmove <ea>,MRn or
			** pmovefd <ea>,MRn (pmove with flush disabled)
			*/
		register int inst2wd=0;	/* 2nd word of instruction format */
		struct arg *temp_arg=NULL;
					/* allows arg1 and arg2 to switch so
					** pmove MRn,<ea> and pmove <ea>,MRn
					** use the same source */
		register char flush_dis; /* boolean for pmovefd instruction */

		if ( size != UNSPEC )
			werror("PMOVE instruction determines size");
		flush_dis = (strlen(inst->name) == 7) ? 1 : 0;
		switch (arg2->atype) { /* switch args if not illegal pmovefd */
			case CRPREG:
			case SRPREG:
			case TCREG:
			case TTREG:
			case MMUREG:	/* switch args */
				temp_arg=arg2;
				arg2=arg1;
				arg1=temp_arg;
				break;
			default:		/* must be <ea>  */
				if (flush_dis)
				    yyerror("illegal operands to pmovefd");
		}

		switch (arg1->atype) { /* switch to create inst2wd */
			case CRPREG:
				inst2wd |= 1 << 10;
			case SRPREG:
				inst2wd |= 2 << 10;
			case TCREG:
				inst2wd |= 0x4000 | (flush_dis << 8) |
					((temp_arg == NULL) << 9);
				break;
			case TTREG:
				inst2wd = 0x0800 | (flush_dis << 8) |
					((temp_arg == NULL) << 9) |
					(arg1->areg1->opcode << 10);
				break;
			case MMUREG:
				inst2wd = 0x6000 | ((temp_arg == NULL) << 9);
				break;
			default:		/* must be <ea>  */
				yyerror("illegal operands to pmove");
		}
		eff_add(arg2,CONTROL,size);
		gen_inst_word();
		generate(BITSPOW,0,inst2wd,NULLSYM);
		gen_eff_add();
		return;
		}
#endif /* M68030 */

#ifdef	M68040
	case CINV:
	{
		strcpy(instr_buf,inst->name);
		if ( size != UNSPEC )
			werror("CINVL/P instruction is unsized");
		SET_CACHE_VALUE(
			"CINVL/P cache specifier must be 'nc | ic | dc | bc'"
		);
		if (instr_buf[4] == 'l')
			object = inst->opcode | 
				(arg1->xp->xvalue << 6) | (1 << 3);   
		else	object = inst->opcode | 
				(arg1->xp->xvalue << 6) | (2 << 3);   
		generate (BITSPOW,0,(object | arg2->areg1->opcode),NULLSYM);
		return;
	}
	case CPUSH:
	{
		strcpy(instr_buf,inst->name);
		if ( size != UNSPEC )
			werror("CPUSHL/P instruction is unsized");
		SET_CACHE_VALUE(
			"CPUSHL/P cache specifier must be 'nc | ic | dc | bc'"
		);
		if (instr_buf[5] == 'l')
			object = inst->opcode | 
				(arg1->xp->xvalue << 6) | (1 << 3);   
		else	object = inst->opcode | 
				(arg1->xp->xvalue << 6) | (2 << 3);   
		generate (BITSPOW,0,(object | arg2->areg1->opcode),NULLSYM);
		return;
	}
	case MOVE16:
	{
		if ( size != UNSPEC )
			werror("size of MOVE16 instruction is LINE ");

		if ((arg1->atype == AINC) && (arg2->atype == AINC))
		{
			object |= (1 << 5) | arg1->areg1->opcode;
			generate(BITSPOW,0,object,NULLSYM);
			generate(BITSPOW,0,(arg2->areg1->opcode << 12),NULLSYM);
			return;
		}
		else
		{
			if ((arg1->atype == AINC) && (arg2->atype == AIMM))
			{
				object |= arg1->areg1->opcode;
				generate(BITSPOW,0,object,NULLSYM);
				generate(BITSPOW,0,(arg2->xp->xvalue >> 16),NULLSYM);
				generate(BITSPOW,0,(arg2->xp->xvalue),NULLSYM);
			}
			else if ((arg1->atype == AIMM) && (arg2->atype == AINC))
			{
				object |= (1 << 3) | arg2->areg1->opcode;
				generate(BITSPOW,0,object,NULLSYM);
				generate(BITSPOW,0,(arg1->xp->xvalue >> 16),NULLSYM);
				generate(BITSPOW,0,(arg1->xp->xvalue),NULLSYM);
			}
			else if ((arg1->atype == AIREG) && (arg2->atype == AIMM))
			{
				object |= (2 << 3) | arg1->areg1->opcode;
				generate(BITSPOW,0,object,NULLSYM);
				generate(BITSPOW,0,(arg2->xp->xvalue >> 16),NULLSYM);
				generate(BITSPOW,0,(arg2->xp->xvalue),NULLSYM);
			}
			else if ((arg1->atype == AIMM) && (arg2->atype == AIREG))
			{
				object |= (3 << 3) | arg2->areg1->opcode;
				generate(BITSPOW,0,object,NULLSYM);
				generate(BITSPOW,0,(arg1->xp->xvalue >> 16),NULLSYM);
				generate(BITSPOW,0,(arg1->xp->xvalue),NULLSYM);
			}
			else yyerror("bad operands specified for move16 instruction");
			return;
		}
	}
#endif	/* M68040 */
	case CMP2:
		{	/* cmp2 %rn,<ea> 
		 	 * syntactic change in the operand ordering. 
			 * make it consistent with other cmp instructions.
			 */
		register int inst2wd;		/* second word of instruction format */
		if (!as20)
			werror("a 68020 instruction");
		if (size == UNSPEC)
		{
			size = W;
			werror("size needed on CMP2 instruction,size default to word");
		}
		if (  size != B && size != W && size != L )
			yyerror("illegal size attribute");

		object |= size << 9;
		inst2wd = 0;
		if (arg1->atype == AAREG)	/* An */
		{
			inst2wd |= 0x8000;	/* turn on bit 15 */
			inst2wd |= arg1->areg1->opcode << 12;
		}
		else
		if (arg1->atype == ADREG)	/* Dn */
			inst2wd |= arg1->areg1->opcode << 12;
		else
			yyerror("first operand must be a general register %dn or %an");

		eff_add(arg2,CONTROL,size);
		gen_inst_word();
		generate(BITSPOW,0,inst2wd,NULLSYM);
		gen_eff_add();
		return;
		}
#endif /*m68k*/

#ifdef m68k
	case CALLM:		
		if ( size != UNSPEC )
			yyerror("callm instruction should have no size attribute");
		if (arg1->atype != AIMM)
			yyerror("callm first operand must be absolute, immediate operand");
		if ( arg1->xp->xvalue < 0 || arg1->xp->xvalue > 255 )
		{
			werror("callm argument count must fit into a byte");
			arg1->xp->xvalue &= 0xff;
		}
		eff_add (arg2,CONTROL,size);
		gen_inst_word();
		generate(BITSPOW,
				(arg1->xp->xsymptr == NULLSYM) ? 0 : GENRELOC,
				arg1->xp->xvalue,arg1->xp->xsymptr);
		gen_eff_add();
		return;
#endif /*m68k*/

	case EXG:
	    {
		register unsigned short regs = (arg1->areg1->opcode << 9) | arg2->areg1->opcode;
					/* assume normal register placement */

		if (size != L && size != UNSPEC)
			yyerror("exg requires long attribute, if any");
		if (arg1->atype == ADREG && arg2->atype == ADREG)
			object |= 0x40;		/* Data <==> data.	*/
		else if (arg1->atype == AAREG && arg2->atype == AAREG)
			object |= 0x48;		/* Address <==> address.*/
		else				/* Address <==> data */
		{
			object |= 0x88;
			if (arg1->atype == AAREG) /* must reverse regs. */
			    regs = (arg2->areg1->opcode << 9) | arg1->areg1->opcode;
		}
		object |= regs;
		generate(BITSPOW,0,object,NULLSYM);
		return;
	    }

	case LINK:
		/* 68000/68010 allow unsized only, 68020 allows size = L or W */
		if (size != UNSPEC && !(as20 && (size == W || size == L)) )
			yyerror("link instruction has incorrect size attribute");
		if ( (arg2->xp->xvalue < -(1<<15) || arg2->xp->xvalue >= (1<<15))
			&& !as20)
			werror("link displacement must fit into a word (masked)");
		if (  as20 && size == UNSPEC && 
		   ((long)arg2->xp->xsymptr || 
	  	   arg2->xp->xvalue < -(1<<15) ||arg2->xp->xvalue >= (1<<15) ))
			size = L;
		if (size == L)
			object = 0x4808;
		if (arg1->atype != AAREG)
			yyerror("address register required as first operand of link instruction");
		else
			object |= arg1->areg1->opcode;
		generate(BITSPOW,0,object,NULLSYM);
		if (arg2->atype != AIMM)
			yyerror("immediate operand required as second operand of link instruction");
		else
		if ( size == L )
			generate(2*BITSPOW,(arg2->xp->xsymptr == NULL) ? 0 : GENRELOC,
				arg2->xp->xvalue,arg2->xp->xsymptr);
		else
			generate(BITSPOW,(arg2->xp->xsymptr == NULL) ? 0 : GENRELOC,
				arg2->xp->xvalue &= 0xffff,arg2->xp->xsymptr);
		return;

	case DADD:
		if (size != UNSPEC && size != B)
			yyerror("BCD instructions are byte instructions");
		size = B;
	case XADD:
		if (size == UNSPEC)
			size = W;
		if (arg1->atype == ADEC && arg2->atype == ADEC)
			object |= 0x8;
		else if (arg1->atype != ADREG || arg2->atype != ADREG)
		{
			yyerror("only data register or predecrement address register pairs allowed as operands");
			return;
		}
		object |= (arg2->areg1->opcode << 9);
		object |= (size << 6);
		object |= arg1->areg1->opcode;
		generate(BITSPOW,0,object,NULLSYM);
		return;

	case LOGAND:
	case LOGOR:
	case LOGEOR:
		if (arg1->atype == AIMM)
		{
			strcpy(instr_buf,inst->name);
			/* hoss#8 */
			if (instr_buf[strlen(instr_buf) - 1] != 'i')  
						/* Already ANDi/ORi/EORi */
			   strcat(instr_buf,"i");
			inst = lookup(instr_buf,N_INSTALL,MNEMON)->itp;
			object = inst->opcode;
			if (arg2->atype == CCREG)
			{
				object |= 0x3c;
				generate(BITSPOW,0,object,NULLSYM);
				if(arg1->xp->xvalue < 0 || arg1->xp->xvalue > 255)
					werror("immediate value must fix into a byte");
				generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg1->xp->xvalue&=0xff,arg1->xp->xsymptr);
				return;
			}
			else if (arg2->atype == SRREG)
			{
				object |= 0x7c;
				generate(BITSPOW,0,object,NULLSYM);
				if(arg1->xp->xvalue < 0 || arg1->xp->xvalue > 65535)
					werror("immediate value must fix into a word");
				generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg1->xp->xvalue&=0xffff,arg1->xp->xsymptr);
				return;
			}
			if (size == UNSPEC)
				size = W;
			object |= (size << 6);
			eff_add(arg2,DATALT,size);
			gen_inst_word();	/* preparation for second word */
			if (size == B)
				generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg1->xp->xvalue & 0xff,arg1->xp->xsymptr);
			else if (size == W)
				generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg1->xp->xvalue & 0xffff,arg1->xp->xsymptr);
			else
				generate(2*BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg1->xp->xvalue,arg1->xp->xsymptr);
#ifndef m68k
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			gen_eff_add();
#endif
			return;
		}
		if (size == UNSPEC)
			size = W;
		if (inst->tag == LOGEOR)
		{
			if (arg1->atype != ADREG)
			{
				yyerror("first operand for eor must be a data register");
				return;
			}
			object |= (arg1->areg1->opcode << 9);
			object |= (1 << 8);
			object |= (size << 6);
			eff_add(arg2,DATALT,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
			return;
		}
		else if (arg2->atype == ADREG)
		{
			/* and/or <ea>,Dn */
			object |= (arg2->areg1->opcode << 9);
			object |= (size << 6);
			eff_add(arg1,DATAA,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
			return;
		}
		else
		{
			/* and/or Dn,<ea> */
			object |= (arg1->areg1->opcode << 9);
			object |= (1 << 8);
			object |= (size << 6);
			eff_add(arg2,ALTMEM,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
			return;
		}

	case ASHFT:
	case LSHFT:
	case ROT:
	case ROTX:
		if (arg1->atype == AIMM && 
		    arg2->atype != ADREG && arg2->atype != AAREG &&
		    arg2->atype != APCOFF && arg2->atype != APCNDX &&
		    arg2->atype != AIMM)
		{
			if(arg1->xp->xvalue != 1)
				yyerror("rotate/shift of memory only by 1");
			if (size != W && size != UNSPEC)
				yyerror("rotate/shift of memory requires word attribute, if any");
			eff_add(arg2,ALTMEM,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
			return;
		}
		else
		{
			/* Turn off the immedate, size and i/r	*/
			/*	fields so they can be set.	*/
			if(arg2->atype != ADREG)
				yyerror("rotate/shift greater than 1 req's (%d) register");
			object &= ~0xeff;
			if (inst->tag == LSHFT)
				object |= 0x8;
			else if (inst->tag == ROT)
				object |= 0x18;
			else if (inst->tag == ROTX)
				object |= 0x10;
			if (size == UNSPEC)
				size = W;
			object |= (size << 6);
			if (arg1->atype == ADREG)
			{
				object |= 0x20;
				object |= (arg1->areg1->opcode << 9);
				object |= arg2->areg1->opcode;
				generate(BITSPOW,0,object,NULLSYM);
				return;
			}
			else if (arg1->atype == AIMM)
			{
				object |= arg2->areg1->opcode;

				/* Shift amount has upper limit of 8.	*/
				/* Eight is encoded as 0 (since a shift	*/
				/* by zero makes little sense).  Shifts	*/
				/* are broken up into groups of shift-	*/
				/* by-eight plus any remaining shift.	*/
				/* Note that the shift amount must be	*/
				/* known in the first pass.		*/

				if (arg1->xp->xvalue <= 0 )
					yyerror("rotate/shift range is 1-8");
				while (arg1->xp->xvalue >= 8)
				{
					generate(BITSPOW,0,object,NULLSYM);
					arg1->xp->xvalue -= 8;
				}
				if (arg1->xp->xvalue > 0)
				{
					object |= (arg1->xp->xvalue << 9);
					generate(BITSPOW,0,object,NULLSYM);
				}
				return;
			}
			else
				yyerror("invalid operands for shift/rotate instruction");
			return;
		}

	case LEA:
		if (size != UNSPEC && size != L)
			yyerror("lea instruction has incorrect size attribute");
		if (arg2->atype != AAREG)
			yyerror("lea instruction requires an address register for second operand");
		else
			object |= (arg2->areg1->opcode << 9);
		eff_add(arg1,CONTROL,size);
#ifndef m68k
		generate(BITSPOW,action,object,NULLSYM);
		if (numwords == 1)
			generate(BITSPOW,action,word,symptr);
		else if (numwords == 2)
			generate(2*BITSPOW,action,word,symptr);
#else
		generate(BITSPOW,ea.action1,object,NULLSYM);
		gen_eff_add();
#endif
		return;

	case MULS:
	case MULU:
	case DIVS:
	case DIVU:
	case TMULS:
	case TMULU:
	case TDIVS:
	case TDIVU:
	{
		/*	mulx.w   <ea>,Dn	16*16 -> 32      
		 *	divx.w   <ea>,Dn	32/16 -> 32
		 *	mulx.l   <ea>,Dn	32*32 -> 32
		 *	divx.l   <ea>,Dn	32/32 -> 32
		 *	tmulx.l  <ea>,Dn	32*32 -> 32
		 *	tdivx.l  <ea>,Dn	32/32 -> 32
		 */
		register int inst2wd;
		if (size != UNSPEC && size != W && !(as20 && size == L) )
			yyerror("instruction has incorrect size attribute");
		if (arg2->atype != ADREG)
			yyerror("instruction requires data register for second operand");

		if  ((size == UNSPEC || size == W) &&
		     (inst->tag == MULS || inst->tag == MULU ||
			  inst->tag == DIVS || inst->tag == DIVU)
			)	/* generate word form format */
		{
			object |= (arg2->areg1->opcode << 9);
			eff_add(arg1,DATAA,size);
#ifndef m68k
		   	generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
		   	generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif /*m68k*/
			return;
		}

#ifdef m68k
		if (  ((size == UNSPEC || size == L) && 
			  (inst->tag == TMULS || inst->tag == TMULU ||
			   inst->tag == TDIVS || inst->tag == TDIVU ) )
			||((size == L) && 
			  (inst->tag == MULS || inst->tag == MULU ||
			   inst->tag == DIVS || inst->tag == DIVU ) )

			)	/* generate long form format */
		{
			/* mulx.l and divx.l will be equivalent to tmulx.l and tdivx.l
			 * respectively
			 */
		    if ( inst->tag == MULS || inst->tag == MULU )
			   object = 0x4c00;
		    if ( inst->tag == DIVS || inst->tag == DIVU )
			   object = 0x4c40;

			eff_add(arg1,DATAA,size);

			inst2wd = arg2->areg1->opcode << 12 | arg2->areg1->opcode;	
			if ( (inst->tag == TMULS || inst->tag == TDIVS) ||
				 (inst->tag == MULS || inst->tag == DIVS) )
				inst2wd |= 0x800;  /* turn on bit 11 for signed mul/div */

			gen_inst_word();
			generate (BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
			return;
		}
		else
		{
			yyerror("instruction has incorrect size attribute");
			return;
		}
#endif /*m68k*/
	}
	case DBCC:
		if (size != UNSPEC && size != W)
			yyerror("dbCC instruction requires word attribute, if any");
		if (arg1->atype != ADREG)
			yyerror("dbCC instruction requires data register for first operand");
		else
			object |= arg1->areg1->opcode;
		generate(BITSPOW,0,object,NULLSYM);
		generate(BITSPOW,BRLOC,arg2->xp->xvalue,arg2->xp->xsymptr);
		return;

	case CMP:
		/* Keep in mind the syntactic change in the operand	*/
		/*	ordering.					*/
		if (size == UNSPEC)
		{
			yyerror("size needed on compare instruction");
			size = W;
		}
		if (arg1->atype == AINC && arg2->atype == AINC)
		{
			/*		cmpm				*/
			object |= (arg1->areg1->opcode << 9);
			object |= 0x108;
			object |= (size << 6);
			object |= arg2->areg1->opcode;
			generate(BITSPOW,0,object,NULLSYM);
			return;
		}
		else if (arg1->atype == AAREG)
		{
			/*		cmpa				*/
			if (size == B)
			{
				yyerror("byte size attribute illegal for compare address instruction");
				return;
			}
			object |= (arg1->areg1->opcode << 9);
			object |= (size == W) ? 0xc0 : 0x1c0;
			eff_add(arg2,ALL,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
			return;
		}
		else if (arg2->atype == AIMM)
		{
			/*		cmpi				*/
			/* hanna
				strcpy(instr_buf,inst->name);
				strcat(instr_buf,"i");
			*/
			strcpy(instr_buf,"cmpi");
			inst = lookup(instr_buf,N_INSTALL,MNEMON)->itp;
			object = inst->opcode;
			object |= (size << 6);
			if (size == L)
				eff_add(arg1,DATALT | 0x4000,size);
			else
				eff_add(arg1,DATALT | 0x2000,size);
			gen_inst_word();	/* preparation for second word */
			if (size == B)
				generate(BITSPOW,(arg2->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg2->xp->xvalue & 0xff,arg2->xp->xsymptr);
			else if (size == W)
				generate(BITSPOW,(arg2->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg2->xp->xvalue & 0xffff,arg2->xp->xsymptr);
			else
				generate(2*BITSPOW,(arg2->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg2->xp->xvalue,arg2->xp->xsymptr);
#ifndef m68k
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			gen_eff_add();
#endif
			return;
		}
		else if (arg1->atype == ADREG)
		{
			object |= (arg1->areg1->opcode << 9);
			object |= (size << 6);
			eff_add(arg2,ALL,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
			return;
		}
		else
		{
			yyerror("invalid operands for compare instruction");
			return;
		}

	case BCHG:
	case BTST:
		if (size != UNSPEC)
			werror("bit instructions should not have a size attribute, long will be used if target is a data register otherwise byte will be used");
		if (arg1->atype == ADREG)
		{
			object |= (arg1->areg1->opcode << 9);
			if (inst->tag == BTST)
				eff_add(arg2,DATNIM,size);
			else
				eff_add(arg2,DATALT,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
		}
		else if (arg1->atype == AIMM)
		{
			/* Object code for bit instructions is 0x01?0	*/
			/*	when the bit number is dynamic (in a	*/
			/*	register), but 0x08?0 for a static bit	*/
			/*	number (bit number is immediate).	*/
			/*	Object code is converted by turning off	*/
			/*	the 1 bit and turning on the 8 bit.	*/
			object ^= 0x0100;
			object |= 0x0800;
			if (inst->tag == BTST)
				eff_add(arg2,DATNIM,size);
			else
				eff_add(arg2,DATALT,size);
			gen_inst_word();	
			generate(BITSPOW,
				(arg1->xp->xsymptr == NULLSYM) ? 0 : GENRELOC,
				arg1->xp->xvalue,arg1->xp->xsymptr);
#ifndef m68k
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			gen_eff_add();
#endif
		}
		else
			yyerror("incorrect first operand to bit instruction");
		return;

	case ADD:
		if (arg1->atype == AIMM &&
			    arg1->xp->xsymptr == NULLSYM &&
			    arg1->xp->xvalue > 0 &&
			    arg1->xp->xvalue <= 8)
		{ /* addq/subq */
			strcpy(instr_buf,inst->name);
			if( instr_buf[3]!='q')	/* Already addq or subq? */
				if( instr_buf[3] == 'i' ) /* Already addi or subi? */
		 		{
					instr_buf[3] = 'q';   /* replace it by addq/subq */
					werror("immediate data is <= 8. Quick is used instead of immediate");
				}
				else
					strcat(instr_buf,"q");
			inst = lookup(instr_buf,N_INSTALL,MNEMON)->itp;
			object = inst->opcode;
			if (arg1->xp->xvalue != 8)
				object |= (arg1->xp->xvalue << 9);
			if (size == UNSPEC)
				size = W;
			if (size == B && arg2->atype == AAREG)
			{
				yyerror("additive instruction with address destination cannot have byte attribute");
				size = W;
			}
			object |= (size << 6);
			eff_add(arg2,ALT,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
			return;
		}
		else if (arg2->atype == AAREG)
		{ /* adda/suba	*/
			if (size == B)
				yyerror("additive instruction with address destination cannot have byte attribute");
			object |= (arg2->areg1->opcode << 9);
			object |= (size == L) ? 0x1c0 : 0xc0;
			eff_add(arg1,ALL,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
			return;
		}
		else if (arg1->atype == AIMM)
		{
			/* addi/subi, where destination is not %an, and
			** literal is not between 1 and 8
			*/
			strcpy(instr_buf,inst->name);
			if( instr_buf[3]!='i')	/* Already addi or subi? */
				if( instr_buf[3] == 'q')   /* Already addq or subq? */
				{
					instr_buf[3] = 'i';   /* replace it by addi/subi */
					werror("immediate data is > 8. Immediate is used instead of quick");
				}
				else
					strcat(instr_buf,"i");
			inst = lookup(instr_buf,N_INSTALL,MNEMON)->itp;
			object = inst->opcode;
			if (size == UNSPEC)
				size = W;
			object |= (size << 6);
			if (size == L)
				eff_add(arg2,DATALT | 0x4000,size);
			else
				eff_add(arg2,DATALT | 0x2000,size);
			gen_inst_word();
			if (size == B)
				generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg1->xp->xvalue & 0xff,arg1->xp->xsymptr);
			else if (size == W)
				generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg1->xp->xvalue & 0xffff,arg1->xp->xsymptr);
			else
				generate(2*BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
					 arg1->xp->xvalue,arg1->xp->xsymptr);
#ifndef m68k
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			gen_eff_add();
#endif
			return;
		}
		else if (arg2->atype == ADREG)
		{
			/* add/sub <ea>,Dn */
			object |= (arg2->areg1->opcode << 9);
			if (size == UNSPEC)
				size = W;
			object |= (size << 6);
			eff_add(arg1,ALL,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
			return;
		}
		else if (arg1->atype == ADREG)
		{
			/* add/sub Dn,<ea> */
			object |= (arg1->areg1->opcode << 9);
			object |= 0x100;
			object |= (size << 6);
			eff_add(arg2,ALTMEM,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
			return;
		}
		else
			yyerror("illegal operand combination for additive instruction");
		return;

	case MOVM:
		if (size == B)
			yyerror("movm instruction allows only word and long attribute");
		if (size == L)
			object |= 0x40;
		if (arg2->atype == AIMM)
		{
			object |= 0x400;
			eff_add(arg1,CONPOST | 0x2000,size);
			gen_inst_word();
			generate(BITSPOW,
			(arg2->xp->xsymptr==NULLSYM)?0:GENRELOC,
				 arg2->xp->xvalue,arg2->xp->xsymptr);
#ifndef m68k
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			gen_eff_add();
#endif
		}
		else if (arg1->atype != AIMM)
			yyerror("incorrect operands for movm instruction");
		else
		{
			eff_add(arg2,CONPRE | 0x2000,size);
			gen_inst_word();
			generate(BITSPOW,
				(arg1->xp->xsymptr == NULLSYM) ? 0 : GENRELOC,
				arg1->xp->xvalue,arg1->xp->xsymptr);
#ifndef m68k
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			gen_eff_add();
#endif
		}
		return;

	case MOVS:
		{
			register int inst2wd;

			if( !as10 )
				werror("a 68010 instruction");
			if ((inst2wd=(arg1->atype == AAREG)) || arg1->atype == ADREG)
			{
				eff_add(arg2,ALTMEM,size);
				inst2wd = (inst2wd << 15) | (arg1->areg1->opcode<< 12) | (1 << 11);
			}
			else if ((inst2wd=(arg2->atype == AAREG)) || arg2->atype == ADREG)
			{
				eff_add(arg1,ALTMEM,size);
				inst2wd = (inst2wd << 15) | (arg2->areg1->opcode<< 12);
			}
			else
				yyerror("one register operand req'd for movs");
			object |= (size == UNSPEC ? W : size) << 6;
			gen_inst_word();
			generate(BITSPOW,0,inst2wd,NULLSYM);
#ifndef m68k
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			gen_eff_add();
#endif
			return;
		}

	case MOVP:
		if (size == B)
			yyerror("movp instruction allows only word and long attribute");
		if (arg1->atype == ADREG && arg2->atype == AOFF)
		{
			object |= (arg1->areg1->opcode << 9);
			object |= (((size == L) ? 7 : 6) << 6);
			object |= arg2->areg1->opcode;
			generate(BITSPOW,0,object,NULLSYM);
			generate(BITSPOW,(arg2->xp->xsymptr == NULL) ? 0 : GENRELOC,
				 arg2->xp->xvalue,arg2->xp->xsymptr);
		}
		else if (arg1->atype == AOFF && arg2->atype == ADREG)
		{
			object |= (arg2->areg1->opcode << 9);
			object |= (((size == L) ? 5 : 4) << 6);
			object |= arg1->areg1->opcode;
			generate(BITSPOW,0,object,NULLSYM);
			generate(BITSPOW,(arg1->xp->xsymptr == NULL) ? 0 : GENRELOC,
				 arg1->xp->xvalue,arg1->xp->xsymptr);
		}
		else
			yyerror("incorrect operands for movp instruction");
		return;

	case MOV:
		strcpy(instr_buf,inst->name);	/* hoss#9 */
		if (arg2->atype == CCREG)
		{
			/* move to CCR */
			object = 0x44c0;
			eff_add(arg1,DATAA,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
			return;
		}
		else if (arg1->atype == CCREG)
		{
			if( !as10 )
				werror("a 68010 instruction");
			object = 0x42c0;
			eff_add(arg2,DATALT,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
			return;
		}
		else if (arg1->atype == CTLREG )
		{
			if (size == UNSPEC )
				size = L;
			else if(size != L)
				yyerror("only long size attribute allowed");
			if (arg2->atype == AAREG)
			{
				if (arg1->areg1->opcode == USPREG)
				{	/* old form is one word */
					object = 0x4e68;
					object |= arg2->areg1->opcode;
					generate(BITSPOW,0,object,NULLSYM);
					return;
				}
				object = (0x8 | arg2->areg1->opcode) << 12;
			}
			else if (arg2->atype == ADREG)
				object = (arg2->areg1->opcode) << 12;
			else
				yyerror("destination operand must be a register");
			if( !as10 )
				werror("a 68010 instruction");
			object |= arg1->areg1->opcode;
			generate(BITSPOW,0,0x4e7a,NULLSYM);
			generate(BITSPOW,0,object,NULLSYM);
			return;
		}
		else if (arg2->atype == CTLREG )
		{
			if (size == UNSPEC )
				size = L;
			else if(size != L)
				yyerror("only long size attribute allowed");
			if (arg1->atype == AAREG)
			{
				if (arg2->areg1->opcode == USPREG)
				{
					object = 0x4e60;
					object |= arg1->areg1->opcode;
					generate(BITSPOW,0,object,NULLSYM);
					return;
				}
				object = (0x8 | arg1->areg1->opcode) << 12;
			}
			else if (arg1->atype == ADREG)
				object = (arg1->areg1->opcode) << 12;
			else
				yyerror("source operand must be a register");
			if( !as10 )
				werror("a 68010 instruction");
			object |= arg2->areg1->opcode;
			generate(BITSPOW,0,(0x4e7a | 0x1),NULLSYM);
			generate(BITSPOW,0,object,NULLSYM);
			return;
		}
		else if (arg2->atype == SRREG)
		{
			/* move to SR */
			object = 0x46c0;
			eff_add(arg1,DATAA,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
			return;
		}
		else if (arg1->atype == SRREG)
		{
			object = 0x40c0;
			eff_add(arg2,DATALT,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
			return;
		}
		/* hoss#9 beg */
		else if (instr_buf[4] == 'q' &&
			 arg1->atype == AIMM &&
			 (arg1->xp->xvalue < -128 || arg1->xp->xvalue > 127))
			{
				yyerror("immediate operand of moveq out of range");
				return;
			}
		/* hoss#9 end */
		else if (arg1->atype == AIMM && arg2->atype == ADREG &&
			 (size == L || size == UNSPEC) &&
			 arg1->xp->xsymptr == NULLSYM &&
			 arg1->xp->xvalue >= -128 && arg1->xp->xvalue <= 127)
		{
			/* move quick */
			object = 0x7000;
			object |= (arg2->areg1->opcode << 9);
			object |= (arg1->xp->xvalue & 0xff);
			generate(BITSPOW,0,object,NULLSYM);
			return;
		}
		else if (arg2->atype == AAREG)
		{
			/* movea */
			object = 0x0040;
			if (size == B)
				yyerror("mov to address register cannot have byte attribute");
			object |= (((size == L) ? 0x2 : 0x3) << 12);
			object |= (arg2->areg1->opcode << 9);
			eff_add(arg1,ALL,size);
#ifndef m68k
			generate(BITSPOW,action,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
#else
			generate(BITSPOW,ea.action1,object,NULLSYM);
			gen_eff_add();
#endif
			return;
		}
		else
		{
#ifndef m68k
			register unsigned short	savaction;
			register long		savword;
			register int		savnum;
			register symbol		*savsym;
#else
			struct eatype		sav_ea;
			register long		tmpobject;
#endif

			if (size == UNSPEC)
				size = W;
			eff_add(arg2,DATALT | 0x1000,size);
			/* Pick off mode and register for destination,	*/
			/*	and move them up to where they belong.	*/
#ifndef m68k
			savword = inst->opcode;
			savword |= ((object & 07) << 9);
			savword |= ((object & 070) << 3);
			object = savword;
			savnum = numwords;
			savaction = action;
			savsym = symptr;
			savword = word;
#else
			tmpobject = inst->opcode;
			tmpobject |= ((object & 07) << 9);
			tmpobject |= ((object & 070) << 3);
			object = tmpobject;
			sav_ea = ea;
#endif
			if (size == B)
				object |= 0x1000;
			else if (size == W)
				object |= 0x3000;
			else
				object |= 0x2000;
			eff_add(arg1,ALL | 0x1000,size);
#ifndef m68k
			if (action == BRLOC) /* pc-relative addressing */
				generate(BITSPOW,action,object,NULLSYM);
			else
			if (action == 0 && savaction == 0)
				generate(BITSPOW,0,object,NULLSYM);
			else
				generate(BITSPOW,MOVE,object,NULLSYM);
			if (numwords == 1)
				generate(BITSPOW,action,word,symptr);
			else if (numwords == 2)
				generate(2*BITSPOW,action,word,symptr);
			if (savnum == 1)
				generate(BITSPOW,savaction,savword,savsym);
			else if (savnum == 2)
				generate(2*BITSPOW,savaction,savword,savsym);
#else
			if (ea.action1 == BRLOC) /* pc-relative addressing */
				generate(BITSPOW,ea.action1,object,NULLSYM);
			else
			if (ea.action1 == 0 && sav_ea.action1 == 0)
				generate(BITSPOW,0,object,NULLSYM);
			else
				generate(BITSPOW,MOVE,object,NULLSYM);
			gen_eff_add();
			ea = sav_ea;
			gen_eff_add();
#endif
			return;
		}

	default:
		yyerror("unknown two-operand instruction usage");
	}	/* end switch */
}	/* gen2op()	*/


gen3op(inst,size,arg1,arg2,arg3)
register instr		*inst;
register int		size;
register struct arg	*arg1,
			*arg2,
			*arg3;
{

	object = inst->opcode;
    
	switch ((int)(unsigned char)inst->tag)
	{

#ifdef M68881
	case FMOV:
	{	/* fmov %fpm,<ea>{&k}
		 * fmov %fpm,<ea>{%dn}
		 */
		register int inst2wd;		/* second word of instruction format */
		if (size == UNSPEC) size = P;
		if ( size != P)
			yyerror("instruction has illegal size attribute");

		object |=  cp_id << 9;
		if (arg1->atype != FPREG)
			yyerror ("instruction has illegal register for first operand");
		if (arg2->atype == FPREG)
			yyerror ("instruction has illegal register for second operand");

		eff_add(arg2,ALTMEM,size);
		inst2wd = arg1->areg1->opcode << 7;
		if (arg3->atype == AIMM)
			inst2wd |= (0x6c00 | arg3->xp->xvalue & 0x7f);
		else
		if (arg3->atype == ADREG)
			inst2wd |= (0x7c00 | arg3->areg1->opcode << 4);
		else
			yyerror("instruction has illegal register for third operand");
		gen_inst_word();
		generate(BITSPOW,0,inst2wd,NULLSYM);
		gen_eff_add();
		return;
	} /* fmov */

	case FMOVM:
	{	register int csi,	/* control,status,iaddr combination */
		inst2wd;

		if ( size != L && size != UNSPEC )
			werror("instrucion has illegal size attribute, size set to L");
		size = L;
		object |= cp_id << 9;

		/* fmovm <ea>,%control/%status
		 * fmovm <ea>,%control/%iaddr
		 * fmovm <ea>,%status/%iaddr
		 */
		if ((arg2->atype == FPCONTROL || arg2->atype == FPSTATUS ||
			arg2->atype == FPIADDR ) &&
			(arg3->atype == FPCONTROL || arg3->atype == FPSTATUS ||
			arg3->atype == FPIADDR ) )
		{
			csi = (arg2->areg1->opcode | arg3->areg1->opcode) ;
			if ( csi != 0x6 && csi != 0x5 && csi !=  0x3 )
				yyerror("instruction has illegal operand combinations");
			eff_add (arg1,MEMORY,size); 
			inst2wd = 0x8000 | csi << 10;
			gen_inst_word();
			generate(BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
			return;
		}
		else
		/* fmovm %control/%status,<ea>
		 * fmovm %control/%iaddr,<ea>
		 * fmovm %status/%iaddr,<ea>
		 */
		if ((arg1->atype == FPCONTROL || arg1->atype == FPSTATUS ||
			arg1->atype == FPIADDR ) &&
			(arg2->atype == FPCONTROL || arg2->atype == FPSTATUS ||
			arg2->atype == FPIADDR ) )
		{
			csi = (arg1->areg1->opcode | arg2->areg1->opcode) ;
			if ( csi != 0x6 && csi != 0x5 && csi !=  0x3 )
				yyerror("instruction has illegal operand combinations");
			eff_add (arg3,ALTMEM,size); 
			inst2wd = 0xa000 | csi << 10;
			gen_inst_word();
			generate(BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
			return;
		}
		else
		{
			yyerror("instruction has illegal operand combinations");
			return;
		}
	}	/* fmovm */

	case FSINCOS:
	{	/* fsincos <ea>,%fpn:%fpq
		 * fsincos %fpm,%fpn:%fpq
		 */
		register int inst2wd;
		if (size != UNSPEC && size != B && size != W && size != L &&
			size != S && size != D && size != X && size != P)
			yyerror("instruction has illegal size attribute");
		if ( arg2->atype != FPREG)
			yyerror("instruction requires floating point register for second operand");
		if ( arg3->atype != FPREG)
			yyerror("instruction requires floating point register for third operand");
		object |= cp_id << 9;
		inst2wd = (arg3->areg1->opcode << 7 | arg2->areg1->opcode);
		if ( arg1->atype == FPREG)
		{
			if ( size != UNSPEC && size != X )
				/* hoss#0 */
				werror ("instruction requires size X, if source is a floating point register... changed the size to X");
		
			size = X;
			inst2wd |= (0x0030 | arg1->areg1->opcode << 10);
			generate (BITSPOW,0,object,NULLSYM);
			generate (BITSPOW,0,inst2wd,NULLSYM);
			return;
		}
		else
		{
			if (arg1->atype == FPCONTROL ||
				arg1->atype == FPIADDR ||
				arg1->atype == FPSTATUS )
				yyerror("instruction has illegal register for first operand");
			if (size == UNSPEC) size = X;
			inst2wd |= (0x4030 | SRCFMT[size] << 10);
			if ( size == B || size == W || size == L || size == S)
				eff_add (arg1,DATAA,size);
			else
				eff_add (arg1,MEMORY,size);
			gen_inst_word();
			generate(BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
			return;
		}
	} /* fsincos */
#endif /*M68881*/

#ifdef m68k
	case BFTST:
	case BFCHG:
		/* bfchg, bfclr, bfset, bftst <ea>{offset:width} */
		{
		register int inst2wd;		/* second word of instruction format */

		if (size != UNSPEC)
			yyerror("bit field instructions are unsized");

		if (inst->tag == BFTST)
			eff_add(arg1,DnCONTROL,size);	/* allow data register direct or */
		                            		/* control addressing mode */
		else
			eff_add(arg1,DnALTCON,size);	/* allow data register direct or */
		                            		/* alterable control addressing mode */
		
		if (arg2->atype == AIMM)	/* offset is an immediate value */
		{
			inst2wd = (arg2->xp->xvalue & 0x001f) << 6;
			if ( arg2->xp->xvalue < 0 || arg2->xp->xvalue > 31 )
				werror("offset must be between 0 and 31");
		}
		else
		if (arg2->atype == ADREG)	/* offset is from a data register */
			inst2wd = arg2->areg1->opcode << 6 | 0x800;
		else
			yyerror("bit field instruction offset should be either immediate operand  or data register");

		if (arg3->atype == AIMM)	/* width  is an immediate value */
		{
			inst2wd |= (arg3->xp->xvalue & 0x001f) ;
			if ( arg3->xp->xvalue < 1 || arg3->xp->xvalue > 32 )
				werror("width must be between 1 and 32");
		}
		else
		if (arg3->atype == ADREG)	/* width is from a data register */
			inst2wd |= arg3->areg1->opcode  | 0x20;
		else
			yyerror("bit field instruction width requires immediate operand or data register");

		gen_inst_word();
		generate(BITSPOW,0,inst2wd,NULLSYM);
		gen_eff_add();
		return;
		} /* bfchg, bfclr, bfset, bftst */
#endif /*m68k*/

#ifdef m68k
	case CAS:
	{	/* cas dn,dn,<ea> */	
		register int inst2wd;	/* second word of instruction format */
		if ( size == B )
			object |= 0x200;
		else
		if ( size == W )
			object |= 0x400;
		else
		if ( size == L )
			object |= 0x600;
		else
			yyerror ("cas instruction required size attribute");
	
		inst2wd = 0;
		if ( arg1->atype != ADREG )
			yyerror ("data register required as first operand of cas instruction");
		else
			inst2wd |= arg1->areg1->opcode;

		if ( arg2->atype != ADREG )
			yyerror ("data register required as second operand of cas instruction");
		else
			inst2wd |= arg2->areg1->opcode << 6;
		eff_add (arg3,ALTMEM,size);
		gen_inst_word();
		generate (BITSPOW, 0, inst2wd, NULLSYM);
		gen_eff_add();
		return;
	}	/* cas */
#endif /*m68k*/
#ifdef m68k
	case MULS:
	case MULU:
	case DIVS:
	case DIVU:
	case TDIVS:
	case TDIVU:
	{
		/*	mulx.l  <ea>,Dh:Dl	32*32 ->64
		 *	divx.l  <ea>,Dr:Dq	64/32 ->32r:32q
		 *	tdivx.l <ea>,Dr:Dq	32/32 ->32r:32q
		 */
		register int inst2wd;

		if ( size != L && size != UNSPEC )
			yyerror("instruction has incorrect size attribute");

		/* generate long form format */
		if (inst->tag == MULS || inst->tag == MULU)
			object = 0x4c00;
		else
			object = 0x4c40;

		inst2wd = 0;
		if ( inst->tag != TDIVS && inst->tag != TDIVU )
			inst2wd =   0x400;	/* turn on bit 10 for 64 bit dividend */
			                 	/* or 64 bit product */
		inst2wd |=  (arg3->areg1->opcode << 12);
		inst2wd |=  arg2->areg1->opcode ;
		if (inst->tag == MULS || inst->tag == DIVS || inst->tag == TDIVS) 
			inst2wd |= 0x800;	/* turn on bit 11 for signed */
				                /* multiply or division */

		eff_add(arg1,DATAA,size);
		gen_inst_word();
		generate (BITSPOW,0,inst2wd,NULLSYM);
		gen_eff_add();
		return;
	} /* muls, mulu, divs, divu */
#endif /*m68k*/

#ifdef M68030
	case PFLUSH:
		{     /* pflush <function code>,#<mask>,<ea> */
		register int inst2wd;	/* second word of instruction format */
		if ( size != UNSPEC )
			werror("PFLUSH instruction is unsized");
		if (strlen(inst->name)!=6)
			yyerror("no operands expected for pflusha");
		inst2wd = 0x3800;
		inst2wd |= function_code(arg1);

		if (arg2->atype != AIMM)
			yyerror("second operand of wrong type -- immediate mask expected");
		inst2wd |= (arg2->xp->xvalue & 0x7) << 5;
		if ((unsigned)arg2->xp->xvalue > 0x7)
			werror("mask too large");

		eff_add(arg3,CONTROL,size);
		gen_inst_word();
		generate(BITSPOW,0,inst2wd,NULLSYM);
		gen_eff_add();
		return;
		}
	case PTEST:
		{     /* ptest[rw] <function code>,<ea>,#<level> */
		register int inst2wd;	/* second word of instruction format */
		if ( size != UNSPEC )
			werror("PTEST instruction is unsized");
		if ( inst->name[5] == 'r' )
			inst2wd = 0x8200;
		else
			inst2wd = 0x8000;
		inst2wd |= function_code(arg1);

		if (arg3->atype != AIMM)
			yyerror("third operand of wrong type -- immediate level expected");
		inst2wd |= (arg3->xp->xvalue & 0x7) << 10;
		if ((unsigned)arg3->xp->xvalue > 0x7)
			werror("level too large");

		eff_add(arg2,CONTROL,size);
		gen_inst_word();
		generate(BITSPOW,0,inst2wd,NULLSYM);
		gen_eff_add();
		return;
		}
#endif /* M68030 */

	case PACK:
	case UNPK:
		/* pack -(%ax),-(%ay),&xxx */
		/* pack %dx,%dy,&xxx */
		/* unpk -(%ax),-(%ay),&xxx */
		/* unpk %dx,%dy,&xxx */
		if ( !as20 ) 
			werror("a 68020 instruction");
		if ( size != UNSPEC )
			werror("PACK/UNPK instructions are unsized");

		if (arg1->atype == ADEC && arg2->atype == ADEC)  /* -(Ax) */
			object |= 0x8;              /* set R/M field */
		else
		if (arg1->atype != ADREG || arg2->atype != ADREG)
			yyerror("only data register or predecrement address register pairs allowed as operands");

		if (arg3->atype != AIMM)
			yyerror("immediate value required for the third operand");
		else
		if ( arg3->xp->xvalue < -(1<<15) || arg3->xp->xvalue >= (1<<15))
			werror("extension must fit into a word (masked)");

		/* pack destination and source registers */
		object |= (arg2->areg1->opcode << 9 );          
		object |=  arg1->areg1->opcode;
		generate(BITSPOW,0,object,NULLSYM);
		generate(BITSPOW,(arg3->xp->xsymptr == NULL) ? 0 : GENRELOC,
			arg3->xp->xvalue &= 0xffff,arg3->xp->xsymptr);
		return;

	default:
		yyerror("unknown three-operands instruction usage");
	}	/* end switch */
}	/* gen3op() */

gen4op(inst,size,arg1,arg2,arg3,arg4)
register instr		*inst;
register int		size;
register struct arg	*arg1,
			*arg2,
			*arg3,
			*arg4;
{

	object = inst->opcode;
    
	switch ((int)(unsigned char)inst->tag)
	{
#ifdef M68881
	case FMOVM:
	{	register int	csi,	/* control,status,iaddr */
			inst2wd;

		if ( size != L && size != UNSPEC )
			werror("instrucion has illegal size attribute, size set to L");
		size = L;
		object |= cp_id << 9;

		/* fmovm <ea>,%control/%status/%iaddr
		 */
		if ( (arg2->atype == FPCONTROL || arg2->atype == FPSTATUS ||
				arg2->atype == FPIADDR)
			&& (arg3->atype == FPCONTROL || arg3->atype == FPSTATUS ||
				arg3->atype == FPIADDR)
			&& (arg4->atype == FPCONTROL || arg4->atype == FPSTATUS ||
				arg4->atype == FPIADDR) )
		{
			csi = (arg2->areg1->opcode|arg3->areg1->opcode|arg4->areg1->opcode);
			if ( csi != 0x7 )
				yyerror("instruction has illegal operand combinations");

			eff_add (arg1,MEMORY,size); 
			inst2wd = 0x8000 | csi << 10;
			gen_inst_word();
			generate(BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
			return;
		}
		else
		/* fmovm %control/%status/%iaddr,<ea>
		 */
		if ( (arg1->atype == FPCONTROL || arg1->atype == FPSTATUS ||
				arg1->atype == FPIADDR)
			&& (arg2->atype == FPCONTROL || arg2->atype == FPSTATUS ||
				arg2->atype == FPIADDR)
			&& (arg3->atype == FPCONTROL || arg3->atype == FPSTATUS ||
				arg3->atype == FPIADDR) )
		{
			csi = (arg1->areg1->opcode|arg2->areg1->opcode|arg3->areg1->opcode);
			if ( csi != 0x7 )
				yyerror("instruction has illegal operand combinations");
		 
			eff_add (arg4,ALTMEM,size); 
			inst2wd = 0xa000 | csi << 10;
			gen_inst_word();
			generate(BITSPOW,0,inst2wd,NULLSYM);
			gen_eff_add();
			return;
		}
		else
		{
			yyerror("instruction has illegal operand combinations");
			return;
		}
	}	/* fmovm */

#endif /*M68881*/

#ifdef m68k
	case BFFFO:
		/* bfffo,bfexts,bfextu  <ea>{offset:width},Dn */
		{
		register int inst2wd;		/* second word of instruction format */

		if (size != UNSPEC)
			yyerror("bit field instructions are unsized");

		eff_add(arg1,DnCONTROL,size);	/* allow data register direct or */
		                            	/* control addressing mode */

		if (arg2->atype == AIMM)	/* offset is an immediate value */
		{
			inst2wd = (arg2->xp->xvalue & 0x001f) << 6;
			if ( arg2->xp->xvalue < 0 || arg2->xp->xvalue > 31 )
				werror("offset must be between 0 and 31");
		}
		else
		if (arg2->atype == ADREG)	/* offset is from a data register */
			inst2wd = arg2->areg1->opcode << 6 | 0x800;
		else
			yyerror("bit field instruction offset requires immediate operand or data register");

		if (arg3->atype == AIMM)	/* width  is an immediate value */
		{
			inst2wd |= (arg3->xp->xvalue & 0x001f) ;
			if ( arg3->xp->xvalue < 1 || arg3->xp->xvalue > 32 )
				werror("width must be between 1 and 32");
		}
		else
		if (arg3->atype == ADREG)	/* width is from a data register */
			inst2wd |= arg3->areg1->opcode  | 0x20;
		else
			yyerror("bit field instruction width requires immediate operand or data register");

		if (arg4->atype == ADREG)
			inst2wd |= arg4->areg1->opcode  << 12;
		else
			yyerror("destination should be a data register");

		gen_inst_word();
		generate(BITSPOW,0,inst2wd,NULLSYM);
		gen_eff_add();
		return;
		}

	case BFINS:
		{  /* bfins Dn,<ea>{offset:width} */
		register int inst2wd;		/* second word of instruction format */

		if (size != UNSPEC)
			yyerror("BFINS instructions are unsized");

		eff_add(arg2,DnALTCON,size);	/* allow data register direct or */
		                            	/* alterable control addressing mode */
		if (arg1->atype == ADREG)
			inst2wd = arg1->areg1->opcode  << 12;
		else
			yyerror("source should be from a data register");

		if (arg3->atype == AIMM)	/* offset is an immediate value */
		{
			inst2wd |= (arg3->xp->xvalue & 0x001f) << 6;
			if ( arg3->xp->xvalue < 0 || arg3->xp->xvalue > 31 )
				werror("offset must be between 0 and 31");
		}
		else
		if (arg3->atype == ADREG)	/* offset is from a data register */
			inst2wd |= arg3->areg1->opcode << 6 | 0x800;
		else
			yyerror("BFINS instruction offset requires either immediate operand or data register");

		if (arg4->atype == AIMM)	/* width  is an immediate value */
		{
			inst2wd |= (arg4->xp->xvalue & 0x001f) ;
			if ( arg4->xp->xvalue < 1 || arg4->xp->xvalue > 32 )
				werror("width must be between 1 and 32");
		}
		else
		if (arg4->atype == ADREG)	/* width is from a data register */
			inst2wd |= arg4->areg1->opcode  | 0x20;
		else
			yyerror("BFINS instruction width requires immediate operand or data register");


		gen_inst_word();
		generate(BITSPOW,0,inst2wd,NULLSYM);
		gen_eff_add();
		return;
		}
#endif /*m68k*/

#ifdef M68030	
	case PTEST:
		{     /* ptest[rw] <function code>,<ea>,#<level>,An */
		register int inst2wd;	/* second word of instruction format */
		if ( size != UNSPEC )
			werror("PTEST instruction is unsized");
		if ( inst->name[5] == 'r' )
			inst2wd = 0x8300;
		else
			inst2wd = 0x8100;
		inst2wd |= function_code(arg1);

		if (arg3->atype != AIMM)
			yyerror("third operand of wrong type -- immediate level expected");
		inst2wd |= (arg3->xp->xvalue & 0x7) << 10;
		if ((unsigned)arg3->xp->xvalue > 0x7)
			werror("level too large");

		if (arg4->atype != AAREG)
			yyerror("fourth operand must be %An");
		inst2wd |= arg4->areg1->opcode << 5;

		eff_add(arg2,CONTROL,size);
		gen_inst_word();
		generate(BITSPOW,0,inst2wd,NULLSYM);
		gen_eff_add();
		return;
		}
#endif	/* M68030 */

	default:
		yyerror("unknown four-operands instruction usage");
	}	/* end switch */
}	/* gen4op() */


gen6op(inst,size,arg1,arg2,arg3,arg4,arg5,arg6)
register instr		*inst;
register int		size;
register struct arg	*arg1,
			*arg2,
			*arg3,
			*arg4,
			*arg5,
			*arg6;
{

	object = inst->opcode;
    
	switch ((int)(unsigned char)inst->tag)
	{
		case CAS2:
		{	/* cas2 Dw1:Dw2,Do1:Do2,(Rz1):(Rz2) */
			register int inst2wd,	/* second word of instruction format */
			             inst3wd;	/* third word of instruction format  */
				
			long longword;
		if (!as20)
			werror("a 68020 instruction");
		if ( size == B )
			object |= 0x200;
		else
		if ( size == W )
			object |= 0x400;
		else
		if ( size == L )
			object |= 0x600;
		else
			yyerror ("cas2 instruction required size attribute");
	
		inst2wd = 0;
		inst3wd = 0;
		if ( arg1->atype != ADREG || arg2->atype != ADREG )
			yyerror ("test values to be compared against memory operands must be contained in data registers");
		else
		{
			inst2wd |= arg1->areg1->opcode;
			inst3wd |= arg2->areg1->opcode;
		}

		if ( arg3->atype != ADREG || arg4->atype != ADREG )
			yyerror ("update values to be written to memory operand locations must be held in data registers");
		else
		{
			inst2wd |= arg3->areg1->opcode << 6;
			inst3wd |= arg4->areg1->opcode << 6;
		}

		if ( arg5->atype == ADREG )
			inst2wd |= arg5->areg1->opcode << 12;
		else
		if ( arg5->atype == AAREG )
			inst2wd |= arg5->areg1->opcode << 12 | 0x8000;
		else
			yyerror ("address of tested operands must be contained in either data or address register");

		if ( arg6->atype == ADREG )
			inst3wd |= arg6->areg1->opcode << 12;
		else
		if ( arg6->atype == AAREG )
			inst3wd |= arg6->areg1->opcode << 12 | 0x8000;
		else
			yyerror ("address of tested operands must be contained in either data or address register");

		generate (BITSPOW, 0, object, NULLSYM);
		longword = ( longword = inst2wd ) << 16 | inst3wd;
		generate (2*BITSPOW, 0, longword, NULLSYM);
		return;
		}	/* cas2 */
	default:
		yyerror("unknown six-operands instruction usage");
	}	/* end switch */
}	/* gen6op() */


flags(flag)
char	flag;
{
	char	errmsg[64];

	switch (flag){
		case 'r': /* put .data into .text space */
			sprintf(errmsg,
			"-r flag no longer supported.  Use -Xnostrwrite" );
			werror(errmsg);
			break;
		default:
			sprintf(errmsg,"illegal flag (%c) - ignored",flag);
			werror(errmsg);
	}
}	/* flag()	*/


#ifdef m68k
/*
 *	gen_eff_add()
 *
 *	common generation for effective address 
 *	created in eff_add()
 */

gen_eff_add()
{

	if( ea.len1 == BITSPBY && ea.action1==BRLOC)
	{	/* only the low order 8 bits need to be relocated */
		generate(BITSPBY,0,ea.ext1>>BITSPBY,NULLSYM);
		generate(BITSPBY,ea.action1,0,ea.sym1);
	}
	else
	if( ea.len1 != INVALID )
		generate(ea.len1*BITSPOW,ea.action1,ea.ext1,ea.sym1);
	if( ea.len2 != INVALID )
		generate(ea.len2*BITSPOW,ea.action2,ea.ext2,ea.sym2);
	if( ea.len3 != INVALID )
		generate(ea.len3*BITSPOW,ea.action3,ea.ext3,ea.sym3);
}
#endif /*m68k*/

/*
 *	gen_inst_word()
 *	
 *	generate instruction word:
 *	common module used by instructions that may have 2 to 3
 *	more instruction words before extension words. 
 *
 */
gen_inst_word()
{
#ifndef m68k
			if (action == BRLOC)
				generate(BITSPOW,action,object,NULLSYM);
			else
			if (action == ABSOPT)
				generate(BITSPOW,IOPT,object,NULLSYM);
			else
			if (action == ABSBR)
				generate(BITSPOW,IABSOPT,object,NULLSYM);
			else
				generate(BITSPOW,INSTI,object,NULLSYM);
#else
			if (ea.action1 == BRLOC)
				generate(BITSPOW,BRLOC,object,NULLSYM);
			else
			if (ea.action1 == ABSOPT)
				generate(BITSPOW,IOPT,object,NULLSYM);
			else
			if (ea.action1 == ABSBR)
				generate(BITSPOW,IABSBR,object,NULLSYM);
			else
				generate(BITSPOW,INSTI,object,NULLSYM);
#endif
}

#ifdef	M68881
/*
 *	GET_FP_VALUE
 *		Calls  do_fp_num, which converts a floating point ascii
 *	string to its binary value, and puts the returned result into
 *	3 long extension words.
 *
 *	Inputs:
 *		size		instruction extension 
 *
 *	Other parameters:
 *		FP_STR		global variables containing
 *			mantissa string, exponent string,
 *			sign of mantissa, sign of exponent, and
 *			number of digits to the right of the decimal point.
 *
 *		Returns	ea.ext1, ea.ext2, ea.ext3
 */
get_fp_value(size)
register int size;
{
	int	fp_num[6];

	switch(size)
	{
		case S:
			fp_extension = FP_S;
			fp_precision = FP_S;
			break;
		case D:
			fp_extension = FP_D;
			fp_precision = FP_D;
			break;
		case X:
			fp_extension = FP_X;
			fp_precision = FP_X;
			break;
		case P:
			fp_extension = FP_P;
			fp_precision = FP_P;
			break;
		default:
			aerror("Illegal size for floating point conversion");
	}	/* end switch */

	do_fp_num(fp_num);

	ea.ext1 = (fp_num[5] & 0xffff) ;
	ea.ext1 = (ea.ext1 << 16 | fp_num[4] & 0xffff);

	ea.ext2 = (fp_num[3] & 0xffff) ;
	ea.ext2 = (ea.ext2 << 16 | fp_num[2] & 0xffff);

	ea.ext3 = (fp_num[1] & 0xffff) ;
	ea.ext3 = (ea.ext3 << 16 | fp_num[0] & 0xffff);
}	/* end get_fp_value */
#endif	/*M68881*/


#ifdef	M68030 		/*  added a new functions since 3/4 "p"
			** instructions use <fc> (function_code) */
int
function_code(arg1)
struct arg *arg1;
{register int inst2wd=0;		/* the second instruction word */
	switch (arg1->atype) {
		case AIMM:
			inst2wd |= 0x10;
			inst2wd |= (arg1->xp->xvalue & 0x7);
			if ((unsigned)arg1->xp->xvalue > 0x7)
				werror("function code too large");
			break;
		case ADREG:	/* Dn */
			inst2wd |= 0x8;
			inst2wd |= arg1->areg1->opcode;
			break;
		case CTLREG:	/* sfc or dfc */
			if (arg1->areg1->opcode > 1)
				yyerror("first operand must be %sfc, %dfc, %dn of immediate");
			inst2wd |= arg1->areg1->opcode;
			break;
		default:
			yyerror("first operand of wrong type");
	}
	return(inst2wd);
}
#endif		/* M68030 */
#ifdef M68040
int
set_cache_value( c )
	char c;
{
	switch( c )
	{
		case 'n':
		case 'N':
			return( 0 );
		case 'd':
		case 'D':
			return( 1 );
		case 'i':
		case 'I':
			return( 2 );
		case 'b':
		case 'B':
			return( 3 );
		default:
			return( -1 );
	}
}
#endif /* M68040 */
