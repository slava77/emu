#include "emu/dqm/calibration/Test_CFEB04.h"

#include <stdexcept>    // std::out_of_range
#include <numeric>      // std::accumulate
#include <cmath>        // fabs
#include <limits>       // std::numeric_limits

using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;

using namespace std;
/**
 * @brief Constructor for Test CFEB04: Amplifier Gain
 * @param dfile Events Data file name
 */
Test_CFEB04::Test_CFEB04(std::string dfile):
  Test_Generic(dfile)
{

  testID = "CFEB04";
  nExpectedEvents = 8000; // Number of expected events for this test
  dduID=0;
  binCheckMask=0x1FFB7BF6; /// Examiner mask
  ltc_bug=2;

  gdata.clear();
  tpdata.clear();
  htree.clear();

  logger = Logger::getInstance(testID);

}

/*
 * @brief Initialization of CSC data
 * @param cscID CSC ID string "ME+1.1.01"
 */
void Test_CFEB04::initCSC(std::string cscID)
{

  nCSCEvents[cscID]=0;

  // Initialize internal data structire for Gains analysis
  LOG4CPLUS_INFO(logger, "Data Format Version: " << theFormatVersion );
  GainData gaindata;
  gaindata.Nbins = getNumStrips(cscID, theFormatVersion);
  gaindata.Nlayers = 6;
  memset(gaindata.content, 0, sizeof (gaindata.content));
  memset(gaindata.fit, 0, sizeof (gaindata.fit));
  gdata[cscID] = gaindata;

  // Initialize internal data structure for Pulse Peaking time
  TPeakData tpeakdata;
  tpeakdata.Nlayers =6;
  memset(tpeakdata.content, 0, sizeof (tpeakdata.content));
  tpdata[cscID] = tpeakdata;

  TestData cscdata;
  TestData2D cfebdata;
  cfebdata.Nbins = getNumStrips(cscID, theFormatVersion);
  cfebdata.Nlayers = 6;
  memset(cfebdata.content, 0, sizeof (cfebdata.content));
  memset(cfebdata.cnts, 0, sizeof (cfebdata.cnts));
  LOG4CPLUS_INFO(logger, "Number of strips: " << cfebdata.Nbins );

  test_step tstep;
  tstep.active_strip=0;
  tstep.dac_step=1;
  tstep.evt_cnt=0;
  tstep.max_adc = -1;
  test_steps[cscID]=tstep;

  htree[dduID][cscID]=tstep;


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

  // R01 - Gain slope A
  cscdata["R01"]=cfebdata;

  // R02 - Gain intercept B
  // cscdata["R02"]=cfebdata;

  // R03 - Gain non-linearity
  cscdata["R03"]=cfebdata;

  // R04 - Saturation
  cscdata["R04"]=cfebdata;

  // R05 - Normalized gains for data analyses
  cscdata["R05"]=cfebdata;

  // R06 - tpeak data for cathode timing
  cfebdata.Nbins = getNumStrips(cscID, theFormatVersion)/16;
  cscdata["R06"]=cfebdata;

  tdata[cscID] = cscdata;;

  bookTestsForCSC(cscID);
}

/*
 * @brief Event analyzer
 * @param data pointer to event data buffer
 * @param dataSize event buffer size
 * @param errorStat buffer check status from DDU reader
 * @param nodeNumber RUI number
 */
void Test_CFEB04::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
{

  nTotalEvents++;

  // Perform Examiner binary checks
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

  // Unpack DDU event
  CSCDDUEventData dduData((uint16_t *) data, &bin_checker);

  // Extract L1A
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

  // Detect if DDU L1A de-synchronization
  if ((DDUstats[dduID].l1a_cntr-DDUstats[dduID].first_l1a) != (DDUstats[dduID].evt_cntr-1))
    {
      LOG4CPLUS_WARN(logger, "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << dduID
                     << " Desynched L1A: " << ((DDUstats[dduID].l1a_cntr-DDUstats[dduID].first_l1a) - (DDUstats[dduID].evt_cntr-1)));
    }

  // Vector of unpacked CSC blocks
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
    {
      if  (DDUstats[dduID].empty_evt_cntr==0)
        {
          LOG4CPLUS_INFO(logger, "No LTC/TTC double L1A bug in data");
          ltc_bug=1;
        }
      else
        {
          LOG4CPLUS_WARN(logger, "Found LTC/TTC double L1A bug in data. Expected Events x2: " << (nExpectedEvents * 2) );
        }
    }

  int dacSwitch=25*ltc_bug;
  int stripSwitch=500*ltc_bug;

  if (currL1A % stripSwitch==1)
    {
      DDUstats[dduID].strip=currL1A/ stripSwitch + 1;
    }
  if (currL1A% dacSwitch ==1)
    {
      DDUstats[dduID].dac=(currL1A / dacSwitch) % 20;
      DDUstats[dduID].strip=currL1A / stripSwitch + 1;
      DDUstats[dduID].empty_evt_cntr=0;

      fSwitch=true;
      std::map<std::string, test_step> & cscs = htree[dduID];
      for (std::map<std::string, test_step>::iterator itr = cscs.begin(); itr != cscs.end(); ++itr)
        {
          itr->second.evt_cnt = 0;
        }
    }

  // Loop over CSCs and do preliminary analysis/fill data structures
  for (std::vector<CSCEventData>::iterator chamberDataItr = chamberDatas.begin();
       chamberDataItr != chamberDatas.end(); ++chamberDataItr)
    {
      analyzeCSC(*chamberDataItr);
    }

  DDUstats[dduID].last_empty=chamberDatas.size();

}

