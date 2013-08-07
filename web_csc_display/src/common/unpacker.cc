#include <stdio.h>
#include <stdlib.h>
#include "csc_display/unpacker.h"
#include "emu/daq/reader/RawDataFile.h"

/***************** getting the basic information about the data *****************/
int getTotalEvent(emu::daq::reader::RawDataFile* fd, const char* filename) {
    int total_events = 0;
    
    fd->open(filename);
    while (fd->readNextEvent()) total_events++;
    fd->close();
    
    return total_events;
}

int getChambers(emu::daq::reader::RawDataFile* fd, const char* filename, int** chambers, int* chambersLen) {
    fd->open(filename);
    fd->readNextEvent();
    const char* buf = fd->data();
    int evt_size = fd->dataLength();
    
    int rtval;
    
    // check the binary of the file first
    CSCDCCExaminer bin_checker;
    rtval = check_file_binary(&bin_checker, buf, evt_size);

    // if binary checker is error, then return its error code
    if (rtval == 4) return 4;
    
    // Unpack top level
    CSCDDUEventData dduData((uint16_t *) buf, &bin_checker);

    // Unpack all founded CSC
    std::vector<CSCEventData> chamberDatas;
    chamberDatas.clear();
    chamberDatas = dduData.cscData();

    // get the chambers size
    int nCSCs = chamberDatas.size();
    *chambersLen = nCSCs;
    
    *chambers = (int*) malloc(sizeof(int) * (*chambersLen));
    
    for (int i = 0; i < nCSCs; i++) {
        CSCEventData& data = chamberDatas[i];
        
        // DMB Found, Unpacking of DMB Header and trailer
        const CSCDMBHeader* dmbHeader = data.dmbHeader();
        
        // getting the chamber ID
        int crateID       = dmbHeader->crateID();
        int dmbID         = dmbHeader->dmbID();
        int chamberID     = (((crateID) << 4) + dmbID) & 0xFFF;
        
        (*chambers)[i] = chamberID;
    }
    return 0;
}


/****************** unpack data from raw file (things get more complex here) ******************/
template<typename T> inline CSCCFEBDataWord const * const timeSample( T const & data, int nCFEB,int nSample,int nLayer, int nStrip) {
  return data.cfebData(nCFEB)->timeSlice(nSample)->timeSample(nLayer,nStrip);
}

template<typename T> inline CSCCFEBTimeSlice const * const timeSlice( T const & data, int nCFEB, int nSample) {
  return (CSCCFEBTimeSlice *)(data.cfebData(nCFEB)->timeSlice(nSample));
}

// get the data from file pointer fd for event #event_num and chamberID=chamberID
int check_file_binary(CSCDCCExaminer* bin_checker, const char *buf, int evt_size) {
    ///** Create and configure binary buffer checker object
    bin_checker->output1().hide();
    bin_checker->output2().hide();
    bin_checker->crcALCT(true);
    bin_checker->crcTMB (true);
    bin_checker->crcCFEB(true);
    bin_checker->modeDDU(true);

    uint32_t binCheckMask = 0x16EBF7F6;
    uint32_t dduBinCheckMask = 0x02080016;

    ///** Binary check of the buffer
    uint32_t BinaryErrorStatus = 0, BinaryWarningStatus = 0;
    const uint16_t *tmp = reinterpret_cast<const uint16_t *>(buf);
    bin_checker->setMask(binCheckMask);
    if ( bin_checker->check(tmp,evt_size/sizeof(short)) < 0 ) {
        ///**  No ddu trailer found - force checker to summarize errors by adding artificial trailer
        const uint16_t dduTrailer[4] = { 0x8000, 0x8000, 0xFFFF, 0x8000 };
        tmp = dduTrailer;
        bin_checker->check(tmp,uint32_t(4));
    }

    BinaryErrorStatus   = bin_checker->errors();
    BinaryWarningStatus = bin_checker->warnings();

    // This was commented in the past
    if (((BinaryErrorStatus & dduBinCheckMask)>0) || ((BinaryWarningStatus & 0x2) > 0) ) {
        std::cerr << "Event is skipped because of format errors" << std::endl;
        return 4; /// Reject bad event data
    }

    return 0;
}

