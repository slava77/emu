# standards
#INCLUDE		= /usr/src/linux-2.4/include
K_VER=$(shell uname -r)
INCLUDE=$(shell if [ -e /usr/src/linux-$(K_VER)/include ]; \
	then echo "/usr/src/linux-$(K_VER)/include"; \
	elif [ -e /usr/src/linux-2.4/include ]; \
	then echo "/usr/src/linux-2.4/include"; \
        elif [ -e /usr/src/linux/include ]; \
        then echo "/usr/src/linux/include"; \
	else echo "/usr/include"; fi)
CC 		= gcc
CFLAGS 		= -D__KERNEL__ -I$(INCLUDE) -DMODULE -Wall -O2

TARGET	= eth_hook_3
SRC	= eth_hook_3.c

all: $(TARGET).o

clean:
	rm -f *.o *~ core


