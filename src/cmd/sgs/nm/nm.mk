#ident	"@(#)nm.mk	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."


include	$(IROOT)/macdefs

# POSIX

SVID_FLAG =

all install clean clobber:
		cd mach; $(MAKE) -$(MAKEFLAGS)f makefile  $@
