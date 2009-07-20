#include "emu/dqm/calibration/Test_CFEB02.h"

using namespace XERCES_CPP_NAMESPACE;


//= For debug purposes =//
static int F_LAYER=1;
static int F_CFEB=2;
static int F_STRIP=2;
static char* F_CSC = "ME+2.2.28";



Test_CFEB02::Test_CFEB02(std::string dfile): Test_Generic(dfile)
{
  testID = "CFEB02";
  nExpectedEvents = 10000;
  //  binCheckMask=0x16CFF3F6;
  binCheckMask=0x1FEBF3F6;
  //  binCheckMask=0xF7CB3BF6;
  logger = Logger::getInstance(testID);
}


void Test_CFEB02::initCSC(std::string cscID)
{


  //= Init per CSC events counters
  nCSCEvents[cscID]=0;

  //= Init data structures for storing SCA data and results
  CFEBSCAData scadata;
  scadata.Nbins = getNumStrips(cscID);
  scadata.Nlayers = NLAYERS;
  memset(scadata.content, 0, sizeof (scadata.content));
  sdata[cscID] = scadata;

  TestData cscdata;
  TestData2D cfebdata;
  cfebdata.Nbins = getNumStrips(cscID);
  cfebdata.Nlayers = NLAYERS;
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

  // Q6
  cscdata["_Q6"]=cfebdata;

  // Q7
  cscdata["_Q7"]=cfebdata;

  // R01 - Overall pedestal
  cscdata["R01"]=cfebdata;

  // R02 - Overall noise
  cscdata["R02"]=cfebdata;

  // R03 - RMS of SCA pedestals
  cscdata["R03"]=cfebdata;

  // R04 - P12 pedestal
  cscdata["R04"]=cfebdata;

  // R05 - P12 noise
  cscdata["R05"]=cfebdata;

  // R06 - P345 pedestal
  cscdata["R06"]=cfebdata;

  // R07 - P345 Noise
  cscdata["R07"]=cfebdata;

  // R08 - C(12)(12) covariance matrix element
  cscdata["R08"]=cfebdata;

  // R09 - C(12)3 covariance matrix element
  cscdata["R09"]=cfebdata;

  // R10 - C(12)4 covariance matrix element
  cscdata["R10"]=cfebdata;

  // R11 - C(12)5 covariance matrix element
  cscdata["R11"]=cfebdata;

  // R12 - C33 covariance matrix element
  cscdata["R12"]=cfebdata;

  // R13 - C34 covariance matrix element
  cscdata["R13"]=cfebdata;

  // R14 - C35 covariance matrix element
  cscdata["R14"]=cfebdata;

  // R15 - C44 covariance matrix element
  cscdata["R15"]=cfebdata;

  // R16 - C45 covariance matrix element
  cscdata["R16"]=cfebdata;

  // R17 - C55 covariance matrix element
  cscdata["R17"]=cfebdata;

  // R18 - C46 covariance matrix element
  cscdata["R18"]=cfebdata;

  // R19 - C56 covariance matrix element
  cscdata["R19"]=cfebdata;

  // R20 - C66 covariance matrix element
  cscdata["R20"]=cfebdata;

  // R21 - C57 covariance matrix element
  cscdata["R21"]=cfebdata;

  // R22 - C67 covariance matrix element
  cscdata["R22"]=cfebdata;

  // R23 - C77 covariance matrix element
  cscdata["R23"]=cfebdata;

  tdata[cscID] = cscdata;;

  bookTestsForCSC(cscID);
}

void Test_CFEB02::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
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
      // std::cout << "Evt#" << std::dec << nTotalEvents << ": Nonzero Binary Errors Status is observed: 0x"<< std::hex << bin_checker.errors()
      //	<< " mask:0x" << std::hex << binCheckMask << std::dec << std::endl;
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


