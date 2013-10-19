#include "emu/dqm/calibration/Test_GasGain.h"

using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;

const unsigned int Test_GasGain::Nbins_wires =16; // is a divisor of the number of wire groups of all chamber types
const unsigned int Test_GasGain::Nbins_strips=16; // is a better choice than 20 as it's a divisor of the number of strips of all chamber types
const unsigned int Test_GasGain::NStrips_per_CFEB=16;
const unsigned int Test_GasGain::NLayer=NLAYERS;
const unsigned int Test_GasGain::nsegments=5;


template<typename T> inline CSCCFEBDataWord const * const
timeSample( T const & data, int nCFEB,int nSample,int nLayer, int nStrip)
{
  //strips means strip per cfeb so 1- 16
  return data.cfebData(nCFEB)->timeSlice(nSample)->timeSample(nLayer,nStrip);
}

template<typename T> inline CSCCFEBTimeSlice const * const
timeSlice( T const & data, int nCFEB, int nSample)
{
  //here strip means strip per cfeb so 1 to 16
  return (CSCCFEBTimeSlice *)(data.cfebData(nCFEB)->timeSlice(nSample));
}

Test_GasGain::Test_GasGain(std::string dfile): Test_Generic(dfile)
{
  testID = "GasGain";
  nExpectedEvents = 100000;
  dduID=0;
  binCheckMask=0x1FFB7BF6;
  ltc_bug=2;
 

  logger = Logger::getInstance(testID);


}


void Test_GasGain::initCSC(std::string cscID)
{

  nCSCEvents[cscID]=0;

  TestData cscdata;
  TestData2D gasgaindata;
  // gasgaindata.Nbins = getNumStrips(cscID)/16*5;
  gasgaindata.Nbins = getNumStrips(cscID) / NStrips_per_CFEB * nsegments;
  gasgaindata.Nlayers = NLayer;
  memset(gasgaindata.content, 0, sizeof (gasgaindata.content));
  memset(gasgaindata.cnts, 0, sizeof (gasgaindata.cnts));

  // Channels mask
  if (tmasks.find(cscID) != tmasks.end())
    {
      cscdata["_MASK"]=tmasks[cscID];
    }
  else
    {
      cscdata["_MASK"]=gasgaindata;
    }

  for (int i=0; i<TEST_DATA2D_NLAYERS; i++)
  for (int j=0; j<TEST_DATA2D_NBINS; j++) gasgaindata.content[i][j]=BAD_VALUE;

  // R01 - Peak bin with strip with max 5-bin sum
  cscdata["V_R01"]=gasgaindata;
  //R02 - Mean cluster sum of strip with max 5-bin sum
  cscdata["V_R02"]=gasgaindata;
  //R02 - Truncated Mean cluster sum of strip with max 5-bin sum
  cscdata["R03"]=gasgaindata;

  tdata[cscID] = cscdata;

  bookTestsForCSC(cscID);

  MonHistos& cschistos = mhistos[cscID];

  // const unsigned int NLayer=6;

  const unsigned int N_CFEBS=getNumStrips(cscID)/NStrips_per_CFEB; 
  //unused  const unsigned int N_Samples=16;
  //unused  const unsigned int NStrips_per_CFEB=16;
  //unused  const unsigned int NStrips=80;
  // const unsigned int nsegments=5;

  TH1F* v02 = reinterpret_cast<TH1F*>(cschistos["V02"]);


  const unsigned int histo_vector_size=NLayer*nsegments*N_CFEBS;
 
  for (unsigned int i_layer=0;i_layer<NLayer;i_layer++){
    for (unsigned int i_cfeb=0;i_cfeb<N_CFEBS;i_cfeb++){
      for (unsigned int i_segment=0;i_segment<nsegments;i_segment++){
        char histo_name_st[50];
	//unused        int test= sprintf(histo_name_st,"hist_%d_%d_%d",i_layer,i_cfeb,i_segment);
        char title[50];
	//unused        int test_title=sprintf(title, "Layer %d, HV segment %d cfeb %d",i_layer+1,i_segment+1,i_cfeb+1);
        const char* hist_title=title;
        const char* histo_name=histo_name_st;
	//unused        unsigned int index=i_segment+i_cfeb*nsegments+i_layer*N_CFEBS*nsegments;
        hist_cluster_sum.push_back((TH1F*)v02->Clone(histo_name));
        hist_cluster_sum[hist_cluster_sum.size()-1]->SetTitle(hist_title);
      }
    }
    for(unsigned int i_strip_bin=0; i_strip_bin<Nbins_strips;i_strip_bin++){
      for(unsigned int i_wire_bin=0; i_wire_bin<Nbins_wires;i_wire_bin++){
        char histo_name_st[50];
	//unused        int test= sprintf(histo_name_st,"hist_%d_%d_%d",i_layer,i_strip_bin,i_wire_bin);
        char title[50];
	//unused        int test_title=sprintf(title, "Layer %d, strip bin %d, wire bin %d",i_layer+1,i_strip_bin+1,i_wire_bin+1);
        const char* hist_title=title;
        const char* histo_name=histo_name_st;
	//unused        unsigned int index=i_layer*Nbins_wires*Nbins_strips+i_strip_bin*Nbins_wires+i_wire_bin;
        hist_cluster_sum_fine.push_back((TH1F*)v02->Clone(histo_name));
        hist_cluster_sum_fine[hist_cluster_sum_fine.size()-1]->SetTitle(hist_title);       
      }
    }
  }
  if(hist_cluster_sum.size()!=histo_vector_size){
    cout<<"wrong size of cluster sum histogram "<<hist_cluster_sum.size()<<"/"<<histo_vector_size<<endl;
  }

}

