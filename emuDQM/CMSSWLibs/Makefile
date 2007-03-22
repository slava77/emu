# temporarily in CVS for DQM needs
include ../config/compdef.mk
include ../config/site.mk
include ../config/xdaq.mk

CXX     = g++

CXXFLAGS  = -pedantic -Wno-long-long -Wall -fPIC

LOG4CPLUS = $(XDAQ_ROOT)/daq/extern/log4cplus/$(XDAQ_OS)$(XDAQ_PLATFORM)/include
DEFINES   = -DLOG4CPLUS_LOGGER  $(INPDEF) $(OUTDEF)

#------------------------------------------------------
INCLUDES += -I./ -I$(CMSSW) -I$(BOOST) -I$(LOG4CPLUS)

LDIR = lib
ODIR = obj

# CMSSW DataFormats/CSCDigi 
CSCDIGI   = $(CMSSW)/DataFormats/CSCDigi
SRCDIGI_PATH   = $(CSCDIGI)/src
SRCS_DIGI      = $(SRCDIGI_PATH)/CSCALCTDigi.cc $(SRCDIGI_PATH)/CSCCLCTDigi.cc\
            $(SRCDIGI_PATH)/CSCWireDigi.cc $(SRCDIGI_PATH)/CSCComparatorDigi.cc \
	    $(SRCDIGI_PATH)/CSCStripDigi.cc $(SRCDIGI_PATH)/CSCRPCDigi.cc \
	    $(SRCDIGI_PATH)/CSCCorrelatedLCTDigi.cc
OBJS_DIGI      = $(addprefix $(ODIR)/, $(notdir $(SRCS_DIGI:%.cc=%.o)))


# CMSSW EventFilter/CSCRawToDigi
CSCRAW   = $(CMSSW)/EventFilter/CSCRawToDigi
SRCRAW_PATH    = $(CSCRAW)/src
SRCS_RAW      = $(SRCRAW_PATH)/CSCCFEBData.cc $(SRCRAW_PATH)/CSCDCCHeader.cc\
	$(SRCRAW_PATH)/CSCDDUEventData.cc $(SRCRAW_PATH)/CSCDCCEventData.cc\
	$(SRCRAW_PATH)/CSCDDUHeader.cc $(SRCRAW_PATH)/bitset_append.cc \
	$(SRCRAW_PATH)/CSCDMBHeader.cc $(SRCRAW_PATH)/CSCEventData.cc\
	$(SRCRAW_PATH)/CSCCFEBTimeSlice.cc \
	$(SRCRAW_PATH)/CSCTMBHeader.cc $(SRCRAW_PATH)/CSCALCTHeader.cc \
	$(SRCRAW_PATH)/CSCCLCTData.cc $(SRCRAW_PATH)/CSCAnodeData.cc \
	$(SRCRAW_PATH)/CSCTMBScope.cc $(SRCRAW_PATH)/CSCTMBData.cc \
	$(SRCRAW_PATH)/CSCBadCFEBWord.cc $(SRCRAW_PATH)/CSCBadCFEBTimeSlice.cc \
	$(SRCRAW_PATH)/CSCRPCData.cc $(SRCRAW_PATH)/CSCTMBTrailer.cc
OBJS_RAW      = $(addprefix $(ODIR)/, $(notdir $(SRCS_RAW:%.cc=%.o)))

# CMSSW DQMServices/QualityTests
QLTY   = $(CMSSW)/DQMServices/QualityTests
SRCQLTY_PATH    = $(QLTY)/src
SRCS_QLTY     = $(SRCQLTY_PATH)/Comp2RefChi2ROOT.cc \
	$(SRCQLTY_PATH)/Comp2RefEqualROOT.cc \
	$(SRCQLTY_PATH)/Comp2RefKolmogorovROOT.cc \
	$(SRCQLTY_PATH)/ContentsWithinRangeROOT.cc \
	$(SRCQLTY_PATH)/MeanWithinExpectedROOT.cc \
	$(SRCQLTY_PATH)/QStatisticalTests.cc \
	$(SRCQLTY_PATH)/RuleAllContentAlongDiagonal.cc \
	$(SRCQLTY_PATH)/RuleAllContentWithinFixedRange.cc \
	$(SRCQLTY_PATH)/RuleAllContentWithinFloatingRange.cc \
	$(SRCQLTY_PATH)/RuleCSC01.cc \
        $(SRCQLTY_PATH)/RuleFixedFlatOccupancy1d.cc \
	$(SRCQLTY_PATH)/RuleFlatOccupancy1d.cc
OBJS_QLTY      = $(addprefix $(ODIR)/, $(notdir $(SRCS_QLTY:%.cc=%.o)))


$(ODIR)/%.o: $(CSCDIGI)/src/%.cc $(CSCDIGI)/interface/%.h
	$(CXX) -c $(CXXFLAGS) $(DEFINES) $(INCLUDES) $< -o $@

$(ODIR)/%.o: $(CSCRAW)/src/%.cc $(CSCRAW)/interface/%.h
	$(CXX) -c $(CXXFLAGS) $(DEFINES) $(INCLUDES) $< -o $@

$(ODIR)/%.o: $(QLTY)/src/%.cc $(QLTY)/interface/%.h
	$(CXX) -c $(CXXFLAGS) $(DEFINES) $(ROOTCFLAGS) $(INCLUDES) $< -o $@

all: libs 
_all: all

libs: $(LDIR)/libCSCDigi.a $(LDIR)/libCSCDigi.so $(LDIR)/libCSCRawToDigi.a $(LDIR)/libCSCRawToDigi.so $(LDIR)/libQualityTests.a $(LDIR)/libQualityTests.so


$(LDIR)/libCSCDigi.so: $(OBJS_DIGI)
	@echo "===> Linking DataFormats/CSCDigi libraries..."
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -shared $(OBJS_DIGI) -o $@

$(LDIR)/libCSCDigi.a: $(OBJS_DIGI)
	ar cr $@ $(OBJS_DIGI)
	@echo "Done."

$(LDIR)/libCSCRawToDigi.so: $(OBJS_RAW)
	@echo "===> Linking CMSSW EventFilter/CSCRawToDigi libraries..."
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -shared $(OBJS_RAW)  -o $@
	

$(LDIR)/libCSCRawToDigi.a: $(OBJS_RAW)
	ar cr $@ $(OBJS_RAW)
	@echo "Done."

$(LDIR)/libQualityTests.so: $(OBJS_QLTY)
	@echo "===> Linking CMSSW DQMServices/QualityTests libraries..."
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -shared $(OBJS_QLTY)  -o $@

$(LDIR)/libQualityTests.a: $(OBJS_QLTY)
	ar cr $@ $(OBJS_QLTY)
	@echo "Done."


clean:
	@rm -f $(LDIR)/*.a $(LDIR)/*.so $(ODIR)/*.o *.o program core*
