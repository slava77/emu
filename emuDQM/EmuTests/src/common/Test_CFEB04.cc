#include "Test_CFEB04.h"

using namespace XERCES_CPP_NAMESPACE;

Test_CFEB04::Test_CFEB04(std::string dfile): Test_Generic(dfile) {
  testID = "CFEB04";
  nExpectedEvents = 25;
  dduID=0;
  binCheckMask=0xFFFB3BF6;
}


void Test_CFEB04::initCSC(std::string cscID) {
  nCSCEvents[cscID]=0;

  CFEBSCAData scadata;
  scadata.Nbins = getNumStrips(cscID);
  scadata.Nlayers = 6;
  memset(scadata.content, 0, sizeof (scadata.content));
  sdata[cscID] = scadata;

  TestData cscdata;
  TestData2D cfebdata;
  cfebdata.Nbins = getNumStrips(cscID);
  cfebdata.Nlayers = 6;	
  memset(cfebdata.content, 0, sizeof (cfebdata.content));
  memset(cfebdata.cnts, 0, sizeof (cfebdata.cnts));

  test_step tstep;
  tstep.active_strip=0;
  tstep.dac_step=1;
  tstep.evt_cnt=1;
  test_steps[cscID]=tstep;


  // Channels mask
  if (tmasks.find(cscID) != tmasks.end()) {
    cscdata["_MASK"]=tmasks[cscID];
  } else {
    cscdata["_MASK"]=cfebdata;
  }

  // mv0 - initial pedestals
  cscdata["_MV0"]=cfebdata;

  // rms0 - initial rms
  cscdata["_RMS0"]=cfebdata;

  // Q12
  cscdata["_Q12"]=cfebdata;

  // Q345
  cscdata["_Q345"]=cfebdata;

  // Q3
  cscdata["_Q3"]=cfebdata;

  // Q4
  cscdata["_Q4"]=cfebdata;

  // Q5
  cscdata["_Q5"]=cfebdata;

  // R01 - Gain slope A
  cscdata["R01"]=cfebdata;

  // R02 - Gain intercept B
  cscdata["R02"]=cfebdata;

  // R03 - Gain non-linearity
  cscdata["R03"]=cfebdata;

  // R04 - Saturation
  cscdata["R04"]=cfebdata;

  // R05 - Normalized gains for data analyses
  cscdata["R05"]=cfebdata;

  tdata[cscID] = cscdata;;

  bookTestsForCSC(cscID);
}

