#include "emu/dqm/calibration/Test_25_ALCTTrigger.h"
#include <boost/algorithm/string.hpp>


using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;
using namespace boost::algorithm;

Test_25_ALCTTrigger::Test_25_ALCTTrigger(std::string dfile): Test_Generic(dfile)
{
  testID    = "25_ALCTTrigger";
  nExpectedEvents   = 15000;
  dduID     = 0;
  binCheckMask  = 0x1FEBF3F6;
  logger = Logger::getInstance(testID);

  dataFileName = dfile;

}

void Test_25_ALCTTrigger::initCSC(std::string cscID)
{
  //= Init per CSC events counters
  nCSCEvents[cscID]=0;

  TestData cscdata;
  TestData2D afebdata;
  afebdata.Nbins = getNumWireGroups(cscID);
  afebdata.Nlayers = NLAYERS;
  memset(afebdata.content, 0, sizeof (afebdata.content));
  memset(afebdata.cnts, 0, sizeof (afebdata.cnts));

  for(int i = 0; i < TEST_DATA2D_NLAYERS; i++)
    {
      for(int j = 0; j < TEST_DATA2D_NBINS; j++)
        {
          afebdata.content[i][j] = 0.;
          afebdata.cnts[i][j] = 0;
        }
    }

  // Channels mask
  if (amasks.find(cscID) != amasks.end())
    {
      cscdata["_MASK"]=amasks[cscID];
    }
  else
    {
      cscdata["_MASK"]=afebdata;
    }

  for(int i = 0; i < cscdata["_MASK"].Nlayers; i++)
    {
      for(int j = 0; j < cscdata["_MASK"].Nbins; j++)
        {
          cscdata["_MASK"].content[i][j] = (i == 3) ? 0 : 1;
        } // use mask to only pass layer/plane 4 through
    }



  cscdata["R01"]=afebdata;
  cscdata["R02"]=afebdata;
  cscdata["R03"]=afebdata; // all lct
  cscdata["R04"]=afebdata; // quality
  cscdata["R05"]=afebdata; // accel lct

  tdata[cscID] = cscdata;

  bookTestsForCSC(cscID);

  //cout << "bins (#wg): " << getNumWireGroups(cscID) << endl;

  first = true;
}

