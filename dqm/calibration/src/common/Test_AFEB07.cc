#include "emu/dqm/calibration/Test_AFEB07.h"

using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;

Test_AFEB07::Test_AFEB07(std::string dfile): Test_Generic(dfile)
{
  testID = "AFEB07";
  nExpectedEvents = 12800;
  dduID=0;
  //  binCheckMask=0x16CFF3F6;
  binCheckMask=0x1FEBF3F6;
  //  binCheckMask=0xF7CB3BF6;
  ltc_bug=2;
  fSTEPsetup=0;
  logger = Logger::getInstance(testID);
}


void Test_AFEB07::initCSC(std::string cscID)
{


  //= Init per CSC events counters
  nCSCEvents[cscID]=0;

  DelayScanData dsdata;
  dsdata.Nbins = getNumWireGroups(cscID);
  dsdata.Nlayers = 6;

  memset(dsdata.content, 0, sizeof (dsdata.content));

  dscan_data[cscID] = dsdata;

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
  cscdata["R03"]=afebdata;

  tdata[cscID] = cscdata;;

  bookTestsForCSC(cscID);
}

void Test_AFEB07::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
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
      DDUstats[dduID].dac=0;
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
          ///* =VB= Really BAD! Need to modify ltc_bug detection for AFEB tests
          if (nExpectedEvents < 12800 ) ltc_bug=1;
        }
      else
        {
          LOG4CPLUS_WARN(logger, "Found LTC/TTC double L1A bug in data");
        }
    }


  int delaySwitch=400*ltc_bug;

  if ((unsigned int)currL1A > nExpectedEvents) return;
  if (currL1A% delaySwitch ==1)
    {
      DDUstats[dduID].dac=currL1A / delaySwitch ;
      DDUstats[dduID].empty_evt_cntr=0;

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


void Test_AFEB07::analyzeCSC(const CSCEventData& data)
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
  if ((cscID == "") /* || (cscID.find("ME+1.1") == 0) || (cscID.find("ME-1.1") ==0) */) return;


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
  //unused TestData& cscdata = tdata[cscID];

  MonHistos& cschistos = mhistos[cscID];

  test_step& tstep = htree[dduID][cscID];// test_steps[cscID];

  DelayScanData& dsdata = dscan_data[cscID];

  int curr_delay = DDUstats[dduID].dac;
  if (curr_delay >= DELAY_SCAN_STEPS)
    {
      LOG4CPLUS_ERROR(logger, cscID << ": Delay Value is out of range " << curr_delay);
      return;
    }


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
                  int disc_on = 0;
                  int prev_tbin = -1;
                  for (uint32_t n=0; n < tbins.size(); n++)
                    {
                      if ((disc_on==0) && (n < ACTIVETBINS_LIMIT))
                        {
                          v02->Fill(wg-1, (nLayer-1)*32+tbins[n]);
                          dsdata.content[curr_delay][nLayer-1][wg-1].tbins[tbins[n]]++;
                          // printf("activebins %d, ALCT[%d,][%d,] tbin %d, event %d \n",n+1,nLayer,wg,tbins[n],nCSCEvents[cscID]);
                          disc_on = 1;
                        }
                      if ((prev_tbin >=0) && (tbins[n] != (prev_tbin+1)) ) disc_on = 0;
                      prev_tbin = tbins[n];

                    }
                  dsdata.content[curr_delay][nLayer-1][wg-1].cnt++;

                }
            }
        }
    }
}