void Test_CFEB04::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber) {
  nTotalEvents++;

  //   uint32_t BinaryErrorStatus = 0, BinaryWarningStatus = 0;
  const uint16_t *tmp = reinterpret_cast<const uint16_t *>(data);
  if( bin_checker.check(tmp,dataSize/sizeof(short)) < 0 ){
    //   No ddu trailer found - force checker to summarize errors by adding artificial trailer
    const uint16_t dduTrailer[4] = { 0x8000, 0x8000, 0xFFFF, 0x8000 };
    tmp = dduTrailer; bin_checker.check(tmp,uint32_t(4));
  }

  /*if (bin_checker.errors() & 0x4 != 0) {
	// DDU Header Missing
	return;
  }
*/
  if (dduID != (bin_checker.dduSourceID()&0xFF)) {
	// std::cout << "Evt#" << std::dec << nTotalEvents << ": DDU#" << (bin_checker.dduSourceID()&0xFF) << std::endl;
        std::cout << "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << (bin_checker.dduSourceID()&0xFF) << " First event"<< std::endl;
	dduID = bin_checker.dduSourceID()&0xFF;
	dduL1A[dduID]=0;
	DDUstats[dduID].evt_cntr=0;
	DDUstats[dduID].first_l1a=-1;
	DDUstats[dduID].dac=0;
        DDUstats[dduID].strip=1;
  }

  dduID = bin_checker.dduSourceID()&0xFF;
  DDUstats[dduID].evt_cntr++;
  // dduL1A[dduID]++;
  // currL1A=dduL1A[dduID];
 
  if((bin_checker.errors() & binCheckMask)!= 0) {
    std::cout << "Evt#" << std::dec << nTotalEvents << ": Nonzero Binary Errors Status is observed: 0x"<< std::hex << bin_checker.errors() << std::endl;
    doBinCheck();
    return;
  }

  CSCDDUEventData dduData((uint16_t *) data);
 
  currL1A=(int)(dduData.header().lvl1num());
  if (DDUstats[dduID].evt_cntr ==1) {
	DDUstats[dduID].first_l1a = currL1A;
	std::cout << "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << dduID << " first l1a:" << DDUstats[dduID].first_l1a << std::endl;
  } else if (DDUstats[dduID].first_l1a==-1) {
	DDUstats[dduID].first_l1a = currL1A-DDUstats[dduID].evt_cntr+1;
	std::cout << "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << dduID << " first l1a :" << DDUstats[dduID].first_l1a << " after " << currL1A-DDUstats[dduID].evt_cntr << " bad events" << std::endl;
  }

  DDUstats[dduID].l1a_cntr=currL1A;

  if ((DDUstats[dduID].l1a_cntr-DDUstats[dduID].first_l1a) != (DDUstats[dduID].evt_cntr-1)) {
	std::cout << "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << dduID << " desynched l1a: " << ((DDUstats[dduID].l1a_cntr-DDUstats[dduID].first_l1a) - (DDUstats[dduID].evt_cntr-1)) << std::endl;
  }
  
//  std::cout << "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << dduID << " L1A:" << currL1A <<  " evt cntr:" << DDUstats[dduID].evt_cntr << std::endl;  
 
  std::vector<CSCEventData> chamberDatas;
  chamberDatas = dduData.cscData();

  if (chamberDatas.size() >0) {
        DDUstats[dduID].csc_evt_cntr++;
        if ((DDUstats[dduID].csc_evt_cntr%500==1))
  	{
        DDUstats[dduID].dac=(DDUstats[dduID].csc_evt_cntr/25)%20;
        DDUstats[dduID].strip=DDUstats[dduID].csc_evt_cntr/500+1;
        DDUstats[dduID].empty_evt_cntr=0;
        std::cout << "DDUEvt#" << std::dec << nTotalEvents << " " << DDUstats[dduID].csc_evt_cntr 
	<< ": DDU#" << dduID << " Switch strip:" << DDUstats[dduID].strip
        << " dac:" << DDUstats[dduID].dac << std::endl;
  	}

  } else {
        DDUstats[dduID].empty_evt_cntr++;
  }


  for(std::vector<CSCEventData>::iterator chamberDataItr = chamberDatas.begin(); 
      chamberDataItr != chamberDatas.end(); ++chamberDataItr) {
      analyzeCSC(*chamberDataItr);
  }
/* 
  if (chamberDatas.size() >0) {
	DDUstats[dduID].csc_evt_cntr++;
	if ((DDUstats[dduID].csc_evt_cntr/25>=1) && (DDUstats[dduID].csc_evt_cntr%25==0))
  {
        DDUstats[dduID].dac=(DDUstats[dduID].csc_evt_cntr/25)%20;
        DDUstats[dduID].strip=DDUstats[dduID].csc_evt_cntr/500+1;
        DDUstats[dduID].empty_evt_cntr=0;
        std::cout << "DDUEvt#" << std::dec << nTotalEvents << " " << DDUstats[dduID].csc_evt_cntr << ": DDU#" << dduID << " Switch strip:" << DDUstats[dduID].strip
        << " dac:" << DDUstats[dduID].dac << std::endl;
  }

  } else {
	DDUstats[dduID].empty_evt_cntr++;
  }
*/
  DDUstats[dduID].last_empty=chamberDatas.size();
	
}