int getDataFromFD(emu::daq::reader::RawDataFile* fd, const char* filename, int event_num, int chamberID, data_type* upevt, bool hack) {
    int retval;
    
    fd->open(filename);
    // int trial = 0;
    // while (1) {
        // try {
            // if (trial++ < 5) {
                // fd->open(filename);
                // break;
            // }
            // else {
                // throw 101;
            // }
        // }
        // catch (int e) {
            // // throw an error
            // throw 102;
        // }
        // catch (...) {
            // // do nothing
        // }
    // }
    
    // event_num must be >= 1, so this loop must be executed
    for (int i = 0; i < event_num; i++) {
        retval = fd->readNextEvent();
        if (retval == 0) break;
    }
    
    // if reach end of file, set the file at the beginning
    if (retval == 0) {
        fd->close();
        fd->open(filename);
    }
    // set hack mode ???
    bool HACK_ME11_2DMB = hack;
    
    // get the raw data at the current event
    const char *buf = fd->data();
    int evt_size = fd->dataLength();
    
    // check the binary of the file first
    CSCDCCExaminer bin_checker;
    retval = check_file_binary(&bin_checker, buf, evt_size);
    // if error, return its error code
    if (retval == 4) return 4;
    
    // reset the data
    memset(upevt, 0x0, sizeof(data_type));
    
    // Unpack top level
    CSCDDUEventData dduData((uint16_t *) buf, &bin_checker);
    
    // Unpack all founded CSC
    std::vector<CSCEventData> chamberDatas;
    chamberDatas.clear();
    chamberDatas = dduData.cscData();
    
    int nCSCs = chamberDatas.size();
    for (int ic = 0; ic < nCSCs; ic++) {
        /// process_chamber_data(chamberDatas[i]);
        CSCEventData& data = chamberDatas[ic];
        
        //    DMB Found
        //    Unpacking of DMB Header
        const CSCDMBHeader* dmbHeader = data.dmbHeader();

        ///** Unpacking of Chamber Identification number
        int crateID       = dmbHeader->crateID();
        int dmbID         = dmbHeader->dmbID();
        int ChamberID     = (((crateID) << 4) + dmbID) & 0xFFF;
        
        // if no specified chamber ID, then set its default value
        if (chamberID == 0) chamberID = ChamberID;
        
        // skip it if the chamber ID is not same
        if (chamberID != ChamberID) continue;
        
        (*upevt).chamber_type_id = 0;
        if(crateID == 10 && dmbID <= 3) {
            (*upevt).chamber_type_id = 11; // ME11
        }
        
        // TODO: Add actual Map conversion

        CSCReadoutMappingFromFile cscMapping;
        
        //uint32_t id = cscMapping.chamber(iendcap, istation, crateID, dmbID, -1);
        uint32_t id = 604021272; // for chamber1 types
        uint32_t binCheckMask = 0x16EBF7F6;
        //id=604029960;
        CSCDetId cid( id );
        
        unsigned long errors = bin_checker.errorsForChamber(ChamberID);
        if ((errors & binCheckMask) > 0)
        {
            printf("Skipped CSC_%d_%d because of format errors\n", crateID, dmbID);
            continue;
        }

        ///** Get FEBs Data Available Info
        int cfeb_dav2 = 0;
        for (int i=0; i<5; i++) cfeb_dav2 = cfeb_dav2 + (int)((dmbHeader->cfebAvailable()>>i) & 0x1);

        ///** ALCT Found
        if (data.nalct()) {
        
            (*upevt).alct_active = 1;
            const     CSCALCTHeader*  alctHeader      = data.alctHeader();
            int                       fwVersion       = alctHeader->alctFirmwareVersion();
            const     CSCALCTTrailer* alctTrailer     = data.alctTrailer();
            const     CSCAnodeData*   alctData        = data.alctData();

            if (alctHeader && alctTrailer) {

                (*upevt).alct_nbucket = alctHeader->NTBins();
                (*upevt).alct_l1a_num = alctHeader->L1Acc();
                (*upevt).alct_full_bxn = alctHeader->BXNCount();
                //cout<<"ALCT nbins"<<alctHeader->NTBins()<<endl;
                ///** Process ALCTs
                vector<CSCALCTDigi> alctsDatasTmp = alctHeader->ALCTDigis();
                vector<CSCALCTDigi> alctsDatas;

                for (uint32_t lct=0; lct<alctsDatasTmp.size(); lct++) {
                    if (alctsDatasTmp[lct].isValid())
                        alctsDatas.push_back(alctsDatasTmp[lct]);
                }

                for (uint32_t lct=0; lct<alctsDatas.size(); lct++) {
                    // TODO: Add support for more than 2 ALCTs
                    if (lct>=2) continue;
                    (*upevt).alct_valid_patt[lct]   = (alctsDatas[lct].isValid())?1:0;
                    (*upevt).alct_patt_quality[lct] = alctsDatas[lct].getQuality();
                    (*upevt).alct_accel_muon[lct]   = alctsDatas[lct].getAccelerator();
                    (*upevt).alct_wire_group[lct]   = alctsDatas[lct].getKeyWG();
                    (*upevt).alct_patb[lct]         = alctsDatas[lct].getCollisionB();

                    // if ME13, 31, 41, correct wire group
                    // if (correct_wg) (*upevt).alct_wire_group[lct] -= 16;
                    // bxn for an lct is calculated as full_bxn + number of lct bin
                    // should take remainder from division of this number by number of LHC clocks per orbit
                    int alct_bxn = alctsDatas[lct].getBX();
                    if (fwVersion == 2007) alct_bxn = (alct_bxn + alctHeader->BXNCount())&0x1F;
                    (*upevt).alct_bxn[lct] = alct_bxn;
                }


                ///** Process anode raw data
                for (int nLayer=1; nLayer<=6; nLayer++) {
                    vector<CSCWireDigi> wireDigis = alctData->wireDigis(nLayer);
                    for (vector<CSCWireDigi>:: iterator wireDigisItr = wireDigis.begin(); wireDigisItr != wireDigis.end(); ++wireDigisItr) {
                        int wg = wireDigisItr->getWireGroup();
                        vector<int> tbins = wireDigisItr->getTimeBinsOn();
                        int tbin = wireDigisItr->getTimeBin();
                        
                        for (uint32_t n=0; n < tbins.size(); n++) {
                            tbin = tbins[n];
                            (*upevt).alct_dump[nLayer-1][wg-1] |= 1<<tbin;
                            //std::cout<<"layer/wg : "<<nLayer-1<<"/"<<wg-1<<std::endl;
                        }
                    }
                }
            }
            else {
                cout << "ALCT trailer/header " << alctTrailer << "/" << alctHeader << std::endl;
            }
        }
        else {
            cout << "Chamber " << ChamberID << ": NO ALCT found" << std::endl;
        }


        ///** CLCT Found
        if (data.nclct()) {
            (*upevt).clct_active = 1;
            const CSCTMBHeader* tmbHeader = data.tmbHeader();
            const CSCTMBTrailer* tmbTrailer=data.tmbData()->tmbTrailer();

            if (tmbHeader && tmbTrailer) {
                (*upevt).clct_nbucket = tmbHeader->NTBins();
                //cout<<"tmb header NTbins "<<tmbHeader->NTBins()<<endl;

                //** Process CLCTs
                vector<CSCCLCTDigi> clctsDataTmp= data.tmbHeader()->CLCTDigis(cid.rawId());
                vector<CSCCLCTDigi> clctsData;

                for (uint32_t i_clct=0; i_clct<clctsDataTmp.size();i_clct++) {
                    if(clctsDataTmp[i_clct].isValid()) {
                        clctsData.push_back(clctsDataTmp[i_clct]);
                    }
                }

                //std::cout<<"CLCT Data: Size: "<<clctsDataTmp.size()<<","<<clctsData.size()<<std::endl;

                for (uint32_t clct=0; clct<clctsData.size(); clct++) {
                    // TODO: Add support for more than 2 CLCTs
                    if (clct>=2) continue;
                    (*upevt).clct_valid_patt[clct]   = (clctsData[clct].isValid())?1:0;
                    (*upevt).clct_patt_number[clct] = clctsData[clct].getPattern();
                    (*upevt).clct_bend[clct]   = clctsData[clct].getBend();
                    (*upevt).clct_half_strip[clct]   = clctsData[clct].getKeyStrip();

                    // take rather clctsData[clct].getBX()?

                    (*upevt).clct_bxn[clct] = clctsData[clct].getFullBX();
                }
            }
            else {
                cout<<"tmb header and trailer not ok"<<endl;
            }

            // nclct>0 checked before

            ///** Process cathode raw data - layers count from 1
            for (int nLayer=1; nLayer<=6; nLayer++) {
                vector<CSCComparatorDigi> comparatorDigis = data.clctData()->comparatorDigis(nLayer);

                for(vector<CSCComparatorDigi>::iterator compDigisItr= comparatorDigis.begin();compDigisItr != comparatorDigis.end();++compDigisItr) {

                    int strip=compDigisItr->getStrip();
                    vector<int> tbins = compDigisItr->getTimeBinsOn();
                    int tbin=compDigisItr->getTimeBin();
                    for(uint32_t n=0; n < tbins.size(); n++) {
                        tbin = tbins[n];
                        (*upevt).clct_dump_strips[nLayer-1][strip-1] |= 1<<tbin;
                    }

                    (*upevt).clct_dump_halfstrips[nLayer-1][strip-1][compDigisItr->getComparator()] = 1;
                }
            }
        }//close check on number of clct's
        else {
            cout<<"Chamber "<<ChamberID<<": NO CLCT found"<<endl;
        }


        ///** CFEBs Found
        //int N_CFEBs=5;
        int N_CFEBs=5;
        int N_Layers = 6, N_Strips = 16;
        int TrigTime, SCA_BLK, NmbTimeSamples, ADC, OutOfRange;

        (*upevt).sca_clock_phase = 0;
        for (int nCFEB = 0; nCFEB < N_CFEBs; ++nCFEB) {
            if (data.cfebData(nCFEB) !=0) {
                NmbTimeSamples= (data.cfebData(nCFEB))->nTimeSamples();
                (*upevt).nsca_sample = NmbTimeSamples;

                if (dmbHeader->cfebAvailable() & (0x1 << nCFEB)) { /* Is this CFEB present? */
                    (*upevt).active_febs[nCFEB] = 1;
                }

                for (int nLayer = 1; nLayer <= N_Layers; ++nLayer) {
                    for (int nSample = 0; nSample < NmbTimeSamples; ++nSample) {
                        if (timeSlice(data, nCFEB, nSample) == 0) {
                            //std::cerr <<  "CFEB" << nCFEB << " nSample: " << nSample << " - B-Word" << std::endl;
                            continue;
                        }

                        SCA_BLK  = (int)(timeSlice(data, nCFEB, nSample)->scaControllerWord(nLayer).sca_blk);
                        TrigTime = (int)(timeSlice(data, nCFEB, nSample)->scaControllerWord(nLayer).trig_time);

                        for (int nStrip = 1; nStrip <= N_Strips; ++nStrip) {
                            ADC = (int) ((timeSample(data, nCFEB, nSample, nLayer, nStrip)->adcCounts) & 0xFFF);
                            OutOfRange = (int) ((timeSample(data, nCFEB, nSample, nLayer, nStrip)->adcOverflow) & 0x1);

                            (*upevt).adc_out_of_range[nLayer-1][nCFEB*N_Strips+nStrip-1][nSample] = OutOfRange;
                            (*upevt).sca[nLayer-1][nCFEB*N_Strips+nStrip-1][nSample] = ADC;

                            (*upevt).sca_block[nLayer-1][nCFEB*N_Strips+nStrip-1][nSample]  = SCA_BLK;
                            (*upevt).sca_trig_time[nLayer-1][nCFEB*N_Strips+nStrip-1][nSample] = TrigTime;


                            // Start 2DMB HACK -->  (see comment about this hack above)
                            // if the first chamber doesn't have any CLCT data, then the hack does not work
                            if(HACK_ME11_2DMB && ic==0 && nCFEB < 2){
                                (*upevt).adc_out_of_range[nLayer-1][(nCFEB+5)*N_Strips+nStrip-1][nSample] = OutOfRange;
                                (*upevt).sca[nLayer-1][(nCFEB+5)*N_Strips+nStrip-1][nSample] = ADC;

                                (*upevt).sca_block[nLayer-1][(nCFEB+5)*N_Strips+nStrip-1][nSample]  = SCA_BLK;
                                (*upevt).sca_trig_time[nLayer-1][(nCFEB+5)*N_Strips+nStrip-1][nSample] = TrigTime;
                            }
                            // <-- End 2DMB HACK
                        }
                    }
                }
            }
        /// (*upevt).sca_clock_phase |= ((temp[0][ifeb][0] >> 12) & 1) << nCFEB;
        }
    }
    return 0;
}

