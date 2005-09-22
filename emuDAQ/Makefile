##
#
# This is the TriDAS/emu/emuDAQ Project Makefile
#  (this Makefile will soon become obsolete:
#   only project and package Makefiles should remain)
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project=emu

ifeq ($(Set),framework)
Packages=\
	emuDAQ/DDUReadout \
	emuDAQ/GenericRUI \
	emuDAQ/EmuRUI \
	emuDAQ/TriggerAdapter \
	emuDAQ/drivers/bigphysarea_xxxx
endif

ifeq ($(Set),DDUReadout)
Packages= emuDAQ/DDUReadout
endif

ifeq ($(Set),GenericRUI)
Packages= emuDAQ/GenericRUI
endif

ifeq ($(Set),EmuRUI)
Packages= emuDAQ/EmuRUI
endif

ifeq ($(Set),TriggerAdapter)
Packages= emuDAQ/TriggerAdapter
endif

ifeq ($(Set),drivers)
Packages=emuDAQ/drivers/bigphysarea_xxxx
endif


include $(XDAQ_ROOT)/config/Makefile.rules
