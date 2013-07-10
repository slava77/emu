#include "emu/dqm/calibration/Test_11_AFEBNoise.h"
#include <boost/algorithm/string.hpp>


using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;
using namespace boost::algorithm;

Test_11_AFEBNoise::Test_11_AFEBNoise(std::string dfile): Test_Generic(dfile)
{
  testID    = "11_AFEBNoise";
  nExpectedEvents   = 10000;
  binCheckMask  = 0x1FEBF3F6;
  logger = Logger::getInstance(testID);
  
  duration_ms = 30000;
  
  if(dfile.find("3600V") != std::string::npos || dfile.find("3600_V") != std::string::npos) {
    voltageStr = " 3600V";
    LOG4CPLUS_INFO(logger, "Voltage found:" << voltageStr);
  } else if(dfile.find("3800V")  != std::string::npos|| dfile.find("3800_V") != std::string::npos) {
    voltageStr = " 3800V";
    LOG4CPLUS_INFO(logger, "Voltage found:" << voltageStr);
  } else {
    voltageStr = ""; 
  }
}

void Test_11_AFEBNoise::initCSC(std::string cscID)
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
  

  for (int i=0; i<TEST_DATA2D_NLAYERS; i++) {
    for (int j=0; j<TEST_DATA2D_NBINS; j++) {
      afebdata.content[i][j]=0.;
      afebdata.cnts[i][j]=0;
	}
  }


  cscdata["R01"]=afebdata; //cnts: alct wire occupancy
  cscdata["R02"]=afebdata; //cnts: isolated hit occupancy
  cscdata["R03"]=afebdata; //cnts: second tbins for separate tbins
  cscdata["R04"]=afebdata; //cnts: alct wire occupancy - first tbin
  cscdata["R05"]=afebdata; //cnts: second tbins for first tbin
  
  cscdata["R10"]=afebdata; //wire_array

  tdata[cscID] = cscdata;

  bookTestsForCSC(cscID);
   
}

void Test_11_AFEBNoise::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
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
    doBinCheck();
    //    return;
  }

  CSCDDUEventData dduData((uint16_t *) data, &bin_checker);

  std::vector<CSCEventData> chamberDatas;
  chamberDatas = dduData.cscData();
  CSCDDUHeader dduHeader  = dduData.header();



  int nCSCs = chamberDatas.size();

  //= Loop over found chambers and fill data structures
  for (int i=0; i < nCSCs; i++)
  {
    analyzeCSC(chamberDatas[i]);
  }


}


void Test_11_AFEBNoise::analyzeCSC(const CSCEventData& data)
{


  const CSCDMBHeader* dmbHeader = data.dmbHeader();
  const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();
  if (!dmbHeader && !dmbTrailer)
  {
    return;
  }
  int csctype=0, cscposition=0;
  std::string cscID = getCSCFromMap(dmbHeader->crateID(), dmbHeader->dmbID(), csctype, cscposition);
  if (cscID == "") return;

  cscTestData::iterator td_itr = tdata.find(cscID);
  if ( (td_itr == tdata.end()) || (tdata.size() == 0) )
  {
    LOG4CPLUS_INFO(logger, "Found " << cscID);
    initCSC(cscID);
    addCSCtoMap(cscID, dmbHeader->crateID(), dmbHeader->dmbID());
  }
  nCSCEvents[cscID]++;
  
  //unused  int evtNum = nCSCEvents[cscID];
  int nwires = getNumWireGroups(cscID);
  
  TestData& cscdata = tdata[cscID];
	
  TestData2D& r01 = cscdata["R01"];
  TestData2D& r02 = cscdata["R02"];
    
  TestData2D& r03 = cscdata["R03"];
  TestData2D& r04 = cscdata["R04"];
  TestData2D& r05 = cscdata["R05"];
    
  TestData2D& wire_array = cscdata["R10"];
  

  for(int i = 0; i < NLAYERS; i++) {
	for(int j = 0; j < nwires; j++) {
		wire_array.content[i][j] = -10;
	}
  }
        
  MonHistos& cschistos = mhistos[cscID];
  
  TH2F* v01 = reinterpret_cast<TH2F*>(cschistos["V01"]); //wire occupancy
  TH2F* v02 = reinterpret_cast<TH2F*>(cschistos["V02"]); //time bin occupancy
  if (data.nalct())
  {
    const CSCAnodeData* alctData = data.alctData();
    if (alctData)
    {
      for (int nLayer=1; nLayer<=6; nLayer++)
      {
		num_wires_hit[nLayer-1] = 0;
		
        vector<CSCWireDigi> wireDigis = alctData->wireDigis(nLayer);
        for (vector<CSCWireDigi>:: iterator wireDigisItr = wireDigis.begin();
             wireDigisItr != wireDigis.end(); ++wireDigisItr)
        {
          int wg = wireDigisItr->getWireGroup();
          v01->Fill(wg-1, nLayer-1);
          vector<int> tbins = wireDigisItr->getTimeBinsOn();
		  int disc_on = 0;
		  int disc_on_2 = 0;
          int prev_tbin = -1;
          for (uint32_t n=0; n < tbins.size(); n++)
          {
		  
			if ((prev_tbin >=0) && (tbins[n] != (prev_tbin+1)) )
				disc_on = 0;
			
			prev_tbin = tbins[n];
			

			if(disc_on_2 == 0) {
				disc_on_2 = 1;
				r04.cnts[nLayer-1][wg-1]++;

			} else {
				if(disc_on == 0) {
					/*cout << "evt" << evtNum << " afterpulses===="
					     << "layer " << nLayer << " wire " << wg
						 << " tbin " << tbins[n] << endl;*/
					 r05.cnts[nLayer-1][wg-1]++;
						 
				}
			}
			
			if(disc_on == 0) {
				disc_on = 1;
				v02->Fill(wg-1, (nLayer-1)*32+tbins[n]);
				r01.cnts[nLayer-1][wg-1]++;
			} else {
				r03.cnts[nLayer-1][wg-1]++;
			}
			
          }
		  	  
			wire_array.content[nLayer-1][num_wires_hit[nLayer-1]]=wg-1;
			num_wires_hit[nLayer-1]++; 
			last_wire[nLayer-1]=wg-1;

        }
      }
    }
	
	//r02 counts hits that are isolated
	bool startFill = false;
	for(int ilayer = 0; ilayer < NLAYERS; ilayer++) {
	
		if(num_wires_hit[ilayer] == 1) {
			startFill = true;
			r02.cnts[ilayer][last_wire[ilayer]]++;
		}
		else if(num_wires_hit[ilayer] > 1) {
		
			for(int iwire = 0; iwire < num_wires_hit[ilayer]; iwire++)
			{
				if(iwire==0)
				{
					if(wire_array.content[ilayer][iwire]!=(wire_array.content[ilayer][iwire+1]-1))
					{
						if(startFill) r02.cnts[ilayer][(int)wire_array.content[ilayer][iwire]]++;
					}
				}
				else if(iwire==(num_wires_hit[ilayer]-1)) 
				{
					if(wire_array.content[ilayer][iwire]!=(wire_array.content[ilayer][iwire-1]+1))  
					{
						if(startFill) r02.cnts[ilayer][(int)wire_array.content[ilayer][iwire]]++;
					}
				}
				else
				{
					if(  wire_array.content[ilayer][iwire] !=
					    (wire_array.content[ilayer][iwire+1]-1)
					  && wire_array.content[ilayer][iwire] !=
					    (wire_array.content[ilayer][iwire-1]+1)  ) 
					{
						if(startFill) r02.cnts[ilayer][(int)wire_array.content[ilayer][iwire]]++;
						
					} 
				}
			}
		}
	}

  }
}