/**
 * @brief Event analyzer for single CSC
 * @param data CSCEventData object
 */
void Test_CFEB04::analyzeCSC(const CSCEventData& data)
{

  // == Check for presence of DMB Header and Trailer
  const CSCDMBHeader* dmbHeader = data.dmbHeader();
  const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();
  if (!dmbHeader && !dmbTrailer)
    {
      return;
    }

  theFormatVersion = data.getFormatVersion();

  // == Get CSC ID string
  int csctype=0, cscposition=0;
  std::string cscID = getCSCFromMap(dmbHeader->crateID(), dmbHeader->dmbID(), csctype, cscposition);

  // == Do not process unmapped CSCs
  if (cscID == "") return;

  // == Init CSC if it's not in the list
  cscTestData::iterator td_itr = tdata.find(cscID);
  if ( (td_itr == tdata.end()) || (tdata.size() == 0) )
    {
      LOG4CPLUS_INFO(logger, "Found " << cscID);
      initCSC(cscID);
      addCSCtoMap(cscID, dmbHeader->crateID(), dmbHeader->dmbID());
    }
  nCSCEvents[cscID]++;

  // == Define aliases to access chamber specific data

  TestData& cscdata = tdata[cscID];

  TestData2D& r04 = cscdata["R04"];

  MonHistos& cschistos = mhistos[cscID];

  test_step& tstep = htree[dduID][cscID];// test_steps[cscID];

  GainData& gaindata = gdata[cscID];

  TPeakData& tpeakdata = tpdata[cscID];

  int curr_dac = DDUstats[dduID].dac;
  int curr_strip =  DDUstats[dduID].strip;

  tstep.evt_cnt++;

  TH2F* v02 = reinterpret_cast<TH2F*>(cschistos["V02"]);
  TH2F* v03 = reinterpret_cast<TH2F*>(cschistos["V03"]);
  TH1F* v04 = reinterpret_cast<TH1F*>(cschistos["V04"]);

  unsigned int l1a_cnt = dmbHeader->l1a();
  if (l1a_cnt < l1a_cntrs[cscID]) l1a_cnt+=256;

  // == L1A increment
  if (v04) v04->Fill(l1a_cnt-l1a_cntrs[cscID]);

  l1a_cntrs[cscID]=dmbHeader->l1a();

  // == Check if CFEB Data Available
  if (dmbHeader->cfebAvailable())
    {

      int nCFEBs = getNumStrips(cscID, theFormatVersion)/16;
      for (int icfeb=0; icfeb<nCFEBs; icfeb++)   // loop over cfebs in a given chamber
        {

          CSCCFEBData * cfebData =  data.cfebData(icfeb);
          if (!cfebData || !cfebData->check()) continue;

          for (unsigned int layer = 1; layer <= 6; layer++)   // loop over layers in a given chamber
            {

              int nTimeSamples= cfebData->nTimeSamples();
              double Qmax=gaindata.content[curr_dac][layer-1][icfeb*16+curr_strip-1][NSAMPLES].max;

              if (!cfebData->timeSlice(0)->checkCRC() || !cfebData->timeSlice(1)->checkCRC())
                {
                  LOG4CPLUS_WARN(logger, cscID << " CRC check failed for time sample 1 and 2");
                  continue;
                }

              // == Calculate pedestal
              double Q12=((cfebData->timeSlice(0))->timeSample(layer,curr_strip,cfebData->isDCFEB())->adcCounts
                          + (cfebData->timeSlice(1))->timeSample(layer,curr_strip,cfebData->isDCFEB())->adcCounts)/2.;

              for (int itime=0; itime<nTimeSamples; itime++)   // loop over time samples (8 or 16)
                {

                  if (!(cfebData->timeSlice(itime)->checkCRC()))
                    {
                      LOG4CPLUS_WARN(logger,"Evt#" << std::dec << nCSCEvents[cscID] << ": " << cscID
                                     << " CRC failed, cfeb" << icfeb << ", layer" << layer << ", strip" << curr_strip
                                     << ", dac" << curr_dac << ", time sample " << itime);
                      // continue;
                    }

                  CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,curr_strip,cfebData->isDCFEB());

                  int Qi = (int) ((timeSample->adcCounts)&0xFFF);

                  gaindata.content[curr_dac][layer-1][icfeb*16+curr_strip-1][itime].cnt++;
                  gaindata.content[curr_dac][layer-1][icfeb*16+curr_strip-1][itime].mv += Qi-Q12;
                  gaindata.content[curr_dac][layer-1][icfeb*16+curr_strip-1][itime].rms += pow(Qi-Q12,2);

                  if (Qi-Q12>Qmax)
                    {
                      Qmax=Qi-Q12;

                      if (curr_dac==DAC_STEPS-1) r04.content[layer-1][icfeb*16+curr_strip-1] = Qi; // Saturation

                      gaindata.content[curr_dac][layer-1][icfeb*16+curr_strip-1][NSAMPLES].max=Qmax;
                    }


                }

              // == Calibration curve
              if (v03)
                {
                  v03->Fill(curr_dac, Qmax);
                }

              // == Signal line shape at 6th DAC step
              if (curr_dac==5)
                {
                  double Q12=((cfebData->timeSlice(0))->timeSample(layer,curr_strip,cfebData->isDCFEB())->adcCounts
                              + (cfebData->timeSlice(1))->timeSample(layer,curr_strip,cfebData->isDCFEB())->adcCounts)/2.;

                  for (int itime=0; itime<nTimeSamples; itime++)   // loop over time samples (8 or 16)
                    {
                      CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,curr_strip,cfebData->isDCFEB());
                      int Qi = (int) ((timeSample->adcCounts)&0xFFF);
                      if (v02)
                        {
                          v02->Fill(itime, Qi-Q12);
                        }
                    }
                }

	      if (curr_dac==5){
		vector<double> adcCounts( nTimeSamples, 0. );
		for (int itime=0; itime<nTimeSamples; itime++){
		  adcCounts[itime] = (double) ( ( (cfebData->timeSlice( itime ))->timeSample( layer, curr_strip, cfebData->isDCFEB() )->adcCounts)&0xFFF );
		}
		Test_CFEB04::extremum_t peak = correctedPulsePeak( adcCounts );
		tpeakdata.content[layer-1][icfeb].cnt++;
		tpeakdata.content[layer-1][icfeb].mv  += peak.x_corr;
		tpeakdata.content[layer-1][icfeb].rms += pow(peak.x_corr,2);
		//LOG4CPLUS_DEBUG(logger, "layer=" << layer << " CFEB=" << icfeb << " T_peak=" << peakTime << " count=" << tpeakdata.content[layer-1][icfeb].cnt );
	      }

            } // layers loop end
        } // cfebs loop end


    } // CFEB data available

}

