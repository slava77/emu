#include "emu/dqm/calibration/Test_AFEB05.h"

using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;

Test_AFEB05::Test_AFEB05(std::string dfile): Test_Generic(dfile)
{
  testID = "AFEB05";
  nExpectedEvents = 6000;
  nEvents=500;
  dduID=0;
  //  binCheckMask=0x16CFF3F6;
  binCheckMask=0x1FEBF3F6;
  //  binCheckMask=0xF7CB3BF6;
  ltc_bug=2;
  logger = Logger::getInstance(testID);
}


void Test_AFEB05::initCSC(std::string cscID)
{


  //= Init per CSC events counters
  nCSCEvents[cscID]=0;

  TestData cscdata;
  TestData2D afebdata;
  afebdata.Nbins = getNumWireGroups(cscID);
  afebdata.Nlayers = NLAYERS;
  memset(afebdata.content, 0, sizeof (afebdata.content));
  memset(afebdata.cnts, 0, sizeof (afebdata.cnts));


  // Channels mask
  if (amasks.find(cscID) != amasks.end())
    {
      cscdata["_MASK"]=amasks[cscID];
    }
  else
    {
      cscdata["_MASK"]=afebdata;
    }

  for (int i=0; i<TEST_DATA2D_NLAYERS; i++)
    for (int j=0; j<TEST_DATA2D_NBINS; j++) afebdata.content[i][j]=0.;


  cscdata["R01"]=afebdata;
  cscdata["R02"]=afebdata;
  cscdata["R03"]=afebdata;

  tdata[cscID] = cscdata;;

  bookTestsForCSC(cscID);
}

void Test_AFEB05::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
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
      DDUstats[dduID].layer=1;
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
                      << " First L1A:" << DDUstats[dduID].first_l1a << " after "
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

  fSwitch=false;

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
          ltc_bug=1;
          nEvents = 1000;
        }
      else
        {
          LOG4CPLUS_WARN(logger, "Found LTC/TTC double L1A bug in data");
        }
    }


  int layerSwitch=1000*ltc_bug;

  if (currL1A% layerSwitch ==1)
    {
      DDUstats[dduID].layer=currL1A / layerSwitch + 1;
      DDUstats[dduID].empty_evt_cntr=0;

      fSwitch=true;
      std::map<std::string, test_step> & cscs = htree[dduID];
      for (std::map<std::string, test_step>::iterator itr = cscs.begin(); itr != cscs.end(); ++itr)
        {
          itr->second.evt_cnt = 0;
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


void Test_AFEB05::analyzeCSC(const CSCEventData& data)
{

  int ly_pairs[6] = { 2, 1, 4, 3, 6, 5 };

  const CSCDMBHeader* dmbHeader = data.dmbHeader();
  const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();
  if (!dmbHeader && !dmbTrailer)
    {
      return;
    }


  int csctype=0, cscposition=0;
  std::string cscID = getCSCFromMap(dmbHeader->crateID(), dmbHeader->dmbID(), csctype, cscposition);
  // std::string cscID(Form("CSC_%03d_%02d", data.dmbHeader().crateID(), data.dmbHeader().dmbID()));
  // == Do not process unmapped CSCs and ME1/1 chambers
  if ((cscID == "") || (cscID.find("ME+1.1") == 0) || (cscID.find("ME-1.1") ==0) ) return;


  cscTestData::iterator td_itr = tdata.find(cscID);
  if ( (td_itr == tdata.end()) || (tdata.size() == 0) )
    {
      LOG4CPLUS_INFO(logger, "Found " << cscID);
      initCSC(cscID);
      addCSCtoMap(cscID, dmbHeader->crateID(), dmbHeader->dmbID());
    }
  nCSCEvents[cscID]++;

  // std::cout << nCSCEvents[cscID] << " " << cscID << std::endl;
  // == Define aliases to access chamber specific data
  TestData& cscdata = tdata[cscID];

  TestData2D& r01 = cscdata["R01"];
  TestData2D& r02 = cscdata["R02"];
  TestData2D& r03 = cscdata["R03"];

  MonHistos& cschistos = mhistos[cscID];

  test_step& tstep = htree[dduID][cscID];// test_steps[cscID];

  int curr_layer =  DDUstats[dduID].layer;
  int pair_layer = ly_pairs[curr_layer - 1];

  tstep.evt_cnt++;

  TH2F* v01 = reinterpret_cast<TH2F*>(cschistos["V01"]);
  if (data.nalct())
    {
      const CSCAnodeData* alctData = data.alctData();
      if (alctData)
        {
          for (int nLayer=1; nLayer<=6; nLayer++)
            {
              vector<CSCWireDigi> wireDigis = alctData->wireDigis(nLayer);
              for (vector<CSCWireDigi>:: iterator wireDigisItr = wireDigis.begin();
                   wireDigisItr != wireDigis.end(); ++wireDigisItr)
                {
                  int wg = wireDigisItr->getWireGroup();
                  v01->Fill(wg-1, nLayer-1);
                  if (curr_layer == nLayer)
                    {
                      r01.cnts[curr_layer-1][wg-1]++;
                    }
                  if ( nLayer != curr_layer && nLayer == pair_layer)
                    {
                      r02.cnts[pair_layer-1][wg-1]++;
                    }
                  if ( nLayer != curr_layer && nLayer != pair_layer)
                    {
                      r03.cnts[nLayer-1][wg-1]++;
                    }
                  /*          if (curr_layer != nLayer)
                        std::cout << nCSCEvents[cscID]<< " " <<  cscID << ": Un-expected hit in layer#" << nLayer << " " << curr_layer << " wg#" << wg << std::endl;
                  */
                }
            }
        }
    }
}


void Test_AFEB05::finishCSC(std::string cscID)
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

      //unused    TestData2D& mask = cscdata["_MASK"];

      TestData2D& r01 = cscdata["R01"];
      TestData2D& r02 = cscdata["R02"];
      TestData2D& r03 = cscdata["R03"];

      CSCtoHWmap::iterator itr = cscmap.find(cscID);

      if (itr != cscmap.end())
        {

          // map->cratedmb(itr->second.first,itr->second.second,&mapitem);
          int dmbID = itr->second.second;
          if (dmbID >= 6) --dmbID;
          int id = 10*itr->second.first+dmbID;

          CSCMapItem::MapItem mapitem = cratemap->item(id);

          for (int i=0; i<NLAYERS; i++)
            {
              for (int j=0; j<getNumWireGroups(cscID); j++)
                {
                  r01.content[i][j] = (double)(r01.cnts[i][j])/nEvents;
                  r02.content[i][j] = (double)(r02.cnts[i][j])/nEvents;
                  r03.content[i][j] = (double)(r03.cnts[i][j])/nEvents;
                }
            }

          // == Save results to text files
          std::string rpath = "Test_"+testID+"/"+outDir;
          std::string path = rpath+"/"+cscID+"/";

          std::ofstream res_out;


          //      }

        }

    }
}

bool Test_AFEB05::checkResults(std::string cscID)
{
  bool isValid=true;
  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end())
    {
      //unused    TestData& cscdata= td_itr->second;
      //unused    TestData2D& r01 = cscdata["R01"];

      //unused    int badChannels=0;
    }

  return isValid;
}

