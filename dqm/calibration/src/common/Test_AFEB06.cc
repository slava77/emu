#include "emu/dqm/calibration/Test_AFEB06.h"
#include <boost/algorithm/string.hpp>


using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;
using namespace boost::algorithm;


Test_AFEB06::Test_AFEB06(std::string dfile): Test_Generic(dfile)
{
  testID    = "AFEB06";
  nExpectedEvents   = 14000;
  dduID     = 0;
  //  binCheckMask=0x16CFF3F6;
  binCheckMask  = 0x1FEBF3F6;
  //  binCheckMask=0xF7CB3BF6;
  ltc_bug   = 2;
  logger = Logger::getInstance(testID);

  num_tpamps    = 2;
  num_thresh    = 70;
  first_thresh  = 1;
  thresh_step   = 1;
  tpamp_first   = 14;
  tpamp_step    = 18;
  ev_per_thresh = nExpectedEvents / (num_thresh * num_tpamps);// 100;

  pass    = 0;

  afeb_cal_params.clear();
}


void Test_AFEB06::initCSC(std::string cscID)
{
  //= Init per CSC events counters
  nCSCEvents[cscID]=0;

  ThresholdScanData thdata;
  thdata.Nbins = getNumWireGroups(cscID);
  thdata.Nlayers = 6;

  memset(thdata.content, 0, sizeof (thdata.content));

  tscan_data[cscID] = thdata;

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

  for (int i=0; i<TEST_DATA2D_NLAYERS; i++)
    for (int j=0; j<TEST_DATA2D_NBINS; j++) afebdata.content[i][j]=0.;


  cscdata["R01"]=afebdata;
  cscdata["R02"]=afebdata;
  cscdata["R04"]=afebdata;
  cscdata["R05"]=afebdata;
  cscdata["R06"]=afebdata;
  cscdata["R08"]=afebdata;
  cscdata["R09"]=afebdata;
  cscdata["R11"]=afebdata;
  cscdata["R12"]=afebdata;

  afebdata.Nbins = getNumAFEBs(cscID);
  afebdata.Nlayers = 1;
  cscdata["R03"]=afebdata;
  cscdata["R07"]=afebdata;
  cscdata["R10"]=afebdata;
  cscdata["R13"]=afebdata;

  tdata[cscID] = cscdata;;

  bookTestsForCSC(cscID);

  if (!loadAFEBCalibParams(cscID))
    {
      LOG4CPLUS_WARN(logger, cscID << ": Unable to load AFEB calibration constants.");
    }

}

void Test_AFEB06::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
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

  if (dduID != (bin_checker.dduSourceID()&0xFF))
    {

      LOG4CPLUS_DEBUG(logger, "DDUEvt#" << std::dec << nTotalEvents << ": DDU#" << (bin_checker.dduSourceID()&0xFF) << " First event");
      dduID = bin_checker.dduSourceID()&0xFF;
      dduL1A[dduID]=0;
      DDUstats[dduID].evt_cntr=0;
      DDUstats[dduID].first_l1a=-1;
    }

  dduID = bin_checker.dduSourceID()&0xFF;
  DDUstats[dduID].evt_cntr++;

  if (bin_checker.errors() != 0)
    {
      // std::cout << "Evt#" << std::dec << nTotalEvents << ": Nonzero Binary Errors Status is observed: 0x"<< std::hex << bin_checker.errors()
      //  << " mask:0x" << std::hex << binCheckMask << std::dec << std::endl;
      doBinCheck();
      //    return;
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
  CSCDDUHeader dduHeader  = dduData.header();


  // std::cout << nTotalEvents << " " << chamberDatas.size() << std::endl;

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
          // ltc_bug=1;
        }
      else
        {
          LOG4CPLUS_WARN(logger, "Found LTC/TTC double L1A bug in data");
        }
    }

  // int threshSwitch = ev_per_thresh*ltc_bug; /// # of events before AFEB threshold value switch
  int threshSwitch = nExpectedEvents / (num_thresh * num_tpamps);

  int passSwitch = threshSwitch * num_thresh; /// # of events before test pulse pass switch

  if (currL1A % passSwitch == 1)
    {
      DDUstats[dduID].pass=currL1A / passSwitch;
      DDUstats[dduID].empty_evt_cntr=0;

      // std::cout << dduID << "Scan pass switch " << (DDUstats[dduID].pass+1) << std::endl;

      std::map<std::string, test_step> & cscs = htree[dduID];
      for (std::map<std::string, test_step>::iterator itr = cscs.begin(); itr != cscs.end(); ++itr)
        {
          itr->second.evt_cnt = 0;
        }
    }


  if (currL1A % threshSwitch == 1)
    {
      DDUstats[dduID].thresh= first_thresh + ((currL1A - DDUstats[dduID].pass*passSwitch)/ threshSwitch)*thresh_step;
      DDUstats[dduID].empty_evt_cntr=0;

      // std::cout << "Threshold switch to " << DDUstats[dduID].thresh << std::endl;

      std::map<std::string, test_step> & cscs = htree[dduID];
      for (std::map<std::string, test_step>::iterator itr = cscs.begin(); itr != cscs.end(); ++itr)
        {
          itr->second.evt_cnt = 0;
        }
    }

  int nCSCs = chamberDatas.size();

  //= Loop over found chambers and fill data structures
  for (int i=0; i < nCSCs; i++)
    {
      analyzeCSC(chamberDatas[i]);
    }

  DDUstats[dduID].last_empty=chamberDatas.size();

}


