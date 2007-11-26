include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

Project=emu
Package=emuFarmer

Sources= EmuFarmer.cc EmuFarmerV.cc EmuXalanTransformerProblemListener.cc EmuProcessDescriptor.cc

IncludeDirs = \
	$(XDAQ_ROOT)/include/ \
	./include

LibraryDirs = \
	$(XDAQ_ROOT)/lib

UserCFlags =
# UserCCFlags = -g -Wall -Werror -pedantic-errors
# UserCCFlags = -g -Wall  -Weffc++ -pedantic -pedantic-errors
UserCCFlags = -g -Wall -pedantic -pedantic-errors
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#

Libraries = 

#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuFarmer

StaticLibrary=

ifdef Executable
Libraries=toolbox xoap xerces-c libxalan-c
Executables= $(Executable).cc
endif

include $(XDAQ_ROOT)/config/Makefile.rules