Test_CFEB04::extremum_t Test_CFEB04::findMaximum( vector<double>& v ) const {
  // Find the index of the maximum, and then fit a parabola to the three points (max and its neighbors) to refine the position.
  // If the maximum is the first or the last element, return its value.

  Test_CFEB04::extremum_t m;
  
  // The index of the maximum:
  vector<double>::const_iterator iMax = max_element( v.begin(), v.end() );
  if ( v.size() < 3 ){
    m.x      = double ( iMax - v.begin() );
    m.y      = *iMax;
    m.x_corr = m.x;
    m.y_corr = m.y;
    return m;
  }
  vector<double>::const_iterator iPrev = iMax;
  vector<double>::const_iterator iNext = iMax;
  ++iNext;
  if ( iMax == v.begin() ){ // the max is the first element
    if ( *iMax == *iNext ){ // the first two elements are equal
      // Take the second element as max:
      iPrev = iMax;
      iMax  = iNext;
      ++iNext;
    }
    else{
      m.x      = double ( iMax - v.begin() );
      m.y      = *iMax;
      m.x_corr = m.x;
      m.y_corr = m.y;
      return m;
    }
  }
  else if ( iNext == v.end() ){ // the max is the last element
    --iPrev;
    if ( *iPrev == *iMax ){ // the last two elements are equal
      // Take the penultimate element as max:
      iNext = iMax;
      iMax = iPrev;
      --iPrev;
    }
    else{
      m.x      = double ( iMax - v.begin() );
      m.y      = *iMax;
      m.x_corr = m.x;
      m.y_corr = m.y;
      return m;
    }
  }
  else{ // max is neither the first nor the last element
    --iPrev;
  }

  m.x      = double( iMax - v.begin() );
  m.y      = *iMax;
  m.x_corr = m.x;
  m.y_corr = m.y;

  // Refine the position of the maximum by fitting a parabola.

  // Sum_(i=0..2) p_i x^i = y_i
  // V * p = y   ==>  p = V^-1 * y
  // Maximum of parabola at p_1 / p_2 / 2
  vector<double> x( 3 );
  vector<double> y( 3 );
  vector< vector<double> > V( 3, vector<double>( 3, 0. ) ); // Vandermonde matrix
  x[0] = m.x-1.; y[0] = *iPrev ;
  x[1] = m.x   ; y[1] = *iMax ;
  x[2] = m.x+1.; y[2] = *iNext;
  for ( size_t i=0; i<3; ++i ){
    V[i][0] =        1.;
    V[i][1] =      x[i];
    V[i][2] = x[i]*x[i];
  }
  vector<double> p( invert3x3Matrix( V ) * y );
  if ( fabs( p[2] ) > std::numeric_limits<double>::epsilon() ){
    m.x_corr = - p[1]/(2*p[2]);
    m.y_corr = p[0] + p[1]*m.x_corr + p[2]*m.x_corr*m.x_corr;
  }
  //   cout << "x     " << m.x     <<  endl;
  //   cout << "y     " << m.y     <<  endl;
  //   cout << "x_corr " << m.x_corr <<  endl;
  //   cout << "y_corr " << m.y_corr <<  endl;
  return m;
}

