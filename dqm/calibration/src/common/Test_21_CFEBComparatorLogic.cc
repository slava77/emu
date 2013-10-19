#include "emu/dqm/calibration/Test_21_CFEBComparatorLogic.h"

using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;


Test_21_CFEBComparatorLogic::Test_21_CFEBComparatorLogic(std::string dfile):
  Test_Generic(dfile)
{
  testID = "21_CFEBComparatorLogic";

  //need to load from xml at some point
  events_per_hstrip = 250;
  hstrips_per_run = 32;
  hstrip_step = 1;
  hstrip_first =1;

  n_ME11_TMB_DCFEBs = -1;//don't use by default

  nExpectedEvents = events_per_hstrip*hstrips_per_run;

  //do we need this?  binCheckMask=0x16EBF7F6; // same value used in Lisa's test16

  logger = Logger::getInstance(testID);
  
}

Test_21_CFEBComparatorLogic::~Test_21_CFEBComparatorLogic()
{
}

void Test_21_CFEBComparatorLogic::initCSC(std::string cscID)
{
 
  //= Init per CSC events counters
  nCSCEvents[cscID]=0;

  //= Init data structures for storing data and results
  int nLayers = NLAYERS;
  
  
  //isME11() in emu::dqm::utils uses cscID.find("ME+1/1")
  // instead of cscID.find("ME+1.1") so it doesn't work
  bool isME11 = (this->isME11(cscID) || emu::dqm::utils::isME11(cscID));
  int n_TMB_DCFEBs = getNumCFEBs(cscID); 
  if (isME11 && n_ME11_TMB_DCFEBs > 0 )
    {
      n_TMB_DCFEBs = n_ME11_TMB_DCFEBs;
    }
  //this is a TMB data based test
  int n_TMB_strips = n_TMB_DCFEBs*16;

  int nBins = n_TMB_strips*2;

  tdata[cscID]["R01"].Nlayers = nLayers;
  tdata[cscID]["R01"].Nbins = nBins;

  // Channels mask
  if(tmasks.find(cscID) != tmasks.end())
    {
      tdata[cscID]["_MASK"] = tmasks[cscID];
    } else 
    {
      tdata[cscID]["_MASK"].Nlayers = nLayers;
      tdata[cscID]["_MASK"].Nbins = nBins;

	  int halfstripsPerBoard = 16*2;
	  int ME11AB = 2; //0 for a, 1 for b, 2 for 4-1 mode or 4-3 mode
	  //hardcoded right now, presumably will be taken from xml later?
	  int nBoards = n_TMB_strips/16;

	  if(!isME11) ME11AB = -1;

	  int maskFirst = 0;
	  int maskLast = halfstripsPerBoard*nBoards;

	  int maskBoundary = (ME11AB == 2) ? (4*halfstripsPerBoard) : 0;
	  //no boundary if only me11a or me11b, or nonme11 of course

	  for(int i = 0; i < nLayers; i++) {
	    tdata[cscID]["_MASK"].content[i][maskFirst] = 1;
	    tdata[cscID]["_MASK"].content[i][maskLast-1] = 1;
	    if(isME11) {
	      tdata[cscID]["_MASK"].content[i][maskBoundary] = 1;
	      tdata[cscID]["_MASK"].content[i][maskBoundary-1] = 1;
	    } // only ME11-type need boundary mask (between A,B sides)
	  } // mask edge half-strips since not possible to be pulsed
    }
      
  //initialize local arrays here as well
  TestData2D& ldata = halfStrips[cscID];
  ldata.clearArrays();
  ldata.Nlayers = nLayers;
  ldata.Nbins = nBins;

  ldata = halfStripsMultiple[cscID];
  ldata.clearArrays();
  ldata.Nlayers = nLayers;
  ldata.Nbins = nBins;

  ldata = halfStripsFake[cscID];
  ldata.clearArrays();
  ldata.Nlayers = nLayers;
  ldata.Nbins = nBins;

  
  bookTestsForCSC(cscID);
}

void Test_21_CFEBComparatorLogic::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
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

  if (bin_checker.errors() != 0)
    {
      //doBinCheck();
      //return;
    }

  CSCDDUEventData dduData((uint16_t *) data, &bin_checker);
  dduData.setDebug(false);
  dduData.setErrorMask(0xFFFF);

  std::vector<CSCEventData> chamberDatas;
  chamberDatas = dduData.cscData();
  CSCDDUHeader dduHeader  = dduData.header();

  int nCSCs = chamberDatas.size();

  for (std::vector<CSCEventData>::iterator chamberDataItr = chamberDatas.begin();
       chamberDataItr != chamberDatas.end(); ++chamberDataItr)
  {
    analyzeCSC(*chamberDataItr);
  }

}

