/*	dumprestor.h	1.1	*/
/*	3.0 SID #	1.1	*/


#ifndef DUMPRESTOR_H
#ident	"@(#)dumprestor.h	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."
#define DUMPRESTOR_H

#define NTREC   	20
#define MLEN    	16
#define MSIZ    	4096

#define TS_TAPE 	1
#define TS_INODE	2
#define TS_BITS 	3
#define TS_ADDR 	4
#define TS_END  	5
#define TS_CLRI 	6
#define MAGIC   	(int)60011
#define CHECKSUM	(int)84446
struct	spcl
{
	int	c_type;
	time_t	c_date;
	time_t	c_ddate;
	int	c_volume;
	daddr_t	c_tapea;
	ino_t	c_inumber;
	int	c_magic;
	int	c_checksum;
	struct	dinode	c_dinode;
	int	c_count;
	char	c_addr[BSIZE];
} spcl;

struct	idates
{
	char	id_name[16];
	char	id_incno;
	time_t	id_ddate;
};

#endif /* DUMPRESTOR_H */