void Test_AFEB06::analyzeCSC(const CSCEventData& data)
{


  const CSCDMBHeader* dmbHeader = data.dmbHeader();
  const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();
  if (!dmbHeader && !dmbTrailer)
    {
      return;
    }


  int csctype=0, cscposition=0;
  std::string cscID = getCSCFromMap(dmbHeader->crateID(), dmbHeader->dmbID(), csctype, cscposition);
  // std::string cscID(Form("CSC_%03d_%02d", data.dmbHeader().crateID(), data.dmbHeader().dmbID()));
  // == Do not process unmapped CSCs and ME1/1 chambers
  if (cscID == "") return;
  // if ((cscID.find("ME+1.1") == 0) || (cscID.find("ME-1.1") ==0) ) return;


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

  MonHistos& cschistos = mhistos[cscID];

  test_step& tstep = htree[dduID][cscID];// test_steps[cscID];

  ThresholdScanData& thdata = tscan_data[cscID];

  int curr_thresh =  DDUstats[dduID].thresh;
  int curr_pass = DDUstats[dduID].pass;

  tstep.evt_cnt++;

  TH2F* v01 = reinterpret_cast<TH2F*>(cschistos["V01"]);
  TH2F* v02 = reinterpret_cast<TH2F*>(cschistos["V02"]);
  if (data.nalct())
    {
      const CSCAnodeData* alctData = data.alctData();
      if (alctData)
        {
          for (int nLayer=1; nLayer<=6; nLayer++)
            {
              vector<CSCWireDigi> wireDigis = alctData->wireDigis(nLayer);
              for (vector<CSCWireDigi>:: iterator wireDigisItr = wireDigis.begin();
                   wireDigisItr != wireDigis.end(); ++wireDigisItr)
                {
                  int wg = wireDigisItr->getWireGroup();
                  v01->Fill(wg-1, nLayer-1);
                  vector<int> tbins = wireDigisItr->getTimeBinsOn();
                  for (uint32_t n=0; n < tbins.size(); n++)
                    {
                      v02->Fill(wg-1, (nLayer-1)*32+tbins[n]);

                    }

                  thdata.content[curr_pass][nLayer-1][wg-1][curr_thresh-1]++;

                }
            }
        }
    }
}


