#include "Test_CFEB03.h"

using namespace XERCES_CPP_NAMESPACE;

Test_CFEB03::Test_CFEB03(std::string dfile): Test_Generic(dfile) {
  testID = "CFEB03";
  nExpectedEvents = 4000;
  dduID=0;
  binCheckMask=0xFFFB3BF6;
}


void Test_CFEB03::initCSC(std::string cscID) {
  nCSCEvents[cscID]=0;
  GainData gaindata;
  gaindata.Nbins = getNumStrips(cscID);
  gaindata.Nlayers = 6;
  memset(gaindata.content, 0, sizeof (gaindata.content));
  std::cout << "here " << sizeof (gaindata.content) << std::endl;
  
  gdata[cscID] = gaindata;
  std::cout << "here" << std::endl;

  TestData cscdata;
  TestData2D cfebdata;
  cfebdata.Nbins = getNumStrips(cscID);
  cfebdata.Nlayers = 6;	
  memset(cfebdata.content, 0, sizeof (cfebdata.content));
  memset(cfebdata.cnts, 0, sizeof (cfebdata.cnts));

  test_step tstep;
  tstep.active_strip=0;
  tstep.dac_step=1;
  tstep.evt_cnt=0;
  test_steps[cscID]=tstep;

  htree[dduID][cscID]=tstep;


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

void Test_CFEB03::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber) {
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
    //  std::cout << "Evt#" << std::dec << nTotalEvents << ": Nonzero Binary Errors Status is observed: 0x"<< std::hex << bin_checker.errors() << std::endl;
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

  fSwitch=false;
  /*
    for(std::vector<CSCEventData>::iterator chamberDataItr = chamberDatas.begin();
    chamberDataItr != chamberDatas.end(); ++chamberDataItr) {
    analyzeCSC(*chamberDataItr);
    }
  */

  if (chamberDatas.size() >0) {
    DDUstats[dduID].csc_evt_cntr++;
  } else {
    DDUstats[dduID].empty_evt_cntr++;
  }

  if (currL1A%1000==1)
    {
      // DDUstats[dduID].dac=(currL1A/50)%20;
      DDUstats[dduID].strip=currL1A/1000+1;
      // DDUstats[dduID].empty_evt_cntr=0;
      /*std::cout << "DDUEvt#" << std::dec << nTotalEvents << " " << DDUstats[dduID].csc_evt_cntr 
	<< ": DDU#" << dduID << " Switch strip:" << DDUstats[dduID].strip
        << " dac:" << DDUstats[dduID].dac << std::endl;
      */
    }
  if (currL1A%50==1) {
    DDUstats[dduID].dac=(currL1A/50)%10;
    DDUstats[dduID].strip=currL1A/1000+1;
    DDUstats[dduID].empty_evt_cntr=0;
    std::cout << "DDUEvt#" << std::dec << currL1A << " " << DDUstats[dduID].csc_evt_cntr << " " << DDUstats[dduID].empty_evt_cntr
	      << ": DDU#" << dduID << " Switch strip:" << DDUstats[dduID].strip
	      << " dac:" << DDUstats[dduID].dac << std::endl;
    fSwitch=true;
    std::map<std::string, test_step> & cscs = htree[dduID];
    for (std::map<std::string, test_step>::iterator itr = cscs.begin(); itr != cscs.end(); ++itr) {
      std::cout << itr->first << " " << itr->second.evt_cnt << " " << nCSCEvents[itr->first]<< std::endl;
      itr->second.evt_cnt = 0;
    }
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


void Test_CFEB03::analyzeCSC(const CSCEventData& data) {

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

  test_step& tstep = htree[dduID][cscID];// test_steps[cscID];

  GainData& gaindata = gdata[cscID];

  int curr_dac = DDUstats[dduID].dac;
  int curr_strip =  DDUstats[dduID].strip;
 

  if (fSwitch) {
    //	std::cout << "DDUEvt#" << std::dec << nTotalEvents << " " << nCSCEvents[cscID] << " " << cscID << " " 
    //               << " ("<< tstep.evt_cnt << ") "<< nCSCBadEvents[cscID] << std::endl;
    //	tstep.evt_cnt=0;
  }
  tstep.evt_cnt++;
  
  //  return;

  TH2F* v02 = reinterpret_cast<TH2F*>(cschistos["V02"]);
  TH2F* v03 = reinterpret_cast<TH2F*>(cschistos["V03"]);
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
	double Qmax=0;// gaindata.content[curr_dac][layer-1][icfeb*16+curr_strip-1].max;
	double Qmv=0;
        double Q12=((cfebData->timeSlice(0))->timeSample(layer,curr_strip)->adcCounts
		    + (cfebData->timeSlice(1))->timeSample(layer,curr_strip)->adcCounts)/2.;

	for (int itime=0;itime<nTimeSamples;itime++){ // loop over time samples (8 or 16)
          CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,curr_strip);

          int Qi = (int) ((timeSample->adcCounts)&0xFFF);
	  if (Qi>Qmax) {
                Qmax=Qi;
                if (curr_dac==DAC_STEPS-1) r04.content[layer-1][icfeb*16+curr_strip-1] = Qi;
          }
	  gaindata.content[curr_dac][layer-1][icfeb*16+curr_strip-1][itime].mv += Qi;
          gaindata.content[curr_dac][layer-1][icfeb*16+curr_strip-1][itime].rms += pow(Qi,2);
          gaindata.content[curr_dac][layer-1][icfeb*16+curr_strip-1][itime].cnt++;
        }

        if (v03) {v03->Fill(curr_dac, Qmax-Q12);}

        /*
	for (int itime=0;itime<nTimeSamples;itime++){ // loop over time samples (8 or 16)
	  CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,curr_strip);
	  
	  int Qi = (int) ((timeSample->adcCounts)&0xFFF);
	  Qmv+=Qi;

	  if (Qi>Qmax) { 
		Qmax=Qi;	    
	  	if (curr_dac==DAC_STEPS-1) r04.content[layer-1][icfeb*16+curr_strip-1] = Qi;
	  }

	}
	Qmv /=nTimeSamples;
	if (v03) {v03->Fill(curr_dac, Qmax-Q12);}
	gaindata.content[curr_dac][layer-1][icfeb*16+curr_strip-1].mv += Qmv;
	gaindata.content[curr_dac][layer-1][icfeb*16+curr_strip-1].rms += pow(Qmv,2);
        gaindata.content[curr_dac][layer-1][icfeb*16+curr_strip-1].cnt++;
	gaindata.content[curr_dac][layer-1][icfeb*16+curr_strip-1].max += Qmax;
	gaindata.content[curr_dac][layer-1][icfeb*16+curr_strip-1].max_rms += pow(Qmax,2);
	gaindata.content[curr_dac][layer-1][icfeb*16+curr_strip-1].max_cnt++;
        */


	if (curr_dac==5) {
	  //for(int strip = 1; strip <= 16; ++strip) { // loop over cfeb strips 
	    double Q12=((cfebData->timeSlice(0))->timeSample(layer,curr_strip)->adcCounts
			+ (cfebData->timeSlice(1))->timeSample(layer,curr_strip)->adcCounts)/2.;
	    int Qmax=0;
	    for (int itime=0;itime<nTimeSamples;itime++){ // loop over time samples (8 or 16)
	      CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,curr_strip);
	      int Qi = (int) ((timeSample->adcCounts)&0xFFF);
	      if (v02) { v02->Fill(itime, Qi-Q12);}
/*
	      if ((Qi)>QmaxArr[icfeb][layer-1][strip-1]) {
		QmaxArr[icfeb][layer-1][strip-1]=Qi;
		QmaxStrip[icfeb][layer-1]=strip;
	      }
*/
	    }
	  // }
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
      //std::cout << "DDUEvt#" << std::dec << nTotalEvents << " " << nCSCEvents[cscID] << " " << cscID << " DAC step switch from " << tstep.dac_step
      //        << " ("<< tstep.evt_cnt << ") "<< nCSCBadEvents[cscID] << std::endl;
      tstep.dac_step++;
      //	tstep.evt_cnt=1;
      tstep.max_adc=maxADC;		
    } else {
      tstep.evt_cnt++;
    }


    if ((maxStrip-1)==tstep.active_strip) {
      if (maxStrip>1) {
	//		std::cout << "DDUEvt#" << std::dec << currL1A << " " << nCSCEvents[cscID] << " " << cscID << " Strip switch from " << tstep.active_strip  
	//		<< " to " << maxStrip << " ("<< tstep.evt_cnt << ") "<< nCSCBadEvents[cscID] << std::endl;
      }
      tstep.active_strip=maxStrip;
      tstep.dac_step=1;
      //	tstep.evt_cnt=1;
      tstep.max_adc=0;
    } 
    return;
    if ((currL1A/50>0) && (currL1A%50==1)) {
      std::cout << "Evt#" << std::dec << nCSCEvents[cscID] << " " << cscID << " Setting switch" << std::endl;
    } 

  } // CFEB data available
  
}
/*
void Test_CFEB03::finish() {
  for (cscTestData::iterator td_itr = tdata.begin(); td_itr != tdata.end(); ++td_itr) {
    std::string cscID = td_itr->first;
    finishCSC(cscID);
  }
}
*/
void Test_CFEB03::finishCSC(std::string cscID) 
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

    //    CFEBSCAData& scadata = sdata[cscID];
    GainData& gaindata = gdata[cscID];
    MonHistos& cschistos = mhistos[cscID];
    TH1F* v01 = reinterpret_cast<TH1F*>(cschistos["V01"]);