Test_CFEB04::extremum_t Test_CFEB04::correctedPulsePeak(vector<double>& points) const
{
  // Correct for the asymmetric, non-parabolic peak of the pulse.
  
  Test_CFEB04::extremum_t m = findMaximum( points );

  const double timeSamplePitch = 50.; // ns

  double x0 =   m.x_corr         * timeSamplePitch;
  double dX = ( m.x_corr - m.x ) * timeSamplePitch;
  double  A =   m.y_corr;
  
  double kp[4] = {1.022, -0.027, 7.6, 63.};
  double taup[4] = {-1.5, -2.5, -5, 50};

  double k = kp[0] + kp[1]*cos(2*M_PI*((dX-kp[2])/kp[3]));
  double tau = taup[0] + taup[1]*cos(2*M_PI*((dX-taup[2])/taup[3]));

  Test_CFEB04::extremum_t m_corr; // the corrected maximum
  m_corr.x      = m.x_corr * timeSamplePitch;
  m_corr.y      = m.y_corr;
  m_corr.x_corr = x0+tau;
  m_corr.y_corr = A*k;

  //   cout << "x0     " << x0 <<  endl;
  //   cout << "A      " << A <<  endl;
  //   cout << "dX     " << dX <<  endl;
  //   cout << "x0+tau " << x0+tau <<  endl;
  //   cout << "A*k    " << A*k <<  endl;

  return m_corr;
}

vector< vector<double> > Test_CFEB04::invert3x3Matrix( const vector< vector<double> >& m ) const {
  vector< vector<double> > inverse( 3, vector<double>( 3, 0. ) );
  double det = 
    + m[0][0] * ( m[1][1]*m[2][2] - m[1][2]*m[2][1] )
    + m[0][1] * ( m[1][2]*m[2][0] - m[1][0]*m[2][2] )
    + m[0][2] * ( m[1][0]*m[2][1] - m[1][1]*m[2][0] );

  if ( fabs( det ) < std::numeric_limits<double>::epsilon() ) return inverse;

  inverse[0][0] = m[1][1]*m[2][2] - m[1][2]*m[2][1]; inverse[0][1] = m[0][2]*m[2][1] - m[0][1]*m[2][2]; inverse[0][2] = m[0][1]*m[1][2] - m[0][2]*m[1][1];
  inverse[1][0] = m[1][2]*m[2][0] - m[1][0]*m[2][2]; inverse[1][1] = m[0][0]*m[2][2] - m[0][2]*m[2][0]; inverse[1][2] = m[0][2]*m[1][0] - m[0][0]*m[1][2];
  inverse[2][0] = m[1][0]*m[2][1] - m[1][1]*m[2][0]; inverse[2][1] = m[0][1]*m[2][0] - m[0][0]*m[2][1]; inverse[2][2] = m[0][0]*m[1][1] - m[0][1]*m[1][0];

  for ( int i=0; i<3; ++i ) for ( int j=0; j<3; ++j ) inverse[i][j] /= det;

  return inverse;
}

