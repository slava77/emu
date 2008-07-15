#include "Test_CFEB03.h"

using namespace XERCES_CPP_NAMESPACE;

Test_CFEB03::Test_CFEB03(std::string dfile): Test_Generic(dfile) {
  testID = "CFEB03";
  nExpectedEvents = 4000;
  dduID=0;
  binCheckMask=0x1FFB7BF6;
  ltc_bug=2;
}


void Test_CFEB03::initCSC(std::string cscID) {
  nCSCEvents[cscID]=0;
  XtalkData xtalkdata;
  xtalkdata.Nbins = getNumStrips(cscID);
  xtalkdata.Nlayers = 6;
  memset(xtalkdata.content, 0, sizeof (xtalkdata.content));
  
  xdata[cscID] = xtalkdata;

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

  // R01 - Pulse Maximum Amplitude
  cscdata["R01"]=cfebdata;

  // R02 - Pulse Peak position in Time
  cscdata["R02"]=cfebdata;

  // R03 - Pulse FHWM
  cscdata["R03"]=cfebdata;

  // R04 - Left Crosstalk
  cscdata["R04"]=cfebdata;

  // R05 - Right Crosstalk
  cscdata["R05"]=cfebdata;

  tdata[cscID] = cscdata;;

  bookTestsForCSC(cscID);
}

