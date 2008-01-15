#include "Test_CFEB02.h"

Test_CFEB02::Test_CFEB02(std::string dfile): dataFile(dfile) {
  init();
  nExpectedEvents = 10000;
}

void Test_CFEB02::init() {
  nTotalEvents = 0;
  nBadEvents = 0;
  bin_checker.output1().hide();
  bin_checker.output2().hide();
  bin_checker.crcALCT(true);
  bin_checker.crcTMB (true);
  bin_checker.crcCFEB(true);
  bin_checker.modeDDU(true);
}

void Test_CFEB02::setCSCMapFile(std::string filename)
{
        if (filename != "") {
                cscMapFile = filename;
                cscMapping  = CSCReadoutMappingFromFile(cscMapFile);
        }


}

std::string Test_CFEB02::getCSCTypeLabel(int endcap, int station, int ring )
{
        std::string label = "Unknown";
        std::ostringstream st;
        if ((endcap > 0) && (station>0) && (ring>0)) {
                if (endcap==1) {
                        st << "ME+" << station << "." << ring;
                        label = st.str();
                } else if (endcap==2) {
                        st << "ME-" << station << "." << ring;
                        label = st.str();
                } else {
                        label = "Unknown";
                }
        }
        return label;
}


std::string Test_CFEB02::getCSCFromMap(int crate, int slot)
{
  int iendcap = -1;
  int istation = -1;
  int iring = -1;
  int cscposition = -1;

  int id = cscMapping.chamber(iendcap, istation, crate, slot, -1);
  if (id==0) {
        return "";
  }
  CSCDetId cid( id );
  iendcap = cid.endcap();
  istation = cid.station();
  iring = cid.ring();
  cscposition = cid.chamber();

  std::string tlabel = getCSCTypeLabel(iendcap, istation, iring );
  return tlabel+"."+Form("%02d", cscposition);
}


TestData Test_CFEB02::initCSC(std::string cscID) {
  nCSCEvents[cscID]=0;

  CFEBSCAData scadata;
  scadata.Nbins = 80;
  scadata.Nlayers = 6;
  memset(scadata.content, 0, sizeof (scadata.content));
  sdata[cscID] = scadata;

  TestData cscdata;
  CFEBData cfebdata;
  cfebdata.Nbins = 80;
  cfebdata.Nlayers = 6;	
  memset(cfebdata.content, 0, sizeof (cfebdata.content));
  memset(cfebdata.cnts, 0, sizeof (cfebdata.cnts));

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

  // R01 - Overall pedestal
  cscdata["R01"]=cfebdata;

  // R02 - Overall noise
  cscdata["R02"]=cfebdata;

  // R03 - P12 pedestal
  cscdata["R03"]=cfebdata;

  // R04 - P12 noise
  cscdata["R04"]=cfebdata;

  // R05 - P345 pedestal
  cscdata["R05"]=cfebdata;

  // R06 - P345 noise
  cscdata["R06"]=cfebdata;
  
  // R07 - C(12)(12) covariance matrix element
  cscdata["R07"]=cfebdata;
  
  // R08 - C(12)3 covariance matrix element
  cscdata["R08"]=cfebdata;
 
  // R09 - C(12)4 covariance matrix element
  cscdata["R09"]=cfebdata;
  
  // R10 - C(12)5 covariance matrix element
  cscdata["R10"]=cfebdata;

  // R11 - C33 covariance matrix element
  cscdata["R11"]=cfebdata;

  // R12 - C34 covariance matrix element
  cscdata["R12"]=cfebdata;

  // R13 - C35 covariance matrix element
  cscdata["R13"]=cfebdata;

  // R14 - C44 covariance matrix element
  cscdata["R14"]=cfebdata;

  // R15 - C45 covariance matrix element
  cscdata["R15"]=cfebdata;

  // R16 - C45 covariance matrix element
  cscdata["R16"]=cfebdata;

  // R17 - RMS of SCA pedestals
  cscdata["R17"]=cfebdata;
  
  return cscdata;
}

