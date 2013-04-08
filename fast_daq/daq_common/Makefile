#***************************************************************************
#
# Purpose: Makefile for the daq_common.
#          This make file only works with "gmake", the GNU version of make.
#
# Author: David Chrisman
# Date: 2 Sept. 1999
#
#***************************************************************************

CMSWGS  = FALSE
VERSION = pro

ifeq ($(CMSWGS), TRUE)
  HDIR   = $(DAQHOME)/$(VERSION)/daq_common
  AFSDIR = .@sys/lib
else
  HDIR   = $(DAQHOME)/daq_common
  AFSDIR = .no_afs/lib
endif

LDIR = $(HDIR)/lib
SDIR = $(HDIR)/src
ODIR = $(HDIR)/obj
IDIR = $(HDIR)/include

#***************************************************************************
## names for commands
SHELL      = /bin/sh
CC         = gcc
LEX        = flex
ifeq ($(VERSIONING_BUILDALL),TRUE)
VC1        =
VC2        = 
else
# clean those items that depend on daq_common
#VC1	   = @rm -f $(BDIR)/lisas_test_manager ; \
#	rm -f $(DAQHOME)/run_control/Daq_Window.class
#VC2	   = @echo lisas_test_manager and run_control depend on daq_common 
#	and must be rebuilt for version control.
VC1=
VC2=
endif

#***************************************************************************
## options for compiling

# Uncomment the next line when debugging
#DEBUG      = -Wall -g -DDEBUG -DDEBUG_ALCT2
DEBUG      = -Wall -ggdb

# Uncomment the next line when NOT debugging
# DEBUG      = -W -O

CCOPT      = $(DEBUG) -Df2cFortran

CXXFLAGS	= -O -m64 -Wall -fno-rtti -fno-exceptions -fPIC

#***************************************************************************
## object files           

OBJECTS_ONLINE =  $(ODIR)/daq_ipc.o $(ODIR)/get_next_event.o \
                  $(ODIR)/lex_cable_map.o  \
                  $(ODIR)/crcdivide.o $(ODIR)/test_ipc.o 
#			      $(ODIR)/pedestal.o

OBJECTS_OFFLINE = $(ODIR)/crcdivide.o \
                  $(ODIR)/get_next_event_offline.o \
                  $(ODIR)/lex_cable_map_offline.o 
#				  $(ODIR)/pedestal.o 

#***************************************************************************
## other includes and libs
LOCINC     = -I$(IDIR) -I$(CERNDIR)/include/cfortran \
             -I$(DAQHOME)/alct_jtag/include

all : versioning $(OBJECTS_ONLINE) $(OBJECTS_OFFLINE)
	sed "s-#define KEY_FILE \"/tmp/daq_ipc_.*-#define KEY_FILE \"/tmp/daq_ipc_$(USER)\"         /* Directory to use in ftok calls */-g" include/daq_ipc.h > include/daq_ipc.h.sed
	mv include/daq_ipc.h.sed include/daq_ipc.h
	rm -rf $(LDIR)
	ln -s $(AFSDIR) $(LDIR)
	ar -r lib/daq_common.a $(OBJECTS_ONLINE)
	ar -r lib/daq_common_offline.a $(OBJECTS_OFFLINE)

versioning:
	$(VC1)
	$(VC2)

$(ODIR)/daq_ipc.o : $(SDIR)/daq_ipc.c $(IDIR)/daq_conf.h  $(IDIR)/daq_ipc.h
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/daq_ipc.o -c $(SDIR)/daq_ipc.c

$(ODIR)/get_next_event.o : $(SDIR)/get_next_event.c $(IDIR)/daq_module.h  \
                           $(IDIR)/csc_event.h
	$(CC) $(CCOPT) -DOFFLINE_ONLY $(LOCINC) -o $(ODIR)/get_next_event.o \
          -c $(SDIR)/get_next_event.c

$(ODIR)/get_next_event_offline.o : $(SDIR)/get_next_event.c \
                                   $(IDIR)/daq_conf.h  \
                                   $(IDIR)/daq_ipc.h
	$(CC) $(CCOPT) -DOFFLINE_ONLY $(LOCINC) \
          -o $(ODIR)/get_next_event_offline.o -c $(SDIR)/get_next_event.c

$(ODIR)/pedestal.o : $(SDIR)/pedestal.c $(IDIR)/csc_event.h \
          $(IDIR)/csc_parameters.h $(IDIR)/pedestal.h $(IDIR)/daq_module.h \
          $(IDIR)/daq_histo.h
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/pedestal.o \
          -c $(SDIR)/pedestal.c

$(ODIR)/lex_cable_map.o : $(SDIR)/lex_cable_map.c $(IDIR)/daq_module.h \
                          $(IDIR)/csc_parameters.h
	$(CC) $(CCOPT) $(LOCINC) -o $(ODIR)/lex_cable_map.o \
          -c $(SDIR)/lex_cable_map.c

$(ODIR)/lex_cable_map_offline.o : $(SDIR)/lex_cable_map.c \
			$(IDIR)/daq_module.h $(IDIR)/csc_event.h
	$(CC) $(CCOPT) -DOFFLINE_ONLY $(LOCINC) \
          -o $(ODIR)/lex_cable_map_offline.o -c $(SDIR)/lex_cable_map.c

$(SDIR)/lex_cable_map.c: $(SDIR)/lex_cable_map.l
	$(LEX) -B -Pcable_map -o$(SDIR)/lex_cable_map.c $(SDIR)/lex_cable_map.l

$(ODIR)/crcdivide.o : $(SDIR)/crcdivide.c
	$(CC) -O -o $(ODIR)/crcdivide.o -c $(SDIR)/crcdivide.c

# added to get run_control to build, and release interdependency with
# test_manager
$(ODIR)/test_ipc.o : $(SDIR)/test_ipc.cpp $(IDIR)/test_ipc.h
	g++ $(CCOPT) $(LOCINC) $(CXXFLAGS) -o $(ODIR)/test_ipc.o  \
		-c  $(SDIR)/test_ipc.cpp
clean :
	cd $(ODIR) ; rm -f *.o
	cd $(LDIR) ; rm -f *.a
	rm -rf $(LDIR)
	rm -rf $(SDIR)/lex_cable_map.c
