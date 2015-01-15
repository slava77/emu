Installation:
untar provided emu/dqm/cmssw/dist/CMSSW_unpacker_postLS1.tgz distribution into desired folder (ex. $HOME)
set symbolic link from CMSSW_X_Y_Z to CMSSW

Compilation:
set CMMSW env variable in emu/dqm/config/site.mk pointing to CMSSW/src installation
set "NEW_UNPACKER2013 = yes" for post-LS1 unpacking in emu/dqm/config/site.mk

NOTE:
To compile CMSSW EventFilter/CSCRawToDigi some hack implemented in FWCore/MessageLogger/interface/MessageLogger.h to override calls to actual MessageLogger from CMSSW with calls to log4cplus from XDAQ