void Test_CFEB03::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber) {
  nTotalEvents++;

  const uint16_t *tmp = reinterpret_cast<const uint16_t *>(data);
  bin_checker.setMask( binCheckMask);
  if( bin_checker.check(tmp,dataSize/sizeof(short)) < 0 ){
    //   No ddu trailer found - force checker to summarize errors by adding artificial trailer
    const uint16_t dduTrailer[4] = { 0x8000, 0x8000, 0xFFFF, 0x8000 };
    tmp = dduTrailer; bin_checker.check(tmp,uint32_t(4));
  }

  if (dduID != (bin_checker.dduSourceID()&0xFF)) {

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
 
  if((bin_checker.errors() & binCheckMask)!= 0) {
    // std::cout << "Evt#" << std::dec << nTotalEvents << ": Nonzero Binary Errors Status is observed: 0x"<< std::hex << bin_checker.errors() << std::endl;
    doBinCheck();

  }

  CSCDDUEventData dduData((uint16_t *) data, &bin_checker);
 
  currL1A=(int)(dduData.header().lvl1num());
  if (DDUstats[dduID].evt_cntr ==1) {
    DDUstats[dduID].first_l1a = currL1A;
    std::cout << "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << dduID 
	      << " first l1a:" << DDUstats[dduID].first_l1a << std::endl;
  } else if (DDUstats[dduID].first_l1a==-1) {
    DDUstats[dduID].first_l1a = currL1A-DDUstats[dduID].evt_cntr+1;
    std::cout << "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << dduID 
	      << " first l1a :" << DDUstats[dduID].first_l1a << " after " 
	      << currL1A-DDUstats[dduID].evt_cntr << " bad events" << std::endl;
  }

  DDUstats[dduID].l1a_cntr=currL1A;

  if ((DDUstats[dduID].l1a_cntr-DDUstats[dduID].first_l1a) != (DDUstats[dduID].evt_cntr-1)) {
    std::cout << "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << dduID 
	      << " desynched l1a: " << ((DDUstats[dduID].l1a_cntr-DDUstats[dduID].first_l1a) - (DDUstats[dduID].evt_cntr-1)) << std::endl;
  }
  
  std::vector<CSCEventData> chamberDatas;
  chamberDatas = dduData.cscData();

  fSwitch=false;

  if (chamberDatas.size() >0) {
    DDUstats[dduID].csc_evt_cntr++;
  } else {
    DDUstats[dduID].empty_evt_cntr++;
  }

  // === set ltc_bug=2 in case of LTC double L1A bug
  // TODO: automatic detection of LTC L1A bug
  //  int ltc_bug=1;
  if ((DDUstats[dduID].evt_cntr == 8) && (DDUstats[dduID].empty_evt_cntr==0)) {
    std::cout << "No LTC/TTC double L1A bug in data" << std::endl;
    ltc_bug=1;
  }

  int dacSwitch=25*ltc_bug;
  int stripSwitch=250*ltc_bug;

  if (currL1A % stripSwitch==1)
    {
      DDUstats[dduID].strip=currL1A/ stripSwitch + 1;
    }
  if (currL1A% dacSwitch ==1) {
    DDUstats[dduID].dac=(currL1A / dacSwitch) % 10;
    DDUstats[dduID].strip=currL1A / stripSwitch + 1;
    DDUstats[dduID].empty_evt_cntr=0;

    fSwitch=true;
    std::map<std::string, test_step> & cscs = htree[dduID];
    for (std::map<std::string, test_step>::iterator itr = cscs.begin(); itr != cscs.end(); ++itr) {
      itr->second.evt_cnt = 0;
    }
  }

  for(std::vector<CSCEventData>::iterator chamberDataItr = chamberDatas.begin();
      chamberDataItr != chamberDatas.end(); ++chamberDataItr) {
    analyzeCSC(*chamberDataItr);
  }

  DDUstats[dduID].last_empty=chamberDatas.size();
	
}


void Test_CFEB03::analyzeCSC(const CSCEventData& data) 
{

  const CSCDMBHeader* dmbHeader = data.dmbHeader();
  const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();
  if (!dmbHeader && !dmbTrailer) {
    return;
  }

  int csctype=0, cscposition=0; 
  std::string cscID = getCSCFromMap(dmbHeader->crateID(), dmbHeader->dmbID(), csctype, cscposition); 

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

  TestData& cscdata = tdata[cscID];

  TestData2D& r04 = cscdata["R04"];
  
  MonHistos& cschistos = mhistos[cscID];

  test_step& tstep = htree[dduID][cscID];// test_steps[cscID];

  XtalkData& xtalkdata = xdata[cscID];

  int curr_dac = DDUstats[dduID].dac;
  int curr_strip =  DDUstats[dduID].strip;
 

  if (fSwitch) {
    //	std::cout << "DDUEvt#" << std::dec << nTotalEvents << " " << nCSCEvents[cscID] << " " << cscID << " " 
    //               << " ("<< tstep.evt_cnt << ") "<< nCSCBadEvents[cscID] << std::endl;
    //	tstep.evt_cnt=0;
  }
  tstep.evt_cnt++;
  

  TH2F* v02 = reinterpret_cast<TH2F*>(cschistos["V02"]);
  TH2F* v03 = reinterpret_cast<TH2F*>(cschistos["V03"]);
  TH1F* v04 = reinterpret_cast<TH1F*>(cschistos["V04"]);

  unsigned int l1a_cnt = dmbHeader->l1a();
  if (l1a_cnt < l1a_cntrs[cscID]) l1a_cnt+=256;

  if (v04) v04->Fill(l1a_cnt-l1a_cntrs[cscID]);

  l1a_cntrs[cscID]=dmbHeader->l1a(); 

  // == Check if CFEB Data Available 
  if (dmbHeader->cfebAvailable()){
    for (int icfeb=0; icfeb<getNumStrips(cscID)/16;icfeb++) { // loop over cfebs in a given chamber
      CSCCFEBData * cfebData =  data.cfebData(icfeb);
      if (!cfebData || !cfebData->check()) {
	 
	continue;
      }
      
      for (unsigned int layer = 1; layer <= 6; layer++){ // loop over layers in a given chamber
	int nTimeSamples= cfebData->nTimeSamples();
	double Qmax=xtalkdata.content[curr_dac][layer-1][icfeb*16+curr_strip-1][NSAMPLES-1].max;

	if (!cfebData->timeSlice(0)->checkCRC() || !cfebData->timeSlice(1)->checkCRC()) {
	  std::cout << cscID << " CRC check failed for time sample 1 and 2" << std::endl;
	  continue;
	}
        double Q12=((cfebData->timeSlice(0))->timeSample(layer,curr_strip)->adcCounts
		    + (cfebData->timeSlice(1))->timeSample(layer,curr_strip)->adcCounts)/2.;

	double Q12_left=0;
	double Q12_right=0;
	if ((icfeb*16+curr_strip-1) >= 1) {
		int cfeb = icfeb;	
		int strip = curr_strip -1;
		if (strip == 0) { cfeb -= 1; strip=16;}
		CSCCFEBData *  cfebData = data.cfebData(cfeb);
		Q12_left=((cfebData->timeSlice(0))->timeSample(layer,strip)->adcCounts
                    + (cfebData->timeSlice(1))->timeSample(layer,strip)->adcCounts)/2.;
	}

	if ((icfeb*16+curr_strip-1) < getNumStrips(cscID)-1 ) {
		int cfeb = icfeb;
		int strip = curr_strip+1;
                if (strip == 17) { cfeb += 1; strip =1;}
		CSCCFEBData * cfebData =  data.cfebData(cfeb);
		Q12_right=((cfebData->timeSlice(0))->timeSample(layer,strip)->adcCounts
                    + (cfebData->timeSlice(1))->timeSample(layer,strip)->adcCounts)/2.;
        }
        

	for (int itime=0;itime<nTimeSamples;itime++){ // loop over time samples (8 or 16)

	  if (!(cfebData->timeSlice(itime)->checkCRC())) {
	    std::cout << "Evt#" << std::dec << nCSCEvents[cscID] << ": " << cscID << " CRC failed, cfeb" << icfeb << ", layer" << layer << ", strip" << curr_strip << ", dac" << curr_dac << ", time sample " << itime << std::endl;
	    // continue; 
	  }
	  CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,curr_strip);	  
          int Qi = (int) ((timeSample->adcCounts)&0xFFF);
	  int Qi_left = 0;
	  int Qi_right = 0;

	  if ((icfeb*16+curr_strip-1) >= 1) {
		int cfeb = icfeb;
                int strip = curr_strip -1;
                if (strip == 0) { cfeb -= 1; strip=16;}
                CSCCFEBData *  cfebData = data.cfebData(cfeb);
		CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,strip);
	        Qi_left = (int) ((timeSample->adcCounts)&0xFFF);
		if (v03) { v03->Fill(itime, Qi_left-Q12_left);}

          }

	  if ((icfeb*16+curr_strip-1) < getNumStrips(cscID)-1 ) {
		int cfeb = icfeb;
                int strip = curr_strip+1;
                if (strip == 17) { cfeb += 1; strip =1;}
                CSCCFEBData * cfebData =  data.cfebData(cfeb);
                CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,strip);
                Qi_right = (int) ((timeSample->adcCounts)&0xFFF);
		if (v03) { v03->Fill(itime, Qi_right-Q12_right);}

          }

	  xtalkdata.content[curr_dac][layer-1][icfeb*16+curr_strip-1][itime].cnt++;
	  xtalkdata.content[curr_dac][layer-1][icfeb*16+curr_strip-1][itime].mv += Qi-Q12;
	  xtalkdata.content[curr_dac][layer-1][icfeb*16+curr_strip-1][itime].left += Qi_left-Q12_left;
	  xtalkdata.content[curr_dac][layer-1][icfeb*16+curr_strip-1][itime].right += Qi_right-Q12_right;
	
	  if (Qi-Q12>Qmax) {
            Qmax=Qi-Q12;
            if (curr_dac==TIME_STEPS-1) r04.content[layer-1][icfeb*16+curr_strip-1] = Qi;
            xtalkdata.content[curr_dac][layer-1][icfeb*16+curr_strip-1][NSAMPLES-1].max=Qmax;
          }
	  if (v02) { v02->Fill(itime, Qi-Q12);}


        }

	//    if (v03) {v03->Fill(curr_dac, Qmax);}
/*
	if (curr_dac==5) {
	  double Q12=((cfebData->timeSlice(0))->timeSample(layer,curr_strip)->adcCounts
		      + (cfebData->timeSlice(1))->timeSample(layer,curr_strip)->adcCounts)/2.;

	  for (int itime=0;itime<nTimeSamples;itime++){ // loop over time samples (8 or 16)
	    CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,curr_strip);
	    int Qi = (int) ((timeSample->adcCounts)&0xFFF);
	    if (v02) { v02->Fill(itime, Qi-Q12);}
	  }
	}
*/	

      } 
    }


  } // CFEB data available
  
}


