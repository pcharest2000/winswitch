# Project Title
#
# @file
# @version 0.1

# Makefile for 9wm-xcb
PKGCONFIG = $(shell which pkg-config)
CFLAGS = -O2 $(shell $(PKGCONFIG) --cflags gtk+-3.0)
CFLAGS_DBG = -g -O0 -DDEBUG -DDEBUG_EV $(CFLAGS)
LDFLAGS =  -lxcb -lxcb-util  -lxcb-ewmh  -lxcb-image  -lcairo  -lfreetype $(shell $(PKGCONFIG) --libs gtk+-3.0)
PREFIX=/opt
BIN = $(PREFIX)/bin

MANDIR = $(PREFIX)/man/man1
MANSUFFIX = 1

SRCS = main.c
SRCS_DBG = main.c
OBJS = $(SRCS:.c=.o)
OBJS_DBG = $(SRCS_DBG:.c=.g)
HDRS = main.h

all: winswitch

debug: winswitch.dbg

.SUFFIXES : .g

.c.g:
	$(CC) $(CFLAGS_DBG) -c $< -o $*.g

winswitch.dbg: $(OBJS_DBG)
	$(CC) $(CFLAGS_DBG) -o $@ $(OBJS_DBG) $(LDFLAGS)

winswitch: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): $(HDRS)

$(OBJS_DBG): $(HDRS)

# install: 9wm-xcb
# 	cp 9wm-xcb $(BIN)/9wm

# install.man:
# 	cp 9wm.man $(MANDIR)/9wm.$(MANSUFFIX)

# trout: 9wm.man
# 	troff -man 9wm.man >trout

# vu: trout
# 	xditview trout

clean:
	rm -f winswitch winswitcher.dbg *.o *.g core

# end