void Test_GasGain::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
{


  nTotalEvents++;

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
      DDUstats[dduID].dac=0;
      DDUstats[dduID].strip=1;
    }

  dduID = bin_checker.dduSourceID()&0xFF;
  DDUstats[dduID].evt_cntr++;

  if ((bin_checker.errors() & binCheckMask)!= 0)
    {
      // std::cout << "Evt#" << std::dec << nTotalEvents << ": Nonzero Binary Errors Status is observed: 0x"<< std::hex << bin_checker.errors() << std::endl;
      doBinCheck();

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
    if  (DDUstats[dduID].empty_evt_cntr==0)
      {
        LOG4CPLUS_DEBUG(logger, "No LTC/TTC double L1A bug in data");
        ltc_bug=1;
      }
    else
      {
        LOG4CPLUS_DEBUG(logger, "Found LTC/TTC double L1A bug in data");
      }
  
  for (std::vector<CSCEventData>::iterator chamberDataItr = chamberDatas.begin();
       chamberDataItr != chamberDatas.end(); ++chamberDataItr)
    {
      analyzeCSC(*chamberDataItr);
    }

  DDUstats[dduID].last_empty=chamberDatas.size();
 
}


void Test_GasGain::analyzeCSC(const CSCEventData& data)
{

  const CSCDMBHeader* dmbHeader = data.dmbHeader();
  const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();
  if (!dmbHeader && !dmbTrailer)
    {
      return;
    }

  int csctype=0, cscposition=0;
  std::string cscID = getCSCFromMap(dmbHeader->crateID(), dmbHeader->dmbID(), csctype, cscposition);

  // == Do not process unmapped CSCs
  if (cscID == "") return;

  cscTestData::iterator td_itr = tdata.find(cscID);
  if ( (td_itr == tdata.end()) || (tdata.size() == 0) )
    {
      LOG4CPLUS_INFO(logger, "Found " << cscID);
      initCSC(cscID);
      addCSCtoMap(cscID, dmbHeader->crateID(), dmbHeader->dmbID());
    }
  nCSCEvents[cscID]++;

  // == Define aliases to access chamber specific data
  //unused  uint32_t& nEvents=nCSCEvents[cscID];
  //unused  TestData& cscdata = tdata[cscID];

  MonHistos& cschistos = mhistos[cscID];

  // const unsigned int NLayer=6;

  const unsigned int N_CFEBS=getNumStrips(cscID)/NStrips_per_CFEB; 
  //unused  const unsigned int N_Samples=16;
  const unsigned int NStrips=getNumStrips(cscID);//80;
  // const unsigned int nsegments=5;
  const unsigned int NWires=getNumWireGroups(cscID);

  // std::cout << "NStrips = " << NStrips << std::endl;

  TH1F* v01 = reinterpret_cast<TH1F*>(cschistos["V01"]);
  TH1F* v02 = reinterpret_cast<TH1F*>(cschistos["V02"]);
  
  TH1F* v04 = reinterpret_cast<TH1F*>(cschistos["V04"]);

  //unused  const unsigned int histo_vector_size=NLayer*nsegments*N_CFEBS;
 
  unsigned int l1a_cnt = dmbHeader->l1a();
  if (l1a_cnt < l1a_cntrs[cscID]) l1a_cnt+=256;

  if (v04) v04->Fill(l1a_cnt-l1a_cntrs[cscID]);

  l1a_cntrs[cscID]=dmbHeader->l1a();


  // == Check if CFEB Data Available
  if (dmbHeader->cfebAvailable())
    {
      //unused      double max_strip_sum=0;
      //first find radial position of track
      //use ALCT information, if one track is found continue

      if(data.nalct()){
	
	const CSCALCTHeader* alctHeader = data.alctHeader();
	const CSCALCTTrailer* alctTrailer = data.alctTrailer();
	if (!alctHeader && !alctTrailer)
	  {
	    return;
	  }
	if(alctHeader && alctTrailer){

	  const CSCAnodeData* alctData = data.alctData();
	  if(alctData){
	    vector<CSCALCTDigi> alctDigis = alctHeader->ALCTDigis();
	    int ALCTKeyWG=-1;
	    int ALCTKeyWG2=-1;

	    //do we still want only one alct in case we have two good alct's?
	    for(uint32_t alct=0;alct<alctDigis.size();alct++){
	      if(alctDigis[alct].isValid()){
		if(ALCTKeyWG<0){
		  ALCTKeyWG=alctDigis[alct].getKeyWG();
		  v01->Fill(ALCTKeyWG);
		}else{
		  ALCTKeyWG2=alctDigis[alct].getKeyWG();
		}
		//cout<<" event/alct/keywg/ "<<nCSCEvents[cscID]<<"/"<<alct<<"/"<<ALCTKeyWG<<endl;
	      }
	    }
	    if(ALCTKeyWG<0){
	      //cout<<"skip event with no ALCT "<<endl;
	      if(ALCTKeyWG2>0){
            cout<<"ALCT1>0 should not be the case "<<ALCTKeyWG2<<endl;
	      }
	    }
	    if(ALCTKeyWG2>0 && fabs(ALCTKeyWG-ALCTKeyWG2)>5){
	      //cout<<"skip event "<<nCSCEvents[cscID]<<" due to second ALCT "<<ALCTKeyWG <<" "<<ALCTKeyWG2<<endl;
	      return;
	    }
	    unsigned int isegment =0;
	    //hard code segments for now - change later
	    //unused	    int ks[5]={16, 28, 40,  52, 64};



	    for (isegment = 0; isegment < nsegments; isegment++)
	      {



            float limit=float(isegment+1)*float(NWires)/float(nsegments);
            //WG's are smaller than NWires (starting at 0) 
              //if (ALCTKeyWG < ks[isegment]) break;
            if (ALCTKeyWG < limit) break;
	      }
  
	    int ADC=-1;
	    //unused	    int OutOfRange=-1;

	    double strip_sum[NLayer][NStrips];
	    std::vector<float> cluster_sum(NLayer,0);

	    std::vector<int> peak_strip (NLayer,-1);
	    //loop over layers and cfebs
	    for (unsigned int layer=1;layer<=NLayer;layer++){
	      float max_strip_sum=0;
	      //final analysis strip max index goes from 1 to 80
	      int strip_max=0;
	      std::vector<int> tbin_max(NStrips,-1);
	      for (int icfeb=0; icfeb<getNumStrips(cscID)/NStrips_per_CFEB; icfeb++){ 
		// loop over cfebs in a given chamber   	        
		CSCCFEBData * cfebData =  data.cfebData(icfeb);
		if (!cfebData || !cfebData->check()){ 		 
		  continue;
		}	      
		for(unsigned int istrip=1;istrip<=NStrips_per_CFEB;istrip++){
		  //
		  strip_sum[layer-1][icfeb*NStrips_per_CFEB+istrip-1]=0;
		  
		  unsigned int nTimeSamples= cfebData->nTimeSamples(); // Get number of time samples
		  
		  // Do CRC check of first two timesamples for pedestal calculation
		  if (!cfebData->timeSlice(0)->checkCRC() || !cfebData->timeSlice(1)->checkCRC())
		    {
		      LOG4CPLUS_WARN(logger, cscID << " CRC check failed for central strip time sample 1 and 2");
		      continue;
		    }
		  // == Calculate pedestal from first two time samples
		  //istrip means strip per cfeb so 1- 16
		  double Q12=((cfebData->timeSlice(0))->timeSample(layer,istrip)->adcCounts
			      + (cfebData->timeSlice(1))->timeSample(layer,istrip)->adcCounts)/2.;
		  
		  int sca_max_adc=-1;
		  //determine maximum over the remaing time bins (two first are pedestals)
		  for(unsigned int nSample=0;nSample< nTimeSamples;nSample++){
		    if (timeSlice(data, icfeb, nSample) == 0)
		      {
			std::cerr <<  "CFEB" << icfeb << " nSample: " << nSample << " - B-Word" << std::endl;
			continue;
		      }
		      
		    ADC = (int) ((timeSample(data, icfeb, nSample, layer, istrip)->adcCounts) & 0xFFF);
		    if(ADC>sca_max_adc){
		      sca_max_adc=ADC;
		      tbin_max[icfeb*NStrips_per_CFEB+istrip-1]=nSample;
		    }
		  }
		  //if maximum in last two time bins move for cluster sum to nTimeSamples-2
		  int tbin_pulse=tbin_max[icfeb*NStrips_per_CFEB+istrip-1];
		  if((unsigned int)(tbin_max[icfeb*NStrips_per_CFEB+istrip-1])>(nTimeSamples-3)){
		    tbin_pulse=nTimeSamples-3;
		  }
		  if(tbin_max[icfeb*NStrips_per_CFEB+istrip-1]<2){
		    tbin_pulse=2;
		    //subtract for each sample the pedestal (-5*ped) or just in each thing separately
		  }
		  for(unsigned int tbin=(unsigned int)(tbin_pulse-2);tbin<(unsigned int)(tbin_pulse+3);tbin++){

		    ADC = (int) ((timeSample(data, icfeb, tbin, layer, istrip)->adcCounts) & 0xFFF);
		    strip_sum[layer-1][icfeb*NStrips_per_CFEB+istrip-1]+=(double)ADC - Q12;
		  }
		  if(strip_sum[layer-1][icfeb*NStrips_per_CFEB+istrip-1]>max_strip_sum){
		    max_strip_sum=strip_sum[layer-1][icfeb*NStrips_per_CFEB+istrip-1];
		    strip_max=icfeb*NStrips_per_CFEB+istrip;
		  }
		}//loop over strips
	      }//loop over cfebs
	      if(tbin_max[strip_max-1]<3){ 
            //cout<<"event "<<nCSCEvents[cscID]<<" out of time "<<tbin_max[strip_max-1]<<" strip_max/layer/max_strip_sum "<<strip_max<<"/"<<layer<<"/"<<max_strip_sum<<endl;
	      }//tbin max timing
	      else{
		peak_strip[layer-1]=strip_max;
	      }
	      //strip_max goes from 1-80
	      if(strip_max<3){
		strip_max=3;
	      }
	      if(strip_max>(getNumStrips(cscID)-2)){
		strip_max=getNumStrips(cscID)-2;
	      }
	      cluster_sum[layer-1]=0;
	      //strip can go from 1 to 80
	      for(int istrip = (strip_max-2); istrip< (strip_max+3);istrip++){
		//index is strip-1, layer-1
		cluster_sum[layer-1]+=strip_sum[layer-1][istrip-1];
	      }
	    }//loop over layers
	    

	    //cuts to ensure tracks area almost vertical

	    unsigned int lfirst=0;
	    unsigned int llast=0;
	    for(unsigned int ilayer=1;ilayer<=NLayer;ilayer++){
	      if (peak_strip[ilayer-1] > -1 && cluster_sum[ilayer-1] > 150){
		if(lfirst==0){
		  lfirst=ilayer;
		}
		llast=ilayer;
	      }
	    }

	    bool break_point=false;

	    if(lfirst==0){
	      cout<<"skip event "<<nCSCEvents[cscID]<<" with no clusters above threshold"<<endl;
	      break_point=true;
	    }
	    if(lfirst==llast){
	      cout<<"skip event "<<nCSCEvents[cscID]<<" with only one cluster above threshold"<<endl;
	      break_point=true;
	    }
	    if(!break_point){
	      double slope = ( (double)peak_strip[llast-1]-(double)peak_strip[lfirst-1] )/( (double)llast-(double)lfirst );

	      if(fabs(slope)<2){
		for(unsigned int ilayer=1;ilayer<=NLayer;ilayer++){
		  if (peak_strip[ilayer-1] > -1 && cluster_sum[ilayer-1] > 50){
		    //icfeb counts from 0
		    int icfeb=(peak_strip[ilayer-1]-1)/NStrips_per_CFEB;
		    int strip_bin=(peak_strip[ilayer-1]-1) / ( NStrips / Nbins_strips );
		    //r01.content[ilayer-1][icfeb + getNumStrips(cscID)/16*isegment] += cluster_sum[ilayer-1];
		    //r01.cnts[ilayer-1][icfeb + getNumStrips(cscID)/16*isegment]++;
		    unsigned int index=isegment+icfeb*nsegments+(ilayer-1)*N_CFEBS*nsegments;
            //ALCTKeyWG start from 0
            //int wire_bin=ALCTKeyWG/4;
		    int wire_bin=ALCTKeyWG / ( NWires / Nbins_wires );
		    unsigned int index_fine=wire_bin+strip_bin*Nbins_wires+(ilayer-1)*Nbins_strips*Nbins_wires;
		    //cout<<"in analyze csc done"<<index<<" segment/cfeb/layer "<<"/"<<isegment<<"/"<<icfeb<<"/"<<ilayer<<"/"<<hist_cluster_sum.size()<<" peak_strip/keyWG/wire_bin/strip_bin/index fine/max "<<peak_strip[ilayer-1]<<"/"<<ALCTKeyWG<<"/"<<wire_bin<<"/"<<strip_bin<<"/"<<index_fine<<"/"<<hist_cluster_sum_fine.size()<<endl;
            if(index_fine>(hist_cluster_sum_fine.size()-1)){
              cout<<" boundary reached "<<index_fine<<"/"<<hist_cluster_sum_fine.size()<<endl;
            }
		    hist_cluster_sum[index]->Fill(cluster_sum[ilayer-1]);
            hist_cluster_sum_fine[index_fine]->Fill(cluster_sum[ilayer-1]);
		    if (v02) v02->Fill(cluster_sum[ilayer-1]);	
		  }
		}	
	      }else{
            //cout<<"ps1/ps2/lfirst/llast/slope "<<peak_strip[llast-1]<<"/"<<peak_strip[lfirst-1]<<"/"<<lfirst<<"/"<<llast<<"/"<<slope<<endl;
	      }
	    }
	  }//alct data present
	  
	}//alct header and trailer

      }//at least 1 alct
    }//cfeb data available

}