void Test_CFEB03::finishCSC(std::string cscID) 
{
  /* 
     if (nCSCEvents[cscID] < nExpectedEvents/2) {
     std::cout << Form("%s: Not enough events for test analysis (%d events)", cscID.c_str(), nCSCEvents[cscID] ) << std::endl;
     // = Set error 
     return;
     }
  */
  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end()) {
  
    TestData& cscdata= td_itr->second;

    // TestData2D& mask = cscdata["_MASK"];
  
    TestData2D& r01 = cscdata["R01"];
    TestData2D& r02 = cscdata["R02"];
    TestData2D& r03 = cscdata["R03"];
    TestData2D& r04 = cscdata["R04"];
    TestData2D& r05 = cscdata["R05"];


    XtalkData& xtalkdata = xdata[cscID];
    MonHistos& cschistos = mhistos[cscID];
    TH2F* v01 = reinterpret_cast<TH2F*>(cschistos["V01"]);
    //    TH2F* v05 = reinterpret_cast<TH2F*>(cschistos["V05"]);
    // TH2F* v06 = reinterpret_cast<TH2F*>(cschistos["V06"]);


    CSCtoHWmap::iterator itr = cscmap.find(cscID);

    if (itr != cscmap.end()) {

      int dmbID = itr->second.second;
      if (dmbID >= 6) --dmbID;
      int id = 10*itr->second.first+dmbID;

      CSCMapItem::MapItem mapitem = cratemap->item(id);
      int first_strip_index=mapitem.stripIndex;
      int strips_per_layer=mapitem.strips;


      bool fValid=true;

      for (unsigned int layer = 1; layer <= 6; layer++){
	for (int icfeb=0; icfeb<getNumStrips(cscID)/16;icfeb++) { // loop over cfebs in a given chamber
	  for(int strip = 1; strip <=16  ; ++strip) { // loop over cfeb strip	
	    double max=0;
            double peak_time=0;
	    double max_left=0;
	    double max_right=0;
	    //	    time_step& val= xtalkdata.content[0][layer-1][icfeb*16+strip-1][NSAMPLES-1];
	    int cnt=0;	    
	    for (int itime=0; itime < NSAMPLES-1; itime++) {
	      for (int dac=0; dac<10; dac++) {

		time_step& cval = xtalkdata.content[dac][layer-1][icfeb*16+strip-1][itime];
		cnt = cval.cnt;

		if (cval.cnt<13) {
		  std::cout << cscID << ":" << layer << ":" << (icfeb*16+strip) 
			    << " Error> dac=" << dac << " , sample=" << itime << ", cnt="<< cval.cnt << std::endl;
		  fValid=false;
		} else {
		  cval.mv /=cval.cnt;
		  cval.left /=cval.cnt;
		  cval.right /=cval.cnt;
		  //		  double rms= sqrt((cval.rms/cval.cnt)-pow(cval.mv,2));
		  //cval.rms = rms;
		  if ((cval.mv > max) && (dac<7)) {
		    peak_time = itime*50. + dac*6.25;
		    max=cval.mv;
		    cnt=cval.cnt;
		  }

		  //cval.rms = rms;
                  if ((cval.left > max_left) ) {
                    max_left=cval.left;
                  }
		  //cval.rms = rms;
                  if ((cval.right > max_right) ) {
                    max_right=cval.right;
                  }

		}
		if (v01) { v01->Fill(dac,cnt);}

	      }
	    }
	    //val.mv = max;
	    // val.rms = max_rms;
	    // val.cnt = cnt;


	    

	    if (fValid) {
		
	      r01.content[layer-1][icfeb*16+strip-1] = max;
	      r02.content[layer-1][icfeb*16+strip-1] = peak_time;
	      r04.content[layer-1][icfeb*16+strip-1] = max_left/max;
	      r05.content[layer-1][icfeb*16+strip-1] = max_right/max;

	      int fhwm_left=max/2;
	      int fhwm_right=max/2;
	      double fhwm_left_time=0;
	      double fhwm_right_time=400;;
	      double fhwm=0;

	      for (int dac=0; dac<7; dac++) {
	        for (int itime=0; itime < NSAMPLES-1; itime++) {
                  time_step& cval = xtalkdata.content[dac][layer-1][icfeb*16+strip-1][itime];
                  double fhwm_time = itime*50. + dac*6.25;
                  if (fhwm_time < peak_time) {
                    if (abs(max/2-cval.mv) < fhwm_left) { 
                      fhwm_left = abs(max/2-cval.mv);
                      fhwm_left_time=fhwm_time;
                    }
                  } 
                }
              }


	      for (int dac=6; dac>=0; dac--) {
                for (int itime=NSAMPLES-2; itime>=0; itime--) {
                  time_step& cval = xtalkdata.content[dac][layer-1][icfeb*16+strip-1][itime];
                  double fhwm_time = itime*50. + dac*6.25;
		  if (fhwm_time > peak_time) {
                    if (abs(max/2-cval.mv) < fhwm_right) {
                      fhwm_right = abs(max/2-cval.mv);
                      fhwm_right_time=fhwm_time;
                    }
                  }
                }
              }

	      /*
	      for (int itime=0; itime < NSAMPLES-1; itime++) {
		for (int dac=0; dac<7; dac++) {
		  time_step& cval = xtalkdata.content[dac][layer-1][icfeb*16+strip-1][itime];
		  double fhwm_time = itime*50. + dac*6.25;
		  if (fhwm_time < peak_time) {
		    if (abs(max/2-cval.mv) < fhwm_left) { 
		      fhwm_left = abs(max/2-cval.mv);
		      fhwm_left_time=fhwm_time;
		    }
		  } else {
		    if (abs(max/2-cval.mv) < fhwm_right) { 
		      fhwm_right = abs(max/2-cval.mv);
		      fhwm_right_time=fhwm_time;
		    }
		  }
		}
	      }
	      */


	      if (fhwm_right_time > fhwm_left_time) fhwm = fhwm_right_time-fhwm_left_time;
	      r03.content[layer-1][icfeb*16+strip-1] = fhwm;
	
	      std::cout << cscID << ":" << std::dec << layer << ":" << (icfeb*16+strip) 
			<< " peak_amp=" << max << ", peak_time=" <<  peak_time << ", cnt=" << cnt 
			<< ", fhwm=" << fhwm << " l:" << fhwm_left_time << ":" << fhwm_left
			<< " r:" << fhwm_right_time <<":" << fhwm_right  
			<< ", leftXtalk=" << max_left/max << ", rightXtalk=" << max_right/max << std::endl;
	    }

	  }
	}
      }
      /*
	if (fValid) {

	for (unsigned int layer = 1; layer <= 6; layer++){
	for (int icfeb=0; icfeb<getNumStrips(cscID)/16;icfeb++) { // loop over cfebs in a given chamber
	for(int strip = 1; strip <= 16; ++strip) { // loop over cfeb strip

	bool fValidStrip=true;

	for (int dac=0; dac<6; dac++) {
	time_step& val= xtalkdata.content[dac][layer-1][icfeb*16+strip-1][NSAMPLES-1];

	}

	if (fValidStrip) {
	} else {
	}

	for (int dac=0; dac<6; dac++) {

	}

	r01.content[layer-1][icfeb*16+strip-1] = a;
	r02.content[layer-1][icfeb*16+strip-1] = b;
	r03.content[layer-1][icfeb*16+strip-1] = ksi;
	}
	}
	}
      */

      // == Save results to text files
      std::string rpath = "Test_"+testID+"/"+outDir;
      std::string path = rpath+"/"+cscID+"/";

      /*
	if (checkResults(cscID)) { // Check if 20% of channels pedestals and rms are bad
	// == Save results for database transfer Pedestals and RMS
	std::ofstream res_out((path+cscID+"_"+testID+"_DB.dat").c_str());

	for (int layer=0; layer<NLAYERS; layer++) {
	for (int strip=0; strip<strips_per_layer; strip++) {
	res_out << std::fixed << std::setprecision(2) <<  (first_strip_index+layer*strips_per_layer+strip) << "  "
	<< r01.content[layer][strip]  << "  " << r02.content[layer][strip] << "  " << r03.content[layer][strip] << std::endl;
	}
	}
	res_out.close();
	}
      */

    } else {
      std::cout << cscID << ": Invalid" << std::endl;
    }
  }
}