void Test_25_ALCTTrigger::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
{

  nTotalEvents++;

  //= Examiner checks
  const uint16_t *tmp = reinterpret_cast<const uint16_t *>(data);
  bin_checker.setMask( binCheckMask);
  if ( bin_checker.check(tmp,dataSize/sizeof(short)) < 0 )
    {
      //   No ddu trailer found - force checker to summarize errors by adding artificial trailer
      const uint16_t dduTrailer[4] = { 0x8000, 0x8000, 0xFFFF, 0x8000 };
      tmp = dduTrailer;
      bin_checker.check(tmp,uint32_t(4));
    }

  if (dduID != (bin_checker.dduSourceID()&0xFF))
    {

      LOG4CPLUS_DEBUG(logger, "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << (bin_checker.dduSourceID()&0xFF) << " First event");
      dduID = bin_checker.dduSourceID()&0xFF;
      dduL1A[dduID]=0;
      DDUstats[dduID].evt_cntr=0;
      DDUstats[dduID].first_l1a=-1;
    }

  dduID = bin_checker.dduSourceID()&0xFF;
  DDUstats[dduID].evt_cntr++;

  if (bin_checker.errors() != 0)
    {
      // std::cout << "Evt#" << std::dec << nTotalEvents << ": Nonzero Binary Errors Status is observed: 0x"<< std::hex << bin_checker.errors()
      //  << " mask:0x" << std::hex << binCheckMask << std::dec << std::endl;
      doBinCheck();
      //    return;
    }

  CSCDDUEventData dduData((uint16_t *) data, &bin_checker);

  currL1A=(int)(dduData.header().lvl1num());
  if (DDUstats[dduID].evt_cntr ==1)
    {
      DDUstats[dduID].first_l1a = currL1A;
      LOG4CPLUS_DEBUG(logger, "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << dduID
                      << " First L1A:" << DDUstats[dduID].first_l1a);
    }
  else if (DDUstats[dduID].first_l1a==-1)
    {
      DDUstats[dduID].first_l1a = currL1A-DDUstats[dduID].evt_cntr+1;
      LOG4CPLUS_DEBUG(logger, "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << dduID
                      << " First L1A :" << DDUstats[dduID].first_l1a << " after "
                      << currL1A-DDUstats[dduID].evt_cntr << " bad events");
    }

  DDUstats[dduID].l1a_cntr=currL1A;

  if ((DDUstats[dduID].l1a_cntr-DDUstats[dduID].first_l1a) != (DDUstats[dduID].evt_cntr-1))
    {
      LOG4CPLUS_WARN(logger, "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << dduID
                     << " Desynched L1A: " << ((DDUstats[dduID].l1a_cntr-DDUstats[dduID].first_l1a) - (DDUstats[dduID].evt_cntr-1)));
    }


  std::vector<CSCEventData> chamberDatas;
  chamberDatas = dduData.cscData();
  CSCDDUHeader dduHeader  = dduData.header();

  // std::cout << nTotalEvents << " " << chamberDatas.size() << std::endl;

  if (chamberDatas.size() >0)
    {
      DDUstats[dduID].csc_evt_cntr++;
    }
  else
    {
      DDUstats[dduID].empty_evt_cntr++;
    }

  // === set ltc_bug=2 in case of LTC double L1A bug
  // TODO: automatic detection of LTC L1A bug
  //  int ltc_bug=1;
  if (DDUstats[dduID].evt_cntr == 8)
    {
      if  (DDUstats[dduID].empty_evt_cntr==0)
        {
          LOG4CPLUS_INFO(logger, "No LTC/TTC double L1A bug in data");
          // ltc_bug=1;
        }
      else
        {
          LOG4CPLUS_WARN(logger, "Found LTC/TTC double L1A bug in data");
        }
    }


  int nCSCs = chamberDatas.size();

  //= Loop over found chambers and fill data structures
  for (int i=0; i < nCSCs; i++)
    {
      analyzeCSC(chamberDatas[i]);
    }

  DDUstats[dduID].last_empty=chamberDatas.size();

}


void Test_25_ALCTTrigger::analyzeCSC(const CSCEventData& data)
{
  int currVal = 1;
  int last_plane;

  const CSCDMBHeader* dmbHeader = data.dmbHeader();
  const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();
  if (!dmbHeader && !dmbTrailer)
    {
      return;
    }
  int csctype=0, cscposition=0;
  std::string cscID = getCSCFromMap(dmbHeader->crateID(), dmbHeader->dmbID(), csctype, cscposition);
  //cout << dmbHeader->crateID() << " " << dmbHeader->dmbID() << " " << csctype << " " << cscposition << endl;
  if (cscID == "") return;

  cscTestData::iterator td_itr = tdata.find(cscID);
  if ( (td_itr == tdata.end()) || (tdata.size() == 0) )
    {
      LOG4CPLUS_INFO(logger, "Found " << cscID);
      initCSC(cscID);
      addCSCtoMap(cscID, dmbHeader->crateID(), dmbHeader->dmbID());
    }


  if(nCSCEvents[cscID] == 0)
    {
      if(!loadThresholdParams(dataFileName, cscID, dmbHeader->crateID(), dmbHeader->dmbID()))
        {
          LOG4CPLUS_ERROR(logger, "Unable to load threshold parameters for " << cscID);
        }
      else
        {
          LOG4CPLUS_INFO(logger, "Loaded threshold parameters for " << cscID);
        }
    }

  nCSCEvents[cscID]++;

  int evtNum = nCSCEvents[cscID];

  test_step& tstep = htree[dduID][cscID];
  tstep.evt_cnt++;


  TestData& cscdata = tdata[cscID];

  TestData2D& r03 = cscdata["R03"]; // all lct
  TestData2D& r04 = cscdata["R04"]; // quality
  TestData2D& r05 = cscdata["R05"]; // accel lct

  for(int ct=0; ct<6; ct++)
    {
      if(evtNum>=threshold_limit[cscID].events[ct])
        {
          currVal=ct+2;
        }
    }

  int plane;
  if(first)
    {
      last_plane = -1;
      first = false;
    }

  plane = currVal;
  if(plane != last_plane)
    {
      last_plane = plane;
    }

  pattern_threshold = currVal;
  if (pattern_threshold < 2) plane_threshold = 1;
  else plane_threshold = 2;


  if (data.nalct())
    {
      //unused    const CSCAnodeData* alctData = data.alctData();
      const CSCALCTHeader* alctHeader = data.alctHeader();
      const CSCALCTTrailer* alctTrailer = data.alctTrailer();
      if (alctHeader && alctTrailer)
        {
          alct_full_bxn= alctHeader->BXNCount();

          vector<CSCALCTDigi> alctsDatasTmp = alctHeader->ALCTDigis();
          vector<CSCALCTDigi> alctsDatas;

          for (uint32_t lct=0; lct<alctsDatasTmp.size(); lct++)
            {
              if (alctsDatasTmp[lct].isValid())
                alctsDatas.push_back(alctsDatasTmp[lct]);

            }

          for (uint32_t lct=0; lct<alctsDatas.size(); lct++)
            {
              if (lct>=2) continue;
              alct_valid_patt[lct]   = (alctsDatas[lct].isValid())?1:0;
              alct_patt_quality[lct] = alctsDatas[lct].getQuality();
              alct_accel_muon[lct]   = alctsDatas[lct].getAccelerator();
              alct_wire_group[lct]   = alctsDatas[lct].getKeyWG();

            }

          if(alct_valid_patt[0])
            {

              r03.cnts[plane-1][alct_wire_group[0]]++;
              r04.cnts[plane-1][alct_wire_group[0]]+=alct_patt_quality[0];
              if(alct_accel_muon[0])
                r05.cnts[plane-1][alct_wire_group[0]]++;


            }


        }
    }
}


void Test_25_ALCTTrigger::finishCSC(std::string cscID)
{
  if (nCSCEvents[cscID] < nExpectedEvents)
    {
      LOG4CPLUS_ERROR(logger, Form("%s: Not enough events for test analysis (%d events)", cscID.c_str(), nCSCEvents[cscID] ));
      // = Set error
      return;
    }

  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end())
    {

      TestData& cscdata= td_itr->second;
      TestData2D& r01 = cscdata["R01"];
      TestData2D& r02 = cscdata["R02"];
      TestData2D& r03 = cscdata["R03"]; // all lct
      TestData2D& r04 = cscdata["R04"]; // quality
      //unused    TestData2D& r05 = cscdata["R05"]; // accel lct


      for(int plane = 0; plane < 6; plane++)
        {
          float f = 1000./threshold_limit[cscID].time[plane]; //ms to seconds
          for(int j = 0; j < getNumWireGroups(cscID); j++)
            {
              r01.content[plane][j] = f * (float)r03.cnts[plane][j];
              /*cout << "layer " << plane << " wg " << j << " f " << f
                   << " threshold_limit[cscID].time " << threshold_limit[cscID].time[plane] << " r03.cnts "
              	 << r03.cnts[plane][j] << " r01.cont " << r01.content[plane][j]
              	 << endl;*/
            }
        }

      //quality
      for(int plane = 0; plane < 6; plane++)
        {
          for(int j = 0; j < getNumWireGroups(cscID); j++)
            {
              if (r03.cnts[plane][j] > 0)
                r02.content[plane][j] = (double)r04.cnts[plane][j] / (double)r03.cnts[plane][j];
              else r02.content[plane][j] = -1;
            }
        }

    }
}