MonHistos Test_CFEB02::bookMonHistosCSC(std::string cscID) {
  MonHistos cschistos;
  cschistos["V01"] = new TH2F((cscID+"_CFEB02_V01").c_str(), "Signal line", 16, 0, 16, 80, -20, 20);
  cschistos["V01"]->SetOption("colz");
  cschistos["V02"] = new TH1F((cscID+"_CFEB02_V02").c_str(), "Q4 with dynamic ped substraction", 40, -20, 20);
  cschistos["V02"]->SetFillColor(48);

  cschistos["V03"] = new TProfile((cscID+"_CFEB02_V03").c_str(), "SCA Cells Occupancy", 96, 0, 96);
  cschistos["V04"] = new TH1F((cscID+"_CFEB02_V04").c_str(), "SCA Cell Occupancy", 96, 0, 96);
  cschistos["V04"]->SetFillColor(48);
  cschistos["V05"] = new TH1F((cscID+"_CFEB02_V05").c_str(), "SCA Block Occupancy", 12, 0, 12);
  cschistos["V05"]->SetFillColor(48);
  return cschistos;
}


void Test_CFEB02::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber) {
  nTotalEvents++;

//   uint32_t BinaryErrorStatus = 0, BinaryWarningStatus = 0;
  const uint16_t *tmp = reinterpret_cast<const uint16_t *>(data);
  if( bin_checker.check(tmp,dataSize/sizeof(short)) < 0 ){
    //   No ddu trailer found - force checker to summarize errors by adding artificial trailer
    const uint16_t dduTrailer[4] = { 0x8000, 0x8000, 0xFFFF, 0x8000 };
    tmp = dduTrailer; bin_checker.check(tmp,uint32_t(4));
  }

  if(bin_checker.errors() != 0) {
	std::cout << "Evt#" << std::dec << nTotalEvents << ": Nonzero Binary Errors Status is observed: 0x"<< std::hex << bin_checker.errors() << std::endl;
	return;
  }

  CSCDDUEventData dduData((uint16_t *) data);

  std::vector<CSCEventData> chamberDatas;
  chamberDatas = dduData.cscData();

  for(std::vector<CSCEventData>::iterator chamberDataItr = chamberDatas.begin(); 
      chamberDataItr != chamberDatas.end(); ++chamberDataItr) {
    analyzeCSC(*chamberDataItr);
  }
	
}

