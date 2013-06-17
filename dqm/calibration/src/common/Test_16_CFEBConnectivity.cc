#include "emu/dqm/calibration/Test_16_CFEBConnectivity.h"

using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;


Test_16_CFEBConnectivity::Test_16_CFEBConnectivity(std::string dfile):
  Test_Generic(dfile)
{
  testID = "16_CFEBConnectivity";
  //  binCheckMask=0x16CFF3F6;
  //  binCheckMask=0x1FEBF3F6;
  binCheckMask=0x16EBF7F6; // same value used in Lisa's test16
  //  binCheckMask=0xF7CB3BF6;
  logger = Logger::getInstance(testID);
  
  events_per_layer = 1000;
  nExpectedEvents = 3*events_per_layer;
  //events_per_layer is actually for pair of layers
  
  min_adc_hist = new TH1F("min adc","min_adc_hist",16,0.0,16.0);
  max_adc_hist = new TH1F("max adc","max_adc_hist",16,0.0,16.0);

}

Test_16_CFEBConnectivity::~Test_16_CFEBConnectivity()
{
	delete min_adc_hist;
	delete max_adc_hist;
}

void Test_16_CFEBConnectivity::initCSC(std::string cscID)
{

 
  //= Init per CSC events counters
  nCSCEvents[cscID]=0;

  //= Init data structures for storing SCA data and results
  if (adcSum2.find(cscID) == adcSum2.end())
  {
	  TimeBinsTestData empty;
	  adcSum2[cscID] = empty;
  }
  if (adcSum.find(cscID) == adcSum.end())
  {
  	  TimeBinsTestData empty;
  	  adcSum[cscID] = empty;
  }
  for (int t = 1; t<16; ++t) // no need to store time bin 0
  {
	  TestData2D tb_data;
	  tb_data.Nbins = getNumStrips(cscID);
	  tb_data.Nlayers = NLAYERS;
	  memset(tb_data.content, 0, sizeof (tb_data.content));
	  adcSum[cscID][t] = tb_data;
	  adcSum2[cscID][t] = tb_data;
  }

  TestData cscdata;
  TestData2D cfebdata;
  
  cfebdata.Nlayers = NLAYERS;
  
  
  //isME11() in emu::dqm::utils uses cscID.find("ME+1/1")
  // instead of cscID.find("ME+1.1") so it doesn't work
  bool isME11 = false;
  if ((cscID.find("ME+1.1") == 0) || (cscID.find("ME-1.1") ==0 )) {
    isME11 = true;
  }
 
  if(isME11) 
  {
	cout << "is ME11-type chamber" << endl;
	chamberSides = 2;
	cfebdata.Nbins = 64; //me11a
	cscdata["R01"] = cfebdata; //me11a
	
	cfebdata.Nbins = 48; //me11a
	cscdata["R02"] = cfebdata; //me11b
  
  } else {
	chamberSides = 1;
	cfebdata.Nbins = getNumStrips(cscID);
	cscdata["R01"] = cfebdata;
  }
  
  cfebdata.Nbins = getNumStrips(cscID);
  
  
  memset(cfebdata.content, 0, sizeof (cfebdata.content));
  memset(cfebdata.cnts, 0, sizeof (cfebdata.cnts));
  
  // Channels mask
  if (tmasks.find(cscID) != tmasks.end())
    {
      cscdata["_MASK"]=tmasks[cscID];
    }
  else
    {
      cscdata["_MASK"]=cfebdata;
    }

  for (int i=0; i<TEST_DATA2D_NLAYERS; i++)
    for (int j=0; j<TEST_DATA2D_NBINS; j++) cfebdata.content[i][j]=0.;


  


  tdata[cscID] = cscdata;

  bookTestsForCSC(cscID);
}

void Test_16_CFEBConnectivity::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
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