void Test_11_AFEBNoise::finishCSC(std::string cscID)
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
  
    cout << "duration_ms " << duration_ms << endl;

    TestData& cscdata= td_itr->second;
    TestData2D& r01 = cscdata["R01"]; // rate1 in LTM
    TestData2D& r02 = cscdata["R02"]; // rate3
    TestData2D& r03 = cscdata["R03"];
    TestData2D& r04 = cscdata["R04"]; // rate_1
    TestData2D& r05 = cscdata["R05"]; // rate_4
    //singles rate
	for (int ilayer = 0; ilayer < NLAYERS; ilayer++) {
		for (int iwire = 0; iwire < getNumWireGroups(cscID); iwire++) {
		
			r01.content[ilayer][iwire] = ((float)r01.cnts[ilayer][iwire] / ((float)duration_ms * 1E-3));
		}
	}
	
	//isolated hit probability
	for (int ilayer = 0; ilayer < NLAYERS; ilayer++) {
		for (int iwire = 0; iwire < getNumWireGroups(cscID); iwire++) {
			
			r02.content[ilayer][iwire] = ((float)r02.cnts[ilayer][iwire] / ((float)duration_ms * 1E-3));
			
			if(r01.content[ilayer][iwire] > 0) {
				r02.content[ilayer][iwire] /= r01.content[ilayer][iwire];
			} else {
				r02.content[ilayer][iwire] = -99.;
			}
		}
	}
	
	//afterpulsing probability
	for (int ilayer = 0; ilayer < NLAYERS; ilayer++) {
		for (int iwire = 0; iwire < getNumWireGroups(cscID); iwire++) {
		
			r04.content[ilayer][iwire] = ((float)r04.cnts[ilayer][iwire] / ((float)duration_ms * 1E-3));
			
			if(r04.content[ilayer][iwire] > 0) {
				r03.content[ilayer][iwire] = (float)r05.cnts[ilayer][iwire] / (float)r04.cnts[ilayer][iwire];
			} else {
				r03.content[ilayer][iwire] = -99.;
			}
		}
	}
	
  }

  TestCanvases& cnvs = tcnvs[cscID];
  std::string resCodeStrs[3] = {"R01", "R02", "R03"};
  for(int i = 0; i < 3; i++) {
    TestCanvases::iterator c_itr = cnvs.find(resCodeStrs[i]);
    if (c_itr != cnvs.end() ) {
      TestCanvas_6gr1h* cnv = dynamic_cast<TestCanvas_6gr1h*>(c_itr->second);
      if (cnv != NULL) cnv->SetTitle(cnv->GetTitle() + voltageStr);
    }
  }

}

bool Test_11_AFEBNoise::checkResults(std::string cscID)
{
  bool isValid=true;
  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end())
  {
  
  }

  return isValid;
}

void Test_11_AFEBNoise::setTestParams()
{
  LOG4CPLUS_INFO (logger, "Setting additional test parameters.");
  std::map<std::string, std::string>::iterator itr;
  itr = test_params.find("duration_ms");
  if (itr != test_params.end() )
  {
    duration_ms = atoi((itr->second).c_str());
    LOG4CPLUS_INFO (logger, "parameter: duration_ms: " << duration_ms);
  }
	

}