//    TH2F* v03 = reinterpret_cast<TH2F*>(cschistos["V03"]);
    return;
    bool fValid=true;
    //    for (int icfeb=0; icfeb<getNumStrips(cscID)/16;icfeb++) { // loop over cfebs in a given chamber
    for (unsigned int layer = 1; layer <= 6; layer++){
      for (int icfeb=0; icfeb<getNumStrips(cscID)/16;icfeb++) { // loop over cfebs in a given chamber
	for(int strip = 1; strip <=16  ; ++strip) { // loop over cfeb strip	
	  for (int dac=0; dac<7; dac++) {
	    dac_step& val= gaindata.content[dac][layer-1][icfeb*16+strip-1][NSAMPLES-1];
	    double max=0;
	    double max_rms=0;
	    int cnt=0;
	    for (int itime=0; itime < NSAMPLES; itime++) {
		dac_step& cval = gaindata.content[dac][layer-1][icfeb*16+strip-1][itime];
		if (cval.cnt==0) {
			std::cout << cscID << ":" << layer << ":" << (icfeb*16+strip) << " dac=" << dac << ", Error counter=0" << std::endl;
	        	fValid=false;
		} else {
		   cval.mv /=cval.cnt;
	           double rms= sqrt((cval.rms/cval.cnt)-pow(cval.mv,2));
	           cval.rms = rms;
		   if (cval.mv > max) {
			max=cval.mv;
			max_rms=cval.rms;
			cnt=cval.cnt;
		   }
		}
            }
	
	    val.mv = max;
	    val.rms = max_rms;
	    val.cnt = cnt;
//	    dac_step& val= gaindata.content[dac][layer-1][icfeb*16+strip-1][NSAMPLES-1];
/*	    
	      std::cout << cscID << ":" << layer << ":" << (icfeb*16+strip) << " dac=" << dac << ", mv=" << val.mv 
	      << ", rms="  << val.rms << ", cnt="<< val.cnt << ", max=" 
	      << val.max << ", max_rms=" <<  val.max_rms << ", cnt=" << val.max_cnt << std::endl; 
*/	    
	    if (v01) { v01->Fill(cnt);}
	    if (cnt==0) {
	      std::cout << cscID << ":" << layer << ":" << (icfeb*16+strip) << " dac=" << dac << ", Error counter=0" << std::endl;
	      fValid=false;
	      // break;
		
            } else {
	      /*
	      val.mv /= val.cnt;
	      double rms= sqrt((val.rms/val.cnt)-pow(val.mv,2));
	      val.rms = rms;
	      val.max /= val.cnt;
	      double max_rms = sqrt((val.max_rms/val.cnt)-pow(val.max,2));
	      val.max_rms = max_rms;
	      */
	      val.s = pow(max_rms,2) + pow((0.01*max), 2);
	      
	    
	      	    std::cout << cscID << ":" << layer << ":" << (icfeb*16+strip) << " dac=" << dac << ", mv=" << val.mv << ", rms=" << val.rms << ", y=" 
	      		 << max << ", r=" <<  max_rms << ", s=" << val.s << " , x=" << (0.1+0.25*dac) << std::endl; 
	    }
	  }
	}
      }
    }
    if (fValid) {
      //    for (int icfeb=0; icfeb<getNumStrips(cscID)/16;icfeb++) { // loop over cfebs in a given chamber
      double avg_gain=0;
      int avg_gain_cnt=0;
      for (unsigned int layer = 1; layer <= 6; layer++){
	for (int icfeb=0; icfeb<getNumStrips(cscID)/16;icfeb++) { // loop over cfebs in a given chamber

	  for(int strip = 1; strip <= 16; ++strip) { // loop over cfeb strip
	    double X=0, XX=0, Y=0, YY=0, XY=0, S=0, x=0, y=0, s=0; 
	    double a=0, b=0, ksi=0;
	    for (int dac=0; dac<9; dac++) {
	      dac_step& val= gaindata.content[dac][layer-1][icfeb*16+strip-1][NSAMPLES-1];
	      // x=(0.1+0.25*dac);
	      x=11.2 +(28.0*dac);
	      y=val.mv;
	      s=val.s;
	      X+=x/s;
	      XX+=(x*x)/s;
	      Y+=y/s;
	      YY+=(y*y)/s;
	      XY+=(y*x)/s;
	      S+=1/s;
	    }
	    a=(XY*S-X*Y)/(XX*S-X*X);
	    b=(Y-a*X)/S;
	    avg_gain+=1/a;
	    avg_gain_cnt++;
	    
	    ksi=YY+a*a*XX+b*b*S-2*a*XY-2*b*Y+2*a*b*X;
	    std::cout << cscID << ":" << layer << ":" << (icfeb*16+strip) << " a=" << a << ", g=" << 1/a << ", b=" << b << ", ksi=" << ksi << std::endl;
	    r01.content[layer-1][icfeb*16+strip-1] = a;
	    r02.content[layer-1][icfeb*16+strip-1] = b;
	    r03.content[layer-1][icfeb*16+strip-1] = ksi;
//            r04.content[layer-1][icfeb*16+strip-1] = gaindata.content[14][layer-1][icfeb*16+strip-1].max;	    	    
	  }
	}
      }

      avg_gain/=avg_gain_cnt;
      for (unsigned int layer = 1; layer <= 6; layer++){
        for (int strip=0; strip<getNumStrips(cscID);strip++) { // loop over cfebs in a given chamber
		r05.content[layer-1][strip]=1/(r01.content[layer-1][strip]*avg_gain);
        }
      }

    } else {
      std::cout << cscID << ": Invalid" << std::endl;
    }
	
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
	/*
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
	*/
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