void Test_AFEB06::finishCSC(std::string cscID)
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

      TestData& cscdata= td_itr->second;

      // TestData2D& mask = cscdata["_MASK"];

      TestData2D& r01 = cscdata["R01"];
      TestData2D& r02 = cscdata["R02"];
      TestData2D& r04 = cscdata["R04"];

      TestData2D& r05 = cscdata["R05"];
      TestData2D& r06 = cscdata["R06"];
      TestData2D& r08 = cscdata["R08"];

      TestData2D& r09 = cscdata["R09"];
      TestData2D& r11 = cscdata["R11"];
      TestData2D& r12 = cscdata["R12"];

      TestData2D& r03 = cscdata["R03"];
      TestData2D& r07 = cscdata["R07"];
      TestData2D& r10 = cscdata["R10"];
      TestData2D& r13 = cscdata["R13"];

      ThresholdScanData& thdata = tscan_data[cscID];
      AFEBCalibParams& cal_params = afeb_cal_params[cscID];

      int nAFEBs = emu::dqm::utils::getNumAFEBs(cscID);

      CSCtoHWmap::iterator itr = cscmap.find(cscID);

      if (itr != cscmap.end())
        {

          // map->cratedmb(itr->second.first,itr->second.second,&mapitem);
          int dmbID = itr->second.second;
          if (dmbID >= 6) --dmbID;
          int id = 10*itr->second.first+dmbID;

          CSCMapItem::MapItem mapitem = cratemap->item(id);


          for (int pass=0; pass<2; pass++)
            {

              for (int i=0; i<NLAYERS; i++)
                {
                  for (int j=0; j<getNumWireGroups(cscID); j++)
                    {

                      float  chisq, mean, par[3], rms;

                      /*
                                  // == debug output
                                  if ((i==2) && (j==2) )
                                  {
                                      std::cout << cscID << " l"<< (i+1) << ":wg" << (j+1) << ": ";
                                      for (int k=0; k<num_thresh; k++)
                                      {
                                          int thresh = first_thresh + k*thresh_step;
                                          std::cout << " " << thdata.content[pass][i][j][thresh-1];
                                      }
                                      std::cout << std::endl;

                                  }
                      */

                      calc_thresh(MAX_CALIB_POINTS, thdata.content[pass][i][j] , par, &chisq);


                      if (chisq >= 0.)
                        {
                          mean = par[1]+first_thresh;
                          rms = par[2];
                        }
                      else
                        {
                          mean = -1;
                          rms = -1;
                          LOG4CPLUS_WARN(logger, cscID <<
                                         Form(": Layer %d wiregroup %2d has threshold = %6.2f and noise = %6.2f "
                                              "\tpar0 %f, mean %f, rms %f, chisq %f",
                                              i + 1, j + 1, mean, rms,
                                              par[0], par[1]+first_thresh, par[2], chisq) );
                          /*
                                      std::cout << cscID << " l"<< (i+1) << ":wg" << (j+1) << ": ";
                                      for (int k=0; k<num_thresh; k++)
                                      {
                                          int thresh = first_thresh + k*thresh_step;
                                          std::cout << " " << thdata.content[pass][i][j][thresh-1];
                                      }
                                      std::cout << std::endl;
                          */
                        }

                      if (pass==0)   ///* Thresholds and Noise for 1st Pulse DAC setting
                        {
                          r02.content[i][j] = mean;
                          r01.content[i][j] = rms;
                        }
                      else if (pass == 1) ///* Thresholds and Noise for 2nd Pulse DAC setting
                        {
                          r06.content[i][j] = mean;
                          r05.content[i][j] = rms;
                        }
                    }
                }

            }


          ///* Calculate averaged per AFEB board Threshold for 1st Pulse DAC
          for (int iafeb = 0; iafeb < nAFEBs; iafeb++)
            {
              int ilayer = 2 * (iafeb%3);
              int icolumn = iafeb/3;

              for (int ichan = 0; ichan<8; ichan++)
                {
                  int iwire  = 8 * icolumn + ichan;
                  r03.content[0][iafeb] += r02.content[ilayer][iwire];
                  r03.content[0][iafeb] += r02.content[ilayer+1][iwire];
                }
              r03.content[0][iafeb] /= 16.;
              r03.content[0][iafeb] += 0.5;
            }


          ///* Calculate AFEB Threshold Offsets for 1st Pulse DAC
          for (int iwire = 0; iwire < getNumWireGroups(cscID); iwire++)
            {
              for (int ilayer = 0; ilayer < NLAYERS; ilayer++)
                {
                  r04.content[ilayer][iwire] =
                    r02.content[ilayer][iwire] - r03.content[0][getAFEBPos(ilayer,iwire)];
                }

            }


          ///* Calculate averaged per AFEB board Threshold for 2nd Pulse DAC
          for (int iafeb = 0; iafeb < nAFEBs; iafeb++)
            {
              int ilayer = 2 * (iafeb%3);
              int icolumn = iafeb/3;

              for (int ichan = 0; ichan<8; ichan++)
                {
                  int iwire  = 8 * icolumn + ichan;
                  r07.content[0][iafeb] += r06.content[ilayer][iwire];
                  r07.content[0][iafeb] += r06.content[ilayer+1][iwire];
                }
              r07.content[0][iafeb] /= 16.;
              r07.content[0][iafeb] += 0.5;
            }


          ///* Calculate AFEB Threshold Offsets for 2nd Pulse DAC
          for (int iwire = 0; iwire < getNumWireGroups(cscID); iwire++)
            {
              for (int ilayer = 0; ilayer < NLAYERS; ilayer++)
                {
                  r08.content[ilayer][iwire] =
                    r06.content[ilayer][iwire] - r07.content[0][getAFEBPos(ilayer,iwire)];
                }

            }


          ///* Calculate Threshold slop (in DACthr/fC) vs AFEB
          // float delta_pulse = (calibration_pulse[1] - calibration_pulse[0])*CALIBRATION_PULSE_A1;  tpamp_step = 18 for P5

          float delta_pulse = tpamp_step*CALIBRATION_PULSE_A1;
          for (int iafeb = 0; iafeb < nAFEBs; iafeb++)
            {
              r10.content[0][iafeb] = (r07.content[0][iafeb] - r03.content[0][iafeb]) / delta_pulse / cal_params.capacitances[iafeb];
            }


          for (int i=0; i<NLAYERS; i++)
            {
              for (int j=0; j<getNumWireGroups(cscID); j++)
                {
                  ///* Calculate AFEB Threshold Slopes tpamp_step = 18 for P5
                  r09.content[i][j] = (r06.content[i][j]-r02.content[i][j]) /
                                      ( tpamp_step* /* (calibration_pulse[1]-calibration_pulse[0]) */
                                        cal_params.capacitances[getAFEBPos(i,j)]*
                                        CALIBRATION_PULSE_A1 );

                  ///* Calculate AFEB Threshold for 20 fC
                  /*
                        float q1 = (CALIBRATION_PULSE_A0+CALIBRATION_PULSE_A1 * 14)
                                   * cal_params.capacitances[getAFEBPos(i,j)] ;
                        float q2 = (CALIBRATION_PULSE_A0+CALIBRATION_PULSE_A1 * 32)
                                   * cal_params.capacitances[getAFEBPos(i,j)] ;
                  */

                  float q1 = (CALIBRATION_PULSE_A0+CALIBRATION_PULSE_A1 * tpamp_first)
                             * cal_params.capacitances[getAFEBPos(i,j)] ;
                  float q2 = (CALIBRATION_PULSE_A0+CALIBRATION_PULSE_A1 * (tpamp_first + tpamp_step) )
                             * cal_params.capacitances[getAFEBPos(i,j)] ;

                  if(fabs(q2-q1) > 1.)
                    r11.content[i][j] =
                      ((q2-20.)*r02.content[i][j]+ (20.-q1)*r06.content[i][j])/(q2-q1);
                  else
                    r11.content[i][j] = -99.; //not defined

                }

            }


          ///* Calculate averaged per AFEB board Threshold for 20 fC
          for (int iafeb = 0; iafeb < nAFEBs; iafeb++)
            {
              int ilayer = 2 * (iafeb%3);
              int icolumn = iafeb/3;

              for (int ichan = 0; ichan<8; ichan++)
                {
                  int iwire  = 8 * icolumn + ichan;
                  r13.content[0][iafeb] += r11.content[ilayer][iwire];
                  r13.content[0][iafeb] += r11.content[ilayer+1][iwire];
                }
              r13.content[0][iafeb] /= 16.;
              r13.content[0][iafeb] += 0.5;
            }

          ///* Calculate AFEB Threshold Offsets for 20 fC
          for (int iwire = 0; iwire < getNumWireGroups(cscID); iwire++)
            {
              for (int ilayer = 0; ilayer < NLAYERS; ilayer++)
                {
                  r12.content[ilayer][iwire] =
                    r11.content[ilayer][iwire] - r13.content[0][getAFEBPos(ilayer,iwire)];
                }

            }



          // == Save results to text files
          std::string rpath = "Test_"+testID+"/"+outDir;
          std::string path = rpath+"/"+cscID+"/";


          std::ofstream res_out((path+cscID+"_"+testID+"_R13.results").c_str());

          res_out << "#----------------------------------------------------------------------------\n"
                  << "#Results of AFEB Threshold Test for " << cscID
                  << "\n#----------------------------------------------------------------------------\n\n"

                  << "#Calculated threshold for 20 fC, in DACthr\n"
                  << "#AFEB #   DAC thr for 20 fC\n"
                  << "#----------------------------------------------------------------------------" << std::endl;

          // if chamber type 1/3, 3/1 or 4/1 - add 6 dummy AFEBs in front of all the rest
          // in file test_13_13.result, because ALCT slow control library
          // requires the exact number of AFEBs in the threshold config file
          // (Madorsky 12/07/03)

          if ((cscID.find("ME+4.1") == 0) || (cscID.find("ME-4.1") == 0) ||
              (cscID.find("ME+3.1") == 0) || (cscID.find("ME-3.1") == 0) ||
              (cscID.find("ME+1.3") == 0) || (cscID.find("ME-1.3") == 0) )
            {
              for (int iafeb = 0; iafeb < 6; iafeb++)
                {
                  res_out << std::setw(5) <<  (iafeb+1) << "\t" << std::setw(4) << (int)r13.content[0][iafeb] << std::endl;
                }

              for (int iafeb = 0; iafeb < nAFEBs; iafeb++)
                {
                  res_out << std::setw(5)<< (iafeb+7) << "\t" << std::setw(4) << (int)r13.content[0][iafeb] << std::endl;
                }
            }
          else
            {

              for (int iafeb = 0; iafeb < nAFEBs; iafeb++)
                {
                  res_out << std::setw(5)<< (iafeb+1) << "\t" << std::setw(4) << (int)r13.content[0][iafeb] << std::endl;
                }
            }

          res_out << "\n# " << now() << std::endl;
          res_out.close();



          //      }

        }

    }
}