/**
 * @brief finalize analysis for single CSC
 * @param cscID CSC ID string
 */
void Test_CFEB04::finishCSC(std::string cscID)
{
  /*
     if (nCSCEvents[cscID] < nExpectedEvents/2) {
     std::cout << Form("%s: Not enough events for test analysis (%d events)", cscID.c_str(), nCSCEvents[cscID] ) << std::endl;
     // = Set error
     return;
     }
  */

  // == Look for CSC in the list
  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end())   // if CSC present in the list
    {

      TestData& cscdata= td_itr->second;

      TestData2D& mask = cscdata["_MASK"];

      TestData2D& r01 = cscdata["R01"];
      // TestData2D& r02 = cscdata["R02"]; // Removed gain intercept results
      TestData2D& r03 = cscdata["R03"];
      TestData2D& r05 = cscdata["R05"];
      TestData2D& r06 = cscdata["R06"];


      TPeakData& tpeakdata = tpdata[cscID];
      GainData& gaindata = gdata[cscID];
      MonHistos& cschistos = mhistos[cscID];
      TH2F* v01 = reinterpret_cast<TH2F*>(cschistos["V01"]);
      TH2F* v05 = reinterpret_cast<TH2F*>(cschistos["V05"]);
      TH1F* v06 = reinterpret_cast<TH1F*>(cschistos["V06"]);

      ResultsCodes& rcodes = rescodes[cscID];

      CSCtoHWmap::iterator itr = cscmap.find(cscID);

      if (itr != cscmap.end())
        {

          int dmbID = itr->second.second;
          if (dmbID >= 6) --dmbID;
          int id = 10*itr->second.first+dmbID;

          CSCMapItem::MapItem mapitem = cratemap->item(id);
          int first_strip_index=mapitem.stripIndex;
          // int strips_per_layer=mapitem.strips;
          int strips_per_layer = getNumStrips(cscID, theFormatVersion);

          double avg_gain=0;
          int avg_gain_cnt=0;

          for (unsigned int layer = 1; layer <= 6; layer++)
            {
              int nCFEBs = getNumStrips(cscID, theFormatVersion)/16;
              for (int icfeb=0; icfeb<nCFEBs; icfeb++)   // loop over cfebs in a given chamber
                {
                  {
		    
                    {
                      double tpeak_avg=0;
                      int tpeak_cnt=0;
                      double tpeak_rms=0.0;
                      dac_step& val= tpeakdata.content[layer-1][icfeb];
                      if (val.cnt<16)
                        {
                          LOG4CPLUS_DEBUG(logger, cscID << ":" << "CFEB" << (icfeb+1) << ":layer" << layer
                                          << ", cnt="<< val.cnt);
                        }
                      else
                        {
                          tpeak_avg=val.mv/val.cnt;
                          tpeak_cnt=val.cnt;
                          tpeak_rms=sqrt((val.rms/val.cnt)-pow(val.mv,2));
			  // LOG4CPLUS_DEBUG(logger, "layer=" << layer 
			  // 		  << " CFEB=" << icfeb 
			  // 		  << " <T_peak>=" << tpeak_avg
			  // 		  << " count=" << tpeak_cnt );
                        }
                      r06.content[layer-1][icfeb]=tpeak_avg;
                    }



                    for (int strip = 1; strip <=16  ; ++strip)   // loop over cfeb strip
                      {

                        bool fValidStripData=true;
                        for (int dac=0; dac<DAC_STEPS; dac++)
                          {

                            bool fValidDAC=true;

                            dac_step& val= gaindata.content[dac][layer-1][icfeb*16+strip-1][NSAMPLES];

                            double max=0;
                            double max_rms=0;
                            int cnt=0;
                            int peak_time=0;

                            for (int itime=0; itime < NSAMPLES; itime++)
                              {

                                dac_step& cval = gaindata.content[dac][layer-1][icfeb*16+strip-1][itime];
                                cnt = cval.cnt;

                                if (cval.cnt<13)
                                  {
                                    LOG4CPLUS_DEBUG(logger, cscID << ":" << layer << ":" << (icfeb*16+strip)
                                                    << " Error> dac=" << dac << " , sample=" << itime << ", cnt="<< cval.cnt);
                                    if (dac<10) fValidStripData=false;
                                    fValidDAC=false;
                                  }
                                else
                                  {
                                    cval.mv /=cval.cnt;
                                    double rms= sqrt((cval.rms/cval.cnt)-pow(cval.mv,2));
                                    cval.rms = rms;
                                    if (cval.mv > max)
                                      {
                                        peak_time = itime;
                                        max=cval.mv;
                                        max_rms=cval.rms;
                                        cnt=cval.cnt;
                                      }
                                  }

                              }

                            val.mv = max;
                            val.rms = max_rms;
                            val.cnt = cnt;

                            if (v01)
                              {
                                v01->Fill(dac,cnt);
                              }
                            if (!fValidDAC)
                              {
                                rcodes["V01"] = 4;
                              }

                            if (cnt>0 && fValidDAC)
                              {

                                pulse_fit& fit = gaindata.fit[dac][layer-1][icfeb*16+strip-1];

                                fit.max.tbin = peak_time;
                                fit.max.value = (int)max;

                                if (peak_time>0)
                                  {
                                    dac_step& cval = gaindata.content[dac][layer-1][icfeb*16+strip-1][peak_time-1];
                                    fit.left.tbin = peak_time-1;
                                    fit.left.value = (int)cval.mv;
                                  }
                                else
                                  {
                                    fit.left.tbin = 0;
                                    fit.left.value = 0;
                                  }

                                if (peak_time<NSAMPLES)
                                  {
                                    dac_step& cval = gaindata.content[dac][layer-1][icfeb*16+strip-1][peak_time+1];
                                    fit.right.tbin = peak_time+1;
                                    fit.right.value = (int)cval.mv;
                                  }
                                else
                                  {
                                    fit.right.tbin = 0;
                                    fit.right.value = 0;
                                  }

                                // time_sample pulse_fit  = CalculateCorrectedPulseAmplitude(fit);
                                // if (v06) {v06->Fill(pulse_fit.tbin);}

                                val.s = pow(max_rms,2) + pow((0.01*max), 2);

                                /*  LOG4CPLUS_DEBUG(logger, cscID << ":" << std::dec << layer << ":" << (icfeb*16+strip) << " dac=" << dac << ", cnt=" << val.cnt
                                  << ", mv=" << val.mv << ", rms=" << val.rms
                                  << ", max=" << val.max << ", peak_tbin=" << peak_time
                                  << ", s=" << val.s << ", x=" << (11.2+28.0*dac) << ", corr_peak_amp=" << pulse_fit.value << ", corr_peak_time=" << pulse_fit.tbin
                                  << ", left:" << fit.left.value << ":" << fit.left.tbin
                                  << " peak:" << fit.max.value << ":" << fit.max.tbin
                                  << " right:" << fit.right.value << ":" << fit.right.tbin);
                                */
                              }

                          }

                        if (fValidStripData)
                          {
                            int dac_steps = 10;
                            double X=0., XX=0., Y=0., YY=0., XY=0., S=0., x=0., y=0., s=0.;
                            double a=0., b=0., chi2=0.;
                            double xdac[dac_steps], ydac[dac_steps];
                            bool fValidStrip=true;


                            for (int dac=0; dac<dac_steps; dac++)
                              {
                                dac_step& val= gaindata.content[dac][layer-1][icfeb*16+strip-1][NSAMPLES];

                                x=11.2 +(28.0*dac);
                                xdac[dac] = x;
                                s=val.s;

                                if (s==0)
                                  {
                                    fValidStrip = false;
                                    break;
                                  }

//                                 time_sample pulse_fit = CalculateCorrectedPulseAmplitude(gaindata.fit[dac][layer-1][icfeb*16+strip-1]);
//                                 if ((v06) && (dac==5))
//                                   {
//                                     v06->Fill(pulse_fit.tbin);
//                                   }
//                                 y = pulse_fit.value;

				pulse_fit pf = gaindata.fit[dac][layer-1][icfeb*16+strip-1];
				Test_CFEB04::extremum_t peak;
				peak.x_corr = pf.max.tbin;  // uncorrected default
				peak.y_corr = pf.max.value; // uncorrected default
				vector<double> adcCounts( NSAMPLES, 0. );
				if ( pf.left .tbin >= 0        &&
				     pf.right.tbin <  NSAMPLES    ){
				  adcCounts[pf.left .tbin] = (double) pf.left .value;
				  adcCounts[pf.max  .tbin] = (double) pf.max  .value;
				  adcCounts[pf.right.tbin] = (double) pf.right.value;
				  peak = correctedPulsePeak( adcCounts );
				}
                                if ((v06) && (dac==5))
                                  {
                                    v06->Fill( peak.x_corr );
                                  }
                                y = peak.y_corr;


                                ydac[dac] = y;

                                X+=x/s;
                                XX+=(x*x)/s;
                                Y+=y/s;
                                YY+=(y*y)/s;
                                XY+=(y*x)/s;
                                S+=1/s;

                              }

                            if (fValidStrip)
                              {
                                a=(XY*S-X*Y)/(XX*S-X*X); // Gain Slope for 2 parameters fit
                                b=(Y-a*X)/S; // Gain Intercept
                                chi2=YY+a*a*XX+b*b*S-2*a*XY-2*b*Y+2*a*b*X; // Chi squared for 2-parameters fit

                                for (int dac=0; dac<dac_steps; dac++)
                                  {
                                    double residual = ydac[dac]-a*xdac[dac]-b;
                                    if (v05) v05->Fill(dac, residual);
                                  }
                              }
                            else
                              {
                                a = -999;
                                b = -999;
                                chi2 = -999;
                              }

                            if (a>999.) a=999.;
                            if (a<-999.) a=-999.;
                            if (chi2>999.) chi2=999.;
                            if (chi2<-999.) chi2=-999.;

                            if ((a<MAX_VALID_SLOPE) && (a>MIN_VALID_SLOPE))
                              {
                                avg_gain+=1/a;
                                avg_gain_cnt++;
                              }

                            // LOG4CPLUS_DEBUG(logger, cscID << ":" << std::dec << layer << ":" << (icfeb*16+strip) << " a=" << a << ", b=" << b << ", chi2=" << chi2);
                            double gain=a;

                            r01.content[layer-1][icfeb*16+strip-1] = gain;
                            //r02.content[layer-1][icfeb*16+strip-1] = b;
                            r03.content[layer-1][icfeb*16+strip-1] = chi2;
                          }

                      } // strip
                  }
                }
            }


          if (avg_gain_cnt)
            {
              avg_gain /= avg_gain_cnt;
            }
          else
            {
              avg_gain = 0;
            }
          for (unsigned int layer = 1; layer <= 6; layer++)
            {
              for (int strip=0; strip<getNumStrips(cscID, theFormatVersion); strip++)   // loop over cfebs in a given chamber
                {
                  double a = r01.content[layer-1][strip];
                  if ((a<MAX_VALID_SLOPE) && (a>MIN_VALID_SLOPE) && (avg_gain>0) )
                    {
                      r05.content[layer-1][strip]=1/(a*avg_gain);
                    }
                  else
                    {
                      r05.content[layer-1][strip] = -1;
                    }
                }
            }


          // == Save results to text files
          std::string rpath = "Test_"+testID+"/"+outDir;
          std::string path = rpath+"/"+cscID+"/";


          //  if (checkResults(cscID)) { // Check if 20% of channels pedestals and rms are bad
          // == Save results for database transfer of gain slopes, intercepts, non-linearity and normalized gains
          std::ofstream res_out((path+cscID+"_"+testID+"_DB.dat").c_str());
          std::vector<std::string> tests;
          tests.push_back("R01");
          tests.push_back("R03");
          tests.push_back("R05");

          for (int layer=0; layer<NLAYERS; layer++)
            {
              if ( emu::dqm::utils::isME11(cscID) && (theFormatVersion >= 2013)) // Handle post-LS1 ME11s with 7 DCFEBs
                {

                  for (int strip=0; strip<64; strip++)
                    {
                      int ch_index = first_strip_index+layer*80+strip;
                      res_out << std::fixed << std::setprecision(6) <<  ch_index << " "
                              << r01.content[layer][strip] << " "
                              << r03.content[layer][strip] << " "
                              << r05.content[layer][strip] << " "
                              << (int)(mask.content[layer][strip]) << " "
                              << (int)checkChannel(cscdata, tests, layer, strip, cscID)
                              << std::endl;
                    }

                  // Zero 64-80 ME11 channels gap
                  for (int strip=64; strip<80; strip++)
                    {
                      int ch_index = first_strip_index+layer*80+strip;
                      res_out << std::fixed << std::setprecision(2) <<  ch_index << " "
                              << 0  << " " << 0 << " " << 0
                              << " " << 1 << " " << 1  << std::endl;
                    }

                  // Remap post-LS1 ME11a 48 channels
                  for (int strip=64; strip<strips_per_layer; strip++)
                    {
                      int ch_index = emu::dqm::utils::getME11a_first_strip_index(cscID, theFormatVersion) + layer*48 + (strip-64);
                      res_out << std::fixed << std::setprecision(6) <<  ch_index << " "
                              << r01.content[layer][strip] << " "
                              << r03.content[layer][strip] << " "
                              << r05.content[layer][strip] << " "
                              << (int)(mask.content[layer][strip]) << " "
                              << (int)checkChannel(cscdata, tests, layer, strip, cscID)
                              << std::endl;

                    }

                }
              else
                {

                  for (int strip=0; strip<strips_per_layer; strip++)
                    {
                      int ch_index = first_strip_index+layer*strips_per_layer+strip;
                      res_out << std::fixed << std::setprecision(6) <<  ch_index << " "
                              << r01.content[layer][strip] << " "
                              << r03.content[layer][strip] << " "
                              << r05.content[layer][strip] << " "
                              << (int)(mask.content[layer][strip]) << " "
                              << (int)checkChannel(cscdata, tests, layer, strip, cscID)
                              << std::endl;
                    }

                }
            }
          res_out.close();
        }
      else
        {
          LOG4CPLUS_WARN(logger, cscID << ": Invalid");
        }
    }
}