void Test_CFEB04::analyzeCSC(const CSCEventData& data) {

  int conv_blk[16]={0,1,2,3,4,5,6,7,8,8,9,9,10,10,11,11}; // Mapping of SCA blocks 

  const CSCDMBHeader* dmbHeader = data.dmbHeader();
  const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();
  if (!dmbHeader && !dmbTrailer) {
        return;
  }


  int csctype=0, cscposition=0; 
  std::string cscID = getCSCFromMap(dmbHeader->crateID(), dmbHeader->dmbID(), csctype, cscposition); 
  // std::string cscID(Form("CSC_%03d_%02d", data.dmbHeader().crateID(), data.dmbHeader().dmbID()));
  // == Do not process unmapped CSCs
  if (cscID == "") return;

  cscTestData::iterator td_itr = tdata.find(cscID);
  if ( (td_itr == tdata.end()) || (tdata.size() == 0) ) {
    std::cout << "Found " << cscID << std::endl;
    initCSC(cscID);
    addCSCtoMap(cscID, dmbHeader->crateID(), dmbHeader->dmbID());
  }
  nCSCEvents[cscID]++;

  // == Define aliases to access chamber specific data
  uint32_t& nEvents=nCSCEvents[cscID];
  TestData& cscdata = tdata[cscID];
  TestData2D& _mv0 = cscdata["_MV0"];
  TestData2D& _rms0 = cscdata["_RMS0"];
  TestData2D& _q12 = cscdata["_Q12"];
  TestData2D& _q345 = cscdata["_Q345"];
  TestData2D& _q3 = cscdata["_Q3"];
  TestData2D& _q4 = cscdata["_Q4"];
  TestData2D& _q5 = cscdata["_Q5"];
  
  TestData2D& r01 = cscdata["R01"];
  TestData2D& r02 = cscdata["R02"];
  TestData2D& r03 = cscdata["R03"];
  TestData2D& r04 = cscdata["R04"];
  TestData2D& r05 = cscdata["R05"];

  CFEBSCAData& scadata = sdata[cscID];
  
  MonHistos& cschistos = mhistos[cscID];

  test_step& tstep = test_steps[cscID];

  TH2F* v02 = reinterpret_cast<TH2F*>(cschistos["V02"]);
  TH1F* v04 = reinterpret_cast<TH1F*>(cschistos["V04"]);

  int l1a_cnt = dmbHeader->l1a();

  if (v04) v04->Fill(l1a_cnt-l1a_cntrs[cscID]);

  l1a_cntrs[cscID]=l1a_cnt; 

//  return;


  

  int QmaxArr[5][6][16];
  int QmaxStrip[5][6];
  memset(QmaxArr, 0, sizeof (QmaxArr));
  memset(QmaxStrip, 0, sizeof (QmaxStrip));

  std::vector<int> stripQmax;
// == Check if CFEB Data Available 
  if (dmbHeader->cfebAvailable()){
    for (int icfeb=0; icfeb<getNumStrips(cscID)/16;icfeb++) { // loop over cfebs in a given chamber
      CSCCFEBData * cfebData =  data.cfebData(icfeb);
      if (!cfebData) continue;
      
      for (unsigned int layer = 1; layer <= 6; layer++){ // loop over layers in a given chamber
	int nTimeSamples= cfebData->nTimeSamples();
//	   int Qmax=0;
	for(int strip = 1; strip <= 16; ++strip) { // loop over cfeb strips 
	  double Q12=((cfebData->timeSlice(0))->timeSample(layer,strip)->adcCounts
                      + (cfebData->timeSlice(1))->timeSample(layer,strip)->adcCounts)/2.;
	  int Qmax=0;
	  for (int itime=0;itime<nTimeSamples;itime++){ // loop over time samples (8 or 16)
	    CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,strip);
	    int Qi = (int) ((timeSample->adcCounts)&0xFFF);
	    _mv0.content[layer-1][icfeb*16+strip-1] += Qi;
            _mv0.cnts[layer-1][icfeb*16+strip-1]++;
            _rms0.content[layer-1][icfeb*16+strip-1] += pow(Qi,2);
            _rms0.cnts[layer-1][icfeb*16+strip-1]++;

	    if ((Qi)>QmaxArr[icfeb][layer-1][strip-1]) {
		QmaxArr[icfeb][layer-1][strip-1]=Qi;
		QmaxStrip[icfeb][layer-1]=strip;
	    }
	  }
	}
      } 
    }
    int maxStrip=0;
    int maxADC=0;
    for (int icfeb=0; icfeb<getNumStrips(cscID)/16;icfeb++) { // loop over cfebs in a given chamber
	// std::cout <<std::dec << nCSCEvents[cscID] << " " << cscID << " " << icfeb <<": ";
      for (unsigned int layer = 1; layer <= 6; layer++){ // loop over layers in a given chamber
	// int maxStrip=QmaxStrip[icfeb][layer-1];
	// std::cout << maxstrip << "("<< QmaxArr[icfeb][layer-1][maxstrip-1] <<  "), ";
	for(int strip = 1; strip <= 16; ++strip) { // loop over cfeb strips
		if (QmaxArr[icfeb][layer-1][strip-1]>maxADC) {
			maxADC=QmaxArr[icfeb][layer-1][strip-1];
			maxStrip=strip;
		}
	}	
      }
	// std::cout << std::endl;
    }
    // std::cout <<std::dec << nCSCEvents[cscID] << " " << cscID << " " << maxStrip << "(" << maxADC << ")" << std::endl;
    if (maxADC-tstep.max_adc>50) {
	// std::cout << "DDUEvt#" << std::dec << nTotalEvents << " " << nCSCEvents[cscID] << " " << cscID << " DAC step switch from " << tstep.dac_step
        //        << " ("<< tstep.evt_cnt << ") "<< nCSCBadEvents[cscID] << std::endl;
	tstep.dac_step++;
	tstep.evt_cnt=1;
        tstep.max_adc=maxADC;		
    } else {
	tstep.evt_cnt++;
    }


    if ((maxStrip-1)==tstep.active_strip) {
	if (maxStrip>1) {
		std::cout << "DDUEvt#" << std::dec << nTotalEvents << " " << nCSCEvents[cscID] << " " << cscID << " Strip switch from " << tstep.active_strip  
		<< " to " << maxStrip << " ("<< tstep.evt_cnt << ") "<< nCSCBadEvents[cscID] << std::endl;
	}
	tstep.active_strip=maxStrip;
	tstep.dac_step=1;
	tstep.evt_cnt=1;
	tstep.max_adc=0;
    } 

    if ((currL1A/50>0) && (currL1A%50==1)) {
	std::cout << "Evt#" << std::dec << nCSCEvents[cscID] << " " << cscID << " Setting switch" << std::endl;
    } 

  } // CFEB data available
  
}