void Test_CFEB02::analyzeCSC(const CSCEventData& data)
{

  int conv_blk[16]={0,1,2,3,4,5,6,7,8,8,9,9,10,10,11,11}; // Mapping of SCA blocks

  const CSCDMBHeader* dmbHeader = data.dmbHeader();
  const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();
  if (!dmbHeader && !dmbTrailer)
    {
      return;
    }


  int csctype=0, cscposition=0;
  std::string cscID = getCSCFromMap(dmbHeader->crateID(), dmbHeader->dmbID(), csctype, cscposition);
  // std::string cscID(Form("CSC_%03d_%02d", data.dmbHeader().crateID(), data.dmbHeader().dmbID()));
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

  // std::cout << nCSCEvents[cscID] << " " << cscID << std::endl;
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
  TestData2D& _q6 = cscdata["_Q6"];
  TestData2D& _q7 = cscdata["_Q7"];

  TestData2D& r01 = cscdata["R01"];
  TestData2D& r02 = cscdata["R02"];
  //  TestData2D& r03 = cscdata["R03"];
  TestData2D& r04 = cscdata["R04"];
  TestData2D& r05 = cscdata["R05"];
  TestData2D& r06 = cscdata["R06"];
  TestData2D& r07 = cscdata["R07"];
  TestData2D& r08 = cscdata["R08"];
  TestData2D& r09 = cscdata["R09"];
  TestData2D& r10 = cscdata["R10"];
  TestData2D& r11 = cscdata["R11"];
  TestData2D& r12 = cscdata["R12"];
  TestData2D& r13 = cscdata["R13"];
  TestData2D& r14 = cscdata["R14"];
  TestData2D& r15 = cscdata["R15"];
  TestData2D& r16 = cscdata["R16"];
  TestData2D& r17 = cscdata["R17"];
  TestData2D& r18 = cscdata["R18"];
  TestData2D& r19 = cscdata["R19"];
  TestData2D& r20 = cscdata["R20"];
  TestData2D& r21 = cscdata["R21"];
  TestData2D& r22 = cscdata["R22"];
  TestData2D& r23 = cscdata["R23"];

  CFEBSCAData& scadata = sdata[cscID];

  MonHistos& cschistos = mhistos[cscID];
  TH2F* v01 = reinterpret_cast<TH2F*>(cschistos["V01"]);
  TH1F* v02 = reinterpret_cast<TH1F*>(cschistos["V02"]);
  TH1F* v03 = reinterpret_cast<TH1F*>(cschistos["V03"]);
  TH1F* v04 = reinterpret_cast<TH1F*>(cschistos["V04"]);
  TH1F* v05 = reinterpret_cast<TH1F*>(cschistos["V05"]);

  std::ofstream f;
  if (cscID == F_CSC) {
     f.open("dump.txt", std::ios::app);
  } 

  // == Check if CFEB Data Available
  if (dmbHeader->cfebAvailable())
    {
      double Qmax=0;
      for (int icfeb=0; icfeb<getNumStrips(cscID)/16; icfeb++)   // loop over cfebs in a given chamber
        {
          CSCCFEBData * cfebData =  data.cfebData(icfeb);
          if (!cfebData) continue;
          //    if (!cfebData->check()) continue;

          for (int layer = 1; layer <= NLAYERS; layer++)   // loop over layers in a given chamber
            {
              int nTimeSamples= cfebData->nTimeSamples();
              for (int strip = 1; strip <= 16; ++strip)   // loop over cfeb strips
                {
		  bool fSel = false;
		  if (cscID == F_CSC && icfeb == F_CFEB && layer == F_LAYER && strip == F_STRIP) fSel = true;
                  // - Sum of first two ADC samples
                  double Q12=((cfebData->timeSlice(0))->timeSample(layer,strip)->adcCounts
                              + (cfebData->timeSlice(1))->timeSample(layer,strip)->adcCounts)/2.;
                  // - Forth sample
                  double Q4 = (cfebData->timeSlice(3))->timeSample(layer,strip)->adcCounts;
                  // double Qmax=0;

                  if (v02) v02->Fill(Q4-Q12);

                  // int offset=0;
                  int blk_strt=0;
                  int cap=0;

		  if (fSel) f << nCSCEvents[cscID] << " " ; 

                  for (int itime=0; itime<nTimeSamples; itime++)   // loop over time samples (8 or 16)
                    {
                      CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,strip);
                      int Qi = (int) ((timeSample->adcCounts)&0xFFF);
                      if (v01) v01->Fill(itime, Qi-Q12);
                      if (Qi-Q12>Qmax) Qmax=Qi-Q12;
                      // - first 1000 events pedestals and rms for initial calculations
                      if (nEvents <= 1000)
                        {
                          _mv0.content[layer-1][icfeb*16+strip-1] += Qi;
                          _mv0.cnts[layer-1][icfeb*16+strip-1]++;
                          _rms0.content[layer-1][icfeb*16+strip-1] += pow(Qi,2);
                          _rms0.cnts[layer-1][icfeb*16+strip-1]++;
                        }
		      if (fSel) f << Qi << " ";

                    }
		  if (fSel) f << std::endl;

                  if (nEvents > 1000)
                    {
                      bool fEventValid = true;
                      double mv0 = _mv0.content[layer-1][icfeb*16+strip-1];
                      double rms0 = _rms0.content[layer-1][icfeb*16+strip-1];
                      // == Check that charges in all timebins satisfy |Qi - mv0| < 3*rms0
                      for (int itime=0; itime<nTimeSamples; itime++)
                        {
                          CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,strip);
                          int ADC = (int) ((timeSample->adcCounts)&0xFFF);
                          //= Three-sigma cut
                          if ( fabs(ADC-mv0) >= 3*rms0)
                            {
                              fEventValid=false;
                              /*
                              std::cout << "Evt" <<nEvents << " " << cscID << " " << layer << ":"<< (icfeb*16+strip) << " skipped "
                              	  <<  fabs(ADC-mv0) << ">=" << (3*rms0) <<  std::endl;
                              */
                              break;
                            }
                        }
                      if (fEventValid)
                        {
                          double Q12=0, Q345=0, Q3=0, Q4=0, Q5=0, Q6=0, Q7=0;
                          /*
                          double Q12=((cfebData->timeSlice(0))->timeSample(layer,strip)->adcCounts
                          + (cfebData->timeSlice(1))->timeSample(layer,strip)->adcCounts)/2.;
                          double Q345=((cfebData->timeSlice(2))->timeSample(layer,strip)->adcCounts
                          + (cfebData->timeSlice(3))->timeSample(layer,strip)->adcCounts
                          + (cfebData->timeSlice(4))->timeSample(layer,strip)->adcCounts)/3.;
                          double Q3 = (cfebData->timeSlice(2))->timeSample(layer,strip)->adcCounts;
                          double Q4 = (cfebData->timeSlice(3))->timeSample(layer,strip)->adcCounts;
                          double Q5 = (cfebData->timeSlice(4))->timeSample(layer,strip)->adcCounts;
                          double Q6 = (cfebData->timeSlice(5))->timeSample(layer,strip)->adcCounts;
                          double Q7 = (cfebData->timeSlice(6))->timeSample(layer,strip)->adcCounts;
                          */


                          for (int itime=0; itime<nTimeSamples; itime++)
                            {
                              CSCCFEBDataWord* timeSample=(cfebData->timeSlice(itime))->timeSample(layer,strip);
                              int Qi = (int) ((timeSample->adcCounts)&0xFFF);

                              //= Charge values Q per time sample
                              switch (itime)
                                {
                                case 0:
                                  Q12 = Qi;
                                  break;
                                case 1:
                                  Q12 += Qi;
                                  Q12 /= 2.;
                                  break;
                                case 2:
                                  Q345 = Qi;
                                  Q3 = Qi;
                                  break;
                                case 3:
                                  Q345 += Qi;
                                  Q4 = Qi;
                                  break;
                                case 4:
                                  Q345 += Qi;
                                  Q5 = Qi;
                                  Q345 /= 3.;
                                  break;
                                case 5:
                                  Q6 = Qi;
                                  break;
                                case 6:
                                  Q7 = Qi;
                                  break;
                                }


                              CSCCFEBSCAControllerWord scaControllerWord = (cfebData->timeSlice(itime))->scaControllerWord(layer);
                              int scaBlock  = (int)(scaControllerWord.sca_blk);
                              int trigTime = (int)(scaControllerWord.trig_time);
                              int lctPhase = (int)(scaControllerWord.lct_phase);

                              for (int pos=0; pos<8; pos++)
                                {
                                  if (trigTime==(1<<pos)) lctPhase=pos;
                                }

                              if (trigTime!=0)
                                {
                                  cap = lctPhase+itime;
                                  blk_strt =itime;
                                }
                              else
                                {
                                  cap=itime-blk_strt-1;
                                }

                              int scaNumber=8*conv_blk[scaBlock]+cap;

                              // int scaNumber = 8*conv_blk[scaBlock]+(offset+itime)%8;

                              if (scaNumber >= 96)
                                {
                                  LOG4CPLUS_ERROR(logger, "Invalid SCA cell");
                                }

                              scadata.content[layer-1][icfeb*16+strip-1][scaNumber].value += Qi;
                              scadata.content[layer-1][icfeb*16+strip-1][scaNumber].cnt++;
                              v03->Fill(scaNumber);
                              v04->Fill(conv_blk[scaBlock]);

                              r01.content[layer-1][icfeb*16+strip-1] += Qi;
                              r01.cnts[layer-1][icfeb*16+strip-1]++;
                              r02.content[layer-1][icfeb*16+strip-1] += pow(Qi,2);
                              r02.cnts[layer-1][icfeb*16+strip-1]++;
                            }

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

                          _q6.content[layer-1][icfeb*16+strip-1] += Q6;
                          _q6.cnts[layer-1][icfeb*16+strip-1]++;

                          _q7.content[layer-1][icfeb*16+strip-1] += Q7;
                          _q7.cnts[layer-1][icfeb*16+strip-1]++;

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

                          r18.content[layer-1][icfeb*16+strip-1] += Q4*Q6;
                          r18.cnts[layer-1][icfeb*16+strip-1]++;

                          r19.content[layer-1][icfeb*16+strip-1] += Q5*Q6;
                          r19.cnts[layer-1][icfeb*16+strip-1]++;

                          r20.content[layer-1][icfeb*16+strip-1] += Q6*Q6;
                          r20.cnts[layer-1][icfeb*16+strip-1]++;

                          r21.content[layer-1][icfeb*16+strip-1] += Q5*Q7;
                          r21.cnts[layer-1][icfeb*16+strip-1]++;

                          r22.content[layer-1][icfeb*16+strip-1] += Q6*Q7;
                          r22.cnts[layer-1][icfeb*16+strip-1]++;

                          r23.content[layer-1][icfeb*16+strip-1] += Q7*Q7;
                          r23.cnts[layer-1][icfeb*16+strip-1]++;


                        }
                    }
                }
            } // 1000 events
        }
      if (v05) v05->Fill(Qmax);

      // Calculate mv0 and rms0
      if (nEvents == 1000)
        {
          for (int i=0; i<_mv0.Nlayers; i++)
            {
              for (int j=0; j<_mv0.Nbins; j++)
                {

                  // Calculate Mean
                  if (_mv0.cnts[i][j]>0)
                    {
                      _mv0.content[i][j] /= (double)(_mv0.cnts[i][j]);
                    }

                  // Calculate RMS
                  if (_rms0.cnts[i][j]>0)
                    {
                      double rms = sqrt( ((_rms0.content[i][j])/((double)(_rms0.cnts[i][j]))- pow(_mv0.content[i][j],2)) );
                      _rms0.content[i][j]=rms;
                    }

                }
            }
        }

    } // CFEB data available

    if (cscID == F_CSC) {
     f.close();
  } 
}


