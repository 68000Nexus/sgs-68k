/*			*/
/*	ops.head	7.1		*/
/*			*/
/*	ops.head:	4.1 84/06/27	*/

/* MC68000 Optimizer */

#ident	"@(#)f77/f77optim/mach:ops.head	25.1"

/* Definitions of opcode related things. */


#define	MAXOPS	2		/* Two operands per instruction, max. */
typedef	unsigned short OPCODE;	/* Object holding opcode number */

typedef unsigned short FLAGS;	/* holds operation flag bits */

/* This structure contains the invariant information for each instruction.
** For transfers of control (jumps, branches, subroutine calls, etc.), the
** highest numbered operand is assumed to contain the control target.
*/

typedef struct op
{
    char * op_name;		/* pointer to instruction print-name */
    OPCODE op_code;		/* operation code number for instruction */
    unsigned char op_nsrc;	/* number of source operands.  Source operands
				** are those for which the processor does a
				** read cycle.  For example, add.w has two
				** source operands, while mov.w has just one.
				** Source operands are assumed to be consec-
				** utive, starting with the first operand.
				*/
    unsigned char op_dnum;	/* Destination operand number.  If the op has
				** no destination, value is NODEST.  A desti-
				** nation operand is one whose value is changed.
				** In add.l, the second operand is both a source
				** AND a destination operand, whereas the
				** second operand of mov.l is just a dest.
				*/
    unsigned char op_ssize[MAXOPS];
				/* size of each source operand, addressed as
				** 0 - MAXOPS-1, in bytes.
				*/
    unsigned char op_dsize;	/* size of destination operand, in bytes */
    OPCODE op_opposite;		/* op code of "opposite" instruction, in
				** some sense.  For example, for conditional
				** jumps, this is the "reversed" branch.
				*/
    FLAGS op_flags;		/* various flag bits for inst. -- see below */
} OP;


#define	NODEST	0xFF		/* value meaning no destination */
#define	NOOPP	'\0'		/* value meaning no opposite instruction */

/* Define flags for op_flags field.  These are bit flags. */

#define	OF_LAB		0x001	/* op is normal label */
#define	OF_UBRA		0x002	/* op is unconditional branch */
#define	OF_HLAB		0x004	/* op is hard label */
#define	OF_HBRA		0x008	/* op is hard branch */
#define	OF_CBRA		0x010	/* op is conditional branch */
#define	OF_NCBRA	0x020	/* op is conditional branch with null
				** destination (e.g., trapv)
				*/
#define	OF_RCBRA	0x040	/* op is reversible conditional branch */
#define	OF_CC		0x080	/* op affects condition codes */
#define	OF_CCA		0x100	/* op affects condition codes, except if
				** destination is address register
				*/
#define	OF_MODSP	0x200	/* op modifies stack pointer implicitly */
#define	OF_LD		0x400	/* op has funny live/dead characteristics */
#define	OF_VTRACE	0x800	/* op has funny value trace behavior */
#define	OF_NOTAB	0x1000	/* op is not in opcode table */
#define	OF_PSEUDO	0x2000	/* op is a pseudo-op */
#define OF_RT4		0x4000	/* op has a base size of 4 bytes */
/* Define op code numbers.  These numbers form a 1 to 1
** correspondence with the op code names in optab.c .  The order must
** be identical: the ASCII collating sequence for the op names.  The
** numbers must be sequential.
*/

#define OP_MIN		10	/* Min. opcode no. */