bool Test_AFEB06::checkResults(std::string cscID)
{
  bool isValid=true;
  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end())
    {
      // TestData& cscdata= td_itr->second;
      // TestData2D& r01 = cscdata["R01"];

      // int badChannels=0;
    }

  return isValid;
}


/*
  AUTHOR: Alexei A Drozdetski, drozdetski@phys.ufl.edu, CMS EMU Group
  Creation date: 05/01/02
  Last change: 05/20/02

SUBROUTINE OUTPUT:
OK:  return value = 0;  chisq = 0.0
WARNINGS: return value = 1; chisq = 1.0;
        //warning: there is more than zero hits in the last bin OR:
        //all bins - with max value of hits...

ERRORS: return value = -1;
        chisq = -1.0; //error: no events with number of hits more than 0
                      //in any bin
        chisq = -3.0; //error: all_diff_events_sum<=0 here
                      //(first iteration)... it's impossible!:-)
                      //show me corresponding histogram, please.
        chisq = -4.0; //error: sigma<0 here (first iteration)...
                      //it's impossible! :-)
                      //show me corresponding histogram, please.
        chisq = -5.0; //error: all_diff_events_sum < 0 here...
                      // (second iteration), very strange
        chisq = -6.0; //error: sigma<0 here... (second iteration), very strange
*/

