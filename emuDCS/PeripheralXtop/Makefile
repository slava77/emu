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
PackageName=PeripheralXtop
Description="XDAQ to PVSS communication libraries for Peripheral Crates"
PACKAGE_VER_MAJOR=07
PACKAGE_VER_MINOR=07
PACKAGE_VER_PATCH=00

Sources=\
        XtopVersion.cc \
        Chamber.cc \
        DDU.cc \
        LOAD.cc \
        EmuDim.cc

IncludeDirs = \
        $(XDAQ_ROOT)/include \
        $(BUILD_HOME)/emu/base/include

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
StaticLibrary= 

Executables=
TestExecutables= 

include $(XDAQ_ROOT)/config/Makefile.rules
include $(XDAQ_ROOT)/config/mfRPM.rules
