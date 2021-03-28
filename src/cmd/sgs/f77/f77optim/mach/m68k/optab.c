/*	MOT:optab.head	7.1		*/

#ident	"@(#)f77/f77optim/mach:optab.head	25.1"

/* optab.head */
/* MOT: optab.head:	2.1 83/07/08"*/

/*	MC68000 Optimizer	*/

/* This file contains the base data for the op code table.  It
** also contains the op code table lookup routine.
*/

#include "ops.h"
#include "debug.h"

void oerr();
void serr();
/* findop -- find op code
**
** This routine finds the op code table entry, given an operator string.
** The table is ordered lexically, and we use a binary search.  All real
** and pseudo-ops that appear in the input must appear in this table.
**
** One wrinkle in the test algorithm is that, on a match, we must be sure
** the table entry ends where the argument string ends.  If not, the argument
** is less than the table entry.
*/

extern OP optab[];

OP *
findop(ptr,len)
char * ptr;
int len;
{
    int strncmp();
    register int low = 0;
    register int high = OP_MAX-OP_MIN;	/* highest numbered slot in table */
    register int cur;			/* current slot number */
    register int result;		/* result of string comparison */

    /* binary search loop */

	TRACE(findop);
    while ( (cur=(high+low)/2), low <= high )
    {
	result = strncmp( ptr, optab[cur].op_name, len );
	if (result < 0)
	    high = cur-1;
	else if (result > 0)
	    low = cur+1;
	else				/* match, maybe */
	{
	    if (optab[cur].op_name[len] == '\0')
		return( &optab[cur] );	/* found:  return pointer to
					** current slot
					*/
	    high = cur-1;		/* otherwise, treat as "low" */
	}
    }

    /* falling out of loop, the search failed */

    serr("Op not found");		/* couldn't find op code */

/*NOTREACHED*/
}
/* getOP -- get OP pointer, given an op code number
**
** This routine returns a pointer to the unique OP structure for
** an op code number.
*/

OP *
getOP(opnum)
OPCODE opnum;
{
    extern OP oplabel, opswitch;	/* defined below */

	TRACE(getOP);
    if ( OP_MIN <= opnum && opnum <= OP_MAX )
	return( &optab[opnum - OP_MIN] );
    
    if (opnum == LABEL)
	return ( &oplabel );

    if (opnum == SWITCH)
	return ( &opswitch );
    
    oerr("Unknown op code in getOP()");
/*NOTREACHED*/
}
/* Op code table */


/* These operations are defined here because they don't go in the table.
** However, there is still a unique structure for each of them.
*/

static OP oplabel =
    { "",	LABEL,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_VTRACE };

/* The operands for a switch are:  the destination for the original jmp (1)
** and an encoding of the branch table (2).  However, no RAND structure is
** used for the second operand.  Therefore, we show only one operand.
*/
static OP opswitch =
    { "",	SWITCH,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_HBRA | OF_UBRA };

/*	name	code	nsrc	dest	ssize	dsize	oppos	flags	*/