int Test_AFEB06::calc_thresh(int npoints, int* content, float* par, float* chisq)
{
  // Local variables initialization...
  int i,first_index;
  float diff_content[MAX_CALIB_POINTS], all_diff_events_sum;
  float mean, sigma;
  double left_bound, right_bound;
  int array_not_empty,array_not_full;
  float array_max_value;
  int warning_flag=0;

  // Initialization of the threshold parameter array to zero values.
  par[0] = 0.;
  par[1] = 0.;
  par[2] = 0.;

  if (content[npoints-1] != 0)
    {
      content[npoints-1] = 0;
      *chisq = 1.0; //warning: there is more than zero hits in the last bin
      warning_flag=1;
    }

  array_not_empty = 0;
  array_not_full = 0;
  array_max_value = 0.0;
  for (i=0; i<npoints; i++)
    {
      if (content[i] != 0)
        {
          array_not_empty = 1;
        }
      if(content[i] > array_max_value)
        {
          array_max_value = content[i];
        }
    }
  for (i=0; i<npoints; i++)
    {
      if (content[i] != array_max_value)
        {
          array_not_full = 1;
        }
    }
  if (array_not_empty == 0)
    {
      *chisq = -1.0;//error: no events with number of hits more than 0 in any bin
      return -1;
    }

  first_index = npoints-1;

  //looking for the first (left) "good" index of the data array
  for (i=(npoints-1); i>-0.1; i--)
    {
      if(content[first_index] <= content[i])
        {
          first_index = i;
        }
    }

  // Here we starting to calculate MEAN and SIGMA for CONTENT array using all data,
  // all NPOINTS bins. It is th first itteration...

  // initialization of the local variables
  mean = 0.;
  sigma = 0.;
  all_diff_events_sum = 0.;

  // beginning of the MEAN loop calculation...
  for (i=first_index; i<(npoints-1); i++)
    {
      diff_content[i] = -(content[i+1]-content[i]);
      mean = mean + i*diff_content[i];
      all_diff_events_sum = all_diff_events_sum + diff_content[i];
    }
  // checking the all_diff_events_sum value. For all 'good' CONTENT arrays
  // its value should be equal to max_events_per_bin_of_the_CONTENT_array
  // for the first itteration and positive value <= max_events_per_...
  // for the second itteration...
  if (all_diff_events_sum <= 0)
    {
      *chisq = -3.; //error: all_diff_events_sum<=0 here... it's impossible! :-)
      //show me corresponding histogram, please.
      return -1;
    }
  // ... '+ 0.5' bins shift is neccessary just by obvious features of our algorithm
  mean = mean/all_diff_events_sum + 0.5;

  // beginning of the SIGMA loop calculation...
  for (i=first_index; i<(npoints-1); i++)
    {
      //   printf("diff content   i= %d  data= %f \n",i,diff_content[i]);
      sigma = sigma + (i-mean)*(i-mean)*diff_content[i];
    }
  if (sigma < 0.0)
    {
      *chisq = -4.; //error: sigma<0 here... it's impossible! :-)
      //show me corresponding histogram, please.
      return -1;
    }
  sigma = sqrt(sigma/all_diff_events_sum);
  // printf("all_diff_events_sum = %f sigma= %f\n",all_diff_events_sum, sigma);

  // Here we starting to calculate MEAN and SIGMA for CONTENT array using all data,
  // all NPOINTS bins. It is the second itteration...

  // calculation of the bounds for MEAN and SIGMA calculation
  // in the second itteration. We do it in order to recalculate MEAN and SIGMA
  // without taking into account far away lieing noise peaks...
  if (sigma < 2 ) sigma = 2.;
  if (mean - 3*sigma < 0)
    {
      left_bound = 0;
    }
  else
    {
      left_bound = floor(mean - 3*sigma);
    }
  if (mean + 3*sigma > npoints)
    {
      right_bound = npoints;
    }
  else
    {
      right_bound = ceil(mean + 3*sigma);
    }
  // printf("bounds: %f %f \n",(mean - 3*sigma),(mean + 3*sigma));
  // printf("bounds: %f %f \n",left_bound,right_bound);

  // initialization of the local variables...
  all_diff_events_sum = 0.;
  mean = 0.;
  sigma = 0.;

  // FOLLOWING STRUCTURE OF THE mean AND sigma CALCULATION
  // IS THE SAME AS IN THE PREVIOUS HALF OF THE SUBROUTINE.
  // SO, WE WILL NOT REPEAT SOME NOTES HERE AGAIN.

  //AGAIN: looking for the first (left) "good" index of the data array
  for (i=(int)(right_bound-1); i>(int)(left_bound-0.1); i--)
    {
      if(content[first_index] <= content[i])
        {
          first_index = i;
        }
    }


  // beginning of the MEAN loop calculation...
  for (i=first_index; i<(right_bound-1); i++)
    {
      diff_content[i] = -(content[i+1]-content[i]);
      mean = mean + i*diff_content[i];
      all_diff_events_sum = all_diff_events_sum + diff_content[i];
    }
  mean = mean/all_diff_events_sum + 0.5;
  par[0] = all_diff_events_sum;
  par[1] = mean;

  if (all_diff_events_sum <= 0)
    {
      *chisq = -5.;//error: all_diff_events_sum < 0 here...
      return -1;
    }
  // beginning of the SIGMA loop calculation...
  for (i=first_index; i<(right_bound-1); i++)
    {
      sigma = sigma + (i-mean)*(i-mean)*diff_content[i];
    }
  if (sigma < 0.0)
    {
      *chisq = -6.; //error: sigma<0 here...
      return -1;
    }
  sigma = sqrt(sigma/all_diff_events_sum);
  par[2] = sigma;

  // calculation process finished...
  if (warning_flag==1)
    {
      return 1;
    }
  else if (warning_flag==0)
    {
      *chisq = 0.0;
      return 0;
    }
  return 0;

}


