
VERSION		= 0.0.1

PREFIX		= /usr/local

LIBDIR		= lib
BIN				= pm
CFLAGS		= -Wall -MD
SRCDIR		= src
BUILDDIR 	= build
CC				= gcc
INCLUDE		= include
PRJ_FILE	= projects.json
PRJ_PATH	= ${HOME}/.config/

CFLAGS 		+= -I$(INCLUDE)

ifeq (${BUILD_MODE}, dev)
CFLAGS		+= -Og -g -DDEBUG_MODE
PRJ_PATH 	= "."
else
CFLAGS		+= -Os
endif

CFLAGS 		+= -DPROJECTS_FILEPATH=\"${PRJ_PATH}/${PRJ_FILE}\"
CFLAGS 		+= -DPROJECTS_FILEPATH_PATH=\"${PRJ_PATH}\"
CFLAGS		+= -DVERSION=\"${VERSION}\"

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