void Test_CFEB04::finishCSC(std::string cscID) 
{
  
  if (nCSCEvents[cscID] < nExpectedEvents/2) {
    std::cout << Form("%s: Not enough events for test analysis (%d events)", cscID.c_str(), nCSCEvents[cscID] ) << std::endl;
    // = Set error 
    return;
  }

  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end()) {
  
    TestData& cscdata= td_itr->second;

    // TestData2D& mask = cscdata["_MASK"];
    TestData2D& _q12 = cscdata["_Q12"];
    TestData2D& _q3 = cscdata["_Q3"];
    TestData2D& _q4 = cscdata["_Q4"];
    TestData2D& _q5 = cscdata["_Q5"];
  
    TestData2D& r01 = cscdata["R01"];
    TestData2D& r02 = cscdata["R02"];
    TestData2D& r03 = cscdata["R03"];
    TestData2D& r04 = cscdata["R04"];
    TestData2D& r05 = cscdata["R05"];

    CFEBSCAData& scadata = sdata[cscID];
    return;
    
    double rms = 0.;
    double covar = 0;

    for (int i=0; i<r01.Nlayers; i++) {
      for (int j=0; j<r01.Nbins; j++) {
	
	if (_q12.cnts[i][j]) _q12.content[i][j] /= (double)(_q12.cnts[i][j]);
	if (_q3.cnts[i][j]) _q3.content[i][j] /= (double)(_q3.cnts[i][j]);
	if (_q4.cnts[i][j]) _q4.content[i][j] /= (double)(_q4.cnts[i][j]);
	if (_q5.cnts[i][j]) _q5.content[i][j] /= (double)(_q5.cnts[i][j]);

	// == Calculate Overall pedestals and noise
	if (r01.cnts[i][j]) {
	  r01.content[i][j] /= (double)(r01.cnts[i][j]);

	  rms = sqrt( ((r02.content[i][j])/((double)(r02.cnts[i][j]))- pow(r01.content[i][j],2)) );
	  r02.content[i][j]=rms;
	}

	// == Calculate RMS of SCA pedestala
	double sca_mean=0.;
	double sca_mean_sum=0.;
	double sca_mean_sq_sum=0;
	int cells_cnt=0;
	for (int k=0; k<96;k++) {
	  if (scadata.content[i][j][k].cnt) {
	    cells_cnt++;
	    sca_mean=scadata.content[i][j][k].value / scadata.content[i][j][k].cnt;
	    sca_mean_sum+=sca_mean;
	    sca_mean_sq_sum+=pow(sca_mean,2);
	  }
	}
	sca_mean = sca_mean_sum/cells_cnt;
	rms = sqrt(sca_mean_sq_sum/cells_cnt - pow(sca_mean,2));

	// std::cout << Form("%s %d:%d %d %.2f",cscID.c_str(),i,j,cells_cnt, sca_mean) << std::endl;

	r03.content[i][j] = rms;

	// == Calculate time samples 1,2 pedestals and noise
	if (r04.cnts[i][j]) {
	  r04.content[i][j] /= (double)(r04.cnts[i][j]);

	  rms = sqrt( ((r05.content[i][j])/((double)(r05.cnts[i][j]))- pow(r04.content[i][j],2)) );
	  r05.content[i][j]=rms;
	}

      }
    }

    // == Save results to text files
    std::string rpath = "Test_"+testID+"/"+outDir;
    std::string path = rpath+"/"+cscID+"/";

  }
}

