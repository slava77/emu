##
#
# This is the TriDAS/emu/emuDCS/PeripheralCore Package Makefile
#
##

include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

#
# Packages to be built
#
Project=emu
Package=emuDCS/PeripheralXtop

Sources=\
        XtopVersion.cc \
        Chamber.cc \
        LOAD.cc \
        EmuDim.cc

IncludeDirs = 

LibraryDirs = 

TestLibraryDirs =

UserCFlags  = 
UserCCFlags = 
UserDynamicLinkFlags =
UserStaticLinkFlags =
UserExecutableLinkFlags =

# These libraries can be platform specific and
# potentially need conditional processing
#
Libraries =
TestLibraries =

#
# Compile the source files and create a shared library
#
DynamicLibrary= EmuPeripheralXtop
StaticLibrary= EmuPeripheralXtop

Executables=
TestExecutables= 

include $(XDAQ_ROOT)/config/Makefile.rules