void Test_GasGain::finishCSC(std::string cscID)
{
  // cout<<"do i break here"<<endl;
     if (nCSCEvents[cscID] < nExpectedEvents/2) {
     std::cout << Form("%s: Not enough events for test analysis (%d events)", cscID.c_str(), nCSCEvents[cscID] ) << std::endl;
     // = Set error
     return;
     }
  
  
  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end())
    {


      TestData& cscdata= td_itr->second;

      TestData2D& mask = cscdata["_MASK"];

      TestData2D& r01 = cscdata["V_R01"];
      TestData2D& r02 = cscdata["V_R02"];
      TestData2D& r03 = cscdata["R03"];

      MonHistos& cschistos = mhistos[cscID];

      TH2F* v_01 = reinterpret_cast<TH2F*>(cschistos["V0_1"]);
      TH2F* v_02 = reinterpret_cast<TH2F*>(cschistos["V0_2"]);
      TH2F* v_03 = reinterpret_cast<TH2F*>(cschistos["V0_3"]);
      TH2F* v_04 = reinterpret_cast<TH2F*>(cschistos["V0_4"]);
      TH2F* v_05 = reinterpret_cast<TH2F*>(cschistos["V0_5"]);
      TH2F* v_06 = reinterpret_cast<TH2F*>(cschistos["V0_6"]);

      v_01->GetZaxis()->SetRangeUser(200,725);
      v_02->GetZaxis()->SetRangeUser(200,725);
      v_03->GetZaxis()->SetRangeUser(200,725);
      v_04->GetZaxis()->SetRangeUser(200,725);
      v_05->GetZaxis()->SetRangeUser(200,725);
      v_06->GetZaxis()->SetRangeUser(200,725);

      //unused      ResultsCodes& rcodes = rescodes[cscID];
 
 
      CSCtoHWmap::iterator itr = cscmap.find(cscID);


      // == Save results to text files


      if (itr != cscmap.end())
        {

          int dmbID = itr->second.second;
          if (dmbID >= 6) --dmbID;
          int id = 10*itr->second.first+dmbID;

          CSCMapItem::MapItem mapitem = cratemap->item(id);
	  //unused          int first_strip_index=mapitem.stripIndex;
	  //unused          int strips_per_layer=mapitem.strips;

	  // const unsigned int NLayer=6;

	  const unsigned int N_CFEBS=getNumStrips(cscID)/NStrips_per_CFEB; 
      // 	  const unsigned int nsegments=5;
      // const unsigned int Nbins_wires = 16;
      // const unsigned int Nbins_strips=20;


	  for (unsigned int ilayer=0;ilayer<NLayer;ilayer++){
	    //TCanvas* canvas_temp=new TCanvas("c1","temp_canvas",8000,6000);
	    //canvas_temp->Divide(5,5);
	    for (unsigned int icfeb=0;icfeb<N_CFEBS;icfeb++){
	      for (unsigned int isegment=0;isegment<nsegments;isegment++){
            unsigned int index=isegment+icfeb*nsegments+ilayer*N_CFEBS*nsegments;
            //hist_cluster_sum.push_back((TH1F*)v02->Clone(histo_name));
            //cout<<"define histo "<<index<<" title "<<hist_cluster_sum[index]->GetName()<<endl;
            //int maximum_bin=hist_cluster_sum[index]->GetMaximumBin();
            r01.content[ilayer][icfeb + N_CFEBS*isegment] = hist_cluster_sum[index]->GetBinCenter(hist_cluster_sum[index]->GetMaximumBin());
            TH1F* hist_temp= hist_truncated(hist_cluster_sum[index],0.30);
            //cout<<"result/bin content "<<hist_cluster_sum[index]->GetBinCenter(hist_cluster_sum[index]->GetMaximumBin())<<"/"<<hist_cluster_sum[index]->GetBinContent(hist_cluster_sum[index]->GetMaximumBin())<<"/"<<hist_temp->GetMean()<<endl;
            r02.content[ilayer][icfeb + N_CFEBS*isegment] = hist_cluster_sum[index]->GetMean();
            r03.content[ilayer][icfeb + N_CFEBS*isegment] = hist_temp->GetMean();
            //cout<<"content R01/02/03 "<<r01.content[ilayer][icfeb + N_CFEBS*isegment]<<"/"<<r02.content[ilayer][icfeb + N_CFEBS*isegment]<<"/"<<r03.content[ilayer][icfeb + N_CFEBS*isegment]<<endl;
            //row segments, column cfeb index
            //canvas_temp->cd(icfeb+1+N_CFEBS*isegment);
            //hist_cluster_sum[index]->Draw();
            delete hist_temp;
	      }
	    }
        for(unsigned int i_strip_bin=0; i_strip_bin<Nbins_strips;i_strip_bin++){
          for(unsigned int i_wire_bin=0; i_wire_bin<Nbins_wires;i_wire_bin++){
            unsigned int index_fine=ilayer*Nbins_wires*Nbins_strips+i_strip_bin*Nbins_wires+i_wire_bin;            
            TH1F* hist_temp_fine= hist_truncated(hist_cluster_sum_fine[index_fine],0.30);
            if(ilayer==0){
              v_01->SetBinContent(i_strip_bin+1,i_wire_bin+1,hist_temp_fine->GetMean());
            }else if (ilayer==1){
              v_02->SetBinContent(i_strip_bin+1,i_wire_bin+1,hist_temp_fine->GetMean());
            }else if (ilayer==2){
              v_03->SetBinContent(i_strip_bin+1,i_wire_bin+1,hist_temp_fine->GetMean());
            }else if (ilayer==3){
              v_04->SetBinContent(i_strip_bin+1,i_wire_bin+1,hist_temp_fine->GetMean());
            }else if (ilayer==4){
              v_05->SetBinContent(i_strip_bin+1,i_wire_bin+1,hist_temp_fine->GetMean());
            }else if (ilayer==5){
              v_06->SetBinContent(i_strip_bin+1,i_wire_bin+1,hist_temp_fine->GetMean());
            }
            delete hist_temp_fine;
	      }
	    }
	    //char plot_name_st[50];
	    //int test= sprintf(plot_name_st,"layer_%d",ilayer+1);
	    //canvas_temp->Draw();
	    //canvas_temp->SaveAs((path+cscID+testID+"_V_"+plot_name_st+".png").c_str());
	  }

      std::string rpath = "Test_"+testID+"/"+outDir;
      std::string path = rpath+"/"+cscID+"/";

          // == Save results for database transfer
      std::ofstream res_out((path+cscID+"_"+testID+"_DB.dat").c_str());

      std::vector<std::string> tests;
      tests.push_back("V_R01");
	  tests.push_back("V_R02");
	  tests.push_back("R03");
	  // cout<<"do i get here 2"<<endl;
          for (int layer=0; layer<NLAYERS; layer++)
            {

              for (int index=0; index<25; index++)
                {
                  res_out << std::fixed << std::setprecision(2) <<  index << " "
                  << checkChannelConstant("R01",r01.content[layer][index], CHECK_LIMIT) << " " 
                  << checkChannelConstant("R02",r02.content[layer][index], CHECK_LIMIT) << " " 
                  << checkChannelConstant("R03",r03.content[layer][index], CHECK_LIMIT) << " " 
                  << (int)(mask.content[layer][index]) << " "
	  	  << checkChannel(cscdata, tests, layer, index)
		  << std::endl;
                }
            }
          res_out.close();
	  tests.clear();


          res_out.open((path+cscID+"_"+testID+"_DB_GasGain.dat").c_str());

        }
      else
        {
          LOG4CPLUS_WARN(logger, cscID << ": Invalid");
        }
    }
  // cout<<" i get here 6"<<endl;
  
}

