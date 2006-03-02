#
# Makefile for D-Link DGE-550T Linux driver
# Copyright (c) 2001 D-Link Corporation
# Writen by Edward Peng.<edward_peng@dlink.com.tw>
# http://www.dlink.com.tw
#

CC := gcc
OBJS := acenic.o
K_VER=$(shell uname -r)
INCLUDEDIR=$(shell if [ -e /usr/src/linux-$(K_VER)/include ]; \
	then echo "/usr/src/linux-$(K_VER)/include"; \
	elif [ -e /usr/src/linux/include ]; \
	then echo "/usr/src/linux/include"; \
	else echo "/usr/include"; fi)
#DEBUG := -DRIO_DEBUG
CFLAGS := -D__KERNEL__ -DMODULE -O2 -Wall -Wstrict-prototypes -I$(INCLUDEDIR)
#CFLAGS := $(CFLAGS) -DUSE_IO_OPS 

# Experimental functions
# Enable hardware checksum
#CFLAGS := $(CFLAGS) -DTX_HW_CHECKSUM
#CFLAGS := $(CFLAGS) -DRX_HW_CHECKSUM

# Common make rules
%.o:	%.c %.h
	$(CC) $(CFLAGS) $(DEBUG) $(EXTRAS) -c $<
all:    $(OBJS)
clean:
	rm -f $(OBJS)