void Test_AFEB07::finishCSC(std::string cscID)
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

      //unused TestData2D& mask = cscdata["_MASK"];

      TestData2D& r01 = cscdata["R01"];
      TestData2D& r02 = cscdata["R02"];
      TestData2D& r03 = cscdata["R03"];


      MonHistos& cschistos = mhistos[cscID];

      TH2F* v03 = reinterpret_cast<TH2F*>(cschistos["V03"]);
      TH2F* v04 = reinterpret_cast<TH2F*>(cschistos["V04"]);

      CSCtoHWmap::iterator itr = cscmap.find(cscID);

      if (itr != cscmap.end())
        {

          int nAFEBs = emu::dqm::utils::getNumAFEBs(cscID);
          int nwires = getNumWireGroups(cscID);
          double cable_delay[nwires];
          int csc_type = getChamberCableIndex(cscID);
          double delay_offset = getChamberDelayOffset(cscID);
          double bmax, btemp, mtemp, chisq;


          for (int iwg = 0; iwg < nwires; iwg++)
            {
              cable_delay[iwg] = cable_del[csc_type][iwg/8] + delay_offset;
            }


          DelayScanData& dsdata = dscan_data[cscID];

          int dmbID = itr->second.second;
          if (dmbID >= 6) --dmbID;
          int id = 10*itr->second.first+dmbID;

          CSCMapItem::MapItem mapitem = cratemap->item(id);



          for (int i=0; i<NLAYERS; i++)
            {
              // std::cout << cscID << Form(": ly%d ", i+1);
              for (int j=0; j<getNumWireGroups(cscID); j++)
                {
                  int ndelays = 0;
                  // std::cout << Form("ly%d wg%d\n", i+1, j+1);
                  int nbad = 0;
                  for (int k=0; k<DELAY_SCAN_STEPS; k++)
                    {
                      // std::cout << Form("dly%2d", k+1);
                      double sum = 0.;
                      double w_sum = 0.;
                      double e_sum = 0.;
                      double tavg  = -1;
                      double terr  = 1;
                      int active_tbins = 0;

                      int n_max=0;
                      int t_max=0;
                      for (int t = 0; t < MAX_ALCT_TIMEBINS; t++)
                        {
                          // std::cout << Form(" %3d", dsdata.content[k][i][j].tbins[t]);
                          if (dsdata.content[k][i][j].tbins[t]) active_tbins++;
                          if (dsdata.content[k][i][j].tbins[t] >= n_max)
                            {
                              n_max = dsdata.content[k][i][j].tbins[t];
                              t_max = t;
                            }
                        }
                      // std::cout << std::endl;

                      ///* Find start and end timebins with data
                      int t_start = -1;
                      int t_end = -1;
                      for (int t = ((t_max>=3)?(t_max-3):0); t < ((t_max<(MAX_ALCT_TIMEBINS-3))?(t_max+3):MAX_ALCT_TIMEBINS); t++)
                        {
                          if  (dsdata.content[k][i][j].tbins[t] > 0)
                            {
                              t_end = t;
                              if (t_start <0) t_start = t;
                            }

                        }

                      ///* Warn and skip empty
                      if (t_start < 0 || t_end < 0)
                        {
                          LOG4CPLUS_WARN(logger, Form("%s: Delay setting %d -  No Entries in Layer %d, Wiregroup %d", cscID.c_str(), k, i+1, j+1 ));
                          nbad++;
                          continue;
                        }

                      /*
                            std::cout << Form("%s: delay %2d layer %d, wg %3d - t_max %2d, t_start %2d, t_end %2d, width %2d",
                                              cscID.c_str(), k, i+1, j+1, t_max, t_start, t_end, (t_end - t_start) ) << std::endl;
                      */

                      if (t_end - t_start > 4)
                        {
                          LOG4CPLUS_WARN(logger, Form("%s: Delay setting %d - Too many active timebins (%d) Layer %d, Wiregroup %d",
                                                      cscID.c_str(), k, t_end -t_start, i+1, j+1 ));

                        }

                      v04->Fill(k, (t_end - t_start) );

                      if (fSTEPsetup>0) // STEP setup pulsing ( Analysis code is copied from lisas_test_manager test_14.c )
                        {
                          int nmax_t = 0;
                          if (t_max == 0) nmax_t = 1;
                          else if (t_max == (MAX_ALCT_TIMEBINS-1)) nmax_t = MAX_ALCT_TIMEBINS-2;
                          else if (dsdata.content[k][i][j].tbins[t_max-1] > dsdata.content[k][i][j].tbins[t_max+1]) nmax_t = t_max - 1;
                          else nmax_t = t_max + 1;

                          sum = dsdata.content[k][i][j].tbins[t_max] + dsdata.content[k][i][j].tbins[nmax_t];
                          if (sum > 0)
                            {
                              ndelays = k+1;
                              tavg = (t_max * dsdata.content[k][i][j].tbins[t_max] + nmax_t * dsdata.content[k][i][j].tbins[nmax_t]) / sum;
                              double total = 0;
                              double x = 0;
                              for (int t=0; t < MAX_ALCT_TIMEBINS; t++) total += dsdata.content[k][i][j].tbins[t];
                              if ((x = sum / total) < 0.95)
                                printf("Too wide timebin histogram for Layer %d, Wiregroup %d: "
                                       "%f = %f / %f\n", i+1, j+1, x, sum, total);
                              if ((x = sum / total) > 1.00)
                                printf("??? Wrong ratio for Layer %d, Wiregroup %d: "
                                       "%f = %f / %f\n", i+1, j+1, x, sum, total);
                              terr = sqrt(dsdata.content[k][i][j].tbins[t_max] * dsdata.content[k][i][j].tbins[nmax_t] / sum)
                                     / sum;
                              if (terr <= 0) terr = 0.001;

                            }
                          else
                            {
                              printf("No histogram entries for Layer %d, Wiregroup %d\n",
                                     i+1, j+1);
                              tavg = -1;
                              terr = 1;
                              nbad++;
                            }

                        }
                      else // P5 calibration pulsing
                        {


                          for (int t = t_start; t <= t_end; t++)
                            {
                              sum += dsdata.content[k][i][j].tbins[t];
                              w_sum += dsdata.content[k][i][j].tbins[t]*(t);
                              e_sum += dsdata.content[k][i][j].tbins[t]*(t)*(t);

                            }


                          if ( sum > 0 )
                            {

                              ndelays = k+1;
                              tavg = w_sum/sum;
                              terr = sqrt( (e_sum / sum) - tavg*tavg) / sqrt(sum);
                              if (terr <= 0) terr = 0.001;
                            }
                        }

                      dsdata.content[k][i][j].tavg = tavg;
                      dsdata.content[k][i][j].terr = terr;
                      v03->Fill(k, tavg);
                    }

                  if (nbad > 2)
                    {
                      LOG4CPLUS_WARN(logger, Form("%s: Too many empty entries for delay settings (%d times) in Layer %d, Wiregroup %d. Skipping.",
                                                  cscID.c_str(), nbad, i+1, j+1 ));
                      continue;
                    }
                  fitit( dsdata, i, j , ndelays, &mtemp, &btemp, &chisq);

                  /* Convert time measurements from timebins to ns (25 ns / timebin), and correct
                  * for the delay of the test pulse arrival at the AFEB.
                  */
                  r01.content[i][j] = (btemp * 25.) - cable_delay[j];
                  r02.content[i][j] = mtemp * 25.;
                  r03.content[i][j] = chisq;

                }
              // std::cout << std::endl;
            }

          /*
          // Debug dump
          for (int i=0; i<NLAYERS; i++)
                  {
                    for (int j=0; j<getNumWireGroups(cscID); j++)
                      {
           std::cout << "ly" << i+1 << ",wg" << j+1 << ": ";
           for (int k=0; k<DELAY_SCAN_STEPS; k++)
                          {

          	std::cout << dsdata.content[k][i][j].tavg << ", ";
              }
           std::cout << std::endl;
          }
                  }
          */

          int n=0, ngood[nAFEBs], idelay[nAFEBs];
          double avg=0, b_avg[nAFEBs], m_avg[nAFEBs], avg_slope=0, d[nAFEBs];
          double intercept_lower_limit, intercept_upper_limit;

          for (int i=0; i<NLAYERS; i++)
            {
              for (int j=0; j<getNumWireGroups(cscID); j++)
                {

                  /* Find the overall average intercept (to make the results independent of
                    * the exact timing of the test pulse).
                    */
                  if ( r02.content[i][j] >= SLOPE_LOWER_LIMIT &&
                       r02.content[i][j] <= SLOPE_UPPER_LIMIT )
                    {
                      avg +=  r01.content[i][j];
                      n++;
                    }
                }
            }

          if (n > 0) avg = avg / n;
          else avg = r01.content[0][0];

          intercept_lower_limit = avg - 15;
          intercept_upper_limit = avg + 15;

          ///* Adjusting intercept histogram limits
          TestCanvases& cnvs = tcnvs[cscID];
          TestCanvases::iterator c_itr = cnvs.find("R01");
          if (c_itr != cnvs.end() )
            {
              TestCanvas_6gr1h* cnv = dynamic_cast<TestCanvas_6gr1h*>(c_itr->second);
              if (cnv != NULL) cnv->SetLimits(intercept_lower_limit-5, intercept_lower_limit, intercept_upper_limit, intercept_upper_limit+5,0,0);
            }




          /* Average the slopes and intercepts (for good channels) over the 16 channels
          * of each chip. Each 16 channels are from 8 adjacent wire groups x 2 layers.
          */
          for (int chip = 0; chip < nAFEBs; chip++)
            {
              b_avg[chip] = 0;
              m_avg[chip] = 0;
              ngood[chip] = 0;
            }

          for (int i = 0; i < NLAYERS; i++)
            {
              for (int j = 0; j < getNumWireGroups(cscID); j++)
                {
                  int chip = 3 * (j / 8) + (i / 2);
                  if (r01.content[i][j] >= intercept_lower_limit &&
                      r01.content[i][j] <= intercept_upper_limit &&
                      r02.content[i][j] >= SLOPE_LOWER_LIMIT &&
                      r02.content[i][j] <= SLOPE_UPPER_LIMIT)
                    {
                      b_avg[chip] += r01.content[i][j];
                      m_avg[chip] += r02.content[i][j];
                      ngood[chip]++;
                    }
                  else
                    LOG4CPLUS_WARN(logger, Form("%s: BAD Layer %d wiregroup %d chip %d - intercept %.3f, slope %.3f", cscID.c_str(), i, j, chip, r01.content[i][j], r02.content[i][j]) );
                }
            }

          bmax = -1;
          for (int chip = 0; chip < nAFEBs; chip++)
            {
              if (ngood[chip] > 0)
                {
                  b_avg[chip] = b_avg[chip] / ngood[chip];
                  m_avg[chip] = m_avg[chip] / ngood[chip];
                  if (b_avg[chip] > bmax) bmax = b_avg[chip];
                }
            }

          /* Find the setting for each chip which gives this delay. */
          for (int chip = 0; chip < nAFEBs; chip++)
            {
              if (ngood[chip] > 0) d[chip] = (bmax - b_avg[chip]) / m_avg[chip];
              else d[chip] = -1;
            }

          /* Find the average slope and delay offset of all the chips */
          avg = 0;
          avg_slope = 0;
          n = 0;
          for (int chip = 0; chip < nAFEBs; chip++)
            {
              if (ngood[chip] >= 0)
                {
                  avg_slope += m_avg[chip];
                  avg += d[chip] - rint(d[chip]);
                  n++;
                }
            }

          if (n > 0)
            {
              avg_slope = avg_slope / n;
              avg = avg / n;
            }

          if (avg < 0) avg = avg + 1;
          avg = avg * avg_slope;

          /* Find the setting for each chip which gives the maximum delay plus the
           * average offset (this avoid the situation where half the chips have
           * delay 1.4 -> 1 and the other half have delay 1.6 -> 2).
           */
          for (int chip = 0; chip < nAFEBs; chip++)
            {
              if (ngood[chip] > 0) d[chip] = ((bmax + avg) - b_avg[chip])
                                               / m_avg[chip];
              else d[chip] = -1;
              std::cout << Form("chip%d bmax:%.f avg:%.f b_avg:%.f m_avg:%.f dchip:%.f\n", chip, bmax, avg, b_avg[chip], m_avg[chip], d[chip]);
              idelay[chip] = (int)rint(d[chip]);
            }



          ///* == Save results to text files
          std::string rpath = "Test_"+testID+"/"+outDir;
          std::string path = rpath+"/"+cscID+"/";


          std::ofstream res_out((path+cscID+"_"+testID+"_R04.results").c_str());

          res_out << "#----------------------------------------------------------------------------\n"
                  << "#Results of AFEB Delays Test for " << cscID
                  << "\n#----------------------------------------------------------------------------\n\n"

                  << " chip   ngood   slope   intercept   delay   equalized delay\n"
                  << " ----------------------------------------------------------" << std::endl;



          for (int iafeb = 0; iafeb < nAFEBs; iafeb++)
            {
              double y = m_avg[iafeb] * idelay[iafeb] + b_avg[iafeb];
              res_out << Form("  %2d     %2d     %5.2f    %6.2f      %2d         %6.2f",
                              iafeb+1, ngood[iafeb], m_avg[iafeb], b_avg[iafeb], idelay[iafeb], y) << std::endl;
            }

          res_out << "\n# " << now() << std::endl;
          res_out.close();


          //      }

        }

    }
}

