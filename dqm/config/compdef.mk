#################################################################
#
# compdef.mk
#
#  Compiler definitions
#
#  Author: Holger Stoeck
#  Modifications:
#
#  $Id: compdef.mk,v 1.1 2009/07/20 21:53:19 barvic Exp $
#
#  Revision History
#  $Log: compdef.mk,v $
#  Revision 1.1  2009/07/20 21:53:19  barvic
#  Added local xdaq comilation config files
#
#  Revision 1.1  2006/03/15 13:30:32  barvic
#  build files changes
#
#  Revision 1.1  2004/10/01 11:42:25  tfcvs
#  *** empty log message ***
#
#  Revision 1.30  2004/04/15 19:48:21  tfcvs
#  HS - Added SBS version in TrigDAQ.env
#
#  Revision 1.29  2004/04/14 20:22:51  tfcvs
#  HS - Adapted to XDAQ V2.0
#
#  Revision 1.28  2004/04/14 17:23:16  tfcvs
#  HS - HS - Commiting everything before adapting to XDAQ V2.0
#
#  Revision 1.27  2003/12/18 21:34:22  tfcvs
#  HS - Minor change due to a change in ORCA
#
#  Revision 1.26  2003/12/07 19:34:51  tfcvs
#  HS - Changed SBS version to 2.2
#
#  Revision 1.25  2003/11/21 01:21:20  tfcvs
#  HS - Added FED directory to config files
# 
#
#################################################################

##########################
# Compiler section
##########################
CC  = cc
CCC = gcc

##########################
# Flags section
##########################
CCDEFAULT    = -o $*.o -c -O3 -Wall -Wstrict-prototypes \
               -D_POSIX_C_SOURCE -D_POSIX_THREAD_SEMANTICS
XDAQFLAGS    = -DCPU=x86 -DLITTLE_ENDIAN__ -DSOAP__ -DGM__ \
               -DHAL__ -DHALBus__ -DDNS__ -Dtoolbox  -DINLINE -DEVB_FED_DATA
ADAPTERFLAGS = -D$(VMEADAPTER) -DBT1003 -D$(FPGAMETHOD) -D$(BOARDTYPE)
ORCAFLAGS    = -DUNIX -DPORT=UNIX -DL1CSC_STANDALONE -DMYDEBUG -DRVALS=2 -D_BXAON 
CCFLAGS      = $(CCDEFAULT) $(XDAQFLAGS) $(ADAPTERFLAGS) $(ORCAFLAGS)
ROOTCFLAGS   := $(shell root-config --cflags)

##########################
# Include section
##########################
LOINCLUDES   = -I./ -I../ -I../../ -I../../../ \
               -I../../../../ -I../../../../../ -I../../../ORCA
HALINCLUDES  = -I$(HALDIR)/generic/include -I$(HALDIR)/generic/include/linux \
               -I$(XERDIR)/include -I$(HALDIR)/busAdapter/include
ORCAINCLUDES = -I$(ORCADIR) \
               -I$(ORCADIR)/Trigger/L1CSCTrackFinder/interface \
               -I$(ORCADIR)/Trigger/L1CSCTrigger/interface \
               -I$(ORCADIR)/Utilities/GenUtil/interface \
               -I$(ORCADIR)/Utilities/Notification/interface \
               -I$(ORCADIR)/Utilities/Configuration/interface \
               -I$(ORCADIR)/Trigger/L1GlobalTrigger/interface \
               -I$(ORCADIR)/Trigger/L1CommonTrigger/interface 
VMEINCLUDES  = -I$(VMEDIR) \
               -I$(VMEDIR)/SBS/1003/v$(SBSVERSION)/include
EMUINCLUDES  = -I$(EMUDIR) \
               -I$(EMUDIR)/DataFormat/include
ROOTINCLUDES = -I$(ROOTDIR)/include

EXINCLUDES   = $(XDAQINCLUDES) $(HALINCLUDES) \
               $(VMEINCLUDES) $(XDAQINCLUDE) \
               $(ORCAINCLUDES) $(EMUINCLUDES) $(ROOTINCLUDES)

CCINCLUDES   = $(LOINCLUDES) $(EXINCLUDES)

##########################
# Source code file section
##########################
CFILES  := $(shell echo *.c)
OCFILES  = $(CFILES:.c=.o)
CCFILES := $(shell echo *.cpp)
OFILES   = $(CCFILES:.cpp=.o)

##########################
# Library section
##########################
LIBDIR     = lib/Linux__2.2
XERLIBDIR  = $(XERDIR)/lib
HALLIBDIR  = $(HALDIR)/lib/linux/x86
VMELIBDIR  = $(VMEDIR)/lib/Linux__2.2
ORCALIBDIR = $(ORCADIR)/lib/Linux__2.2
EMULIBDIR  = $(EMUDIR)/lib/Linux__2.2
ROOTLIBDIR = $(ROOTDIR)/lib

SCANEASELIBS = -lScanEase_$(BOARD_TYPE) -lScanEaseInterface
HALLIBS      = -lGenericHAL -lSBSLinuxX86BusAdapter \
               -lVMEDummyBusAdapter -lxerces-c
VMEDRVLIBS   = -lbtp -lpthread -lSBSUtilities
ORCALIBS     = -lL1CSCTrackFinder -lL1CSCTrigger \
               -lMCommonData
EMUDAQLIBS   = -lEmuDataFormat

ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)

LIBPATHS   = -L$(HALLIBDIR) -L$(XERLIBDIR) -L$(VMELIBDIR) \
             -L$(ORCALIBDIR) \
             -L$(EMULIBDIR) \
             -L$(ROOTLIBDIR)

##########################
# Linker section
##########################
LL     = g++
LFLAGS =
LMAP   = link.map

#################################################################
#
# End of compdef.mk
#
#################################################################