void Test_CFEB02::analyzeCSC(const CSCEventData& data) {
  int conv_blk[16]={0,1,2,3,4,5,6,7,8,8,9,9,10,10,11,11};
  std::string cscID = getCSCFromMap(data.dmbHeader().crateID(), data.dmbHeader().dmbID());
  
  // std::string cscID(Form("CSC_%03d_%02d", data.dmbHeader().crateID(), data.dmbHeader().dmbID()));
  cscTestData::iterator td_itr = tdata.find(cscID);
  if ( (td_itr == tdata.end()) || (tdata.size() == 0) ) {
    std::cout << "Found " << cscID << std::endl;
    tdata[cscID] = initCSC(cscID);
    mhistos[cscID] = bookMonHistosCSC(cscID);
  }
  nCSCEvents[cscID]++;
  uint32_t& nEvents=nCSCEvents[cscID];

  TestData& cscdata = tdata[cscID];
  CFEBData& _mv0 = cscdata["_MV0"];
  CFEBData& _rms0 = cscdata["_RMS0"];
  CFEBData& _q12 = cscdata["_Q12"];
  CFEBData& _q345 = cscdata["_Q345"];
  CFEBData& _q3 = cscdata["_Q3"];
  CFEBData& _q4 = cscdata["_Q4"];
  CFEBData& _q5 = cscdata["_Q5"];
  
  CFEBData& r01 = cscdata["R01"];
  CFEBData& r02 = cscdata["R02"];
  //  CFEBData& r03 = cscdata["R03"];
  CFEBData& r04 = cscdata["R04"];
  CFEBData& r05 = cscdata["R05"];
  CFEBData& r06 = cscdata["R06"];
  CFEBData& r07 = cscdata["R07"];
  CFEBData& r08 = cscdata["R08"];
  CFEBData& r09 = cscdata["R09"];
  CFEBData& r10 = cscdata["R10"];
  CFEBData& r11 = cscdata["R11"];
  CFEBData& r12 = cscdata["R12"];
  CFEBData& r13 = cscdata["R13"];
  CFEBData& r14 = cscdata["R14"];
  CFEBData& r15 = cscdata["R15"];
  CFEBData& r16 = cscdata["R16"];
  CFEBData& r17 = cscdata["R17"];

  CFEBSCAData& scadata = sdata[cscID];
  
  MonHistos& cschistos = mhistos[cscID];
  TH2F* v01 = reinterpret_cast<TH2F*>(cschistos["V01"]);
  TH1F* v02 = reinterpret_cast<TH1F*>(cschistos["V02"]);
  TProfile* v03 = reinterpret_cast<TProfile*>(cschistos["V03"]);
  TH1F* v04 = reinterpret_cast<TH1F*>(cschistos["V04"]);
  TH1F* v05 = reinterpret_cast<TH1F*>(cschistos["V05"]);

  if (data.dmbHeader().cfebAvailable()){
    for (int icfeb=0; icfeb<5;icfeb++) {//loop over cfebs in a given chamber
      CSCCFEBData * cfebData =  data.cfebData(icfeb);
      if (!cfebData) continue;

      for (unsigned int layer = 1; layer <= 6; layer++){
	int nTimeSamples= cfebData->nTimeSamples();
	for(int strip = 1; strip <= 16; ++strip) {
	  double Q12=((cfebData->timeSlice(0))->timeSample(layer,strip)->adcCounts 
		      + (cfebData->timeSlice(1))->timeSample(layer,strip)->adcCounts)/2.;
	  double Q4 = (cfebData->timeSlice(3))->timeSample(layer,strip)->adcCounts;
	  v02->Fill(Q4-Q12);

	  int offset=0;
	  int blk_strt=0;
	  int cap=0;

	  for (int itime=0;itime<nTimeSamples;itime++){
	    CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,strip);
	    int Qi = (int) ((timeSample->adcCounts)&0xFFF);
	    v01->Fill(itime, Qi-Q12);

	    if (nEvents <= 1000) {
	      _mv0.content[layer-1][icfeb*16+strip-1] += Qi;
	      _mv0.cnts[layer-1][icfeb*16+strip-1]++;
	      _rms0.content[layer-1][icfeb*16+strip-1] += pow(Qi,2);
              _rms0.cnts[layer-1][icfeb*16+strip-1]++;
	    }
	  }
	  if (nEvents > 1000) {
	    bool fEventValid = true;
	    double mv0 = _mv0.content[layer-1][icfeb*16+strip-1];
	    double rms0 = _rms0.content[layer-1][icfeb*16+strip-1];
	    // == Check that charges in first 5 timebins satisfy |Qi - mv0| < 3*rms0
	    for (int itime=0;itime<nTimeSamples;itime++){
	      CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,strip);
	      int ADC = (int) ((timeSample->adcCounts)&0xFFF);
	      if ( fabs(ADC-mv0) >= 3*rms0) { 
		fEventValid=false; 
		/*std::cout << "Evt" <<nEvents << " " << cscID << " " << layer << ":"<< (icfeb*16+strip) << " skipped " 
		  <<  fabs(ADC-mv0) << ">=" << (3*rms0) <<  std::endl;
		*/
		break;
	      }
	    }
	    if (fEventValid) {
	      double Q12=((cfebData->timeSlice(0))->timeSample(layer,strip)->adcCounts 
			  + (cfebData->timeSlice(1))->timeSample(layer,strip)->adcCounts)/2.;
	      double Q345=((cfebData->timeSlice(2))->timeSample(layer,strip)->adcCounts 
			   + (cfebData->timeSlice(3))->timeSample(layer,strip)->adcCounts
			   + (cfebData->timeSlice(4))->timeSample(layer,strip)->adcCounts)/3.;
	      double Q3 = (cfebData->timeSlice(2))->timeSample(layer,strip)->adcCounts;
	      double Q4 = (cfebData->timeSlice(3))->timeSample(layer,strip)->adcCounts;
	      double Q5 = (cfebData->timeSlice(4))->timeSample(layer,strip)->adcCounts;

	      _q12.content[layer-1][icfeb*16+strip-1] += Q12;
	      _q12.cnts[layer-1][icfeb*16+strip-1]++;
		 
	      _q345.content[layer-1][icfeb*16+strip-1] += Q345;
	      _q345.cnts[layer-1][icfeb*16+strip-1]++;

	      _q3.content[layer-1][icfeb*16+strip-1] += Q3;
	      _q3.cnts[layer-1][icfeb*16+strip-1]++;
		 
	      _q4.content[layer-1][icfeb*16+strip-1] += Q4;
	      _q4.cnts[layer-1][icfeb*16+strip-1]++;
		 
	      _q5.content[layer-1][icfeb*16+strip-1] += Q5;
	      _q5.cnts[layer-1][icfeb*16+strip-1]++;

	      r04.content[layer-1][icfeb*16+strip-1] += Q12;
	      r04.cnts[layer-1][icfeb*16+strip-1]++;

	      r05.content[layer-1][icfeb*16+strip-1] += pow(Q12,2);
	      r05.cnts[layer-1][icfeb*16+strip-1]++;

	      r06.content[layer-1][icfeb*16+strip-1] += Q345;
	      r06.cnts[layer-1][icfeb*16+strip-1]++;

	      r07.content[layer-1][icfeb*16+strip-1] += pow(Q345,2);
	      r07.cnts[layer-1][icfeb*16+strip-1]++;
		 
	      r08.content[layer-1][icfeb*16+strip-1] += Q12*Q12;
	      r08.cnts[layer-1][icfeb*16+strip-1]++;

	      r09.content[layer-1][icfeb*16+strip-1] += Q12*Q3;
	      r09.cnts[layer-1][icfeb*16+strip-1]++;

	      r10.content[layer-1][icfeb*16+strip-1] += Q12*Q4;
	      r10.cnts[layer-1][icfeb*16+strip-1]++;

	      r11.content[layer-1][icfeb*16+strip-1] += Q12*Q5;
	      r11.cnts[layer-1][icfeb*16+strip-1]++;

	      r12.content[layer-1][icfeb*16+strip-1] += Q3*Q3;
	      r12.cnts[layer-1][icfeb*16+strip-1]++;

	      r13.content[layer-1][icfeb*16+strip-1] += Q3*Q4;
	      r13.cnts[layer-1][icfeb*16+strip-1]++;

	      r14.content[layer-1][icfeb*16+strip-1] += Q3*Q5;
	      r14.cnts[layer-1][icfeb*16+strip-1]++;

	      r15.content[layer-1][icfeb*16+strip-1] += Q4*Q4;
	      r15.cnts[layer-1][icfeb*16+strip-1]++;
		 
	      r16.content[layer-1][icfeb*16+strip-1] += Q4*Q5;
	      r16.cnts[layer-1][icfeb*16+strip-1]++;

	      r17.content[layer-1][icfeb*16+strip-1] += Q5*Q5;
	      r17.cnts[layer-1][icfeb*16+strip-1]++;

		/*
	      offset = 0;
	      // == Find offset in sca block
	      for (int itime=0;itime<nTimeSamples-1;itime++){
		if ( (cfebData->timeSlice(itime+1))->scaControllerWord(layer).sca_blk != 
		     (cfebData->timeSlice(itime))->scaControllerWord(layer).sca_blk )  
		{
		    std::cout << Form("%d:%d block %d change in sample %d", 
				    layer, icfeb*16+strip, 
				    (int)((cfebData->timeSlice(itime))->scaControllerWord(layer).sca_blk), itime+1 ) << std::endl;
		  offset = nTimeSamples-itime-1;
		  break;
		}
	      }
		*/
	      for (int itime=0;itime<nTimeSamples;itime++){
		CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,strip);
		int Qi = (int) ((timeSample->adcCounts)&0xFFF);


		CSCCFEBSCAControllerWord scaControllerWord = (cfebData->timeSlice(itime))->scaControllerWord(layer);
		int scaBlock  = (int)(scaControllerWord.sca_blk);
		int trigTime = (int)(scaControllerWord.trig_time);
		int lctPhase = (int)(scaControllerWord.lct_phase);

		for(int pos=0;pos<8;pos++){if(trigTime==(1<<pos)) lctPhase=pos;}

		if(trigTime!=0){
		  cap = lctPhase+itime;
		  blk_strt =itime;
		}else{
		  cap=itime-blk_strt-1;
		}
		
		int scaNumber=8*conv_blk[scaBlock]+cap;

		// int scaNumber = 8*conv_blk[scaBlock]+(offset+itime)%8;

		if (scaNumber >= 96) {
		  std::cout << "Invalid SCA cell" << std::endl;
		}

		scadata.content[layer-1][icfeb*16+strip-1][scaNumber].value += Qi;
		scadata.content[layer-1][icfeb*16+strip-1][scaNumber].cnt++;
		v03->Fill(scaNumber, Qi);
		v04->Fill(scaNumber);
		v05->Fill(conv_blk[scaBlock]);

		r01.content[layer-1][icfeb*16+strip-1] += Qi;
		r01.cnts[layer-1][icfeb*16+strip-1]++;
		r02.content[layer-1][icfeb*16+strip-1] += pow(Qi,2);
		r02.cnts[layer-1][icfeb*16+strip-1]++;
	      }
	    }
	  }
	}

      } // 1000 events
    }

    // Calculate mv0 and rms0
    if (nEvents == 1000) {
      for (int i=0; i<_mv0.Nlayers; i++) {
	for (int j=0; j<_mv0.Nbins; j++) {
	  //	  std::cout << (i+1) << ":" << (j+1) << " " << r01.content[i][j] << ", " <<r01.cnts[i][j]
	  //	    << ", " << r02.content[i][j] << ", " << r02.cnts[i][j] << ": ";
	  if (_mv0.cnts[i][j]==0) {
	    // std::cout << cscID << " " << i << ":" << j << " r01entries=0"<< std::endl;
	  } else {
	    _mv0.content[i][j] /= (double)(_mv0.cnts[i][j]);
	  }
	  if (_rms0.cnts[i][j]==0) {
	    // std::cout << cscID << " " << i << ":" << j << " r02 entries=0"<< std::endl;
	  } else {
	    double rms = sqrt(((_rms0.content[i][j])/((double)(_rms0.cnts[i][j]))- pow(_mv0.content[i][j],2)) );
	    _rms0.content[i][j]=rms;
	    //std:: cout << r01.content[i][j] << ", " << r02.content[i][j] << std::endl;
	  }
	
	}
      }
    }


  } // CFEB data available
  
}