bool Test_AFEB07::checkResults(std::string cscID)
{
  bool isValid=true;
  /*
  cscTestData::iterator td_itr =  tdata.find(cscID);
  if (td_itr != tdata.end())
  {
    TestData& cscdata= td_itr->second;
    TestData2D& r01 = cscdata["R01"];

    int badChannels=0;
  }
  */
  return isValid;
}

/* Linear fit */

void  Test_AFEB07::fitit(DelayScanData& dsdata, int layer, int wire , int npoints, double *emm, double* bee, double* chisq)
{

  int       j;
  double    denom, weight;
  double    sum, sumx, sumx2, sumy, sumxy;
  double    resid[DELAY_SCAN_STEPS], yfit[DELAY_SCAN_STEPS];

  sum = 0;
  sumx = 0;
  sumx2 = 0;
  sumy = 0;
  sumxy = 0;

  for (j = 0; j < npoints; j++)
    {
      double yerr = dsdata.content[j][layer][wire].terr;
      double y = dsdata.content[j][layer][wire].tavg;
      double x = j;
      weight = 1 / (yerr * yerr);
      sum = sum + 1. * weight;
      sumx = sumx + x * weight;
      sumx2 = sumx2 + x * x * weight;
      sumy = sumy + y * weight;
      sumxy = sumxy + x * y * weight;
    }

  denom = sumx2 * sum - sumx * sumx;
  if (denom == 0)
    {
      printf("### Denominator is zero in fit ###\n");
      *chisq = -999;
      return;
    }

  *emm = (sumxy * sum - sumx * sumy) / denom;
  *bee = (sumx2 * sumy - sumx * sumxy) / denom;

  *chisq = 0;
  for (j = 0; j < npoints; j++)
    {
      double yerr = dsdata.content[j][layer][wire].terr;
      double y = dsdata.content[j][layer][wire].tavg;
      double x = j;
      yfit[j] = *emm * x + *bee;
      resid[j] = y - yfit[j];
      *chisq += (resid[j] / yerr) * (resid[j] / yerr);
    }
}