#define ABCD_B      	10	/* abcd.b */
#define ADD_B      	11	/* add.b */
#define ADD_L      	12	/* add.l */
#define ADD_W      	13	/* add.w */
#define ADDX_B      	14	/* addx.b */
#define ADDX_L      	15	/* addx.l */
#define ADDX_W      	16	/* addx.w */
#define ALIGN      	17	/* align */
#define AND_B      	18	/* and.b */
#define AND_L      	19	/* and.l */
#define AND_W      	20	/* and.w */
#define ASL_B      	21	/* asl.b */
#define ASL_L      	22	/* asl.l */
#define ASL_W      	23	/* asl.w */
#define ASR_B      	24	/* asr.b */
#define ASR_L      	25	/* asr.l */
#define ASR_W      	26	/* asr.w */
#define BCC      	27	/* bcc */
#define BCC_B      	28	/* bcc.b */
#define BCHG      	29	/* bchg */
#define BCLR      	30	/* bclr */
#define BCS      	31	/* bcs */
#define BCS_B      	32	/* bcs.b */
#define BEQ      	33	/* beq */
#define BEQ_B      	34	/* beq.b */
#define BFCHG      	35	/* bfchg */
#define BFCLR      	36	/* bfclr */
#define BFEXTS      	37	/* bfexts */
#define BFEXTU      	38	/* bfextu */
#define BFFFO      	39	/* bfffo */
#define BFINS      	40	/* bfins */
#define BFSET      	41	/* bfset */
#define BFTST      	42	/* bftst */
#define BGE      	43	/* bge */
#define BGE_B      	44	/* bge.b */
#define BGT      	45	/* bgt */
#define BGT_B      	46	/* bgt.b */
#define BHI      	47	/* bhi */
#define BHI_B      	48	/* bhi.b */
#define BHS      	49	/* bhs */
#define BHS_B      	50	/* bhs.b */
#define BLE      	51	/* ble */
#define BLE_B      	52	/* ble.b */
#define BLO      	53	/* blo */
#define BLO_B      	54	/* blo.b */
#define BLS      	55	/* bls */
#define BLS_B      	56	/* bls.b */
#define BLT      	57	/* blt */
#define BLT_B      	58	/* blt.b */
#define BMI      	59	/* bmi */
#define BMI_B      	60	/* bmi.b */
#define BNE      	61	/* bne */
#define BNE_B      	62	/* bne.b */
#define BPL      	63	/* bpl */
#define BPL_B      	64	/* bpl.b */
#define BR      	65	/* br */
#define BR_B      	66	/* br.b */
#define BRA      	67	/* bra */
#define BRA_B      	68	/* bra.b */
#define BSET      	69	/* bset */
#define BSR      	70	/* bsr */
#define BSR_B      	71	/* bsr.b */
#define BTST      	72	/* btst */
#define BVC      	73	/* bvc */
#define BVC_B      	74	/* bvc.b */
#define BVS      	75	/* bvs */
#define BVS_B      	76	/* bvs.b */
#define BYTE      	77	/* byte */
#define CHK_W      	78	/* chk.w */
#define CLR_B      	79	/* clr.b */
#define CLR_L      	80	/* clr.l */
#define CLR_W      	81	/* clr.w */
#define CMP_B      	82	/* cmp.b */
#define CMP_L      	83	/* cmp.l */
#define CMP_W      	84	/* cmp.w */
#define COMM      	85	/* comm */
#define DATA      	86	/* data */
#define DBCC      	87	/* dbcc */
#define DBCS      	88	/* dbcs */
#define DBEQ      	89	/* dbeq */
#define DBF      	90	/* dbf */
#define DBGE      	91	/* dbge */
#define DBGT      	92	/* dbgt */
#define DBHI      	93	/* dbhi */
#define DBHS      	94	/* dbhs */
#define DBLE      	95	/* dble */
#define DBLO      	96	/* dblo */
#define DBLS      	97	/* dbls */
#define DBLT      	98	/* dblt */
#define DBMI      	99	/* dbmi */
#define DBNE      	100	/* dbne */
#define DBPL      	101	/* dbpl */
#define DBR      	102	/* dbr */
#define DBRA      	103	/* dbra */
#define DBT      	104	/* dbt */
#define DBVC      	105	/* dbvc */
#define DBVS      	106	/* dbvs */
#define DEF      	107	/* def */
#define DIVS_L      	108	/* divs.l */
#define DIVS_W      	109	/* divs.w */
#define DIVU_L      	110	/* divu.l */
#define DIVU_W      	111	/* divu.w */
#define EOR_B      	112	/* eor.b */
#define EOR_L      	113	/* eor.l */
#define EOR_W      	114	/* eor.w */
#define EVEN      	115	/* even */
#define EXG      	116	/* exg */
#define EXT_L      	117	/* ext.l */
#define EXT_W      	118	/* ext.w */
#define EXTB_L      	119	/* extb.l */
#define FABS_B      	120	/* fabs.b */
#define FABS_D      	121	/* fabs.d */
#define FABS_L      	122	/* fabs.l */
#define FABS_S      	123	/* fabs.s */
#define FABS_W      	124	/* fabs.w */
#define FABS_X      	125	/* fabs.x */
#define FADD_B      	126	/* fadd.b */
#define FADD_D      	127	/* fadd.d */
#define FADD_L      	128	/* fadd.l */
#define FADD_S      	129	/* fadd.s */
#define FADD_W      	130	/* fadd.w */
#define FADD_X      	131	/* fadd.x */
#define FBEQ      	132	/* fbeq */
#define FBF       	133	/* fbf */
#define FBGE      	134	/* fbge */
#define FBGL      	135	/* fbgl */
#define FBGLE      	136	/* fbgle */
#define FBGT      	137	/* fbgt */
#define FBLE      	138	/* fble */
#define FBLT      	139	/* fblt */
#define FBNEQ      	140	/* fbneq */
#define FBNGE      	141	/* fbnge */
#define FBNGL      	142	/* fbngl */
#define FBNGLE      	143	/* fbngle */
#define FBNGT      	144	/* fbngt */
#define FBNLE      	145	/* fbnle */
#define FBNLT      	146	/* fbnlt */
#define FBOGE      	147	/* fboge */
#define FBOGL      	148	/* fbogl */
#define FBOGT      	149	/* fbogt */
#define FBOLE      	150	/* fbole */
#define FBOLT      	151	/* fbolt */
#define FBOR      	152	/* fbor */
#define FBSEQ      	153	/* fbseq */
#define FBSF      	154	/* fbsf */
#define FBSNEQ      	155	/* fbsneq */
#define FBST      	156	/* fbst */
#define FBT      	157	/* fbt */
#define FBUEQ      	158	/* fbueq */
#define FBUGE      	159	/* fbuge */
#define FBUGT       	160	/* fbugt */
#define FBULE      	161	/* fbule */
#define FBULT      	162	/* fbult */
#define FBUN      	163	/* fbun */
#define FCMP_B      	164	/* fcmp.b */
#define FCMP_D      	165	/* fcmp.d */
#define FCMP_L      	166	/* fcmp.l */
#define FCMP_S      	167	/* fcmp.s */
#define FCMP_W      	168	/* fcmp.w */
#define FCMP_X      	169	/* fcmp.x */
#define FDIV_B      	170	/* fdiv.b */
#define FDIV_D      	171	/* fdiv.d */
#define FDIV_L      	172	/* fdiv.l */
#define FDIV_S      	173	/* fdiv.s */
#define FDIV_W      	174	/* fdiv.w */
#define FDIV_X      	175	/* fdiv.x */
#define FILE_      	176	/* file */
#define FINT_D      	177	/* fintrz.d */
#define FINT_S      	178	/* fintrz.s */
#define FINT_X      	179	/* fintrz.x */
#define FMOV_B      	180	/* fmov.b */
#define FMOV_D      	181	/* fmov.d */
#define FMOV_L      	182	/* fmov.l */
#define FMOV_S      	183	/* fmov.s */
#define FMOV_W      	184	/* fmov.w */
#define FMOV_X      	185	/* fmov.x */
#define FMOVM      	186	/* fmovm */
#define FMUL_B      	187	/* fmul.b */
#define FMUL_D      	188	/* fmul.d */
#define FMUL_L      	189	/* fmul.l */
#define FMUL_S      	190	/* fmul.s */
#define FMUL_W      	191	/* fmul.w */
#define FMUL_X      	192	/* fmul.x */
#define FNEG_B      	193	/* fneg.b */
#define FNEG_D      	194	/* fneg.d */
#define FNEG_L      	195	/* fneg.l */
#define FNEG_S      	196	/* fneg.s */
#define FNEG_W      	197	/* fneg.w */
#define FNEG_X      	198	/* fneg.x */
#define FRESTORE      	199	/* frestore */
#define FSAVE      	200	/* fsave */
#define FSUB_B      	201	/* fsub.b */
#define FSUB_D      	202	/* fsub.d */
#define FSUB_L      	203	/* fsub.l */
#define FSUB_S      	204	/* fsub.s */
#define FSUB_W      	205	/* fsub.w */
#define FSUB_X      	206	/* fsub.x */
#define FTEST_D      	207	/* ftest.d */
#define FTEST_S      	208	/* ftest.s */
#define GLOBAL      	209	/* global */
#define JMP      	210	/* jmp */
#define JSR      	211	/* jsr */
#define LCOMM      	212	/* lcomm */
#define LEA_L      	213	/* lea.l */
#define LINK      	214	/* link */
#define LINK_L      	215	/* link.l */
#define LINK_W      	216	/* link.w */
#define LN      	217	/* ln */
#define LONG      	218	/* long */
#define LSL_B      	219	/* lsl.b */
#define LSL_L      	220	/* lsl.l */
#define LSL_W      	221	/* lsl.w */
#define LSR_B      	222	/* lsr.b */
#define LSR_L      	223	/* lsr.l */
#define LSR_W      	224	/* lsr.w */
#define MOV_B      	225	/* mov.b */
#define MOV_L      	226	/* mov.l */
#define MOV_W      	227	/* mov.w */
#define MOVM_L      	228	/* movm.l */
#define MOVM_W      	229	/* movm.w */
#define MOVS_B      	230	/* movs.b */
#define MOVS_L      	231	/* movs.l */
#define MOVS_W      	232	/* movs.w */
#define MOVP_L      	233	/* movp.l */
#define MOVP_W      	234	/* movp.w */
#define MULS_L      	235	/* muls.l */
#define MULS_W      	236	/* muls.w */
#define MULU_L      	237	/* mulu.l */
#define MULU_W      	238	/* mulu.w */
#define NBCD_B      	239	/* nbcd.b */
#define NEG_B      	240	/* neg.b */
#define NEG_L      	241	/* neg.l */
#define NEG_W      	242	/* neg.w */
#define NEGX_B      	243	/* negx.b */
#define NEGX_L      	244	/* negx.l */
#define NEGX_W      	245	/* negx.w */
#define NOP      	246	/* nop */
#define NOT_B      	247	/* not.b */
#define NOT_L      	248	/* not.l */
#define NOT_W      	249	/* not.w */
#define OR_B      	250	/* or.b */
#define OR_L      	251	/* or.l */
#define OR_W      	252	/* or.w */
#define ORG      	253	/* org */
#define PEA_L      	254	/* pea.l */
#define RESET      	255	/* reset */
#define ROL_B      	256	/* rol.b */
#define ROL_L      	257	/* rol.l */
#define ROL_W      	258	/* rol.w */
#define ROR_B      	259	/* ror.b */
#define ROR_L      	260	/* ror.l */
#define ROR_W      	261	/* ror.w */
#define ROXL_B      	262	/* roxl.b */
#define ROXL_L      	263	/* roxl.l */
#define ROXL_W      	264	/* roxl.w */
#define ROXR_B      	265	/* roxr.b */
#define ROXR_L      	266	/* roxr.l */
#define ROXR_W      	267	/* roxr.w */
#define RTE      	268	/* rte */
#define RTR      	269	/* rtr */
#define RTS      	270	/* rts */
#define SBCD_B      	271	/* sbcd.b */
#define SCC_B      	272	/* scc.b */
#define SCS_B      	273	/* scs.b */
#define SEQ_B      	274	/* seq.b */
#define SET      	275	/* set */
#define SF_B      	276	/* sf.b */
#define SGE_B      	277	/* sge.b */
#define SGT_B      	278	/* sgt.b */
#define SHI_B      	279	/* shi.b */
#define SHORT      	280	/* short */
#define SHS_B      	281	/* shs.b */
#define SLE_B      	282	/* sle.b */
#define SLO_B      	283	/* slo.b */
#define SLS_B      	284	/* sls.b */
#define SLT_B      	285	/* slt.b */
#define SMI_B      	286	/* smi.b */
#define SNE_B      	287	/* sne.b */
#define SPACE      	288	/* space */
#define SPL_B      	289	/* spl.b */
#define ST_B      	290	/* st.b */
#define STOP      	291	/* stop */
#define SUB_B      	292	/* sub.b */
#define SUB_L      	293	/* sub.l */
#define SUB_W      	294	/* sub.w */
#define SUBX_B      	295	/* subx.b */
#define SUBX_L      	296	/* subx.l */
#define SUBX_W      	297	/* subx.w */
#define SVC_B      	298	/* svc.b */
#define SVS_B      	299	/* svs.b */
#define SWAP_W      	300	/* swap.w */
#define SWBEG      	301	/* swbeg */
#define TAS_B      	302	/* tas.b */
#define TDIVS_L      	303	/* tdivs.l */
#define TDIVU_L      	304	/* tdivu.l */
#define TEXT      	305	/* text */
#define TMULS_L      	306	/* tmuls.l */
#define TMULU_L      	307	/* tmulu.l */
#define TRAP      	308	/* trap */
#define TRAPV      	309	/* trapv */
#define TST_B      	310	/* tst.b */
#define TST_L      	311	/* tst.l */
#define TST_W      	312	/* tst.w */
#define UNLK      	313	/* unlk */

#define OP_MAX		313	/* Max. opcode no. in the 'op' table */

/* The following two directives need opcode numbers,    */
/* but they do not lie in the normal opcode table 'op'. */

#define LABEL		314	/* Label directive opcode num. */

#define SWITCH		315	/* Switch directive opcode num. */