bool Test_CFEB04::checkResults(std::string cscID)
{
  bool isValid=true;
  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end())
    {
      TestData& cscdata= td_itr->second;
      TestData2D& r01 = cscdata["R01"];
      //TestData2D& r02 = cscdata["R02"];

      int badChannels=0;
      // Check Slopes
      for (int i=0; i<r01.Nlayers; i++)
        {
          for (int j=0; j<r01.Nbins; j++)
            {
              if ((r01.content[i][j] >= MAX_VALID_SLOPE) || (r01.content[i][j] <= MIN_VALID_SLOPE)) badChannels++;
            }
        }
      if (badChannels/(float(r01.Nlayers*r01.Nbins)) >=0.2)
        {
          isValid=false;
          LOG4CPLUS_WARN(logger,cscID << ": 20% of channels have bad Gain Slope");
        }
      /*
        badChannels=0;
        // Check Intercepts
        for (int i=0; i<r02.Nlayers; i++) {
        for (int j=0; j<r02.Nbins; j++) {
        if ((r02.content[i][j] > 50) || (r02.content[i][j] < -50)) badChannels++;
        }
        }
        if (badChannels/(float(r02.Nlayers*r01.Nbins)) >=0.2) {
        isValid=false;
        LOG4CPLUS_WARN(logger, cscID << ": 20% of channels have bad Gain Intercept");
        }
      */
    }

  return isValid;
}



