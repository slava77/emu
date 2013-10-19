#include "emu/dqm/calibration/Test_19_CFEBComparators.h"
#include <boost/algorithm/string.hpp>

using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;
using namespace boost::algorithm;


Test_19_CFEBComparators::Test_19_CFEBComparators(std::string dfile):
    Test_Generic(dfile)
{
    testID = "19_CFEBComparators";
    nExpectedEvents = 25200;
    dduID = 0;
    binCheckMask=0x16EBF7F6; // same value used in Lisa

    ltc_bug   = 2;
    logger = Logger::getInstance(testID);


    dmb_tpamps_per_strip = 3;
    thresh_step = 3;
    thresh_first = 0;
    threshs_per_tpamp = 35;
    events_per_thresh = 15;
    strip_step = 1;
    strip_first = 1;
    dmb_tpamp_first = 150;
    dmb_tpamp_step = 48;
    scale_turnoff = 12;
    range_turnoff = 20;
    strips_per_run = 16;

}

Test_19_CFEBComparators::~Test_19_CFEBComparators()
{

}

void Test_19_CFEBComparators::initCSC(std::string cscID)
{

    nCSCEvents[cscID]=0;

    ThresholdScanDataC thdata;
    thdata.Nbins = getNumStrips(cscID);
    thdata.Nlayers = 6;

    memset(thdata.content, 0, sizeof (thdata.content));

    tscan_data[cscID] = thdata;
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

    cscdata["R01"]=cfebdata;//noise dac1 (sigma)
    cscdata["R02"]=cfebdata;//thresh dac1 (mean)
    cscdata["R04"]=cfebdata;//thr offset dac1

    cscdata["R05"]=cfebdata;//noise dac2
    cscdata["R06"]=cfebdata;//thresh dac2
    cscdata["R08"]=cfebdata;//thr offset dac2

    cscdata["R14"]=cfebdata;//noise dac3
    cscdata["R15"]=cfebdata;//thresh dac3
    cscdata["R17"]=cfebdata;//thr offset dac3

    cscdata["R09"]=cfebdata;
    //cscdata["R11"]=cfebdata;
    //cscdata["R12"]=cfebdata;


    cfebdata.Nbins = getNumStrips(cscID)/16;
    cfebdata.Nlayers = 1;
    cscdata["R03"]=cfebdata;//avg per CFEB thresh dac1
    cscdata["R07"]=cfebdata;//avg per CFEB thresh dac2
    cscdata["R16"]=cfebdata;//avg per CFEB thresh dac3

    cscdata["R10"]=cfebdata;
    //cscdata["R13"]=cfebdata;


    tdata[cscID] = cscdata;

    bookTestsForCSC(cscID);


}