void Test_16_CFEBConnectivity::analyzeCSC(const CSCEventData& data)
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
  
  //cout << "cscID: " << cscID << endl;


  cscTestData::iterator td_itr = tdata.find(cscID);
  if ( (td_itr == tdata.end()) || (tdata.size() == 0) )
    {
      LOG4CPLUS_INFO(logger, "Found " << cscID);
      initCSC(cscID);
      addCSCtoMap(cscID, dmbHeader->crateID(), dmbHeader->dmbID());
    }
	
  	
  if(nCSCEvents[cscID] >= nExpectedEvents) {
	
      LOG4CPLUS_WARN(logger, nCSCEvents[cscID] << " exceeded expected number of events (" << nExpectedEvents << ")");
  }
  
  
  int layerpair = (nCSCEvents[cscID]) / events_per_layer;
  
  nCSCEvents[cscID]++;


  TimeBinsTestData& ch_adc_sum = adcSum[cscID];
  TimeBinsTestData& ch_adc_sum2 = adcSum2[cscID];
  
  
  int NCFEBs = getNumStrips(cscID)/16;
  
  if (dmbHeader->cfebAvailable())
    {
	  //for(int iside=0; iside < chamberSides; iside++) {
	  
		//if(iside == 0 && chamberSides == 2) NCFEBs = 4;
		//if(iside == 1 && chamberSides == 2) NCFEBs = 3;
		
		  for (int icfeb=0; icfeb<NCFEBs; icfeb++)   // loop over cfebs in a given chamber
		  {
			CSCCFEBData * cfebData =  data.cfebData(icfeb);
			if (!cfebData) continue;

			  for(int ilayer = 2*layerpair + 1; ilayer < 2*(layerpair+1) + 1; ilayer++)		  
			  {
			  
			  for (int istrip = 1; istrip <= 16; istrip++)   // loop over cfeb strips
			  {
				  int nTimeSamples= cfebData->nTimeSamples();
				  int adc_max = -1;
				  int adc_min = 4096;
				  
				  // loop for calculating min&max bins later
				  int jmin = -1, jmax = 16;
				  
				  
				  for (int j=0; j<nTimeSamples; j++)
				  {
					  CSCCFEBDataWord* timeSample=(cfebData->timeSlice(j))->timeSample(ilayer,istrip);
					 
					  
					  if(timeSample->adcCounts > adc_max)
						  {
							  adc_max = timeSample->adcCounts;
							  jmax = j;
						  }
						  if(timeSample->adcCounts < adc_min)
						  {
							  adc_min = timeSample->adcCounts;
							  jmin = j;
						  }
				  }

				  
				  min_adc_hist->Fill(jmin);
				  max_adc_hist->Fill(jmax);

				  // loop for storing the sums and sums of squares of counts above pedestal 
				  int pedestal = (cfebData->timeSlice(0))->timeSample(ilayer,istrip)->adcCounts;
				  //start at 1 instead of 0 since doing difference w.r.t bin 0
				  for (int j=1; j<nTimeSamples; j++)
				  {
						  int adc_count = (cfebData->timeSlice(j))->timeSample(ilayer,istrip)->adcCounts;
						  double count_diff = (double) adc_count - (double) pedestal;

										
						  int strip_idx = icfeb*16 + istrip - 1;
						  //cout << "count_diff" << count_diff << endl;
						  ch_adc_sum[j].content[ilayer-1][strip_idx] += count_diff;
						  ch_adc_sum[j].cnts[ilayer-1][strip_idx] += 1;
						  ch_adc_sum2[j].content[ilayer-1][strip_idx] += count_diff*count_diff;
						  ch_adc_sum2[j].cnts[ilayer-1][strip_idx] += 1;   

				  }          
			  }
			}
		  }
		//}
	}
}


