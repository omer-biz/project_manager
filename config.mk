LIBDIR		= lib
BIN				= pm
CFLAGS		= -Wall -g -MD
SRCDIR		= src
BUILDDIR 	= build
CC				= gcc

# termbox2 lib dependancy
TBLIB			= libtermbox.a
TBDIR			:= $(LIBDIR)/termbox2
TBARC			:= $(TBDIR)/$(TBLIB)
CFLAGS 		+= -I$(TBDIR)

# nxjson lib dependancy
NJLIB			= libnxjson.a
NJDIR			:= $(LIBDIR)/nxjson
NJARC			:= $(NJDIR)/$(NJLIB)
CFLAGS		+= -I$(NJDIR)

# library archives
LIBARC 		:= $(TBARC) $(NJARC)