bool Test_CFEB03::checkResults(std::string cscID)
{
  bool isValid=true;
  cscTestData::iterator td_itr =  tdata.find(cscID);

  return isValid;

  if (td_itr != tdata.end()) {
    TestData& cscdata= td_itr->second;
    TestData2D& r04 = cscdata["R04"];
    TestData2D& r05 = cscdata["R05"];

    int badChannels=0;
    // Check pedestals
    for (int i=0; i<r04.Nlayers; i++) {
      for (int j=0; j<r04.Nbins; j++) {
	if ((r04.content[i][j] > 10) || (r04.content[i][j] < 3)) badChannels++;
      }
    }
    if (badChannels/(float(r04.Nlayers*r04.Nbins)) >=0.2) {
      isValid=false;
      std::cout << cscID << ": 20% of channels have bad Left Crosstalk" << std::endl;
    }

    badChannels=0;
    // Check noise
    for (int i=0; i<r05.Nlayers; i++) {
      for (int j=0; j<r05.Nbins; j++) {
	if ((r05.content[i][j] > 50) || (r05.content[i][j] < -50)) badChannels++;
      }
    }
    if (badChannels/(float(r05.Nlayers*r05.Nbins)) >=0.2) {
      isValid=false;
      std::cout << cscID << "20% of channels have bad Right Crosstalk" << std::endl;
    }
  }

  return isValid;
}