bool Test_25_ALCTTrigger::checkResults(std::string cscID)
{
  bool isValid=true;
  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end())
    {

    }

  return isValid;
}

bool Test_25_ALCTTrigger::loadThresholdParams(std::string dfile, std::string cscID, int crateID, int dmbID)
{

  std::string line;
  std::stringstream straw, storig;
  std::string fileStr = dfile;
  std::string cscIDslash = cscID;
  std::string timeStamp = fileStr.substr(fileStr.find("_UTC")-14,14);
  std::string runNumber = fileStr.substr(fileStr.find("_EmuRUI")-9,9);
  std::string filePath = fileStr.substr(0,fileStr.find("csc_"));
  replace_all(cscIDslash, ".", "/");
  replace_all(fileStr, "raw", "txt");

  // dmb slot := (dmb id * 2 + 1)
  // tmb slot = dmb slot - 1
  // dmb id cannot be 6 since slots 12 and 13 are for mpc, ccb
  int TMBslot = dmbID*2;

  if(dmbID == 6) {
    LOG4CPLUS_ERROR(logger, "DMBid of 6 is invalid (corresponds to DMBslot of 13)");
    return false;
  }

  storig << filePath << "Test25_CrateId" << setfill('0') << setw(2) << (crateID)
         << "_TMBslot" << setfill('0') << setw(2) << TMBslot << runNumber
         << timeStamp << "_UTC.txt";

  straw << fileStr;

  int evts;
  float timems;

  bool foundTXT = false;

  int count = 0;

  //std::string txtFileName = straw.str(); // string (raw) -- same as raw, with .txt
  std::string txtFileName = storig.str(); // string (original) used by daq
  //storig example: Test25_CrateId10_TMBslot16_00000001_130702_080727_UTC.txt

  ifstream threshparams(txtFileName.c_str());

  if(threshparams)
    {
      foundTXT = true;
      while (!threshparams.eof())
        {
          getline(threshparams, line);
          trim(line);

          if ((line.length() == 0) || (line.find("#") != string::npos)) continue;

          int iparse=sscanf(line.c_str(),"%f         %d", &timems, &evts);

          if(count > 5)
            {
              break;
            }

          if(iparse == 2)
            {
              threshold_limit[cscID].time[count] = (int)timems;
              if(count > 0)
                {
                  threshold_limit[cscID].events[count] = evts + threshold_limit[cscID].events[count-1];
                }
              else
                {
                  threshold_limit[cscID].events[count] = evts;
                }
              //cout << "threshold_limit['" << cscID << "'].time[" << count << "] " << threshold_limit[cscID].time[count] << endl;
              //cout << "threshold_limit['" << cscID << "'].events[" << count << "] " << threshold_limit[cscID].events[count] << endl;
              count++;
            }
        }
      threshparams.close();
    }
  else
    {
      LOG4CPLUS_ERROR(logger, "Unable to load threshold parameters file. Expected txt file: " << txtFileName);
      LOG4CPLUS_ERROR(logger, "Check STEP DAQ to see if text file produced, or rename file if it exists.");
      return false;
    }
  return true;
}