void Test_CFEB02::finishCSC(std::string cscID)
{
  if (nCSCEvents[cscID] < nExpectedEvents/2)
    {
      LOG4CPLUS_ERROR(logger, Form("%s: Not enough events for test analysis (%d events)", cscID.c_str(), nCSCEvents[cscID] ));
      // = Set error
      return;
    }

  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end())
    {

      TestData& cscdata= td_itr->second;

      TestData2D& mask = cscdata["_MASK"];
      // TestData2D& _mv0 = cscdata["_MV0"];
      // TestData2D& _rms0 = cscdata["_RMS0"];
      TestData2D& _q12 = cscdata["_Q12"];
      TestData2D& _q3 = cscdata["_Q3"];
      TestData2D& _q4 = cscdata["_Q4"];
      TestData2D& _q5 = cscdata["_Q5"];
      TestData2D& _q6 = cscdata["_Q6"];
      TestData2D& _q7 = cscdata["_Q7"];


      TestData2D& r01 = cscdata["R01"];
      TestData2D& r02 = cscdata["R02"];
      TestData2D& r03 = cscdata["R03"];
      TestData2D& r04 = cscdata["R04"];
      TestData2D& r05 = cscdata["R05"];
      TestData2D& r06 = cscdata["R06"];
      TestData2D& r07 = cscdata["R07"];
      TestData2D& r08 = cscdata["R08"];
      TestData2D& r09 = cscdata["R09"];
      TestData2D& r10 = cscdata["R10"];
      TestData2D& r11 = cscdata["R11"];
      TestData2D& r12 = cscdata["R12"];
      TestData2D& r13 = cscdata["R13"];
      TestData2D& r14 = cscdata["R14"];
      TestData2D& r15 = cscdata["R15"];
      TestData2D& r16 = cscdata["R16"];
      TestData2D& r17 = cscdata["R17"];
      TestData2D& r18 = cscdata["R18"];
      TestData2D& r19 = cscdata["R19"];
      TestData2D& r20 = cscdata["R20"];
      TestData2D& r21 = cscdata["R21"];
      TestData2D& r22 = cscdata["R22"];
      TestData2D& r23 = cscdata["R23"];

      // ResultsCodes& rcodes = rescodes[cscID];

      CFEBSCAData& scadata = sdata[cscID];

      //    CSCMapItem::MapItem& mapitem;
      CSCtoHWmap::iterator itr = cscmap.find(cscID);

      if (itr != cscmap.end())
        {

          // map->cratedmb(itr->second.first,itr->second.second,&mapitem);
          int dmbID = itr->second.second;
          if (dmbID >= 6) --dmbID;
          int id = 10*itr->second.first+dmbID;

          CSCMapItem::MapItem mapitem = cratemap->item(id);
          int first_strip_index=mapitem.stripIndex;
          int strips_per_layer=mapitem.strips;

          // int strips_per_layer = getNumStrips(cscID);
          double rms = 0.;
          double covar = 0;

          for (int i=0; i<NLAYERS; i++)
            {
              for (int j=0; j<strips_per_layer; j++)
                {

                  if (_q12.cnts[i][j]) _q12.content[i][j] /= (double)(_q12.cnts[i][j]);
                  if (_q3.cnts[i][j]) _q3.content[i][j] /= (double)(_q3.cnts[i][j]);
                  if (_q4.cnts[i][j]) _q4.content[i][j] /= (double)(_q4.cnts[i][j]);
                  if (_q5.cnts[i][j]) _q5.content[i][j] /= (double)(_q5.cnts[i][j]);
                  if (_q6.cnts[i][j]) _q6.content[i][j] /= (double)(_q6.cnts[i][j]);
                  if (_q7.cnts[i][j]) _q7.content[i][j] /= (double)(_q7.cnts[i][j]);

                  // == Calculate Overall pedestals and noise
                  if (r01.cnts[i][j])
                    {
                      r01.content[i][j] /= (double)(r01.cnts[i][j]);

                      rms = sqrt( ((r02.content[i][j])/((double)(r02.cnts[i][j]))- pow(r01.content[i][j],2)) );
                      r02.content[i][j]=rms;
                    }
                  else
                    {
                      r01.content[i][j] = BAD_VALUE;
                      r02.content[i][j] = BAD_VALUE;
                    }

                  // == Calculate RMS of SCA pedestala
                  double sca_mean=0.;
                  double sca_mean_sum=0.;
                  double sca_mean_sq_sum=0;
                  int cells_cnt=0;
                  for (int k=0; k<96; k++)
                    {
                      if (scadata.content[i][j][k].cnt)
                        {
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
                  if (r04.cnts[i][j])
                    {
                      r04.content[i][j] /= (double)(r04.cnts[i][j]);

                      rms = sqrt( ((r05.content[i][j])/((double)(r05.cnts[i][j]))- pow(r04.content[i][j],2)) );
                      r05.content[i][j]=rms;
                    }
                  else
                    {
                      r04.content[i][j] = BAD_VALUE;
                      r05.content[i][j] = BAD_VALUE;
                    }

                  // == Calculate time samples 3,4,5 pedestals and noise
                  if (r06.cnts[i][j])
                    {
                      r06.content[i][j] /= (double)(r06.cnts[i][j]);
                      rms = sqrt( ((r07.content[i][j])/((double)(r07.cnts[i][j]))- pow(r06.content[i][j],2)) );

                      r07.content[i][j]=rms;
                    }
                  else
                    {
                      r06.content[i][j] = BAD_VALUE;
                      r07.content[i][j] = BAD_VALUE;
                    }

                  // == Calculate covariance matrix elements

                  // = C12|12
                  if (r08.cnts[i][j])
                    {
                      covar  =  ((r08.content[i][j]) /(double)(r08.cnts[i][j])) - _q12.content[i][j]*_q12.content[i][j];
                      r08.content[i][j] = covar;
                    }
                  else
                    {
                      r08.content[i][j] = BAD_VALUE;
                    }

                  // = C12|3
                  if (r09.cnts[i][j])
                    {
                      covar =  ((r09.content[i][j]) /(double)(r09.cnts[i][j])) - _q12.content[i][j]*_q3.content[i][j];
                      r09.content[i][j] = covar;
                    }
                  else
                    {
                      r09.content[i][j] = BAD_VALUE;
                    }


                  // = C12|4
                  if (r10.cnts[i][j])
                    {
                      covar =  ((r10.content[i][j]) /(double)(r10.cnts[i][j])) - _q12.content[i][j]*_q4.content[i][j];
                      r10.content[i][j] = covar;
                    }
                  else
                    {
                      r10.content[i][j] = BAD_VALUE;
                    }


                  // = C12|5
                  if (r11.cnts[i][j])
                    {
                      covar =  ((r11.content[i][j]) /(double)(r11.cnts[i][j])) - _q12.content[i][j]*_q5.content[i][j];
                      r11.content[i][j] = covar;
                    }
                  else
                    {
                      r11.content[i][j] = BAD_VALUE;
                    }


                  // = C3|3
                  if (r12.cnts[i][j])
                    {
                      covar =  ((r12.content[i][j]) /(double)(r12.cnts[i][j])) - _q3.content[i][j]*_q3.content[i][j];
                      r12.content[i][j] = covar;
                    }
                  else
                    {
                      r12.content[i][j] = BAD_VALUE;
                    }


                  // = C3|4
                  if (r13.cnts[i][j])
                    {
                      covar =  ((r13.content[i][j]) /(double)(r13.cnts[i][j])) - _q3.content[i][j]*_q4.content[i][j];
                      r13.content[i][j] = covar;
                    }
                  else
                    {
                      r13.content[i][j] = BAD_VALUE;
                    }


                  // = C3|5
                  if (r14.cnts[i][j])
                    {
                      covar =  ((r14.content[i][j]) /(double)(r14.cnts[i][j])) - _q3.content[i][j]*_q5.content[i][j];
                      r14.content[i][j] = covar;
                    }
                  else
                    {
                      r14.content[i][j] = BAD_VALUE;
                    }


                  // = C4|4
                  if (r15.cnts[i][j])
                    {
                      covar =  ((r15.content[i][j]) /(double)(r15.cnts[i][j])) - _q4.content[i][j]*_q4.content[i][j];
                      r15.content[i][j] = covar;
                    }
                  else
                    {
                      r15.content[i][j] = BAD_VALUE;
                    }


                  // = C4|5
                  if (r16.cnts[i][j])
                    {
                      covar =  ((r16.content[i][j]) /(double)(r16.cnts[i][j])) - _q4.content[i][j]*_q5.content[i][j];
                      r16.content[i][j] = covar;
                    }
                  else
                    {
                      r16.content[i][j] = BAD_VALUE;
                    }


                  // = C5|5
                  if (r17.cnts[i][j])
                    {
                      covar =  ((r17.content[i][j]) /(double)(r17.cnts[i][j])) - _q5.content[i][j]*_q5.content[i][j];
                      r17.content[i][j] = covar;
                    }
                  else
                    {
                      r17.content[i][j] = BAD_VALUE;
                    }


                  // = C4|6
                  if (r18.cnts[i][j])
                    {
                      covar =  ((r18.content[i][j]) /(double)(r18.cnts[i][j])) - _q4.content[i][j]*_q6.content[i][j];
                      r18.content[i][j] = covar;
                    }
                  else
                    {
                      r18.content[i][j] = BAD_VALUE;
                    }


                  // = C5|6
                  if (r19.cnts[i][j])
                    {
                      covar =  ((r19.content[i][j]) /(double)(r19.cnts[i][j])) - _q5.content[i][j]*_q6.content[i][j];
                      r19.content[i][j] = covar;
                    }
                  else
                    {
                      r19.content[i][j] = BAD_VALUE;
                    }


                  // = C6|6
                  if (r20.cnts[i][j])
                    {
                      covar =  ((r20.content[i][j]) /(double)(r20.cnts[i][j])) - _q6.content[i][j]*_q6.content[i][j];
                      r20.content[i][j] = covar;
                    }
                  else
                    {
                      r20.content[i][j] = BAD_VALUE;
                    }


                  // = C5|7
                  if (r21.cnts[i][j])
                    {
                      covar =  ((r21.content[i][j]) /(double)(r21.cnts[i][j])) - _q5.content[i][j]*_q7.content[i][j];
                      r21.content[i][j] = covar;
                    }
                  else
                    {
                      r21.content[i][j] = BAD_VALUE;
                    }


                  // = C6|7
                  if (r22.cnts[i][j])
                    {
                      covar =  ((r22.content[i][j]) /(double)(r22.cnts[i][j])) - _q6.content[i][j]*_q7.content[i][j];
                      r22.content[i][j] = covar;
                    }
                  else
                    {
                      r22.content[i][j] = BAD_VALUE;
                    }


                  // = C7|7
                  if (r23.cnts[i][j])
                    {
                      covar =  ((r23.content[i][j]) /(double)(r23.cnts[i][j])) - _q7.content[i][j]*_q7.content[i][j];
                      r23.content[i][j] = covar;
                    }
                  else
                    {
                      r23.content[i][j] = BAD_VALUE;
                    }



                }
            }

          // == Save results to text files
          std::string rpath = "Test_"+testID+"/"+outDir;
          std::string path = rpath+"/"+cscID+"/";

          std::ofstream res_out;
          /*
          res_out.open((path+cscID+"_CFEB02_01.results").c_str());
          res_out << "Layer Strip Pedestal Noise SCA_rms P12_Ped P12_rms P345_Ped P345_rms" << std::endl;
          for (int i=0; i<NLAYERS; i++) {
          for (int j=0; j<strips_per_layer; j++) {
          res_out << std::fixed << std::setprecision(2) << std::setw(5) << (i+1) << std::setw(6) << (j+1)
          << std::setw(9) << r01.content[i][j]  << std::setw(6) << r02.content[i][j]
          << std::setw(8) << r03.content[i][j]  << std::setw(8) << r04.content[i][j]
          << std::setw(8) << r05.content[i][j]  << std::setw(9) << r06.content[i][j]
          << std::setw(9) << r07.content[i][j]
          << std::endl;
          }
          }
          res_out.close();

          res_out.open((path+cscID+"_CFEB02_02.results").c_str());
          res_out << "Layer Strip C12|12 C12|3 C12|4 C12|5  C3|3  C3|4  C3|5  C4|4  C4|5  C5|5  C4|6  C5|6  C6|6  C5|7  C6|7  C7|7" << std::endl;
          for (int i=0; i<NLAYERS; i++) {
          for (int j=0; j<strips_per_layer; j++) {
          res_out << std::fixed << std::setprecision(2) << std::setw(5) << (i+1) << std::setw(6) << (j+1)
          << std::setw(7) << r08.content[i][j]  << std::setw(6) << r09.content[i][j]
          << std::setw(6) << r10.content[i][j]  << std::setw(6) << r11.content[i][j]
          << std::setw(6) << r12.content[i][j]  << std::setw(6) << r13.content[i][j]
          << std::setw(6) << r14.content[i][j]  << std::setw(6) << r15.content[i][j]
          << std::setw(6) << r16.content[i][j]  << std::setw(6) << r17.content[i][j]
          << std::setw(6) << r18.content[i][j]  << std::setw(6) << r19.content[i][j]
          << std::setw(6) << r20.content[i][j]  << std::setw(6) << r21.content[i][j]
          << std::setw(6) << r22.content[i][j]  << std::setw(6) << r23.content[i][j]
          << std::endl;
          }
          }
          res_out.close();
          */


          //      if (checkResults(cscID)) { // Check if 20% of channels pedestals and rms are bad

          // == Save results for database transfer Pedestals and RMS
          // i - layer, j - strip
          res_out.open((path+cscID+"_CFEB02_DB.dat").c_str());
	  std::vector<std::string> tests;
          tests.push_back("R01");
          tests.push_back("R02");

          for (int i=0; i<NLAYERS; i++)
            {
              for (int j=0; j<strips_per_layer; j++)
                {
                  res_out << std::fixed << std::setprecision(2) <<  (first_strip_index+i*strips_per_layer+j) << " "
                  << r01.content[i][j]  << " " << r02.content[i][j] 
		  << " " << (int)(mask.content[i][j])
                  << " " << (int)(checkChannel(cscdata, tests, i, j))
		  << std::endl;
                }
            }
          res_out.close();

          // == Save results for database transfer of Noise Matrix
          res_out.open((path+cscID+"_CFEB02_DB_NoiseMatrix.dat").c_str());
		
          tests.clear();
          tests.push_back("R12");
          tests.push_back("R13");
          tests.push_back("R14");
          tests.push_back("R15");
	  tests.push_back("R16");
          tests.push_back("R17");
          tests.push_back("R18");
          tests.push_back("R19");
	  tests.push_back("R20");
          tests.push_back("R21");
          tests.push_back("R22");
          tests.push_back("R23");

          for (int i=0; i<NLAYERS; i++)
            {
              for (int j=0; j<strips_per_layer; j++)
                {
                  res_out << std::fixed << std::setprecision(6) <<  (first_strip_index+i*strips_per_layer+j) << " "
                  << " " << r12.content[i][j]  << " " << r13.content[i][j]
                  << " " << r15.content[i][j]  << " " << r14.content[i][j]
                  << " " << r16.content[i][j]  << " " << r17.content[i][j]
                  << " " << r18.content[i][j]  << " " << r19.content[i][j]
                  << " " << r20.content[i][j]  << " " << r21.content[i][j]
                  << " " << r22.content[i][j]  << " " << r23.content[i][j]
		  << " " << (int)(mask.content[i][j])
		  << " " << (int)(checkChannel(cscdata, tests, i, j))
                  << std::endl;

                }
            }
          res_out.close();

          //      }

        }

    }
}

bool Test_CFEB02::checkResults(std::string cscID)
{
  bool isValid=true;
  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end())
    {
      TestData& cscdata= td_itr->second;
      TestData2D& r01 = cscdata["R01"];
      TestData2D& r02 = cscdata["R02"];

      int badChannels=0;
      // Check pedestals
      for (int i=0; i<r01.Nlayers; i++)
        {
          for (int j=0; j<r01.Nbins; j++)
            {
              if ((r01.content[i][j] > 1000) || (r01.content[i][j] < 300)) badChannels++;
            }
        }
      if (badChannels/(float(r01.Nlayers*r01.Nbins)) >=0.2)
        {
          isValid=false;
          LOG4CPLUS_WARN(logger, cscID << ": 20% of channels have bad Pedestals");
        }

      badChannels=0;
      // Check noise
      for (int i=0; i<r02.Nlayers; i++)
        {
          for (int j=0; j<r02.Nbins; j++)
            {
              if (r02.content[i][j] > 6.) badChannels++;
            }
        }
      if (badChannels/(float(r02.Nlayers*r01.Nbins)) >=0.2)
        {
          isValid=false;
          LOG4CPLUS_WARN(logger, cscID << ": 20% of channels have bad Noise");
        }
    }

  return isValid;
}