void Test_19_CFEBComparators::analyze(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
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

        nscan = 0;
    }

    dduID = bin_checker.dduSourceID()&0xFF;
    DDUstats[dduID].evt_cntr++;

    if (bin_checker.errors() != 0) doBinCheck();


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
        if  (DDUstats[dduID].empty_evt_cntr==0)
        {
            LOG4CPLUS_INFO(logger, "No LTC/TTC double L1A bug in data");
            // ltc_bug=1;
        }
        else
        {
            LOG4CPLUS_WARN(logger, "Found LTC/TTC double L1A bug in data");
        }


    int nevt = currL1A;

    int prevThreshold = DDUstats[dduID].thresh;
    float prevDAC = DDUstats[dduID].dac;
    //unused  int prevStrip = DDUstats[dduID].strip;

    int strip = (nevt/(dmb_tpamps_per_strip * threshs_per_tpamp * events_per_thresh)) * strip_step + strip_first;

    float dac = ( (nevt % (dmb_tpamps_per_strip * threshs_per_tpamp * events_per_thresh) ) / threshs_per_tpamp / events_per_thresh) * dmb_tpamp_step + dmb_tpamp_first;

    int thresh_first = (int)(dac * scale_turnoff / 16 - range_turnoff);

    int amp = (nevt % (dmb_tpamps_per_strip * threshs_per_tpamp * events_per_thresh) ) / threshs_per_tpamp / events_per_thresh;

    if(thresh_first < 0) thresh_first = 0;

    int threshold = ((nevt % (threshs_per_tpamp * events_per_thresh)) / events_per_thresh) * thresh_step + thresh_first;


    DDUstats[dduID].dac = dac;
    DDUstats[dduID].amp = amp;
    DDUstats[dduID].thresh = threshold;
    DDUstats[dduID].strip = strip;
    //cout << nevt << "- dac " << dac << " amp " << amp << " threshld " << threshold << endl;

    if (dac != prevDAC)
    {

        if(nscan < dmb_tpamps_per_strip)
        {

            prevDAC = dac;
            calibration_pulse[nscan] = dac;
            nscan++;

            DDUstats[dduID].empty_evt_cntr=0;

            std::map<std::string, test_step> & cscs = htree[dduID];
            for (std::map<std::string, test_step>::iterator itr = cscs.begin(); itr != cscs.end(); ++itr)
            {
                itr->second.evt_cnt = 0;
            }
        }

    }

    if (threshold != prevThreshold)
    {

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


void Test_19_CFEBComparators::analyzeCSC(const CSCEventData& data)
{

    const CSCDMBHeader* dmbHeader = data.dmbHeader();
    const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();
    if (!dmbHeader && !dmbTrailer) return;


    int csctype=0, cscposition=0;
    std::string cscID = getCSCFromMap(dmbHeader->crateID(), dmbHeader->dmbID(), csctype, cscposition);
    if (cscID == "")  return;

    cscTestData::iterator td_itr = tdata.find(cscID);
    if ( (td_itr == tdata.end()) || (tdata.size() == 0) )
    {
        LOG4CPLUS_INFO(logger, "Found " << cscID);
        initCSC(cscID);
        addCSCtoMap(cscID, dmbHeader->crateID(), dmbHeader->dmbID());
    }

    nCSCEvents[cscID]++;


    MonHistos& cschistos = mhistos[cscID];

    test_step& tstep = htree[dduID][cscID];

    ThresholdScanDataC& thdata = tscan_data[cscID];

    int curr_thresh =  DDUstats[dduID].thresh;
    int curr_amp = DDUstats[dduID].amp; // pulse number (0, 1, 2)
    int curr_strip = DDUstats[dduID].strip;
    int nCFEBs = getNumStrips(cscID)/16;

    tstep.evt_cnt++;


    //v01 for strip occupancy
    //v02 for time bin occupancy
    TH2F* v01 = reinterpret_cast<TH2F*>(cschistos["V01"]);
    TH2F* v02 = reinterpret_cast<TH2F*>(cschistos["V02"]);

    if (data.nclct())
    {
        CSCCLCTData* clctData = data.clctData();
        if (clctData)
        {
            for (int nLayer=1; nLayer<=6; nLayer++)
            {

                bool alreadyHasHit[nCFEBs];
                for(int i = 0; i < nCFEBs; i++) alreadyHasHit[i] = false;

                vector<CSCComparatorDigi> comparatorDigis = clctData->comparatorDigis(nLayer);

                for (vector<CSCComparatorDigi>:: iterator comparatorDigisItr = comparatorDigis.begin();
                        comparatorDigisItr != comparatorDigis.end(); ++comparatorDigisItr)
                {
                    int strip = comparatorDigisItr->getStrip();

                    v01->Fill(strip-1, nLayer-1);
                    vector<int> tbins = comparatorDigisItr->getTimeBinsOn();
                    for (uint32_t n=0; n < tbins.size(); n++)
                    {

                        v02->Fill(strip-1, (nLayer-1)*32+tbins[n]);

                    }

                    float dac = curr_amp * dmb_tpamp_step + dmb_tpamp_first;
                    int first_thresh = (int)(dac * scale_turnoff / 16 - range_turnoff);

                    int ithresh = (curr_thresh - first_thresh)/thresh_step;
                    int curr_cfeb = (strip-1)/16; //0..nCFEBs-1

                    //if(nCSCEvents[cscID]%2000 == 0) {
                    /*if((strip == 5 || strip == 9) && nLayer == 1) {
                      cout << nCSCEvents[cscID] << "- strip " << strip - 1 << " - currstrip-1 " << curr_strip-1 << " - curramp " << curr_amp << " dac " << dac << " first_thresh "
                      << first_thresh << " curr_thresh " << curr_thresh << " ithresh "
                      << ithresh << endl;
                      }*/


                    if((strip-curr_strip) % 16 == 0) { // only record expected strip hits in each cfeb
                        if(!alreadyHasHit[curr_cfeb]) { // ignore extra hits for same strip in an event
                            alreadyHasHit[curr_cfeb] = true;
                            thdata.content[curr_amp][nLayer-1][strip-1][ithresh]++;
                        } else {
                            cout << "multiple hits for evt " << nCSCEvents[cscID] << " - strip " << strip << " layer " << nLayer << endl;
                        }
                    } // if((strip-curr_strip) % 16 == 0)
                } // for (vector<CSCComparatorDigi>:: iterator comparatorDigisItr ... )
            } // for (int nLayer=1; nLayer<=6; nLayer++)
        } // if (clctData)
    } // if (data.nclct())
} // analyzeCSC

void Test_19_CFEBComparators::finishCSC(std::string cscID)
{

    if (nCSCEvents[cscID] < nExpectedEvents)
    {
        LOG4CPLUS_ERROR(logger, Form("%s: Not enough events for test analysis (%d events)", cscID.c_str(), nCSCEvents[cscID] ));
        // = Set error
        return;
    }   

    cout << "c_p[0] " << calibration_pulse[0] << endl;
    cout << "c_p[1] " << calibration_pulse[1] << endl;
    cout << "c_p[2] " << calibration_pulse[2] << endl << endl;

    cout << "q1 " << INJECT_PULSE_C0 + (INJECT_PULSE_C1*calibration_pulse[0]) << " fC" << endl;
    cout << "q2 " << INJECT_PULSE_C0 + (INJECT_PULSE_C1*calibration_pulse[1]) << " fC" << endl;
    cout << "q3 " << INJECT_PULSE_C0 + (INJECT_PULSE_C1*calibration_pulse[2]) << " fC" << endl << endl;

    cscTestData::iterator td_itr =  tdata.find(cscID);
    if (td_itr != tdata.end())
    {

        TestData& cscdata= td_itr->second;


        //result codes labeled above
        TestData2D& r01 = cscdata["R01"];
        TestData2D& r02 = cscdata["R02"];
        TestData2D& r04 = cscdata["R04"];

        TestData2D& r05 = cscdata["R05"];
        TestData2D& r06 = cscdata["R06"];
        TestData2D& r08 = cscdata["R08"];

        TestData2D& r09 = cscdata["R09"];
        //TestData2D& r11 = cscdata["R11"];
        //TestData2D& r12 = cscdata["R12"];

        TestData2D& r14 = cscdata["R14"];
        TestData2D& r15 = cscdata["R15"];
        TestData2D& r16 = cscdata["R16"];
        TestData2D& r17 = cscdata["R17"];

        TestData2D& r03 = cscdata["R03"];
        TestData2D& r07 = cscdata["R07"];
        TestData2D& r10 = cscdata["R10"];
        //TestData2D& r13 = cscdata["R13"];

        ThresholdScanDataC& thdata = tscan_data[cscID];

        int nCFEBs = getNumStrips(cscID)/16;

        CSCtoHWmap::iterator itr = cscmap.find(cscID);


        if (itr != cscmap.end())
        {

            // map->cratedmb(itr->second.first,itr->second.second,&mapitem);
            int dmbID = itr->second.second;
            if (dmbID >= 6) --dmbID;
            int id = 10*itr->second.first+dmbID;

            CSCMapItem::MapItem mapitem = cratemap->item(id);


            for (int amp=0; amp<dmb_tpamps_per_strip; amp++)
            {

                for (int i=0; i<NLAYERS; i++)
                {
                    for (int j=0; j<getNumStrips(cscID); j++)
                    {

                        float  chisq, mean, par[3], rms;

                        calc_thresh(MAX_CALIB_POINTSC, thdata.content[amp][i][j], par, &chisq);


                        int dac = amp * dmb_tpamp_step + dmb_tpamp_first;
                        int first_thresh = dac * scale_turnoff / 16 - range_turnoff;
                        if(first_thresh < 0) first_thresh = 0;

                        if (chisq >= 0.)
                        {


                            mean = par[1]*(thresh_step)+ first_thresh;
                            rms = par[2]*(thresh_step);

                        }
                        else
                        {
                            mean = -1;
                            rms = -1;

                            LOG4CPLUS_WARN(logger, cscID <<
                                    Form(": Layer %d strip %2d has threshold = %6.2f and noise = %6.2f "
                                        "\tpar0 %f, mean %f, rms %f, chisq %f",
                                        i + 1, j + 1, mean, rms,
                                        par[0], par[1]*thresh_step+first_thresh, par[2]*thresh_step, chisq) );

                        }

                        if (amp==0) { ///* Thresholds and Noise for 1st Pulse DAC setting
                            r02.content[i][j] = mean;
                            r01.content[i][j] = rms;
                        }
                        else if (amp == 1) ///* Thresholds and Noise for 2nd Pulse DAC setting
                        {
                            r06.content[i][j] = mean;
                            r05.content[i][j] = rms;
                        }

                        else if (amp == 2) ///* Thresholds and Noise for 3rd Pulse DAC setting
                        {
                            r15.content[i][j] = mean;
                            r14.content[i][j] = rms;
                        }
                    }
                }

            }


            ///* Calculate averaged per CFEB board Threshold for 1st Pulse DAC
            for (int icfeb = 0; icfeb < nCFEBs; icfeb++)
            {
                int goodchans = 0;
                for(int ilayer = 0; ilayer < NLAYER; ilayer++)
                {

                    for (int ichan = 0; ichan<16; ichan++)
                    {

                        int strip = 16*icfeb + ichan;
                        if(r02.content[ilayer][strip] > 0.)
                        {
                            r03.content[0][icfeb] += r02.content[ilayer][strip];
                            goodchans++;
                        }
                    }
                }
                r03.content[0][icfeb] /= goodchans;
                //r03.content[0][icfeb] += 0.5;
            }


            ///* Calculate CFEB Threshold Offsets for 1st Pulse DAC
            for (int istrip = 0; istrip < getNumStrips(cscID); istrip++)
            {
                for (int ilayer = 0; ilayer < NLAYERS; ilayer++)
                {
                    r04.content[ilayer][istrip] =
                        r02.content[ilayer][istrip] - r03.content[0][istrip/16];

                    r04.content[ilayer][istrip] =
                        r02.content[ilayer][istrip] - r03.content[0][istrip/16];
                }
            }


            ///* Calculate averaged per CFEB board Threshold for 2nd Pulse DAC
            for (int icfeb = 0; icfeb < nCFEBs; icfeb++)
            {
                int goodchans = 0;
                for(int ilayer = 0; ilayer < NLAYER; ilayer++)
                {
                    for (int ichan = 0; ichan<16; ichan++)
                    {
                        int strip = 16*icfeb + ichan;
                        if(r06.content[ilayer][strip] > 0.)
                        {
                            r07.content[0][icfeb] += r06.content[ilayer][strip];
                            goodchans++;
                        }
                    }
                }
                r07.content[0][icfeb] /= goodchans;
                //r07.content[0][icfeb] += 0.5;
            }


            ///* Calculate CFEB Threshold Offsets for 2nd Pulse DAC
            for (int istrip = 0; istrip < getNumStrips(cscID); istrip++)
            {
                for (int ilayer = 0; ilayer < NLAYERS; ilayer++)
                {
                    r08.content[ilayer][istrip] =
                        r06.content[ilayer][istrip] - r07.content[0][istrip / 16];

                }
            }


            ///* Calculate averaged per CFEB board Threshold for 3rd Pulse DAC
            for (int icfeb = 0; icfeb < nCFEBs; icfeb++)
            {
                int goodchans = 0;
                for(int ilayer = 0; ilayer < NLAYER; ilayer++)
                {
                    for (int ichan = 0; ichan<16; ichan++)
                    {
                        int strip = 16*icfeb + ichan;
                        if(r15.content[ilayer][strip] > 0.)
                        {
                            r16.content[0][icfeb] += r15.content[ilayer][strip];
                            goodchans++;
                        }
                    }
                }
                r16.content[0][icfeb] /= goodchans;
                //r16.content[0][icfeb] += 0.5;
            }


            ///* Calculate CFEB Threshold Offsets for 3rd Pulse DAC
            for (int istrip = 0; istrip < getNumStrips(cscID); istrip++)
            {
                for (int ilayer = 0; ilayer < NLAYERS; ilayer++)
                {
                    r17.content[ilayer][istrip] =
                        r15.content[ilayer][istrip] - r16.content[0][istrip / 16];
                }
            }


            ///* Calculate Threshold slop (in DACthr/fC) vs CFEB
            float delta_pulse = (calibration_pulse[1]-calibration_pulse[0])*INJECT_PULSE_C1;

            for (int icfeb = 0; icfeb < nCFEBs; icfeb++)
            {
                r10.content[0][icfeb] = (r07.content[0][icfeb] - r03.content[0][icfeb]) / delta_pulse;
            }

            for (int i=0; i<NLAYERS; i++)
            {
                for (int j=0; j<getNumStrips(cscID); j++)
                {
                    ///* Calculate CFEB Threshold Slopes
                    r09.content[i][j] = (r06.content[i][j]-r02.content[i][j]) /
                        (delta_pulse );

                    /*// Calculate CFEB Threshold for 15 fC
                      float q1 = (INJECT_PULSE_C0+INJECT_PULSE_C1 * calibration_pulse[0]) * INJECT_PULSE_C1;
                      float q2 = (INJECT_PULSE_C0+INJECT_PULSE_C1 * calibration_pulse[1]) * INJECT_PULSE_C1;

                      if(fabs(q2-q1) > 1.)
                      r11.content[i][j] =
                      ((q2-15.)*r02.content[i][j]+ (15.-q1)*r06.content[i][j])/(q2-q1);
                      else
                      r11.content[i][j] = -99.; //not defined
                     */
                }

            }


            ///* Calculate averaged per CFEB board Threshold for 15 fC
            /*for (int icfeb = 0; icfeb < nCFEBs; icfeb++)
              {
              int goodchans = 0;
              for(int ilayer = 0; ilayer < NLAYER; ilayer++)
              {
              for (int ichan = 0; ichan<16; ichan++)
              {
              int strip = 16*icfeb + ichan;
              if(r11.content[ilayer][strip] > 0.)
              {
              r13.content[0][icfeb] += r11.content[ilayer][strip];
              goodchans++;
              }
              }
              }
              r13.content[0][icfeb] /= goodchans;
              r13.content[0][icfeb] += 0.5;
              }


            // Calculate CFEB Threshold Offsets for 15 fC
            for (int istrip = 0; istrip < getNumStrips(cscID); istrip++)
            {
            for (int ilayer = 0; ilayer < NLAYERS; ilayer++)
            {
            r12.content[ilayer][istrip] =
            r11.content[ilayer][istrip] - r13.content[0][istrip/16];

            }

            }*/

        }

    }

}

bool Test_19_CFEBComparators::checkResults(std::string cscID)
{
    return true;
}


//calc_thresh exactly as from Test_AFEB06.cc
int Test_19_CFEBComparators::calc_thresh(int npoints, int* content, float* par, float* chisq)
{
    // Local variables initialization...
    int i,first_index;
    float diff_content[MAX_CALIB_POINTSC], all_diff_events_sum;
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
void Test_19_CFEBComparators::setTestParams()
{

    LOG4CPLUS_INFO (logger, "Setting additional test parameters.");
    std::map<std::string, std::string>::iterator itr;

    itr = test_params.find("dmb_tpamps_per_strip");
    if (itr != test_params.end() )
    {
        dmb_tpamps_per_strip = atoi((itr->second).c_str());
        LOG4CPLUS_INFO (logger, "parameter: dmb_tpamps_per_strip: " << dmb_tpamps_per_strip);
    }
    itr = test_params.find("thresh_step");
    if (itr != test_params.end() )
    {
        thresh_step = atoi((itr->second).c_str());
        LOG4CPLUS_INFO (logger, "parameter: thresh_step: " << thresh_step);
    }
    itr = test_params.find("thresh_first");
    if (itr != test_params.end() )
    {
        thresh_first = atoi((itr->second).c_str());
        LOG4CPLUS_INFO (logger, "parameter: thresh_first: " << thresh_first);
    }
    itr = test_params.find("threshs_per_tpamp");
    if (itr != test_params.end() )
    {
        threshs_per_tpamp = atoi((itr->second).c_str());
        LOG4CPLUS_INFO (logger, "parameter: threshs_per_tpamp: " << threshs_per_tpamp);
    }
    itr = test_params.find("events_per_thresh");
    if (itr != test_params.end() )
    {
        events_per_thresh = atoi((itr->second).c_str());
        LOG4CPLUS_INFO (logger, "parameter: events_per_thresh: " << events_per_thresh);
    }
    itr = test_params.find("strip_step");
    if (itr != test_params.end() )
    {
        strip_step = atoi((itr->second).c_str());
        LOG4CPLUS_INFO (logger, "parameter: strip_step: " << strip_step);
    }
    itr = test_params.find("strip_first");
    if (itr != test_params.end() )
    {
        strip_first = atoi((itr->second).c_str());
        LOG4CPLUS_INFO (logger, "parameter: strip_first: " << strip_first);
    }
    itr = test_params.find("dmb_tpamp_first");
    if (itr != test_params.end() )
    {
        dmb_tpamp_first = atoi((itr->second).c_str());
        LOG4CPLUS_INFO (logger, "parameter: dmb_tpamp_first: " << dmb_tpamp_first);
    }
    itr = test_params.find("dmb_tpamp_step");
    if (itr != test_params.end() )
    {
        dmb_tpamp_step = atoi((itr->second).c_str());
        LOG4CPLUS_INFO (logger, "parameter: dmb_tpamp_step: " << dmb_tpamp_step);
    }
    itr = test_params.find("scale_turnoff");
    if (itr != test_params.end() )
    {
        scale_turnoff = atoi((itr->second).c_str());
        LOG4CPLUS_INFO (logger, "parameter: scale_turnoff: " << scale_turnoff);
    }
    itr = test_params.find("range_turnoff");
    if (itr != test_params.end() )
    {
        range_turnoff = atoi((itr->second).c_str());
        LOG4CPLUS_INFO (logger, "parameter: range_turnoff: " << range_turnoff);
    }
    itr = test_params.find("strips_per_run");
    if (itr != test_params.end() )
    {
        strips_per_run = atoi((itr->second).c_str());
        LOG4CPLUS_INFO (logger, "parameter: strips_per_run: " << strips_per_run);
    }


    nExpectedEvents = dmb_tpamps_per_strip * strips_per_run * events_per_thresh * threshs_per_tpamp;
    nExpectedEvents -= 1; //nja - deal with missing initial event
    LOG4CPLUS_INFO (logger, "nExpectedEvents: " << nExpectedEvents);

}