void Test_CFEB02::finish() {

  struct tm* clock;
  struct stat attrib;
  stat(dataFile.c_str(), &attrib);
  clock = localtime(&(attrib.st_mtime));
  std::string dataTime=asctime(clock);
  time_t now = time(NULL);  
  clock = localtime(&now);
  std::string testTime=asctime(clock);

  std::string filename="Test_CFEB02.root";
  TFile root_res(filename.c_str(), "recreate");
  if (!root_res.IsZombie()) {
    root_res.cd();
    TDirectory* f = root_res.mkdir("Test_CFEB02");

    for (cscTestData::iterator td_itr = tdata.begin(); td_itr != tdata.end(); ++td_itr) {

      std::string cscID = td_itr->first;

      TDirectory * rdir = f->mkdir((cscID).c_str());
      std::string path = cscID+"/";
      TString command = Form("mkdir -p %s", path.c_str());
      gSystem->Exec(command.Data());

      rdir->cd();
   
      if (nCSCEvents[cscID] < nExpectedEvents/2) {
	std::cout << Form("%s: Not enough events for test analysis (%d events)", cscID.c_str(), nCSCEvents[cscID] ) << std::endl;
	// = Set error 
	continue;
      }
      
      TestData& cscdata= td_itr->second;

      CFEBData& _q12 = cscdata["_Q12"];
      CFEBData& _q3 = cscdata["_Q3"];
      CFEBData& _q4 = cscdata["_Q4"];
      CFEBData& _q5 = cscdata["_Q5"];
  
      CFEBData& r01 = cscdata["R01"];
      CFEBData& r02 = cscdata["R02"];
      CFEBData& r03 = cscdata["R03"];
      CFEBData& r04 = cscdata["R04"];
      CFEBData& r05 = cscdata["R05"];
      CFEBData& r06 = cscdata["R06"];
      CFEBData& r07 = cscdata["R07"];
      CFEBData& r08 = cscdata["R08"];
      CFEBData& r09 = cscdata["R09"];
      CFEBData& r10 = cscdata["R10"];
      CFEBData& r11 = cscdata["R11"];
      CFEBData& r12 = cscdata["R12"];
      CFEBData& r13 = cscdata["R13"];
      CFEBData& r14 = cscdata["R14"];
      CFEBData& r15 = cscdata["R15"];
      CFEBData& r16 = cscdata["R16"];
      CFEBData& r17 = cscdata["R17"];

      CFEBSCAData& scadata = sdata[cscID];
    
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

	   // == Calculate time samples 3,4,5 pedestals and noise
	  if (r06.cnts[i][j]) {
	    r06.content[i][j] /= (double)(r06.cnts[i][j]);
	    rms = sqrt( ((r07.content[i][j])/((double)(r07.cnts[i][j]))- pow(r06.content[i][j],2)) );

	    r07.content[i][j]=rms;
	  }

	  // == Calculate covariance matrix elements

	  // = C12|12
	  if (r08.cnts[i][j]) {
	    covar  =  ((r08.content[i][j]) /(double)(r08.cnts[i][j])) - _q12.content[i][j]*_q12.content[i][j];
	    r08.content[i][j] = covar;
	  }

	  // = C12|3
	  if (r09.cnts[i][j]) {
	    covar =  ((r09.content[i][j]) /(double)(r09.cnts[i][j])) - _q12.content[i][j]*_q3.content[i][j];
	    r09.content[i][j] = covar;
	  }

	  // = C12|4
	  if (r10.cnts[i][j]) {
	    covar =  ((r10.content[i][j]) /(double)(r10.cnts[i][j])) - _q12.content[i][j]*_q4.content[i][j];
	    r10.content[i][j] = covar;
	  }

	  // = C12|5
	  if (r11.cnts[i][j]) {
	    covar =  ((r11.content[i][j]) /(double)(r11.cnts[i][j])) - _q12.content[i][j]*_q5.content[i][j];
	    r11.content[i][j] = covar;
	  }

	  // = C3|3
	  if (r12.cnts[i][j]) {
	    covar =  ((r12.content[i][j]) /(double)(r12.cnts[i][j])) - _q3.content[i][j]*_q3.content[i][j];
	    r12.content[i][j] = covar;
	  }

	  // = C3|4
	  if (r13.cnts[i][j]) {
	    covar =  ((r13.content[i][j]) /(double)(r13.cnts[i][j])) - _q3.content[i][j]*_q4.content[i][j];
	    r13.content[i][j] = covar;
	  }
	     
	  // = C3|5
	  if (r14.cnts[i][j]) {
	    covar =  ((r14.content[i][j]) /(double)(r14.cnts[i][j])) - _q3.content[i][j]*_q5.content[i][j];
	    r14.content[i][j] = covar;
	  }

	  // = C4|4
	  if (r15.cnts[i][j]) {
	    covar =  ((r15.content[i][j]) /(double)(r15.cnts[i][j])) - _q4.content[i][j]*_q4.content[i][j];
	    r15.content[i][j] = covar;
	  }

	  // = C4|5
	  if (r16.cnts[i][j]) {
	    covar =  ((r16.content[i][j]) /(double)(r16.cnts[i][j])) - _q4.content[i][j]*_q5.content[i][j];
	    r16.content[i][j] = covar;
	  }
	  
	  // = C5|5
	  if (r17.cnts[i][j]) {
	    covar =  ((r17.content[i][j]) /(double)(r17.cnts[i][j])) - _q5.content[i][j]*_q5.content[i][j];
	    r17.content[i][j] = covar;	
	  }

	}
      }


      TestData::iterator itr = cscdata.find("R01");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R01").c_str(), (cscID+": CFEB02 R01").c_str(), 80, 0.0, 80.0, 140, 300.0, 1000.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
	// cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal mean, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R01 - overall pedestals");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(400,500, 700, 800);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R01");
	cnv->Write();
	delete cnv;

      }

      itr = cscdata.find("R02");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R02").c_str(), (cscID+": CFEB02 R02").c_str(), 80, 0.0, 80.0, 40 , 0.0, 8.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
        // cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal RMS, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R02 - overall noise");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(2.0, 2.0, 6.0, 6.0);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R02");
	cnv->Write();
	delete cnv;
      }

      itr = cscdata.find("R03");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R03").c_str(), (cscID+": CFEB02 R03").c_str(),80, 0.0, 80.0, 60, 0., 6.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
	// cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal RMS, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R03 - RMS of SCA pedestals");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(0.0, 0.0, 3.0, 5.0);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R03");
	cnv->Write();
	delete cnv;
      }

      itr = cscdata.find("R04");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R04").c_str(), (cscID+": CFEB02 R04").c_str(),80, 0.0, 80.0, 140, 300.0, 1000.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
	// cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal mean, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R04 - P12 pedestals");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(400,500, 700, 800);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R04");
	cnv->Write();
	delete cnv;

      }

      itr = cscdata.find("R05");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R05").c_str(), (cscID+": CFEB02 R05").c_str(), 80, 0.0, 80.0, 40 , 0.0, 4.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
	// cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal RMS, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R05 - P12 noise");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(1.0, 1.0, 3.0, 3.0);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R05");
	delete cnv;
      }

      itr = cscdata.find("R06");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R06").c_str(), (cscID+": CFEB02 R06").c_str(), 80, 0.0, 80.0, 140, 300.0, 1000.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
	// cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal mean, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R06 - P345 pedestals");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(400,500, 700, 800);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R06");
	cnv->Write();
	delete cnv;

      }

      itr = cscdata.find("R07");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R07").c_str(), (cscID+": CFEB02 R07").c_str(), 80, 0.0, 80.0, 40 , 0.0, 4.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
	// cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal RMS, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R07 - P345 noise");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(1.0, 1.0, 3.0, 3.0);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R07");
	cnv->Write();
	delete cnv;
      }

      itr = cscdata.find("R08");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R08").c_str(), (cscID+": CFEB02 R08").c_str(), 80, 0.0, 80.0, 40, -20.0, 20.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
	// cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal RMS, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R08 - C(12)(12) covarinace matrix element");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(-10.0, -10.0, 10.0, 10.0);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R08");
	delete cnv;
      }

      itr = cscdata.find("R09");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R09").c_str(), (cscID+": CFEB02 R09").c_str(), 80, 0.0, 80.0, 40, -20.0, 20.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
	// cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal RMS, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R09 - C(12)3 covarinace matrix element");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(-10.0, -10.0, 10.0, 10.0);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R09");
	cnv->Write();
	delete cnv;
      }

      itr = cscdata.find("R10");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R10").c_str(), (cscID+": CFEB02 R10").c_str(), 80, 0.0, 80.0, 40, -20.0, 20.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
	// cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal RMS, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R10 - C(12)4 covarinace matrix element");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(-10.0, -10.0, 10.0, 10.0);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R10");
	cnv->Write();
	delete cnv;
      }

      itr = cscdata.find("R11");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R11").c_str(), (cscID+": CFEB02 R11").c_str(), 80, 0.0, 80.0, 40, -20.0, 20.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
	// cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal RMS, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R11 - C(12)5 covarinace matrix element");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(-10.0, -10.0, 10.0, 10.0);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R11");
	delete cnv;
      }

      itr = cscdata.find("R12");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R12").c_str(), (cscID+": CFEB02 R12").c_str(), 80, 0.0, 80.0, 40, -20.0, 20.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
	// cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal RMS, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R12 - C33 covarinace matrix element");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(-10.0, -10.0, 10.0, 10.0);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R12");
	cnv->Write();	
	delete cnv;
      }

      itr = cscdata.find("R13");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R13").c_str(), (cscID+": CFEB02 R13").c_str(), 80, 0.0, 80.0, 40, -20.0, 20.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
	// cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal RMS, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R13 - C34 covarinace matrix element");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(-10.0, -10.0, 10.0, 10.0);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R13");
	cnv->Write();
	delete cnv;
      }

      itr = cscdata.find("R14");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R14").c_str(), (cscID+": CFEB02 R14").c_str(),80, 0.0, 80.0, 40, -20.0, 20.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
	// cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal RMS, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R14 - C35 covarinace matrix element");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(-10.0, -10.0, 10.0, 10.0);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R14");
	cnv->Write();
	delete cnv;
      }

      itr = cscdata.find("R15");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R15").c_str(), (cscID+": CFEB02 R15").c_str(),80, 0.0, 80.0, 40, -20.0, 20.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
	// cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal RMS, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R15 - C44 covarinace matrix element");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(-10.0, -10.0, 10.0, 10.0);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R15");
	cnv->Write();
	delete cnv;
      }

      itr = cscdata.find("R16");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R16").c_str(), (cscID+": CFEB02 R16").c_str(),80, 0.0, 80.0, 40, -20.0, 20.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
	// cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal RMS, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R16 - C45 covarinace matrix element");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(-10.0, -10.0, 10.0, 10.0);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R16");
	cnv->Write();
	delete cnv;
      }

      itr = cscdata.find("R17");
      if (itr != cscdata.end()) {
	CFEBData data = itr->second;
	CFEBTestCanvas* cnv = new CFEBTestCanvas((cscID+"_CFEB02_R17").c_str(), (cscID+": CFEB02 R17").c_str(),80, 0.0, 80.0, 40, -20.0, 20.0);
	//      Set parameters
	//         cnv->SetTitle("CSC ID: ME+2/2/112");
	// cnv->SetTitle(cscID.c_str());
	cnv->SetXTitle("Strip number");
	cnv->SetYTitle("Pedestal RMS, ADC counts");
	cnv->AddTextTest("Test: CFEB02 - pedestals and noise");
	cnv->AddTextResult("Result: R17 - C55 covarinace matrix element");
	cnv->AddTextDatafile(("Datafile: "+dataFile).c_str());
	cnv->AddTextRun(("Run: " + dataTime).c_str());
	cnv->AddTextAnalysis(("Analysis: " + testTime +", version " + ANALYSIS_VER).c_str());

	cnv->SetLimits(-10.0, -10.0, 10.0, 10.0);

	cnv->Fill(data);

	cnv->Draw();
	cnv->SaveAs(path+cscID+"_CFEB02_R17");
	cnv->Write();
	delete cnv;
      }

      gStyle->SetPalette(1,0);

      MonHistos& cschistos = mhistos[cscID];
    
      MonitoringCanvas* cnv01= new MonitoringCanvas((cscID+"_CFEB02_V01").c_str(), (cscID+"_CFEB02_V01").c_str(), 
						    (cscID + " CFEB02 V01: Signal Line").c_str() ,
						    1, 1, 1200, 800);
      cnv01->SetCanvasSize(1200, 800);
      cnv01->cd(1);
      cschistos["V01"]->Draw();
      //    gStyle->SetOptStat("eomr");
      cnv01->Draw();
      cnv01->Print((path+cscID+"_CFEB02_V01.png").c_str());
      cnv01->Write();
      delete cnv01;

      MonitoringCanvas* cnv02= new MonitoringCanvas((cscID+"_CFEB02_V02").c_str(), (cscID+"_CFEB02_V02").c_str(), 
						    (cscID + " CFEB02 V01: Q4 with dynamic pedestal subtraction").c_str(),
						    1, 1, 1200, 800);
      cnv02->SetCanvasSize(1200, 800);
      cnv02->cd(1);
      cschistos["V02"]->Draw();
      gStyle->SetOptStat("eomr");
      cnv02->Draw();
      cnv02->Print((path+cscID+"_CFEB02_V02.png").c_str());
      cnv02->Write();
      delete cnv02;

      
      MonitoringCanvas* cnv03= new MonitoringCanvas((cscID+"_CFEB02_V03").c_str(), (cscID+"_CFEB02_V03").c_str(), 
						    (cscID + " CFEB02 V03: SCA Cells Occupancy").c_str(),
						    1, 1, 1200, 800);
      cnv03->SetCanvasSize(1200, 800);
      cnv03->cd(1);
      cschistos["V04"]->Draw();
      gStyle->SetOptStat("");
      cnv03->Draw();
      cnv03->Print((path+cscID+"_CFEB02_V03.png").c_str());
      cnv03->Write();
      delete cnv03;
      
      MonitoringCanvas* cnv04= new MonitoringCanvas((cscID+"_CFEB02_V04").c_str(), (cscID+"_CFEB02_V04").c_str(), 
						    (cscID + " CFEB02 V04: SCA Block Occupancy").c_str(),
						    1, 1, 1200, 800);
      cnv04->SetCanvasSize(1200, 800);
      cnv04->cd(1);
      cschistos["V05"]->Draw();
      gStyle->SetOptStat("");
      cnv04->Draw();
      cnv04->Print((path+cscID+"_CFEB02_V04.png").c_str());
      cnv04->Write();
      delete cnv04;

      std::ofstream res_out((path+cscID+"_CFEB02_01.results").c_str());
      res_out << "Layer Strip Pedestal Noise SCA_rms P12_Ped P12_rms P345_Ped P345_rms SCA_rms" << std::endl;
      for (int i=0; i<r03.Nlayers; i++) {
	for (int j=0; j<r03.Nbins; j++) {
	  res_out << fixed << setprecision(2) << setw(5) << (i+1) << setw(6) << (j+1) 
		  << setw(9) << r01.content[i][j]  << setw(6) << r02.content[i][j]
		  << setw(8) << r03.content[i][j]  << setw(8) << r04.content[i][j]
		  << setw(9) << r05.content[i][j]  << setw(9) << r06.content[i][j]
		  << setw(8) << r07.content[i][j]
		  << endl;
	}
      }
      res_out.close();

      res_out.open((path+cscID+"_CFEB02_02.results").c_str());
      res_out << "Layer Strip C12|12 C12|3 C12|4 C12|5  C3|3  C3|4  C3|5  C4|4  C4|5  C5|5" << std::endl;
      for (int i=0; i<r03.Nlayers; i++) {
	for (int j=0; j<r03.Nbins; j++) {
	  res_out << fixed << setprecision(2) << setw(5) << (i+1) << setw(6) << (j+1) 
		  << setw(7) << r08.content[i][j] << setw(6) << r09.content[i][j]  << setw(6) << r10.content[i][j]
		  << setw(6) << r11.content[i][j]  << setw(6) << r12.content[i][j]  << setw(6) << r13.content[i][j]  
		  << setw(6) << r14.content[i][j]  << setw(6) << r15.content[i][j]  << setw(6) << r16.content[i][j]
		  << setw(6) << r17.content[i][j]  << endl;
	}
      }
      res_out.close();
      f->cd();
    }
    root_res.Close();
  }


}