double Test_CFEB04::fivePoleFitTime( int tmax,  double* adc, double t_peak)
{
  double tb[4] = {200., 250., 300., 350.};
  double fpNorm   = adc[1]; // this is tmax bin
  double t0       = 0.;
  double t0peak   = 109.6;   // this is offset of peak from start time t0
  //unused  double p0       = 4./t0peak;
  int n_fit  = 4;
  if ( tmax == 6 ) n_fit = 3;

  double chi_min  = 1.e10;
  double chi_last = 1.e10;
  double tt0      = 0.;
  double chi2     = 0.;
  double del_t    = 110.;

  double x[4];
  double sx2 = 0.;
  double sxy = 0.;
  double fN  = 0.;

  while ( del_t > 0.2)
    {
      sx2 = 0.;
      sxy = 0.;

      for ( int j=0; j < n_fit; ++j )
        {
          double tdif = tb[j] - tt0;
          if(tdif<0.0)
            {
              x[j]=0.0;
            }
          else
            {
              x[j]=-.666555*exp(-0.54e-2*tdif)+0.323179e-3*exp(-0.3654e-1*tdif)*tdif*tdif
                   +4.47482e-7*exp(-0.3654e-1*tdif)*tdif*tdif*tdif*tdif
                   +0.207565e-1*exp(-0.3654e-1*tdif)*tdif+.6665550853*exp(-0.3654e-1*tdif)
                   +0.3354599056e-5*exp(-0.3654e-1*tdif)*tdif*tdif*tdif;
            }
          sx2 += x[j] * x[j];
          sxy += x[j] * adc[j];
        }
      fN = sxy / sx2; // least squares fit over time bins i to adc[i] = fN * fivePoleFunction[i]
      // Compute chi^2
      chi2 = 0.0;
      for (int j=0; j < n_fit; ++j) chi2 += (adc[j] - fN * x[j]) * (adc[j] - fN *x[j]);

      // Test on chi^2 to decide what to do
      if ( chi_last > chi2 )
        {
          if (chi2 < chi_min )
            {
              t0      = tt0;
              fpNorm  = fN;
            }
          chi_last  = chi2;
          tt0       = tt0 + del_t;
        }
      else
        {
          tt0      = tt0 - 2. * del_t;
          del_t    = del_t / 2.;
          tt0      = tt0 + del_t;
          chi_last = 1.0e10;
        }
    }
  return t0 + t0peak;
}