bool Test_AFEB06::loadAFEBCalibParams(std::string cscID)
{

  std::string line;
  std::stringstream st;

  // == Expected Number of AFEBs for this chamber
  int nAFEBs = emu::dqm::utils::getNumAFEBs(cscID);

  AFEBCalibParams& cal_params = afeb_cal_params[cscID];
  memset(&cal_params, 0, sizeof(cal_params));
  cal_params.Nlayers = 6;
  cal_params.Nbins = nAFEBs;
  for (int i=0; i<nAFEBs; i++)
    {
      cal_params.capacitances[i] = 0.25;
      cal_params.gains[i] = 1.0;
    }

  int cnt = 0; ///* Counter of AFEBs read from files

  int N, pos, num, Umv;
  float cap, gain, offset; // Capacitance, Gain, Offset


  st << cscAFEBCalibFolder << "AFEB_" << cscID << ".txt";

  ifstream afeb_file(st.str().c_str());
  if (afeb_file)
    {
      LOG4CPLUS_DEBUG(logger, cscID << ": Opened AFEB calibration parameters file " << st.str());
      // std::cout << "Expected AFEBs: " << nAFEBs <<std::endl;
      // std::cout << "The calibration coefficients are:" << std::endl; ;
      while (!afeb_file.eof())
        {
          getline(afeb_file, line);
          trim(line);
          if ((line.length() == 0) || (line.find("#") != string::npos)) continue;
          // std::cout << line << std::endl;

          int iparse=sscanf(line.c_str(),"%d  %d  %d  %f %d %f %f",
                            &N,&pos,&num,&cap,&Umv,&gain,&offset);
          if ((iparse == 7) && (pos<=nAFEBs) && (pos>0) )
            {
              cal_params.capacitances[pos-1] = cap;
              cal_params.gains[pos-1] = gain;
              cnt++;
              std::cout << "AFEB: " << pos << "  Cin: " << cal_params.capacitances[pos-1] << "\tGain: " << cal_params.gains[pos-1] << std::endl;
            }
        }
      afeb_file.close();

      if (cnt != nAFEBs )
        {
          LOG4CPLUS_WARN(logger, cscID << ": Number of read from file AFEBs " << cnt << " not equal to number of expected AFEBs " << nAFEBs);
        }

    }
  else
    {
      LOG4CPLUS_ERROR(logger, cscID << ": Unable to open AFEB calibration parameters file " << st.str());
      return false;
    }
  return true;
}

void Test_AFEB06::setTestParams()
{

  LOG4CPLUS_INFO (logger, "Setting additional test parameters.");
  std::map<std::string, std::string>::iterator itr;
 
  itr = test_params.find("tpamp_first");
  if (itr != test_params.end() )
    {
      tpamp_first = atoi((itr->second).c_str());
      LOG4CPLUS_INFO (logger, "parameter: tpamp_first: " << tpamp_first);
    }

  itr = test_params.find("tpamp_step");
  if (itr != test_params.end() )
    {
      tpamp_step = atoi((itr->second).c_str());
      LOG4CPLUS_INFO (logger, "parameter: tpamp_step: " << tpamp_step);
    }
  itr = test_params.find("num_tpamps");
  if (itr != test_params.end() )
    {
      num_tpamps = atoi((itr->second).c_str());
      LOG4CPLUS_INFO (logger, "parameter: num_tpamps: " << num_tpamps);
    }
  itr = test_params.find("num_thresh");
  if (itr != test_params.end() )
    {
      num_thresh = atoi((itr->second).c_str());
      LOG4CPLUS_INFO (logger, "parameter: num_thresh: " << num_thresh);
    }

}
