/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) dofile.c: version 25.1 created on 12/2/91 at 17:10:10	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)dofile.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/
/*	ATT: #ident	"sccs:lib/comobj/dofile.c	6.5"	*/

#ident	"@(#)sccs:dofile.c	25.1"

# include	"../../hdr/defines.h"
# include	<sys/fs/s5dir.h>


int	nfiles;
char	had_dir;
char	had_standinp;


do_file(p,func)
register char *p;
int (*func)();
{
	extern char *Ffile;
	char str[FILESIZE];
	char ibuf[FILESIZE];
	char	dbuf[BUFSIZ];
	FILE *iop;
	struct direct dir[2];

	if (p[0] == '-') {
		had_standinp = 1;
		while (gets(ibuf) != NULL) {
			if (exists(ibuf) && (Statbuf.st_mode & S_IFMT) == S_IFDIR) {
				had_dir = 1;
				Ffile = ibuf;
				if((iop = fopen(ibuf,"r")) == NULL)
					return;
				setbuf(iop,dbuf);
				dir[1].d_ino = 0;
				fread(dir,sizeof(dir[0]),1,iop);   /* skip "."  */
				fread(dir,sizeof(dir[0]),1,iop);   /* skip ".."  */
				while(fread(dir,sizeof(dir[0]),1,iop) == 1) {
					if(dir[0].d_ino == 0) continue;
					sprintf(str,"%s/%s",ibuf,dir[0].d_name);
					if(sccsfile(str)) {
						Ffile = str;
						(*func)(str);
						nfiles++;
					}
				}
				fclose(iop);
			}
			else if (sccsfile(ibuf)) {
				Ffile = ibuf;
				(*func)(ibuf);
				nfiles++;
			}
		}
	}
	else if (exists(p) && (Statbuf.st_mode & S_IFMT) == S_IFDIR) {
		had_dir = 1;
		Ffile = p;
		if((iop = fopen(p,"r")) == NULL)
			return;
		setbuf(iop,dbuf);
		dir[1].d_ino = 0;
		fread(dir,sizeof(dir[0]),1,iop);   /* skip "."  */
		fread(dir,sizeof(dir[0]),1,iop);   /* skip ".."  */
		while(fread(dir,sizeof(dir[0]),1,iop) == 1) {
			if(dir[0].d_ino == 0) continue;
			sprintf(str,"%s/%s",p,dir[0].d_name);
			if(sccsfile(str)) {
				Ffile = str;
				(*func)(str);
				nfiles++;
			}
		}
		fclose(iop);
	}
	else {
		Ffile = p;
		(*func)(p);
		nfiles++;
	}
}
