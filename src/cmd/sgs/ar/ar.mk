#ident	"@(#)ar.mk	25.1	12/2/91 Copyright (c) 1991 by Arix Corp."


#	ar.mk ar makefile

SROOT =
IROOT =
include $(IROOT)/macdefs

# POSIX binary

SVID_FLAG =

all install clean clobber:
	cd mach; $(MAKE) -$(MAKEFLAGS)f makefile $@
