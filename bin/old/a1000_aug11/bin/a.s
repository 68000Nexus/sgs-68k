	file	"a.c"
	data	1
	text
	global	main
main:
	tst.b	F%1-256(%sp)	#local2.c
	link.l	%fp,&F%1
	movm.l	&M%1,(4,%sp)
	fmovm	&FPM%1,(FPO%1,%sp)
	data	2
	align	4
L%14:
	byte	'h,'e,'l,'l,'o,0x00
	text
	set	L%14L,6
	mov.l	&L%14,-(%sp)
	jsr	printf
	add.w	&4,%sp
L%12:
	fmovm	(FPO%1,%sp),&FPM%1
	movm.l	(4,%sp),&M%1
	unlk	%fp
	rts
	set	S%1,0
	set	T%1,0
	set	F%1,-4
	set	FPO%1,4
	set	FPM%1,0x0000
	set	M%1,0x0000
	data	1
