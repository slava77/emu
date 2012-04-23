#include "emu/dqm/calibration/Test_AFEB07.h"

using namespace XERCES_CPP_NAMESPACE;

using namespace emu::dqm::utils;

static char* F_CSC = "ME+3.2.20";

Test_AFEB07::Test_AFEB07(std::string dfile): Test_Generic(dfile)
{
    testID = "AFEB07";
    nExpectedEvents = 12800;
    dduID=0;
    //  binCheckMask=0x16CFF3F6;
    binCheckMask=0x1FEBF3F6;
    //  binCheckMask=0xF7CB3BF6;
    ltc_bug=2;
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
    if (tmasks.find(cscID) != tmasks.end())
    {
        cscdata["_MASK"]=tmasks[cscID];
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
        if  (DDUstats[dduID].empty_evt_cntr==0)
        {
            LOG4CPLUS_INFO(logger, "No LTC/TTC double L1A bug in data");
            ltc_bug=1;
        }
        else
        {
            LOG4CPLUS_WARN(logger, "Found LTC/TTC double L1A bug in data");
        }


    int delaySwitch=800*ltc_bug;

    if (currL1A > nExpectedEvents) return;
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
    TestData& cscdata = tdata[cscID];

    TestData2D& r01 = cscdata["R01"];
    TestData2D& r02 = cscdata["R02"];
    TestData2D& r03 = cscdata["R03"];

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
                    for (uint32_t n=0; n < tbins.size(); n++)
                    {
                        if (n < ACTIVETBINS_LIMIT) {
                            v02->Fill(wg-1, (nLayer-1)*32+tbins[n]);
                            // if (n < ACTIVETBINS_LIMIT)
                            dsdata.content[curr_delay][nLayer-1][wg-1].tbins[tbins[n]]++;
                        }

                    }
                    dsdata.content[curr_delay][nLayer-1][wg-1].cnt++;

                    /*
                                if (curr_layer == nLayer)
                                  {
                                    r01.cnts[curr_layer-1][wg-1]++;
                                  }
                                if ( nLayer != curr_layer && nLayer == pair_layer)
                                  {
                                    r02.cnts[pair_layer-1][wg-1]++;
                                  }
                                if ( nLayer != curr_layer && nLayer != pair_layer)
                                  {
                                    r03.cnts[nLayer-1][wg-1]++;
                                  }
                    */
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

        TestData2D& mask = cscdata["_MASK"];
        
        TestData2D& r01 = cscdata["R01"];
        TestData2D& r02 = cscdata["R02"];
        TestData2D& r03 = cscdata["R03"];
        

        MonHistos& cschistos = mhistos[cscID];

        TH2F* v03 = reinterpret_cast<TH2F*>(cschistos["V03"]);

        CSCtoHWmap::iterator itr = cscmap.find(cscID);

        if (itr != cscmap.end())
        {

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

            // map->cratedmb(itr->second.first,itr->second.second,&mapitem);
            int dmbID = itr->second.second;
            if (dmbID >= 6) --dmbID;
            int id = 10*itr->second.first+dmbID;

            CSCMapItem::MapItem mapitem = cratemap->item(id);

            std::ofstream f;
            if (cscID == F_CSC) {
                f.open("dump.txt", std::ios::app);
            }


            for (int i=0; i<NLAYERS; i++)
            {
		// std::cout << cscID << Form(": ly%d ", i+1);
                for (int j=0; j<getNumWireGroups(cscID); j++)
                {
		    int ndelays = 0;
		    // std::cout << Form("wg%d -> ", j+1);
                    for (int k=0; k<DELAY_SCAN_STEPS; k++)
                    {
			double sum = 0.;
			double w_sum = 0.;
			double e_sum = 1.;
			for (int t = 0; t < MAX_ALCT_TIMEBINS; t++)
                        {
			   sum += dsdata.content[k][i][j].tbins[t];
			   w_sum += dsdata.content[k][i][j].tbins[t]*(t+1);
			   if (dsdata.content[k][i][j].tbins[t]) e_sum = e_sum*dsdata.content[k][i][j].tbins[t];
			   
                        }
			if ( sum > 0 ) ndelays = k+1;

			double tavg = w_sum/sum - 1;
			double terr = sqrt(e_sum / sum) / sum;

                        dsdata.content[k][i][j].tavg = tavg;
                        dsdata.content[k][i][j].terr = terr;
                        v03->Fill(k, tavg);
                    }

		    fitit( dsdata, i, j , ndelays, &mtemp, &btemp, &chisq);

		    r01.content[i][j] = (btemp * 25.) - cable_delay[j];
                    r02.content[i][j] = mtemp * 25.;
                    r03.content[i][j] = chisq;

                    // std::cout << Form("%.2f %.2f %f; ", r01.content[i][j], r02.content[i][j], r03.content[i][j]);
		    /*
                    for (int k=0; k<DELAY_SCAN_STEPS; k++)
                    {
                        std::cout << Form("d%d=%.2f, %f; ",k, dsdata.content[k][i][j].tavg, dsdata.content[k][i][j].terr);
                    }
                    std::cout << std::endl;
		    */
                }
		// std::cout << std::endl;
            }
            if (cscID == F_CSC) {
                f.close();
            }

            // == Save results to text files
            std::string rpath = "Test_"+testID+"/"+outDir;
            std::string path = rpath+"/"+cscID+"/";

            std::ofstream res_out;


            //      }

        }

    }
}

bool Test_AFEB07::checkResults(std::string cscID)
{
    bool isValid=true;
    cscTestData::iterator td_itr =  tdata.find(cscID);
    if (td_itr != tdata.end())
    {
        TestData& cscdata= td_itr->second;
        TestData2D& r01 = cscdata["R01"];

        int badChannels=0;
    }

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

/*
void fitit(float *x, float *y, float *yerr, int npoints, float *emm,
           float *bee, float *chisq)
  {

  int       j;
  double    denom, weight;
  double    sum, sumx, sumx2, sumy, sumxy;
  double    resid[MAX_POINTS], yfit[MAX_POINTS];

  sum = 0;
  sumx = 0;
  sumx2 = 0;
  sumy = 0;
  sumxy = 0;

  for (j = 0; j < npoints; j++)
    {
    weight = 1 / (yerr[j] * yerr[j]);
    sum = sum + 1. * weight;
    sumx = sumx + x[j] * weight;
    sumx2 = sumx2 + x[j] * x[j] * weight;
    sumy = sumy + y[j] * weight;
    sumxy = sumxy + x[j] * y[j] * weight;
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
    yfit[j] = *emm * x[j] + *bee;
    resid[j] = y[j] - yfit[j];
    *chisq += (resid[j] / yerr[j]) * (resid[j] / yerr[j]);
    }

  return;
  }
*/


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
    else return 0; // ME 1/1 ?? cable delay
}

///* Return delay offset for chamber
double Test_AFEB07::getChamberDelayOffset(std::string cscID)
{
   return 0;
}