void Test_21_CFEBComparatorLogic::analyzeCSC(const CSCEventData& data)
{
  
  const CSCDMBHeader* dmbHeader = data.dmbHeader();
  const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();
  if (!dmbHeader && !dmbTrailer)
    {
      return;
    }
  int csctype=0, cscposition=0;
  std::string cscID = getCSCFromMap(dmbHeader->crateID(), dmbHeader->dmbID(), csctype, cscposition);
  if (cscID == "")  return;
  
  cscTestData::iterator td_itr = tdata.find(cscID);
  if ( (td_itr == tdata.end()) || (tdata.size() == 0) )
    {
      LOG4CPLUS_INFO(logger, "Found " << cscID);
      initCSC(cscID);
      addCSCtoMap(cscID, dmbHeader->crateID(), dmbHeader->dmbID());
    }
  
  
  if(nCSCEvents[cscID] >= nExpectedEvents) 
    {
      LOG4CPLUS_WARN(logger, nCSCEvents[cscID] << " exceeded expected number of events (" << nExpectedEvents << ")");
    }
  
  nCSCEvents[cscID]++;
  
  int NCFEBs = getNumCFEBs(cscID);
  if ((this->isME11(cscID) || emu::dqm::utils::isME11(cscID)) && n_ME11_TMB_DCFEBs > 0 )
    {
      NCFEBs = n_ME11_TMB_DCFEBs;
    } 
  const CSCTMBHeader * tmbHeader = data.tmbHeader();
  if (tmbHeader && nCSCEvents[cscID] < 2){
    int nCFEBs_TMBHeader = tmbHeader->NCFEBs();
    if (nCFEBs_TMBHeader < NCFEBs)
      {
	LOG4CPLUS_WARN(logger, "Trying "<< NCFEBs<<" NCFEBs, while TMB has only "<< tmbHeader->NCFEBs()<<" use n from TMB");
	NCFEBs = nCFEBs_TMBHeader;
      }
  }

  TestData2D& chamber_halfStrips = halfStrips[cscID];
  TestData2D& chamber_halfStripsMultiple = halfStripsMultiple[cscID];
  TestData2D& chamber_halfStripsFake = halfStripsFake[cscID];
  
  //  LOG4CPLUS_INFO(logger, "got data with size "<<data.size()<<" nclct "<<data.nclct()<<" nalct "<<data.nalct());

  int nCLCTs = data.nclct();
  if (nCLCTs == 0 )
    {
      LOG4CPLUS_ERROR(logger, "nCLCTs == 0 ");
      return;
    }
  CSCCLCTData * clctData = data.clctData(); 

  //figure out the expected halfstrip within one CFEB:
  int current_hstrip = (nCSCEvents[cscID]-1)/events_per_hstrip + hstrip_first; //counts start from 1
  if (current_hstrip< hstrip_first || current_hstrip > hstrips_per_run)
    {
      LOG4CPLUS_ERROR(logger, " unexpected half-strip index (" << current_hstrip << ")");
    }
  
  for (int icfeb=0; icfeb<NCFEBs; icfeb++)   // loop over cfebs in a given chamber
    {
      //layer-index of the current halfstrip: index starts from 0
      int icurrent_hstrip = current_hstrip -1 + icfeb*32; //counts start from 0

      for(int ilayer = 1; ilayer <= 6; ilayer++)		  
	{
	  chamber_halfStrips.cnts[ilayer-1][icurrent_hstrip]++;// shifts are annoying
	  chamber_halfStripsMultiple.cnts[ilayer-1][icurrent_hstrip]++;// shifts are annoying
	  int nOnCurrentStrip = 0;

	  std::vector<CSCComparatorDigi> comparatorDigis(clctData->comparatorDigis(ilayer, icfeb));
	  int nComparatorDigis = comparatorDigis.size();
	  //	  LOG4CPLUS_INFO(logger, "nComparatorDigis " << nComparatorDigis <<" for expected halfstrip "
	  //			 <<current_hstrip<<" layer "<<ilayer<<" cfeb "<<icfeb);
	  for (int iComp = 0; iComp< nComparatorDigis; ++iComp)
	    {
	      const CSCComparatorDigi& compDigi=comparatorDigis[iComp];
	      //	      LOG4CPLUS_INFO(logger, "iComp "<<iComp<<" strip "<< compDigi.getStrip()<<" comparator "<<compDigi.getComparator());
	      int compHalfStrip = (compDigi.getStrip()-1)*2+compDigi.getComparator();
	      //	      LOG4CPLUS_INFO(logger, "compHalfStrip "<<compHalfStrip<<" for "<<icurrent_hstrip);
	      if (icurrent_hstrip == compHalfStrip )
		{
		  nOnCurrentStrip++;
		} 
	      else 
		{
		  chamber_halfStripsFake.cnts[ilayer-1][compHalfStrip]++;
		  chamber_halfStripsFake.content[ilayer-1][compHalfStrip]+=1.0;
		}
	    }//iComp< nComparatorDigis
	  if (nOnCurrentStrip==1) chamber_halfStrips.content[ilayer-1][icurrent_hstrip]+=1.0;
	  if (nOnCurrentStrip>1)
	    {
	      LOG4CPLUS_INFO(logger, "multiple ( "<< nOnCurrentStrip<<" ) comparator digis on halfstrip "
			     <<icurrent_hstrip<<" ilayer "<<ilayer);
	      chamber_halfStripsMultiple.content[ilayer-1][icurrent_hstrip]+=1.*nOnCurrentStrip;
	    }
	  
	}//ilayer <= 6
    }//icfeb<NCFEBs
}