///* Return index in array of chamber types cables
int Test_AFEB07::getChamberCableIndex(std::string cscID)
{
  if  ( (cscID.find("ME+4/1") == 0) || (cscID.find("ME-4/1") ==0)
        || (cscID.find("ME+4.1") == 0) || (cscID.find("ME-4.1") ==0) ) return 6;
  else if ( (cscID.find("ME+3.1") == 0) || (cscID.find("ME-3.1") == 0)
            || (cscID.find("ME+3/1") == 0) || (cscID.find("ME-3/1") == 0)) return 4;
  else if ( (cscID.find("ME+2/1") == 0) || (cscID.find("ME-2/1") ==0)
            || (cscID.find("ME+2.1") == 0) || (cscID.find("ME-2.1") ==0)) return 3;
  else if ( (cscID.find("ME+1/2") == 0) || (cscID.find("ME-1/2") ==0)
            || (cscID.find("ME+1.2") == 0) || (cscID.find("ME-1.2") ==0)) return 1;
  else if ( (cscID.find("ME+1/3") == 0) || (cscID.find("ME-1/3") ==0)
            || (cscID.find("ME+1.3") == 0) || (cscID.find("ME-1.3") ==0)) return 2;
  else if ( (cscID.find("ME+2/2") == 0) || (cscID.find("ME-2/2") ==0)
            || (cscID.find("ME+2.2") == 0) || (cscID.find("ME-2.2") ==0)
            || (cscID.find("ME+3/2") == 0) || (cscID.find("ME-3/2") ==0)
            || (cscID.find("ME+3.2") == 0) || (cscID.find("ME-3.2") ==0)
            || (cscID.find("ME+4/2") == 0) || (cscID.find("ME-4/2") ==0)
            || (cscID.find("ME+4.2") == 0) || (cscID.find("ME-4.2") ==0)) return 5;
  else if ( (cscID.find("ME+1/1") == 0) || (cscID.find("ME-1/1") ==0)
            || (cscID.find("ME+1.1") == 0) || (cscID.find("ME-1.1") ==0)) return 7;
  else return 0;
}

///* Return delay offset for chamber cable
double Test_AFEB07::getChamberDelayOffset(std::string cscID)
{
  return 0;
}

void Test_AFEB07::setTestParams()
{

  LOG4CPLUS_INFO (logger, "Setting additional test parameters.");
  std::map<std::string, std::string>::iterator itr;

  itr = test_params.find("STEPsetup");
  if (itr != test_params.end() )
    {
      fSTEPsetup = atoi((itr->second).c_str());
      LOG4CPLUS_INFO (logger, "parameter: STEP setup flag: " << fSTEPsetup);
    }

}