TH1F* Test_GasGain::hist_truncated(TH1F* hist,float ratio){

  TH1F* test_histo=(TH1F*)hist->Clone("temp_histo");
  if(ratio<0 || ratio>1){
    cout<<"ratio should be between 0 and 1 "<<ratio<<endl;
  }

  //include overflow bin but don't include underflow

  float total_integral=hist->Integral()+hist->GetBinContent(hist->GetNbinsX()+1)+hist->GetBinContent(0);

  float total_to_keep=(1.0-ratio)*total_integral;

  float test_sum=0;

  TAxis * xaxis = hist->GetXaxis();

  float numerator=0;
  float denominator=0;

  int pass=0;
  for(int i=0;i<(hist->GetNbinsX()+1);i++){
    if(test_sum>total_to_keep){
      //if(pass<3){
        //cout<<"limit reached "<<test_sum<<"/"<<total_to_keep<<endl;
      //}
      test_histo->SetBinContent(i, 0);
      test_histo->SetBinError(i, 0);
      pass+=1;
    }
    if(i==0 && hist->GetBinContent(0)!=0){
      cout<<"we should not have underflows "<<hist->GetBinContent(0)<<endl;
    }
    if(test_sum<total_to_keep){
      numerator+=hist->GetBinContent(i)*xaxis->GetBinCenter(i);
      denominator+=hist->GetBinContent(i);
    }
    test_sum+=hist->GetBinContent(i);
  }

  //cout<<"truncated mean calculate w error"<<numerator/denominator<<"/"<< hist->GetRMS()/sqrt(total_to_keep)<<" of histo w error "<<test_histo->GetMean(1)<<"/"<<test_histo->GetMeanError(1)<<" most prob val "<<hist->GetBinCenter(hist->GetMaximumBin())<<endl;

  return test_histo;

}


bool Test_GasGain::checkResults(std::string cscID)
{
  
  bool isValid=true;
  cscTestData::iterator td_itr =  tdata.find(cscID);
  //unused  TestData& cscdata= td_itr->second;
  //unused  TestData2D& mask = cscdata["_MASK"];

  return isValid;
}



