Compilation:
set CMMSW env variable in emuDQM/config/site.mk pointing to CMSSW/src installation
set BOOST env variable on emuDQM/config/site.mk pointing to include directory of BOOST C++ Library
NOTE:
To compile CMSSW EventFilter/CSCRawToDigi some hack implemented in FWCore/MessageLogger/interface/MessageLogger.h to override calls to actual MessageLogger from CMSSW with calls to log4cplus from XDAQ