static OP
optab[] =
/*	MOT:optab.base	7.2		*/
{
    { "abcd.b",	ABCD_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CC },
    { "add.b",	ADD_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CCA },
    { "add.l",	ADD_L,	2,	1,	{4,4},	4,	NOOPP,	OF_CCA },
    { "add.w",	ADD_W,	2,	1,	{2,2},	2,	NOOPP,	OF_CCA },
    { "addx.b",	ADDX_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CC },
    { "addx.l",	ADDX_L,	2,	1,	{4,4},	4,	NOOPP,	OF_CC },
    { "addx.w",	ADDX_W,	2,	1,	{2,2},	2,	NOOPP,	OF_CC },
    { "align",	ALIGN,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "and.b",	AND_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CC | OF_VTRACE },
    { "and.l",	AND_L,	2,	1,	{4,4},	4,	NOOPP,	OF_CC | OF_VTRACE },
    { "and.w",	AND_W,	2,	1,	{2,2},	2,	NOOPP,	OF_CC | OF_VTRACE },
    { "asl.b",	ASL_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CC },
    { "asl.l",	ASL_L,	2,	1,	{1,4},	4,	NOOPP,	OF_CC },
    { "asl.w",	ASL_W,	2,	1,	{1,2},	2,	NOOPP,	OF_CC },
    { "asr.b",	ASR_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CC },
    { "asr.l",	ASR_L,	2,	1,	{1,4},	4,	NOOPP,	OF_CC },
    { "asr.w",	ASR_W,	2,	1,	{1,2},	2,	NOOPP,	OF_CC },
    { "bcc",	BCC,	1,	NODEST,	{0,0},	0,	BCS,	OF_CBRA | OF_RCBRA },
    { "bcc.b",	BCC_B,	1,	NODEST,	{0,0},	0,	BCS_B,	OF_CBRA | OF_RCBRA },
    { "bchg",	BCHG,	2,	1,	{1,1},	1,	NOOPP,	OF_CC },
    { "bclr",	BCLR,	2,	1,	{1,1},	1,	NOOPP,	OF_CC },
    { "bcs",	BCS,	1,	NODEST,	{0,0},	0,	BCC,	OF_CBRA | OF_RCBRA },
    { "bcs.b",	BCS_B,	1,	NODEST,	{0,0},	0,	BCC_B,	OF_CBRA | OF_RCBRA },
    { "beq",	BEQ,	1,	NODEST,	{0,0},	0,	BNE,	OF_CBRA | OF_RCBRA },
    { "beq.b",	BEQ_B,	1,	NODEST,	{0,0},	0,	BNE_B,	OF_CBRA | OF_RCBRA },
    { "bfchg",	BFCHG,	1,	0,	{0,0},	0,	NOOPP,	OF_CC | OF_LD | OF_RT4  },
    { "bfclr",	BFCLR,	1,	0,	{0,0},	0,	NOOPP,	OF_CC | OF_LD | OF_RT4  },
    { "bfexts",	BFEXTS,	1,	1 ,	{0,0},	0,	NOOPP,	OF_CC | OF_LD | OF_RT4  },
    { "bfextu",	BFEXTU,	1,	1 ,	{0,0},	0,	NOOPP,	OF_CC | OF_LD | OF_RT4  },
    { "bfffo",	BFFFO,	1,	1 ,	{0,0},	0,	NOOPP,	OF_CC | OF_LD | OF_RT4  },
    { "bfins",	BFINS,	2,	1 ,	{0,0},	0,	NOOPP,	OF_CC | OF_LD | OF_RT4  },
    { "bfset",	BFSET,	1,	0 ,	{0,0},	0,	NOOPP,	OF_CC | OF_LD | OF_RT4  },
    { "bftst",	BFTST,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_CC | OF_LD | OF_RT4  },
    { "bge",	BGE,	1,	NODEST,	{0,0},	0,	BLT,	OF_CBRA | OF_RCBRA },
    { "bge.b",	BGE_B,	1,	NODEST,	{0,0},	0,	BLT_B,	OF_CBRA | OF_RCBRA },
    { "bgt",	BGT,	1,	NODEST,	{0,0},	0,	BLE,	OF_CBRA | OF_RCBRA },
    { "bgt.b",	BGT_B,	1,	NODEST,	{0,0},	0,	BLE_B,	OF_CBRA | OF_RCBRA },
    { "bhi",	BHI,	1,	NODEST,	{0,0},	0,	BLS,	OF_CBRA | OF_RCBRA },
    { "bhi.b",	BHI_B,	1,	NODEST,	{0,0},	0,	BLS_B,	OF_CBRA | OF_RCBRA },
    { "bhs",	BHS,	1,	NODEST,	{0,0},	0,	BLO,	OF_CBRA | OF_RCBRA },
    { "bhs.b",	BHS_B,	1,	NODEST,	{0,0},	0,	BLO_B,	OF_CBRA | OF_RCBRA },
    { "ble",	BLE,	1,	NODEST,	{0,0},	0,	BGT,	OF_CBRA | OF_RCBRA },
    { "ble.b",	BLE_B,	1,	NODEST,	{0,0},	0,	BGT_B,	OF_CBRA | OF_RCBRA },
    { "blo",	BLO,	1,	NODEST,	{0,0},	0,	BHS,	OF_CBRA | OF_RCBRA },
    { "blo.b",	BLO_B,	1,	NODEST,	{0,0},	0,	BHS_B,	OF_CBRA | OF_RCBRA },
    { "bls",	BLS,	1,	NODEST,	{0,0},	0,	BHI,	OF_CBRA | OF_RCBRA },
    { "bls.b",	BLS_B,	1,	NODEST,	{0,0},	0,	BHI_B,	OF_CBRA | OF_RCBRA },
    { "blt",	BLT,	1,	NODEST,	{0,0},	0,	BGE,	OF_CBRA | OF_RCBRA },
    { "blt.b",	BLT_B,	1,	NODEST,	{0,0},	0,	BGE_B,	OF_CBRA | OF_RCBRA },
    { "bmi",	BMI,	1,	NODEST,	{0,0},	0,	BPL,	OF_CBRA | OF_RCBRA },
    { "bmi.b",	BMI_B,	1,	NODEST,	{0,0},	0,	BPL_B,	OF_CBRA | OF_RCBRA },
    { "bne",	BNE,	1,	NODEST,	{0,0},	0,	BEQ,	OF_CBRA | OF_RCBRA },
    { "bne.b",	BNE_B,	1,	NODEST,	{0,0},	0,	BEQ_B,	OF_CBRA | OF_RCBRA },
    { "bpl",	BPL,	1,	NODEST,	{0,0},	0,	BMI,	OF_CBRA | OF_RCBRA },
    { "bpl.b",	BPL_B,	1,	NODEST,	{0,0},	0,	BMI_B,	OF_CBRA | OF_RCBRA },
    { "br",	BR,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_UBRA },
    { "br.b",	BR_B,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_UBRA },
    { "bra",	BRA,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_UBRA },
    { "bra.b",	BRA_B,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_UBRA },
    { "bset",	BSET,	2,	1,	{1,1},	1,	NOOPP,	OF_CC },
    { "bsr",	BSR,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_LD | OF_VTRACE },
    { "bsr.b",	BSR_B,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_LD | OF_VTRACE },
    { "btst",	BTST,	2,	NODEST,	{1,1},	0,	NOOPP,	OF_CC },
    { "bvc",	BVC,	1,	NODEST,	{0,0},	0,	BVS,	OF_CBRA | OF_RCBRA },
    { "bvc.b",	BVC_B,	1,	NODEST,	{0,0},	0,	BVS_B,	OF_CBRA | OF_RCBRA },
    { "bvs",	BVS,	1,	NODEST,	{0,0},	0,	BVC,	OF_CBRA | OF_RCBRA },
    { "bvs.b",	BVS_B,	1,	NODEST,	{0,0},	0,	BVC_B,	OF_CBRA | OF_RCBRA },
    { "byte",	BYTE,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "chk.w",	CHK_W,	2,	NODEST,	{2,2},	0,	NOOPP,	OF_CC | OF_HBRA | OF_NCBRA },
    { "clr.b",	CLR_B,	0,	0,	{0,0},	1,	NOOPP,	OF_CC | OF_LD },
    { "clr.l",	CLR_L,	0,	0,	{0,0},	4,	NOOPP,	OF_CC },
    { "clr.w",	CLR_W,	0,	0,	{0,0},	2,	NOOPP,	OF_CC | OF_LD },
    { "cmp.b",	CMP_B,	2,	NODEST,	{1,1},	0,	NOOPP,	OF_CC | OF_VTRACE },
    { "cmp.l",	CMP_L,	2,	NODEST,	{4,4},	0,	NOOPP,	OF_CC | OF_VTRACE },
    { "cmp.w",	CMP_W,	2,	NODEST,	{2,2},	0,	NOOPP,	OF_CC | OF_VTRACE },
    { "comm",	COMM,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "data",	DATA,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "dbcc",	DBCC,	2,	0,	{2,0},	2,	DBCS,	OF_CBRA | OF_HBRA },
    { "dbcs",	DBCS,	2,	0,	{2,0},	2,	DBCC,	OF_CBRA | OF_HBRA },
    { "dbeq",	DBEQ,	2,	0,	{2,0},	2,	DBNE,	OF_CBRA | OF_HBRA },
    { "dbf",	DBF,	2,	0,	{2,0},	2,	DBT,	OF_CBRA | OF_HBRA },
    { "dbge",	DBGE,	2,	0,	{2,0},	2,	DBLT,	OF_CBRA | OF_HBRA },
    { "dbgt",	DBGT,	2,	0,	{2,0},	2,	DBLE,	OF_CBRA | OF_HBRA },
    { "dbhi",	DBHI,	2,	0,	{2,0},	2,	DBLS,	OF_CBRA | OF_HBRA },
    { "dbhs",	DBHS,	2,	0,	{2,0},	2,	DBLO,	OF_CBRA | OF_HBRA },
    { "dble",	DBLE,	2,	0,	{2,0},	2,	DBGT,	OF_CBRA | OF_HBRA },
    { "dblo",	DBLO,	2,	0,	{2,0},	2,	DBHS,	OF_CBRA | OF_HBRA },
    { "dbls",	DBLS,	2,	0,	{2,0},	2,	DBHI,	OF_CBRA | OF_HBRA },
    { "dblt",	DBLT,	2,	0,	{2,0},	2,	DBGE,	OF_CBRA | OF_HBRA },
    { "dbmi",	DBMI,	2,	0,	{2,0},	2,	DBPL,	OF_CBRA | OF_HBRA },
    { "dbne",	DBNE,	2,	0,	{2,0},	2,	DBEQ,	OF_CBRA | OF_HBRA },
    { "dbpl",	DBPL,	2,	0,	{2,0},	2,	DBMI,	OF_CBRA | OF_HBRA },
    { "dbr",	DBR,	2,	0,	{2,0},	2,	NOOPP,	OF_CBRA | OF_HBRA },
    { "dbra",	DBRA,	2,	0,	{2,0},	2,	NOOPP,	OF_CBRA | OF_HBRA },
    { "dbt",	DBT,	2,	0,	{2,0},	2,	DBF,	OF_CBRA | OF_HBRA },
    { "dbvc",	DBVC,	2,	0,	{2,0},	2,	DBVS,	OF_CBRA | OF_HBRA },
    { "dbvs",	DBVS,	2,	0,	{2,0},	2,	DBVC,	OF_CBRA | OF_HBRA },
    { "def",	DEF,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "divs.l",	DIVS_L,	2,	1,	{4,4},	8,	NOOPP,	OF_CC },
    { "divs.w",	DIVS_W,	2,	1,	{2,4},	4,	NOOPP,	OF_CC },
    { "divu.l",	DIVU_L,	2,	1,	{4,4},	8,	NOOPP,	OF_CC },
    { "divu.w",	DIVU_W,	2,	1,	{2,4},	4,	NOOPP,	OF_CC },
    { "eor.b",	EOR_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CC | OF_VTRACE },
    { "eor.l",	EOR_L,	2,	1,	{4,4},	4,	NOOPP,	OF_CC | OF_VTRACE },
    { "eor.w",	EOR_W,	2,	1,	{2,2},	2,	NOOPP,	OF_CC | OF_VTRACE },
    { "even",	EVEN,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "exg",	EXG,	2,	1,	{4,4},	4,	NOOPP,	OF_LD | OF_VTRACE },
    { "ext.l",	EXT_L,	1,	0,	{2,0},	4,	NOOPP,	OF_CC },
    { "ext.w",	EXT_W,	1,	0,	{1,0},	2,	NOOPP,	OF_CC },
    { "extb.l",	EXTB_L,	1,	0,	{1,0},	4,	NOOPP,	OF_CC },
    { "fabs.b",	FABS_B,	2,	1,	{1,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fabs.d",	FABS_D,	2,	1,	{8,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fabs.l",	FABS_L,	2,	1,	{4,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fabs.s",	FABS_S,	2,	1,	{4,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fabs.w",	FABS_W,	2,	1,	{2,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fabs.x",	FABS_X,	2,	1,	{12,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fadd.b",	FADD_B,	2,	1,	{1,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fadd.d",	FADD_D,	2,	1,	{8,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fadd.l",	FADD_L,	2,	1,	{4,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fadd.s",	FADD_S,	2,	1,	{4,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fadd.w",	FADD_W,	2,	1,	{2,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fadd.x",	FADD_X,	2,	1,	{12,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fbeq",	FBEQ,	1,	NODEST,	{0,0},	0,	FBNEQ,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbf",	FBF ,	1,	NODEST,	{0,0},	0,	FBT  ,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbge",	FBGE,	1,	NODEST,	{0,0},	0,	FBNGE,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbgl",	FBGL,	1,	NODEST,	{0,0},	0,	FBNGL,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbgle",	FBGLE,	1,	NODEST,	{0,0},	0,	FBNGLE,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbgt",	FBGT,	1,	NODEST,	{0,0},	0,	FBNGT,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fble",	FBLE,	1,	NODEST,	{0,0},	0,	FBNLE,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fblt",	FBLT,	1,	NODEST,	{0,0},	0,	FBNLT,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbneq",	FBNEQ,	1,	NODEST,	{0,0},	0,	FBEQ,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbnge",	FBNGE,	1,	NODEST,	{0,0},	0,	FBGE,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbngl",	FBNGL,	1,	NODEST,	{0,0},	0,	FBGL,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbngle",	FBNGLE,	1,	NODEST,	{0,0},	0,	FBGLE,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbngt",	FBNGT,	1,	NODEST,	{0,0},	0,	FBGT,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbnle",	FBNLE,	1,	NODEST,	{0,0},	0,	FBLE,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbnlt",	FBNLT,	1,	NODEST,	{0,0},	0,	FBLT,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fboge",	FBOGE,	1,	NODEST,	{0,0},	0,	FBULT,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbogl",	FBOGL,	1,	NODEST,	{0,0},	0,	FBUEQ,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbogt",	FBOGT,	1,	NODEST,	{0,0},	0,	FBULE,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbole",	FBOLE,	1,	NODEST,	{0,0},	0,	FBUGT,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbolt",	FBOLT,	1,	NODEST,	{0,0},	0,	FBUGE,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbor",	FBOR,	1,	NODEST,	{0,0},	0,	FBUN,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbseq",	FBSEQ,	1,	NODEST,	{0,0},	0,	FBSNEQ,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbsf",	FBSF,	1,	NODEST,	{0,0},	0,	FBST,	OF_CBRA | OF_RCBRA | OF_RT4   },
    { "fbsneq",	FBSNEQ,	1,	NODEST,	{0,0},	0,	FBSEQ,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbst",	FBST,	1,	NODEST,	{0,0},	0,	FBSF,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbt",	FBT,	1,	NODEST,	{0,0},	0,	FBF,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbueq",	FBUEQ,	1,	NODEST,	{0,0},	0,	FBOGL,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbuge",	FBUGE,	1,	NODEST,	{0,0},	0,	FBOLT,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbugt",	FBUGT ,	1,	NODEST,	{0,0},	0,	FBOLE,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbule",	FBULE,	1,	NODEST,	{0,0},	0,	FBOGT,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbult",	FBULT,	1,	NODEST,	{0,0},	0,	FBOGE,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fbun",	FBUN,	1,	NODEST,	{0,0},	0,	FBOR,	OF_CBRA | OF_RCBRA | OF_RT4  },
    { "fcmp.b",	FCMP_B,	2,	1,	{1,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fcmp.d",	FCMP_D,	2,	1,	{8,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fcmp.l",	FCMP_L,	2,	1,	{4,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fcmp.s",	FCMP_S,	2,	1,	{4,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fcmp.w",	FCMP_W,	2,	1,	{2,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fcmp.x",	FCMP_X,	2,	1,	{12,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fdiv.b",	FDIV_B,	2,	1,	{1,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fdiv.d",	FDIV_D,	2,	1,	{8,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fdiv.l",	FDIV_L,	2,	1,	{4,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fdiv.s",	FDIV_S,	2,	1,	{4,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fdiv.w",	FDIV_W,	2,	1,	{2,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fdiv.x",	FDIV_X,	2,	1,	{12,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "file",	FILE_,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "fintrz.d",	FINT_D,	1,	1,	{8,12},	8,	NOOPP,	OF_CC | OF_RT4  },
    { "fintrz.s",	FINT_S,	1,	1,	{4,12},	4,	NOOPP,	OF_CC | OF_RT4   },
    { "fintrz.x",	FINT_X,	1,	1,	{12,12},	2,	NOOPP,	OF_CC | OF_RT4   },
    { "fmov.b",	FMOV_B,	1,	1,	{1,12},	1,	NOOPP,	OF_CC | OF_RT4   },
    { "fmov.d",	FMOV_D,	1,	1,	{8,12},	8,	NOOPP,	OF_CC | OF_RT4  },
    { "fmov.l",	FMOV_L,	1,	1,	{4,12},	4,	NOOPP,	OF_CC | OF_RT4   },
    { "fmov.s",	FMOV_S,	1,	1,	{4,12},	4,	NOOPP,	OF_CC | OF_RT4   },
    { "fmov.w",	FMOV_W,	1,	1,	{2,12},	2,	NOOPP,	OF_CC | OF_RT4   },
    { "fmov.x",	FMOV_X,	1,	1,	{12,12},	2,	NOOPP,	OF_CC | OF_RT4   },
    { "fmovm",	FMOVM,	1,	1,	{4,0},	4,	NOOPP,	OF_LD | OF_VTRACE | OF_RT4 },
    { "fmul.b",	FMUL_B,	2,	1,	{1,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fmul.d",	FMUL_D,	2,	1,	{8,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fmul.l",	FMUL_L,	2,	1,	{4,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fmul.s",	FMUL_S,	2,	1,	{4,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fmul.w",	FMUL_W,	2,	1,	{2,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fmul.x",	FMUL_X,	2,	1,	{12,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fneg.b",	FNEG_B,	2,	1,	{1,12},	12,	NOOPP,	OF_CC | OF_RT4  },
    { "fneg.d",	FNEG_D,	2,	1,	{8,12},	12,	NOOPP,	OF_CC | OF_RT4  },
    { "fneg.l",	FNEG_L,	2,	1,	{4,12},	12,	NOOPP,	OF_CC | OF_RT4  },
    { "fneg.s",	FNEG_S,	2,	1,	{4,12},	12,	NOOPP,	OF_CC | OF_RT4  },
    { "fneg.w",	FNEG_W,	2,	1,	{2,12},	12,	NOOPP,	OF_CC | OF_RT4  },
    { "fneg.x",	FNEG_X,	2,	1,	{12,12},	12,	NOOPP,	OF_CC | OF_RT4  },
    { "frestore",	FRESTORE,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_CC | OF_RT4  },
    { "fsave",	FSAVE,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_CC | OF_RT4  },
    { "fsub.b",	FSUB_B,	2,	1,	{1,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fsub.d",	FSUB_D,	2,	1,	{8,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fsub.l",	FSUB_L,	2,	1,	{4,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fsub.s",	FSUB_S,	2,	1,	{4,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fsub.w",	FSUB_W,	2,	1,	{2,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "fsub.x",	FSUB_X,	2,	1,	{12,12},	12,	NOOPP,	OF_CC | OF_RT4 },
    { "ftest.d",	FTEST_D,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_CC | OF_RT4  },
    { "ftest.s",	FTEST_S,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_CC | OF_RT4  },
    { "global",	GLOBAL,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "jmp",	JMP,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_UBRA },
    { "jsr",	JSR,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_LD | OF_VTRACE },
    { "lcomm",	LCOMM,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "lea.l",	LEA_L,	1,	1,	{4,0},	4,	NOOPP,	OF_VTRACE },
    { "link",	LINK,	2,	0,	{4,2},	4,	NOOPP,	OF_MODSP },
    { "link.l",	LINK_L,	2,	0,	{4,4},	4,	NOOPP,	OF_MODSP },
    { "link.w",	LINK_W,	2,	0,	{4,2},	4,	NOOPP,	OF_MODSP },
    { "ln",	LN,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "long",	LONG,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "lsl.b",	LSL_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CC },
    { "lsl.l",	LSL_L,	2,	1,	{1,4},	4,	NOOPP,	OF_CC },
    { "lsl.w",	LSL_W,	2,	1,	{1,2},	2,	NOOPP,	OF_CC },
    { "lsr.b",	LSR_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CC },
    { "lsr.l",	LSR_L,	2,	1,	{1,4},	4,	NOOPP,	OF_CC },
    { "lsr.w",	LSR_W,	2,	1,	{1,2},	2,	NOOPP,	OF_CC },
    { "mov.b",	MOV_B,	1,	1,	{1,0},	1,	NOOPP,	OF_VTRACE | OF_CCA },	/* watch CC on %sr, %usp */
    { "mov.l",	MOV_L,	1,	1,	{4,0},	4,	NOOPP,	OF_VTRACE | OF_CCA },	/* watch CC on %sr, %usp */
    { "mov.w",	MOV_W,	1,	1,	{2,0},	2,	NOOPP,	OF_VTRACE | OF_CCA },	/* watch CC on %sr, %usp */
    { "movm.l",	MOVM_L,	1,	1,	{4,0},	4,	NOOPP,	OF_LD | OF_VTRACE },
    { "movm.w",	MOVM_W,	1,	1,	{2,0},	2,	NOOPP,	OF_LD | OF_VTRACE },	/* dest size is 4 if to regs */
    { "movs.b",	MOVS_B,	1,	1,	{1,0},	1,	NOOPP,	OF_VTRACE | OF_CCA },	/* watch CC on %sr, %usp */
    { "movs.l",	MOVS_L,	1,	1,	{4,0},	4,	NOOPP,	OF_VTRACE | OF_CCA },	/* watch CC on %sr, %usp */
    { "movs.w",	MOVS_W,	1,	1,	{2,0},	2,	NOOPP,	OF_VTRACE | OF_CCA },	/* watch CC on %sr, %usp */
    { "movp.l",	MOVP_L,	1,	1,	{4,0},	4,	NOOPP,	OF_VTRACE },
    { "movp.w",	MOVP_W,	1,	1,	{2,0},	4,	NOOPP,	OF_VTRACE },
    { "muls.l",	MULS_L,	2,	1,	{4,4},	8,	NOOPP,	OF_CC |OF_RT4 },
    { "muls.w",	MULS_W,	2,	1,	{2,2},	4,	NOOPP,	OF_CC },
    { "mulu.l",	MULU_L,	2,	1,	{4,4},	8,	NOOPP,	OF_CC },
    { "mulu.w",	MULU_W,	2,	1,	{2,2},	4,	NOOPP,	OF_CC },
    { "nbcd.b",	NBCD_B,	1,	0,	{1,0},	1,	NOOPP,	OF_CC },
    { "neg.b",	NEG_B,	1,	0,	{1,0},	1,	NOOPP,	OF_CC },
    { "neg.l",	NEG_L,	1,	0,	{4,0},	4,	NOOPP,	OF_CC },
    { "neg.w",	NEG_W,	1,	0,	{2,0},	2,	NOOPP,	OF_CC },
    { "negx.b",	NEGX_B,	1,	0,	{1,0},	1,	NOOPP,	OF_CC },
    { "negx.l",	NEGX_L,	1,	0,	{4,0},	4,	NOOPP,	OF_CC },
    { "negx.w",	NEGX_W,	1,	0,	{2,0},	2,	NOOPP,	OF_CC },
    { "nop",	NOP,	0,	NODEST,	{0,0},	0,	NOOPP,	0 },
    { "not.b",	NOT_B,	1,	0,	{1,0},	1,	NOOPP,	OF_CC },
    { "not.l",	NOT_L,	1,	0,	{4,0},	4,	NOOPP,	OF_CC },
    { "not.w",	NOT_W,	1,	0,	{2,0},	2,	NOOPP,	OF_CC },
    { "or.b",	OR_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CC| OF_VTRACE },
    { "or.l",	OR_L,	2,	1,	{4,4},	4,	NOOPP,	OF_CC | OF_VTRACE },
    { "or.w",	OR_W,	2,	1,	{2,2},	2,	NOOPP,	OF_CC| OF_VTRACE },
    { "org",	ORG,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "pea.l",	PEA_L,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_MODSP | OF_VTRACE },
    { "reset",	RESET,	0,	NODEST,	{0,0},	0,	NOOPP,	0 },
    { "rol.b",	ROL_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CC },
    { "rol.l",	ROL_L,	2,	1,	{1,4},	4,	NOOPP,	OF_CC },
    { "rol.w",	ROL_W,	2,	1,	{1,2},	2,	NOOPP,	OF_CC },
    { "ror.b",	ROR_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CC },
    { "ror.l",	ROR_L,	2,	1,	{1,4},	4,	NOOPP,	OF_CC },
    { "ror.w",	ROR_W,	2,	1,	{1,2},	2,	NOOPP,	OF_CC },
    { "roxl.b",	ROXL_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CC },
    { "roxl.l",	ROXL_L,	2,	1,	{1,4},	4,	NOOPP,	OF_CC },
    { "roxl.w",	ROXL_W,	2,	1,	{1,2},	2,	NOOPP,	OF_CC },
    { "roxr.b",	ROXR_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CC },
    { "roxr.l",	ROXR_L,	2,	1,	{1,4},	4,	NOOPP,	OF_CC },
    { "roxr.w",	ROXR_W,	2,	1,	{1,2},	2,	NOOPP,	OF_CC },
    { "rte",	RTE,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_UBRA | OF_HBRA },
    { "rtr",	RTR,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_UBRA | OF_HBRA },
    { "rts",	RTS,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_UBRA | OF_HBRA | OF_LD },
    { "sbcd.b",	SBCD_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CC },
    { "scc.b",	SCC_B,	0,	NODEST,	{0,0},	0,	SCS_B,	OF_CC },
    { "scs.b",	SCS_B,	0,	NODEST,	{0,0},	0,	SCC_B,	OF_CC },
    { "seq.b",	SEQ_B,	0,	NODEST,	{0,0},	0,	SNE_B,	OF_CC },
    { "set",	SET,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "sf.b",	SF_B,	0,	NODEST,	{0,0},	0,	ST_B,	OF_CC },
    { "sge.b",	SGE_B,	0,	NODEST,	{0,0},	0,	SLT_B,	OF_CC },
    { "sgt.b",	SGT_B,	0,	NODEST,	{0,0},	0,	SLE_B,	OF_CC },
    { "shi.b",	SHI_B,	0,	NODEST,	{0,0},	0,	SLS_B,	OF_CC },
    { "short",	SHORT,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "shs.b",	SHS_B,	0,	NODEST,	{0,0},	0,	SLO_B,	OF_CC },
    { "sle.b",	SLE_B,	0,	NODEST,	{0,0},	0,	SGT_B,	OF_CC },
    { "slo.b",	SLO_B,	0,	NODEST,	{0,0},	0,	SHS_B,	OF_CC },
    { "sls.b",	SLS_B,	0,	NODEST,	{0,0},	0,	SHI_B,	OF_CC },
    { "slt.b",	SLT_B,	0,	NODEST,	{0,0},	0,	SGE_B,	OF_CC },
    { "smi.b",	SMI_B,	0,	NODEST,	{0,0},	0,	SPL_B,	OF_CC },
    { "sne.b",	SNE_B,	0,	NODEST,	{0,0},	0,	SEQ_B,	OF_CC },
    { "space",	SPACE,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "spl.b",	SPL_B,	0,	NODEST,	{0,0},	0,	SMI_B,	OF_CC },
    { "st.b",	ST_B,	0,	NODEST,	{0,0},	0,	SF_B,	OF_CC },
    { "stop",	STOP,	0,	NODEST,	{0,0},	0,	NOOPP,	0 },
    { "sub.b",	SUB_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CCA },
    { "sub.l",	SUB_L,	2,	1,	{4,4},	4,	NOOPP,	OF_CCA },
    { "sub.w",	SUB_W,	2,	1,	{2,2},	2,	NOOPP,	OF_CCA },
    { "subx.b",	SUBX_B,	2,	1,	{1,1},	1,	NOOPP,	OF_CC },
    { "subx.l",	SUBX_L,	2,	1,	{4,4},	4,	NOOPP,	OF_CC },
    { "subx.w",	SUBX_W,	2,	1,	{2,2},	2,	NOOPP,	OF_CC },
    { "svc.b",	SVC_B,	0,	NODEST,	{0,0},	0,	SVS_B,	OF_CC },
    { "svs.b",	SVS_B,	0,	NODEST,	{0,0},	0,	SVC_B,	OF_CC },
    { "swap.w",	SWAP_W,	1,	0,	{4,0},	4,	NOOPP,	OF_CC },
    { "swbeg",	SWBEG,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "tas.b",	TAS_B,	1,	0,	{1,0},	1,	NOOPP,	OF_CC },
    { "tdivs.l",	TDIVS_L,	2,	1,	{4,4},	4,	NOOPP,	OF_CC },
    { "tdivu.l",	TDIVU_L,	2,	1,	{4,4},	4,	NOOPP,	OF_CC },
    { "text",	TEXT,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_PSEUDO },
    { "tmuls.l",	TMULS_L,	2,	1,	{4,4},	4,	NOOPP,	OF_CC },
    { "tmulu.l",	TMULU_L,	2,	1,	{4,4},	4,	NOOPP,	OF_CC },
    { "trap",	TRAP,	1,	NODEST,	{0,0},	0,	NOOPP,	OF_HBRA },
    { "trapv",	TRAPV,	0,	NODEST,	{0,0},	0,	NOOPP,	OF_NCBRA },
    { "tst.b",	TST_B,	1,	NODEST,	{1,0},	0,	NOOPP,	OF_CC | OF_VTRACE },
    { "tst.l",	TST_L,	1,	NODEST,	{4,0},	0,	NOOPP,	OF_CC | OF_VTRACE },
    { "tst.w",	TST_W,	1,	NODEST,	{2,0},	0,	NOOPP,	OF_CC | OF_VTRACE },
    { "unlk",	UNLK,	1,	0,	{4,0},	4,	NOOPP,	OF_MODSP }
};
