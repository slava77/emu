/*
 * =====================================================================================
 *
 *       Filename:  DBImporter.h
 *
 *    Description:  DB Importer object
 *
 *        Version:  1.0
 *        Created:  12/11/2008 02:12:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Valdas Rapsevicius (VR), Valdas.Rapsevicius@cern.ch
 *        Company:  CERN, CH
 *
 * =====================================================================================
 */

#ifndef DBIMPORTER_H
#define DBIMPORTER_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TIterator.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TKey.h>

#include "DQM/CSCMonitorModule/interface/CSCDQM_Cache.h"
#include "DQM/CSCMonitorModule/interface/CSCDQM_Utility.h"
#include "DQM/CSCMonitorModule/interface/CSCDQM_Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/MessageLogger/interface/coloredlayout.h"
#include "CSCMonitorObject.h"
#include "CSCDQM_DBWorker.h"

typedef std::map<unsigned int, unsigned int>    UIntUIntMap;
typedef std::map<unsigned int, UIntUIntMap>     UIntUIntUIntMap;
typedef std::map<unsigned int, UIntUIntUIntMap> UIntUIntUIntUIntMap;

// ../csc_00066676_EmuRUI01_Monitor_000.root/DQMData/EMU/hp_EMU_Summary_All_DDUs_Average_Live_Inputs
const static TPRegexp reEMU("^.*/DQMData/EMU/h[0-9p]_EMU_Summary_(.*)$");

// ../csc_00066676_EmuRUI01_Monitor_000.root/DQMData/DDU_99/h1_DDU_99_Word_Count
const static TPRegexp reDDU("^.*/DQMData/DDU_([0-9]{1,3})/h[0-9p]_DDU_([0-9]{1,3})_(.*)$");

// ../csc_00066676_EmuRUI01_Monitor_000.root/DQMData/CSC_028_03/h2_CSC_028_03_ALCT1_Pattern
const static TPRegexp reCSC("^.*/DQMData/CSC_([0-9]{3})_([0-9]{2})/h[0-9p]_CSC_([0-9]{3})_([0-9]{2})_(.*)$");

// ../csc_00066676_EmuRUI01_Monitor_000.root/DQMData/CSC_028_03/h1_CSC_028_03_CLCT_Ly6_Rate
const static TPRegexp reAdd("(Ly_{0,1}|ALCT|CLCT|CFEB)([0-9])");
const static TPRegexp reAddNumber("^.*(Ly_{0,1}|ALCT|CLCT|CFEB)([0-9]).*$");

#define DQMUTL        cscdqm::Utility

class DBImporter {

  public:
    
    DBImporter(const std::string& rootFile_, const unsigned int& runNumber_, const oracle::occi::Connection *conn_); 
    ~DBImporter();  

    void generate();
    void finalize();

  private:

    void loadCache();

    cscdqm::DBWorker worker;
    std::string rootFileName;
    unsigned int runNumber;
    TFile* rootFile;
    cscdqm::Cache cache;

    /** Utility methods */

    void getDDUInputData(const cscdqm::HistoId& hid, const unsigned int id, UIntUIntUIntUIntMap& map) {
      cscdqm::MonitorObject* me = 0;
      if (cache.getEMU(hid, me)) {
        const TH2* h = dynamic_cast<const TH2*>(me->getTH1());

        for (int i = int(h->GetXaxis()->GetXmin()); i <= int(h->GetXaxis()->GetXmax()); i++) {
          for (int j = int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++) {
            uint32_t cnt = uint32_t(h->GetBinContent(i, j));
            if (cnt > 0 && map[i].find(j) != map[i].end()) {
              map[i][j][id] = cnt;
            }
          }
        }
      } else {
        LOG_WARN << "Can not find " << cscdqm::h::names[hid];
      }
    }

    unsigned int getL1OutOffSynData(const cscdqm::HistoId& hid, const unsigned int crateId, const unsigned int dmbId, const unsigned int addId) {
      uint32_t cnt = 0;
      cscdqm::MonitorObject* me = 0;
      if (cache.getCSC(hid, crateId, dmbId, addId, me)) {
        const TH1* h = me->getTH1();
        for (int i = int(h->GetXaxis()->GetXmin()); i <= int(h->GetXaxis()->GetXmax()); i++) {
          if (i != 0) {
            cnt += uint32_t(h->GetBinContent(1, i));
          }
        }
      } else {
        LOG_WARN << "Can not find " << cscdqm::h::names[hid];
      }
      return cnt;
    }


};

#endif

