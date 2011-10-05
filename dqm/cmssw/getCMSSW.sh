#!/bin/bash

CMSSW_REL=$CMSSW_VERSION

if [ -n "$1" ];
then
  CMSSW_REL=$1
fi

cd $HOME
#scramv1 project CMSSW $CMSSW_REL
cd $CMSSW_REL
#eval `scramv1 runtime -sh`
# project CMSSW
cd src
cvs checkout -P IORawData/CSCCommissioning
cvs checkout -P CalibMuon/CSCCalibration
cvs checkout -P EventFilter/CSCRawToDigi
cvs checkout -P EventFilter/CSCTFRawToDigi
cvs checkout -P -r $CMSSW_REL DataFormats/Common
cvs checkout -P -r $CMSSW_REL DataFormats/CSCDigi
cvs checkout -P -r $CMSSW_REL DataFormats/DetId
cvs checkout -P -r $CMSSW_REL DataFormats/MuonDetId
cvs checkout -P -r $CMSSW_REL DataFormats/FEDRawData
cvs checkout -P -r $CMSSW_REL CondFormats/CSCObjects
cvs checkout -P -r $CMSSW_REL CondFormats/DataRecord
cvs checkout -P -r $CMSSW_REL FWCore/Utilities
cvs checkout -P DQM/CSCMonitorModule
cvs checkout -P DQM/RenderPlugins
cvs checkout -P -r $CMSSW_REL DQMServices
cvs checkout -P -r $CMSSW_REL OnlineDB/CSCCondDB
cd $HOME
tar cvzf ${HOME}/${CMSSW_REL}.tgz ${CMSSW_REL}