void Test_21_CFEBComparatorLogic::finishCSC(std::string cscID)
{
		  
  cscTestData::iterator td_itr =  tdata.find(cscID);
  TestData& cscdata = tdata[cscID];
  
  TestData2D& r01 = cscdata["R01"];

  TestData2D& chamber_halfStrips = halfStrips[cscID];

  for(int i = 0; i<TEST_DATA2D_NLAYERS; i++) 
    {
      for(int j = 0; j<TEST_DATA2D_NBINS; j++) 
	{
	  //fill the efficiency
	  int cnt = chamber_halfStrips.cnts[i][j];
	  double den = cnt;
	  double num = chamber_halfStrips.content[i][j];
	  double eff = den > 0 ? num/den : num > 0 ? 42.0 : 0.0;
	  r01.content[i][j] = eff;
	  r01.cnts[i][j] = cnt;
	}//j<TEST_DATA2D_NBINS
    }//i<TEST_DATA2D_NLAYERS
  
}

bool Test_21_CFEBComparatorLogic::checkResults(std::string cscID)
{
  return true;
}

void Test_21_CFEBComparatorLogic::setTestParams()
{

  LOG4CPLUS_INFO (logger, "Setting additional test parameters.");
  std::map<std::string, std::string>::iterator itr;
  
  itr = test_params.find("events_per_hstrip");
  if (itr != test_params.end() )
  {
    events_per_hstrip = atoi((itr->second).c_str());
    LOG4CPLUS_INFO (logger, "parameter: events_per_hstrip: " << events_per_hstrip);
  }
  itr = test_params.find("hstrips_per_run");
  if (itr != test_params.end() )
  {
    hstrips_per_run = atoi((itr->second).c_str());
    LOG4CPLUS_INFO (logger, "parameter: hstrips_per_run: " << hstrips_per_run);
  }
  itr = test_params.find("hstrip_step");
  if (itr != test_params.end() )
  {
    hstrip_step = atoi((itr->second).c_str());
    LOG4CPLUS_INFO (logger, "parameter: hstrip_step: " << hstrip_step);
  }
  itr = test_params.find("hstrip_first");
  if (itr != test_params.end() )
  {
    hstrip_first = atoi((itr->second).c_str());
    LOG4CPLUS_INFO (logger, "parameter: hstrip_first: " << hstrip_first);
  }
  
  itr = test_params.find("n_ME11_TMB_DCFEBs");
  if (itr != test_params.end() )
    {  
      n_ME11_TMB_DCFEBs = atoi((itr->second).c_str());
      LOG4CPLUS_INFO (logger, "parameter: n_ME11_TMB_DCFEBs: " << n_ME11_TMB_DCFEBs);
      if (n_ME11_TMB_DCFEBs > 7)
	{
	  LOG4CPLUS_ERROR(logger, "parameter: n_ME11_TMB_DCFEBs should be less than 7, default to max 7");
	  n_ME11_TMB_DCFEBs = 7;
	}
    }

  nExpectedEvents = events_per_hstrip * hstrips_per_run;
  LOG4CPLUS_INFO (logger, "nExpectedEvents: " << nExpectedEvents);

}