void Test_16_CFEBConnectivity::finishCSC(std::string cscID)
{
		  
	cscTestData::iterator td_itr =  tdata.find(cscID);
	TestData& cscdata = tdata[cscID];
	
    TestData2D& r01 = cscdata["R01"];
	//if(chamberSides == 2) 
    TestData2D& r02 = cscdata["R02"]; 
    	
	// hist bin starts at 1, jmin/jmax at 0.
	int i_max = max_adc_hist->GetMaximumBin();
	int i_min = min_adc_hist->GetMaximumBin();

	int tsmax[2] = {i_max - 2, i_max + 2};
    if (tsmax[0] <= 0) tsmax[0] =  1;
    if (tsmax[1] > 16) tsmax[1] = 15;
    printf("Timesample with ADC min is %d, with ADC max is %d\n", tsmax[0],
           tsmax[1]);


	cout << "avg max/peak time bin: " << i_max << endl;
	cout << "avg min time bin: " << i_min << endl;
	
	double sum[TEST_DATA2D_NLAYERS][TEST_DATA2D_NBINS];
	double sumsq[TEST_DATA2D_NLAYERS][TEST_DATA2D_NBINS];
	int nevents[TEST_DATA2D_NLAYERS][TEST_DATA2D_NBINS];

	TimeBinsTestData& ch_adc_sum = adcSum[cscID];
	TimeBinsTestData& ch_adc_sum2 = adcSum2[cscID];

	for(int i = 0; i<TEST_DATA2D_NLAYERS; i++) {
		for(int j = 0; j<TEST_DATA2D_NBINS; j++) {
			sum[i][j]=0.;
			sumsq[i][j]=0.;
			nevents[i][j]=0;
		}
	}

	for (int ilayer=0; ilayer < TEST_DATA2D_NLAYERS; ++ilayer)
	{
		for (int istrip = 0; istrip < TEST_DATA2D_NBINS; ++istrip)
		{
			for (int i = tsmax[0]; i<=tsmax[1]; i++)
			{
				sum[ilayer][istrip] += ch_adc_sum[i].content[ilayer][istrip];
				sumsq[ilayer][istrip] += ch_adc_sum2[i].content[ilayer][istrip];
			
			}
			// can use index of tsmax[0] since all time bins carry same cnts
			nevents[ilayer][istrip] += ch_adc_sum[tsmax[0]].cnts[ilayer][istrip];
		}
	}

	
	// the stuff below is ported from test_16_finish
	int     ilayer, istrip, n, stripstart;
	float   avg, sigma_sq;

	int nstrips = ch_adc_sum[1].Nbins;
	double adc_diff[TEST_DATA2D_NLAYERS][TEST_DATA2D_NBINS];
	double adc_diff_err[TEST_DATA2D_NLAYERS][TEST_DATA2D_NBINS];

	
	/* Get average max response - min response */
	//cout << "ChamberSides " << chamberSides << endl;
	for(int iside=0; iside < chamberSides; iside++) {
		//cout << "iside " << iside << endl;
		if(iside == 0 && chamberSides == 2) nstrips = 4*16;
		if(iside == 1 && chamberSides == 2) nstrips = 3*16;
		
		for (ilayer = 0; ilayer < TEST_DATA2D_NLAYERS; ilayer++)
		{
			stripstart = 0;
			if(iside == 1) {
				stripstart = 4*16;
				nstrips = 3*16;
			}
			
			for (istrip = stripstart; istrip < stripstart+nstrips; istrip++)
			{
				if(iside == 0) r01.cnts[ilayer][istrip]++;
				else r02.cnts[ilayer][istrip-stripstart]++;
				
				if ((n = nevents[ilayer][istrip]) > 1) 
				{

					avg = sum[ilayer][istrip] / n;
					sigma_sq = (n / (n - 1)) * (sumsq[ilayer][istrip] / n - avg * avg);
					adc_diff[ilayer][istrip] = avg;
					adc_diff_err[ilayer][istrip] = sqrt(sigma_sq / n);
					
					if(iside == 0) r01.content[ilayer][istrip] = avg;
					else r02.content[ilayer][istrip-stripstart] = avg;
					
				}
				else if (n == 1)
				{
					adc_diff[ilayer][istrip]  = sum[ilayer][istrip];
					adc_diff_err[ilayer][istrip] = 10.;
					
					if(iside == 0) r01.content[ilayer][istrip] = sum[ilayer][istrip];
					else r02.content[ilayer][istrip-stripstart] = sum[ilayer][istrip];
					
				}
				else
				{
					adc_diff[ilayer][istrip] = -999.;
					adc_diff_err[ilayer][istrip] = 0.;
					
					if(iside == 0) r01.content[ilayer][istrip] = -999.;
					else r02.content[ilayer][istrip-stripstart] = -999.;
					
				}
			}
		}
	}
	
}

bool Test_16_CFEBConnectivity::checkResults(std::string cscID)
{
  return true;
}

void Test_16_CFEBConnectivity::setTestParams()
{
  LOG4CPLUS_INFO (logger, "Setting additional test parameters.");
  std::map<std::string, std::string>::iterator itr;
  itr = test_params.find("events_per_layer");
  if (itr != test_params.end() )
  {
    events_per_layer = atoi((itr->second).c_str());
    LOG4CPLUS_INFO (logger, "parameter: events_per_layer: " << events_per_layer);
	nExpectedEvents = 3*events_per_layer; //events_per_layer is actually for pair of layers
  }
	

}