void unpack_data_cc(data_type upevt, j_common_type* j_data) {
    /**** Semi-hack ****/
    //// ME1/1 chambers have opposite ordering for the A and B
    //// ends, thus the CLCT key halfstip needs to be inverted
    //// in one end or the other.
    bool invert_CLCT_for_ME11A = 1;
    bool invert_CLCT_for_ME11B = 0;

    int ihalf_strip;

    // What is this?
    bool alct_pattern[5][NLAYER] = { {1, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1},
    {0, 0, 0, 1, 1, 1},
    {0, 0, 0, 0, 1, 1} };

    int i, j, istrip, kstrip, ilayer, ihit, isample, ksample, ilct, iwire, imax, ifirst, ilast;
    float sca_sum, peak_strip_tmp;
    float sca_temp, sca_max;

    int sci_end[4]  = {2, 1, 2, 1};
    int sci_layer[4]  = {2, 2, 1, 1};
    (*j_data).nsamples = upevt.nsca_sample;

    //zero arrays
    for (ilayer=0; ilayer<NLAYER; ilayer++) {
        (*j_data).peak_strip[ilayer] = 0;
        for (isample=0; isample<(*j_data).nsamples; isample++) {
            for (istrip=0; istrip<5; istrip++) {
                (*j_data).sample[istrip][ilayer][isample] = 0;
            }
        }
    }
    for (ilayer=1; ilayer<=NLAYER; ilayer++) {
        for (istrip=1; istrip<=NSTRIP; istrip++) {
            (*j_data).strips[istrip-1][ilayer-1] = 0.0;
            (*j_data).strips_peaks[istrip-1][ilayer-1] = 0.0;
        }
    }
    for (ilayer=1; ilayer<=NLAYER; ilayer++) {
        for (ihit=1; ihit<=NWIRE; ihit++) {
            (*j_data).wires[ihit-1][ilayer-1] = 0;
        }
    }
    for (ilayer=1; ilayer<=2; ilayer++) {
        for (j=1; j<=NSCINT; j++) {
            (*j_data).scint[0][j-1][ilayer-1] = 0;
            (*j_data).scint[1][j-1][ilayer-1] = 0;
        }
    }

    //fill strips array with new data
    for (ilayer=1; ilayer<=NLAYER; ilayer++) {
        peak_strip_tmp = 0.0;
        for (istrip=1; istrip<=NSTRIP; istrip++) {
            imax = 1;

            // Pedestal subtraction: subtract off samples (0+1)/2.0 as
            // our pedestal before searching for the maximum sampling

            // compute the 'pedestal'
            float temp_ped = (0.5*upevt.sca[ilayer-1][istrip-1][0]+0.5*upevt.sca[ilayer-1][istrip-1][1]);

            for (isample=1; isample<=(*j_data).nsamples; isample++) {
                //  then do the pedestal subtraction
                upevt.sca[ilayer-1][istrip-1][isample-1] -= (short)temp_ped;
                   
                // zero out values > max
                if (upevt.sca[ilayer-1][istrip-1][isample-1] > upevt.nsca_sample*4096)  upevt.sca[ilayer-1][istrip-1][isample-1] = 0;//-(upevt.sca[ilayer-1][istrip-1][isample-1]-65536);

                // find peak strip
                if (upevt.sca[ilayer-1][istrip-1][isample-1]> upevt.sca[ilayer-1][istrip-1][imax-1]) imax = isample;
                
            }
            
            if ((imax-2)<1) ifirst = 1;
            else ifirst = imax - 2;

            if ((imax+2)>(*j_data).nsamples) ilast = (*j_data).nsamples;
            else ilast = imax+2;

            sca_sum = 0.0;
            sca_max = upevt.sca[0][0][0]; // firman
            for (ksample=ifirst; ksample<=ilast; ksample++) {
                sca_temp = float(upevt.sca[ilayer-1][istrip-1][ksample-1]); // firman
                sca_sum += sca_temp; // firman
                if (sca_temp > sca_max) sca_max = sca_temp; // firman
            }

            (*j_data).strips[istrip-1][ilayer-1] = sca_sum;
            (*j_data).strips_peaks[istrip-1][ilayer-1] = sca_max; // firman

            if ((*j_data).strips[istrip-1][ilayer-1] > peak_strip_tmp) {
                peak_strip_tmp = (*j_data).strips[istrip-1][ilayer-1];
                (*j_data).peak_strip[ilayer-1] = istrip;
            }
        } // istrip
    } // ilayer

    //fill SCA sample array
    for (ilayer=1; ilayer<=NLAYER; ilayer++) {
        if (((*j_data).peak_strip[ilayer-1]-2) < 1) {
            ifirst = 1;
        }
        else {
            ifirst = (*j_data).peak_strip[ilayer-1] - 2;
        }
        if (((*j_data).peak_strip[ilayer-1]+2) > NSTRIP) {
            ilast = NSTRIP;
        }
        else {
            ilast = (*j_data).peak_strip[ilayer-1] + 2;
        }

        for (istrip=ifirst; istrip<=ilast; istrip++) {
            kstrip = istrip - (*j_data).peak_strip[ilayer-1] + 3;
            // kstrip = istrip - 80 + 3; // firman
            for (isample=1; isample<=(*j_data).nsamples; isample++) {
                (*j_data).sample[kstrip-1][ilayer-1][isample-1] = upevt.sca[ilayer-1][istrip-1][isample-1];
            }
        }
    }

    //fill wire array with new data
    for (ihit=1; ihit<=upevt.num_wire_hits; ihit++) {
        if ((upevt.wire_times_lead[ihit-1]>=MIN_TDC_TO_DISPLAY)&&(upevt.wire_times_lead[ihit-1]<=MAX_TDC_TO_DISPLAY)&&(upevt.wire_times_lead[ihit-1]>(*j_data).wires[upevt.wire_groups[ihit-1]-1][upevt.wire_layers[ihit-1]-1])) {
            (*j_data).wires[upevt.wire_groups[ihit-1]-1][upevt.wire_layers[ihit-1]-1] = upevt.wire_times_lead[ihit-1];
        }
    }

    //fill scint array with new data
    for (ihit=1; ihit<=upevt.num_scint_hits; ihit++) {
        i = (upevt.scint_labels[ihit-1])&(0xff);
        j = upevt.scint_labels[ihit-1]>>8;
        (*j_data).scint[sci_end[j-1]-1][i-1][sci_layer[j-1]-1] = upevt.scint_times[ihit-1];
    }

    //get alct info
    for (ilayer=1; ilayer<=NLAYER; ilayer++) {
        for (iwire=1; iwire<=NWIRE; iwire++) {
            (*j_data).wiregroup[iwire-1][ilayer-1] = false;
            (*j_data).alct[0][iwire-1][ilayer-1] = false;
            (*j_data).alct[1][iwire-1][ilayer-1] = false;
        }
    }

    for (ilayer=1; ilayer<=NLAYER; ilayer++) {
        for (iwire=1; iwire<=NWIRE; iwire++) {
            if (upevt.alct_dump[ilayer-1][iwire-1] != 0) {
                (*j_data).wiregroup[iwire-1][ilayer-1] = true;
            }
        }
    }


    //key wire group layer 3
    for (ilct=1; ilct<=2; ilct++) {
        if (upevt.alct_valid_patt[ilct-1]==1) {
            iwire = upevt.alct_wire_group[ilct-1] + 1;
            (*j_data).alct_q[ilct-1] = upevt.alct_patt_quality[ilct-1];
            if (upevt.alct_accel_muon[ilct-1]==1) {
                for (ilayer=1; ilayer<=NLAYER; ilayer++) {
                    (*j_data).alct[ilct-1][iwire-1][ilayer-1] = true;
                }
            }
            else {
                for (ilayer=1; ilayer<=NLAYER; ilayer++) {
                    for (j=iwire-2; j<=iwire+2; j++) {
                        if (alct_pattern[j+3-iwire-1][ilayer-1]&&j>=1&&j<=NWIRE){
                            (*j_data).alct[ilct-1][j-1][ilayer-1] = true;
                        }
                    }
                }
            }
        }
    }


    //get comparator halfstrip info
    for (ilayer=1; ilayer<=NLAYER; ilayer++) {
        for (istrip=1; istrip<=NSTRIP; istrip++) {
            for (ihalf_strip=0; ihalf_strip<=1; ihalf_strip++) {
                // zero arrays
                (*j_data).halfstrip[ihalf_strip][istrip-1][ilayer-1] = false;
                (*j_data).clct[ihalf_strip][istrip-1][ilayer-1] = false;
            }
        }
    }

    // CLCT key halfstrip layer 3
    for (ilct=1; ilct<=2; ilct++) {
        if (upevt.clct_valid_patt[ilct-1]==1) {
            ihalf_strip = upevt.clct_half_strip[ilct-1]%2;
            istrip = upevt.clct_half_strip[ilct-1]/2 + 1;

            if(NCFEB==7) { // Use NCFEB==7 to determine that this is an ME1/1 chamber
           
                if(invert_CLCT_for_ME11A && istrip > 4*NCFEB_STRIP) {
                    istrip = 1 + 4*NCFEB_STRIP + 3*NCFEB_STRIP - (istrip - 4*NCFEB_STRIP);
                    ihalf_strip = !ihalf_strip;
                }
                if(invert_CLCT_for_ME11B && istrip <= 4*NCFEB_STRIP) {
                    istrip = 1 + 4*NCFEB_STRIP - istrip;
                    ihalf_strip = !ihalf_strip;
                }
            }

            (*j_data).clct[ihalf_strip][istrip-1][2] = true; //changed
            //std::cout<<"Key Halfstrip: (*j_data).clct["<<ihalf_strip<<"]["<<istrip<<"-1][2] = true"<<std::endl;
        }
        else {
            //std::cout<<"Key Halfstrip: No valid pattern"<<std::endl;
        }
    }

    //calculate halfstrip from CFEB data
    for (ilayer=1; ilayer<=NLAYER; ilayer++) {
        for (istrip=1; istrip<=NSTRIP; istrip++) {
            if( upevt.clct_dump_halfstrips[ilayer-1][istrip-1][0] ) (*j_data).halfstrip[0][istrip-1][ilayer-1] = true;
            else if( upevt.clct_dump_halfstrips[ilayer-1][istrip-1][1] ) (*j_data).halfstrip[1][istrip-1][ilayer-1] = true;
        }
    }
    return;
}
