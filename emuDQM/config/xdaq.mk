include $(XDAQ_ROOT)/config/mfAutoconf.rules
include $(XDAQ_ROOT)/config/mfDefs.$(XDAQ_OS)

XdaqIncludeDirs = \
        $(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
        $(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/extern/cgicc/$(XDAQ_OS)$(XDAQ_PLATFORM)/include \
	$(XDAQ_ROOT)/daq/extern/i2o/include \
        $(XDAQ_ROOT)/daq/extern/i2o/include/i2o \
	$(XDAQ_ROOT)/daq/extern/i2o/include/i20/shared \
	$(XDAQ_ROOT)/daq/i2o/include \
	$(XDAQ_ROOT)/daq/i2o/utils/include \
        $(XDAQ_ROOT)/daq/toolbox/include \
        $(XDAQ_ROOT)/daq/toolbox/include/$(XDAQ_OS) \
        $(XDAQ_ROOT)/daq/toolbox/include/solaris \
	$(XDAQ_ROOT)/daq/xdata/include \
        $(XDAQ_ROOT)/daq/xoap/include \
        $(XDAQ_ROOT)/daq/xdaq/include \
	$(XDAQ_ROOT)/daq/xcept/include \
	$(XDAQ_ROOT)/daq/xgi/include \
	$(XDAQ_ROOT)/daq/pt/include \
        $(XDAQ_ROOT)/daq/interface/evb/include \
        $(XDAQ_ROOT)/daq/interface/shared/include \

XdaqLibraryDirs = \
        $(XDAQ_ROOT)/daq/extern/xerces/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/extern/cgicc/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/extern/log4cplus/udpappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/extern/log4cplus/xmlappender/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/extern/asyncresolv/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/extern/mimetic/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
        $(XDAQ_ROOT)/daq/toolbox/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(XDAQ_ROOT)/daq/xoap/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/pt/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/xdata/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/xgi/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/xcept/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
        $(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/lib \
	$(XDAQ_ROOT)/daq/i2o/utils/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM) \
	$(XDAQ_ROOT)/daq/xdaq/lib/$(XDAQ_OS)/$(XDAQ_PLATFORM)

XdaqLibraries = toolbox xdaq xoap xerces-c log4cplus xdata xgi xcept cgicc mimetic xmlappender udpappender peer i2outils asyncresolv
