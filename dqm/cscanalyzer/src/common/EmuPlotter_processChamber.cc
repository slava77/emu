#include "emu/dqm/cscanalyzer/EmuPlotter.h"
#include "emu/dqm/cscanalyzer/CSCStripClusterFinder.h"
#include <TF1.h>

template<typename T> inline CSCCFEBDataWord const * const
timeSample( T const & data, int nCFEB,int nSample,int nLayer, int nStrip)
{
  return data.cfebData(nCFEB)->timeSlice(nSample)->timeSample(nLayer,nStrip);
}

template<typename T> inline CSCCFEBTimeSlice const * const
timeSlice( T const & data, int nCFEB, int nSample)
{
  return (CSCCFEBTimeSlice *)(data.cfebData(nCFEB)->timeSlice(nSample));
}


//  Filling of chamber's histogram
void EmuPlotter::processChamber(const CSCEventData& data, int nodeID=0, int dduID = 0)
{

  if (&data == 0)
  {
    if (debug) LOG4CPLUS_ERROR(logger_,eTag <<
                                 "Zero pointer. DMB data are not available for unpacking");
    return;
  }

  int FEBunpacked = 0;
  int alct_unpacked = 0;
  int tmb_unpacked  = 0;
  int cfeb_unpacked = 0;

  int alct_keywg = -1;
  int clct_kewdistrip = -1;

  bool L1A_out_of_sync = false;

  EmuMonitoringObject* mo = NULL;

  //  DMB Found
  //  Unpacking of DMB Header and trailer
  const CSCDMBHeader* dmbHeader = data.dmbHeader();
  const CSCDMBTrailer* dmbTrailer = data.dmbTrailer();

  if (!dmbHeader && !dmbTrailer)
  {
    if (debug) LOG4CPLUS_ERROR(logger_,eTag <<
                                 "Can not unpack DMB Header or/and Trailer");
    return;
  }


  //  Unpacking of Chamber Identification number
  int crateID = 0xFF;
  int dmbID = 0xF;
  int ChamberID = 0xFFF;


  crateID = dmbHeader->crateID();
  dmbID   = dmbHeader->dmbID();
  ChamberID = (((crateID) << 4) + dmbID) & 0xFFF;

  std::string nodeTag = "EMU";
  std::string dduTag(Form("DDU_%d", dduID));
  std::string cscTag(Form("CSC_%03d_%02d", crateID, dmbID));

  ME_List& nodeME = MEs[nodeTag];


  unsigned long errors = bin_checker.errorsForChamber(ChamberID);
  if ((errors & binCheckMask) > 0 )
  {
    if (debug) LOG4CPLUS_WARN(logger_,eTag  // << " offset: " << offset
                                << "Format Errors " << cscTag << ": 0x" << hex << errors << " Skipped CSC Unpacking");
    return;
  }

  if ( (crateID <= 0) || (dmbID <= 0) || (crateID > 60) || (dmbID > 10) )
  {
    if (debug) LOG4CPLUS_ERROR(logger_, eTag << "Invalid CSC: " << cscTag << ". Skipping");
    /*
    if (isMEvalid(nodeME, "All_DDUs_Format_Errors", mo))
    {
      mo->Fill(dduID, 30);
    }
    */
    return;
  }

  int iendcap = -1;
  int istation = -1;
  // TODO: Add actual Map conversion
  uint32_t id = cscMapping.chamber(iendcap, istation, crateID, dmbID, -1);
  if (id==0)
  {
    if (debug) LOG4CPLUS_ERROR(logger_, eTag << "Can not find map entry for CSC: " << cscTag << ". Skipping");
    /*
    if (isMEvalid(nodeME, "All_DDUs_Format_Errors", mo))
    {
      mo->Fill(dduID, 30);
    }
    */
    return;
  }
  CSCDetId cid( id );


  int CSCtype = 0;
  int CSCposition = 0;
  std::string cscName = getCSCFromMap(crateID, dmbID, CSCtype, CSCposition );


  if ( fCheckMapping &&  (dduID != cscMapping.dduId(cid)) )
  {
    if (debug) LOG4CPLUS_ERROR(logger_, eTag << "DDU->CSC Mapping inconsistency: Read DDU ID" << dduID <<  ", but for detected " << cscName << " it expects DDU ID" <<  cscMapping.dduId(cid) << ". Skipping");
    /*
    if (isMEvalid(nodeME, "All_DDUs_Format_Errors", mo))
    {
      mo->Fill(dduID, 30);
    }
    */
    return;
  }

  double DMBEvents  = 0.0;
  DMBEvents = nDMBEvents[cscTag];

  LOG4CPLUS_DEBUG(logger_,
                  "Unpacking " << cscTag << " (Event: " << nDMBEvents[cscTag]<< ")");

  // == Creating list of histograms for the particular chamber
  map<string, ME_List >::iterator h_itr = MEs.find(cscTag);
  if (h_itr == MEs.end() || (MEs.size()==0))
  {
    LOG4CPLUS_WARN(logger_, eTag <<
                   "List of MEs for " << cscTag << "[" << cscName << "] not found. " << evtSize );
    LOG4CPLUS_DEBUG(logger_,
                    "Booking Histos for " << cscTag);
    MEs[cscTag] = bookMEs("CSC",cscTag);
    MECanvases[cscTag] = bookMECanvases("CSC", cscTag, Form(" Crate ID = %02d. DMB ID = %02d", crateID, dmbID));
    cscCounters[cscTag] = bookCounters();
    // printMECollection(MEs[cscTag]);
  }


  ME_List& cscME = MEs[cscTag];

  CSCCounters& trigCnts = cscCounters[cscTag];
  trigCnts["DMB"] = nDMBEvents[cscTag];

  if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Unpacked", mo))
  {
    mo->Fill(CSCposition, CSCtype);
  }

  // == Efficiency of the chamber
  float DMBEff  = 0.0;
  DMBEff = float(DMBEvents)/float(nEvents);
  if (DMBEff > 1.0)
  {
    if (debug) LOG4CPLUS_ERROR(logger_,  cscTag  << " has efficiency "
                                 << DMBEff << " which is greater than 1");
  }

  // == Unpacking L1A number from DMB header
  // - DMB L1A: 8bits (256)
  // - DDU L1A: 24bits
  int dmbHeaderL1A      = 0;
  int dmb_ddu_l1a_diff  = 0;
  // -- Use 6-bit L1A
  dmbHeaderL1A = dmbHeader->l1a()%64;


  // == Calculate difference between L1A numbers from DDU and DMB
  dmb_ddu_l1a_diff = (int)(dmbHeaderL1A-(int)(L1ANumber%64));
  if (dmb_ddu_l1a_diff != 0) L1A_out_of_sync = true;
  LOG4CPLUS_DEBUG(logger_, "DMB(ID=" << ChamberID  << ") L1A = " << dmbHeaderL1A
                  << " : DMB L1A - DDU L1A = " << dmb_ddu_l1a_diff);

  // == Init CSC Event Display Histo
  EmuMonitoringObject* mo_EventDisplay = 0;
  if (isMEvalid(cscME, "EventDisplay", mo_EventDisplay)) mo_EventDisplay->getObject()->Reset();

  if (mo_EventDisplay)
  {
    mo_EventDisplay->SetBinContent(1, 1, cid.endcap());
    mo_EventDisplay->SetBinContent(1, 2, cid.station());
    mo_EventDisplay->SetBinContent(1, 3, cid.ring());
    mo_EventDisplay->SetBinContent(1, 4, CSCposition);
    mo_EventDisplay->SetBinContent(1, 5, crateID);
    mo_EventDisplay->SetBinContent(1, 6, dmbID);
    mo_EventDisplay->SetBinContent(1, 7, L1ANumber);
  }


  if (isMEvalid(cscME, "DMB_L1A_Distrib", mo)) mo->Fill(dmbHeaderL1A);

  if (isMEvalid(cscME, "DMB_DDU_L1A_diff", mo))
  {
    if (dmb_ddu_l1a_diff < -32)
      mo->Fill(dmb_ddu_l1a_diff + 64);
    else
    {
      if (dmb_ddu_l1a_diff >= 32)
        mo->Fill(dmb_ddu_l1a_diff - 64);
      else
        mo->Fill(dmb_ddu_l1a_diff);
    }
    mo->SetAxisRange(0.1, 1.1*(1.0+mo->getObject()->GetBinContent(mo->getObject()->GetMaximumBin())), "Y");
  }

  if (isMEvalid(cscME,"DMB_L1A_vs_DDU_L1A",mo)) mo->Fill((int)(L1ANumber&0xFF), (int)dmbHeaderL1A);

  //    Unpacking BXN number from DMB header
  int dmbHeaderBXN      = 0;
  int dmb_ddu_bxn_diff  = 0;


  // - DMB BXN: 12bits (4096) call bxn12(), bxn() return 7bits value
  // - DDU BXN: 12bits (4096)
  // -- Use 6-bit BXN
  dmbHeaderBXN = dmbHeader->bxn12();

  // == Calculate difference between BXN numbers from DDU and DMB
  dmb_ddu_bxn_diff = dmbHeaderBXN%64-BXN%64;
  LOG4CPLUS_DEBUG(logger_, "DMB(ID=" << ChamberID  << ") BXN = " << dmbHeaderBXN
                  << " : DMB BXN - DDU BXN = " << dmb_ddu_bxn_diff);
  if (isMEvalid(cscME,"DMB_BXN_Distrib", mo)) mo->Fill((int)(dmbHeader->bxn12()));

  if (isMEvalid(cscME, "DMB_DDU_BXN_diff", mo))
  {
    if (dmb_ddu_bxn_diff < -32) mo->Fill(dmb_ddu_bxn_diff + 64);
    else
    {
      if (dmb_ddu_bxn_diff >= 32)  mo->Fill(dmb_ddu_bxn_diff - 64);
      else mo->Fill(dmb_ddu_bxn_diff);
    }
    mo->SetAxisRange(0.1, 1.1*(1.0+ mo->getObject()->GetBinContent(mo->getObject()->GetMaximumBin())), "Y");
  }

  if (isMEvalid(cscME,"DMB_BXN_vs_DDU_BXN", mo)) mo->Fill(((int)(BXN))%256, ((int)dmbHeaderBXN)%256);

  // == Unpacking CFEB information from DMB header
  int cfeb_dav      = 0;
  int cfeb_dav_num    = 0;
  int cfeb_movlp        = 0;
  int dmb_cfeb_sync   = 0;

  cfeb_dav  = (int)dmbHeader->cfebAvailable();

  for (int i=0; i<5; i++) cfeb_dav_num += (cfeb_dav>>i) & 0x1;

  cfeb_movlp    = (int)dmbHeader->cfebMovlp();
  dmb_cfeb_sync = (int)dmbHeader->dmbCfebSync();

  if (isMEvalid(cscME, "DMB_CFEB_DAV", mo))
  {
    for (int i=0; i<5; i++)
    {
      int cfeb_present = (cfeb_dav>>i) & 0x1;
      if (cfeb_present)
      {
        mo->Fill(i);
      }
    }
  }
  if (isMEvalid(cscME, "DMB_CFEB_DAV_multiplicity", mo)) mo->Fill(cfeb_dav_num);
  if (isMEvalid(cscME, "DMB_CFEB_MOVLP", mo)) mo->Fill(cfeb_movlp);
  if (isMEvalid(cscME, "DMB_CFEB_Sync", mo)) mo->Fill(dmb_cfeb_sync);

  if (isMEvalid(nodeME, "DMB_Unpacked", mo))
  {
    mo->Fill(crateID,dmbID);
  }

  if (isMEvalid(cscME, "DMB_CFEB_Active", mo)) mo->Fill(dmbHeader->cfebActive());


  if (isMEvalid(cscME, "DMB_L1_Pipe", mo)) mo->Fill(dmbTrailer->dmb_l1pipe);

  // == DMB input (7 in total) FIFO stuff goes here
  if (isMEvalid(cscME, "DMB_FIFO_stats", mo))
  {
    if (dmbTrailer->tmb_empty == 1)   mo->Fill(1.0, 0.0);
    if (dmbTrailer->tmb_half == 0)  mo->Fill(1.0, 1.0);
    if (dmbTrailer->tmb_full == 1)  mo->Fill(1.0, 2.0);
    if (dmbTrailer->alct_empty == 1)  mo->Fill(0.0, 0.0);
    if (dmbTrailer->alct_half == 0)   mo->Fill(0.0, 1.0);
    if (dmbTrailer->alct_full == 1)   mo->Fill(0.0, 2.0);

    for (int i=0; i<5; i++)
    {
      if ((int)((dmbTrailer->cfeb_empty>>i) &0x1) == 1) mo->Fill(i+2,0.0);
      if ((int)((dmbTrailer->cfeb_half>>i)  &0x1) == 0) mo->Fill(i+2,1);
      if ((int)((dmbTrailer->cfeb_full>>i)  &0x1) == 1)
      {
        mo->Fill(i+2,2);
      }
    }
    mo->SetEntries((int)DMBEvents);
  }

  // == DMB input timeout (total 15 bits) goes here
  if (isMEvalid(cscME, "DMB_FEB_Timeouts", mo))
  {
    if ((dmbTrailer->tmb_timeout==0) && (dmbTrailer->alct_timeout==0) && (dmbTrailer->cfeb_starttimeout==0) && (dmbTrailer->cfeb_endtimeout==0))
    {
      mo->Fill(0.0);
    }
    else
    {
      if (dmbTrailer->alct_timeout)   mo->Fill(1);
      if (dmbTrailer->tmb_timeout)    mo->Fill(2);
      if (dmbTrailer->alct_endtimeout)  mo->Fill(8);
      if (dmbTrailer->tmb_endtimeout)   mo->Fill(9);
    }
    for (int i=0; i<5; i++)
    {
      if ((dmbTrailer->cfeb_starttimeout>>i) & 0x1)
      {
        mo->Fill(i+3);
      }
      if ((dmbTrailer->cfeb_endtimeout>>i) & 0x1)
      {
        mo->Fill(i+10);
      }
    }
    mo->SetEntries((int)DMBEvents);
  }

  // == Get FEBs Data Available Info
  int alct_dav  = dmbHeader->nalct();
  int tmb_dav   = dmbHeader->nclct();
  int cfeb_dav2 = 0;
  for (int i=0; i<5; i++) cfeb_dav2 = cfeb_dav2 + (int)((dmbHeader->cfebAvailable()>>i) & 0x1);

  // == Fill Histogram for FEB DAV Efficiency

  if ((alct_dav  > 0) && (isMEvalid(cscME, "DMB_FEB_DAV_Rate", mo)))
  {
    mo->Fill(0.0);
    float alct_dav_number   = mo->GetBinContent(1);
    if (isMEvalid(cscME, "DMB_FEB_DAV_Efficiency", mo))
    {
      mo->SetBinContent(1, ((float)alct_dav_number/(float)(DMBEvents)*100.0));
      mo->SetEntries((int)DMBEvents);
    }
  }

  if ((tmb_dav > 0) && (isMEvalid(cscME, "DMB_FEB_DAV_Rate", mo)))
  {
    mo->Fill(1.0);
    float tmb_dav_number    = mo->GetBinContent(2);
    if (isMEvalid(cscME, "DMB_FEB_DAV_Efficiency", mo))
    {
      mo->SetBinContent(2, ((float)tmb_dav_number/(float)(DMBEvents)*100.0));
      mo->SetEntries((int)DMBEvents);
    }
  }


  if ((cfeb_dav2 > 0) && (isMEvalid(cscME, "DMB_FEB_DAV_Rate", mo)))
  {
    mo->Fill(2.0);
    float cfeb_dav2_number = mo->GetBinContent(3);
    if (isMEvalid(cscME, "DMB_FEB_DAV_Efficiency", mo))
    {
      mo->SetBinContent(3, ((float)cfeb_dav2_number/(float)(DMBEvents)*100.0));
      mo->SetEntries((int)DMBEvents);
    }
  }

  float feb_combination_dav = -1.0;
  //      Fill Histogram for Different Combinations of FEB DAV Efficiency
  if (isMEvalid(cscME, "DMB_FEB_Combinations_DAV_Rate", mo))
  {
    if (alct_dav == 0 && tmb_dav == 0 && cfeb_dav2 == 0) feb_combination_dav = 0.0; // Nothing
    if (alct_dav >  0 && tmb_dav == 0 && cfeb_dav2 == 0) feb_combination_dav = 1.0; // ALCT Only
    if (alct_dav == 0 && tmb_dav >  0 && cfeb_dav2 == 0) feb_combination_dav = 2.0; // TMB Only
    if (alct_dav == 0 && tmb_dav == 0 && cfeb_dav2 >  0) feb_combination_dav = 3.0; // CFEB Only
    if (alct_dav == 0 && tmb_dav >  0 && cfeb_dav2 >  0) feb_combination_dav = 4.0; // TMB+CFEB
    if (alct_dav >  0 && tmb_dav >  0 && cfeb_dav2 == 0) feb_combination_dav = 5.0; // ALCT+TMB
    if (alct_dav >  0 && tmb_dav == 0 && cfeb_dav2 >  0) feb_combination_dav = 6.0; // ALCT+CFEB
    if (alct_dav >  0 && tmb_dav >  0 && cfeb_dav2 >  0) feb_combination_dav = 7.0; // ALCT+TMB+CFEB
    mo->Fill(feb_combination_dav);
    float feb_combination_dav_number = mo->GetBinContent((int)(feb_combination_dav+1.0));
    if (isMEvalid(cscME, "DMB_FEB_Combinations_DAV_Efficiency",mo))
    {
      mo->SetBinContent((int)(feb_combination_dav+1.0), ((float)feb_combination_dav_number/(float)(DMBEvents)*100.0));
      mo->SetEntries((int)DMBEvents);
    }
  }

  // == ALCT Found
  if (data.nalct())
  {
    const   CSCALCTHeader*  alctHeader  = data.alctHeader();
    int       fwVersion   = alctHeader->alctFirmwareVersion();
    const   CSCALCTTrailer* alctTrailer   = data.alctTrailer();
    const   CSCAnodeData*   alctData  = data.alctData();

    if (alctHeader && alctTrailer)
    {
      vector<CSCALCTDigi> alctsDatasTmp = alctHeader->ALCTDigis();
      vector<CSCALCTDigi> alctsDatas;

      for (uint32_t lct=0; lct<alctsDatasTmp.size(); lct++)
      {
        if (alctsDatasTmp[lct].isValid())
          alctsDatas.push_back(alctsDatasTmp[lct]);
      }

      FEBunpacked = FEBunpacked +1;
      alct_unpacked = 1;

      // = Set number of ALCT-events to third bin
      if (isMEvalid(cscME, "CSC_Rate", mo))
      {
        mo->Fill(2);
        uint32_t ALCTEvent = (uint32_t)mo->GetBinContent(3);
        trigCnts["ALCT"] = ALCTEvent;
        if (isMEvalid(cscME, "CSC_Efficiency", mo))
        {
          if (nEvents > 0)
          {
            mo->SetBinContent(1, ((float)ALCTEvent/(float)(DMBEvents)*100.0));
            mo->SetEntries((int)DMBEvents);
          }
        }
      }

      if ((alct_dav >0) && (isMEvalid(cscME, "DMB_FEB_Unpacked_vs_DAV", mo)))
      {
        mo->Fill(0.0, 0.0);
      }


      // - ALCT2007 L1A: 12bits (4096)
      // - ALCT2006 L1A: 4bits (16)
      if (isMEvalid(cscME, "ALCT_L1A", mo)) mo->Fill((int)(alctHeader->L1Acc()));


      // -- Use 6-bit L1A
      if (isMEvalid(cscME, "ALCT_DMB_L1A_diff", mo))
      {

        int alct_dmb_l1a_diff = (int)(alctHeader->L1Acc()%64-dmbHeader->l1a()%64);

        if (alct_dmb_l1a_diff != 0) {
          L1A_out_of_sync = true;
          // LOG4CPLUS_WARN(logger_,eTag << cscTag << "ALCT-DMB L1 difference: " << alct_dmb_l1a_diff);
        }

        if (alct_dmb_l1a_diff < -32) mo->Fill(alct_dmb_l1a_diff + 64);
        else
        {
          if (alct_dmb_l1a_diff >= 32) mo->Fill(alct_dmb_l1a_diff - 64);
          else mo->Fill(alct_dmb_l1a_diff);
        }

        mo->SetAxisRange(0.1, 1.1*(1.0+mo->GetBinContent(mo->getObject()->GetMaximumBin())), "Y");

      }


      if (isMEvalid(cscME, "DMB_L1A_vs_ALCT_L1A", mo)) mo->Fill(alctHeader->L1Acc()%256,dmbHeader->l1a());


      // -- ALCT BXN: 12bits (4096)
      // -- Use 6-bit BXN
      if (isMEvalid(cscME, "ALCT_DMB_BXN_diff", mo))
      {

        int alct_dmb_bxn_diff = (int)(alctHeader->BXNCount()-dmbHeader->bxn12());
        if (alct_dmb_bxn_diff > 0) alct_dmb_bxn_diff -= 3564;
        alct_dmb_bxn_diff %= 64;

        mo->Fill(alct_dmb_bxn_diff);
        mo->SetAxisRange(0.1, 1.1*(1.0+mo->GetBinContent(mo->getObject()->GetMaximumBin())), "Y");

      }

      if (isMEvalid(cscME, "ALCT_BXN", mo)) mo->Fill(alctHeader->BXNCount());

      if (isMEvalid(cscME, "ALCT_BXN_vs_DMB_BXN", mo)) mo->Fill((int)((alctHeader->BXNCount())%256), (int)(dmbHeader->bxn12())%256);

      if (isMEvalid(cscME, "ALCT_Number_Rate", mo))
      {
        mo->Fill(alctsDatas.size());
        int nALCT = (int)mo->GetBinContent((int)(alctsDatas.size()+1));
        if (isMEvalid(cscME, "ALCT_Number_Efficiency", mo))
          mo->SetBinContent((int)(alctsDatas.size()+1), (float)(nALCT)/(float)(DMBEvents)*100.0);
      }

      if (isMEvalid(cscME, "ALCT_Word_Count", mo)) mo->Fill((int)(alctTrailer->wordCount()));

      LOG4CPLUS_DEBUG(logger_, "ALCT Trailer Word Count = " << dec
                      << (int)alctTrailer->wordCount());

      if (alctsDatas.size() >= 2)
      {
        if (isMEvalid(cscME, "ALCT1_vs_ALCT0_KeyWG", mo)) mo->Fill(alctsDatas[0].getKeyWG(),alctsDatas[1].getKeyWG());
      }


      EmuMonitoringObject*  mo_CSC_Plus_endcap_ALCT0_dTime = 0;
      isMEvalid(nodeME, "CSC_Plus_endcap_ALCT0_dTime", mo_CSC_Plus_endcap_ALCT0_dTime);
      EmuMonitoringObject*  mo_CSC_Minus_endcap_ALCT0_dTime = 0;
      isMEvalid(nodeME, "CSC_Minus_endcap_ALCT0_dTime", mo_CSC_Minus_endcap_ALCT0_dTime);
      EmuMonitoringObject*  mo_CSC_ALCT0_BXN_mean = 0;
      isMEvalid(nodeME, "CSC_ALCT0_BXN_mean", mo_CSC_ALCT0_BXN_mean);
      EmuMonitoringObject*  mo_CSC_ALCT0_BXN_rms = 0;
      isMEvalid(nodeME, "CSC_ALCT0_BXN_rms", mo_CSC_ALCT0_BXN_rms);


      for (uint32_t lct=0; lct<alctsDatas.size(); lct++)
      {
        // TODO: Add support for more than 2 ALCTs
        if (lct>=2) continue;


        if (isMEvalid(cscME, Form("ALCT%d_KeyWG", lct), mo)) mo->Fill(alctsDatas[lct].getKeyWG());
        if (lct == 0) alct_keywg  = alctsDatas[lct].getKeyWG();

        int alct_dtime = 0;
        if (fwVersion == 2007)
          alct_dtime = alctsDatas[lct].getBX();
        else // Older 2006 Format
          alct_dtime=(int)(alctsDatas[lct].getBX()-(alctHeader->BXNCount()&0x1F));


        if (isMEvalid(cscME, Form("ALCT%d_dTime", lct), mo))
        {
          if (alct_dtime < -16)
          {
            mo->Fill(alct_dtime + 32);
          }
          else
          {
            if (alct_dtime >= 16)  mo->Fill(alct_dtime - 32);
            else mo->Fill(alct_dtime);
          }
          mo->SetAxisRange(0.1, 1.1*(1.0+mo->GetBinContent(mo->getObject()->GetMaximumBin())), "Y");
          double dTime_mean = mo->getObject()->GetMean();
          double dTime_rms = mo->getObject()->GetRMS();

          // == For ALCT0 Fill Summary dTime Histograms
          if (lct == 0)
          {
            if (cid.endcap() == 1)
            {
              if (mo_CSC_Plus_endcap_ALCT0_dTime) mo_CSC_Plus_endcap_ALCT0_dTime->Fill(alct_dtime);
            }
            if (cid.endcap() == 2)
            {
              if (mo_CSC_Minus_endcap_ALCT0_dTime) mo_CSC_Minus_endcap_ALCT0_dTime->Fill(alct_dtime);
            }

            if ( CSCtype && CSCposition && mo_CSC_ALCT0_BXN_mean)
            {
              mo_CSC_ALCT0_BXN_mean->SetBinContent(CSCposition, CSCtype+1, dTime_mean);
            }
            if ( CSCtype && CSCposition && mo_CSC_ALCT0_BXN_rms)
            {
              mo_CSC_ALCT0_BXN_rms->SetBinContent(CSCposition, CSCtype+1 ,dTime_rms);
            }
          }
        }


        if (isMEvalid(cscME, Form("ALCT%d_dTime_vs_KeyWG", lct), mo))
        {
          if (alct_dtime < -16)
          {
            mo->Fill(alctsDatas[lct].getKeyWG(), alct_dtime + 32);
          }
          else
          {
            if (alct_dtime >= 16) mo->Fill(alctsDatas[lct].getKeyWG(), alct_dtime - 32);
            else                      mo->Fill(alctsDatas[lct].getKeyWG(), alct_dtime);
          }
        }

        if (isMEvalid(cscME, Form("ALCT%d_dTime_Profile", lct), mo))
        {
          if (alct_dtime < -16)
          {
            mo->Fill(alctsDatas[lct].getKeyWG(), alct_dtime + 32);
          }
          else
          {
            if (alct_dtime >= 16)    mo->Fill(alctsDatas[lct].getKeyWG(), alct_dtime - 32);
            else                     mo->Fill(alctsDatas[lct].getKeyWG(), alct_dtime);
          }
        }

        int alct_bxn = alctsDatas[lct].getBX();
        if (fwVersion == 2007) alct_bxn = (alct_bxn + alctHeader->BXNCount())&0x1F;

        if (isMEvalid(cscME, Form("ALCT%d_BXN", lct), mo)) mo->Fill(alct_bxn);

        if (isMEvalid(cscME, Form("ALCT%d_Quality", lct), mo)) mo->Fill(alctsDatas[lct].getKeyWG(), alctsDatas[lct].getQuality() );


        if (mo_EventDisplay)
        {
          mo_EventDisplay->SetBinContent(2, alctsDatas[lct].getKeyWG(), alct_bxn + 1 );
          mo_EventDisplay->SetBinContent(3, alctsDatas[lct].getKeyWG(), alctsDatas[lct].getQuality());
        }

        if (isMEvalid(cscME, Form("ALCT%d_Quality_Distr", lct), mo))
        {
          mo->Fill(alctsDatas[lct].getQuality() );
          if (lct == 0)
          {
            EmuMonitoringObject* mo1 = 0;
            if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_ALCT0_Quality", mo1))
              mo1->SetBinContent(CSCposition, CSCtype+1, mo->getObject()->GetMean());
          }
        }

        if (isMEvalid(cscME, Form("ALCT%d_Quality_Profile", lct), mo)) mo->Fill(alctsDatas[lct].getKeyWG(), alctsDatas[lct].getQuality() );

        if (isMEvalid(cscME, Form("ALCT%d_Pattern", lct), mo))
        {
          int pattern = (alctsDatas[lct].getAccelerator()<<1) + alctsDatas[lct].getCollisionB();
          int keywg = alctsDatas[lct].getKeyWG();
          mo->Fill(keywg, pattern );
        }
        if (isMEvalid(cscME, Form("ALCT%d_Pattern_Distr", lct), mo))
        {
          int pattern = (alctsDatas[lct].getAccelerator()<<1) + alctsDatas[lct].getCollisionB();
          mo->Fill(pattern);
        }
      }
      int NumberOfLayersWithHitsInALCT = 0;
      int NumberOfWireGroupsWithHitsInALCT = 0;
      if (alctData)
      {
        EmuMonitoringObject* mo_AFEB_RawHits_TimeBins  = 0;
        isMEvalid(cscME, "AFEB_RawHits_TimeBins" , mo_AFEB_RawHits_TimeBins);

        EmuMonitoringObject*  mo_CSC_Plus_endcap_AFEB_RawHits_Time = 0;
        isMEvalid(nodeME, "CSC_Plus_endcap_AFEB_RawHits_Time", mo_CSC_Plus_endcap_AFEB_RawHits_Time);
        EmuMonitoringObject*  mo_CSC_Minus_endcap_AFEB_RawHits_Time = 0;
        isMEvalid(nodeME, "CSC_Minus_endcap_AFEB_RawHits_Time", mo_CSC_Minus_endcap_AFEB_RawHits_Time);
        EmuMonitoringObject*  mo_CSC_AFEB_RawHits_Time_mean = 0;
        isMEvalid(nodeME, "CSC_AFEB_RawHits_Time_mean", mo_CSC_AFEB_RawHits_Time_mean);
        EmuMonitoringObject*  mo_CSC_AFEB_RawHits_Time_rms = 0;
        isMEvalid(nodeME, "CSC_AFEB_RawHits_Time_rms", mo_CSC_AFEB_RawHits_Time_rms);

        for (int nLayer=1; nLayer<=6; nLayer++)
        {
          bool CheckLayerALCT = true;
          vector<CSCWireDigi> wireDigis = alctData->wireDigis(nLayer);
          for (vector<CSCWireDigi>:: iterator wireDigisItr = wireDigis.begin();
               wireDigisItr != wireDigis.end(); ++wireDigisItr)
          {
            int wg = wireDigisItr->getWireGroup();

            // int tbin = wireDigisItr->getBeamCrossingTag();
            vector<int> tbins = wireDigisItr->getTimeBinsOn();
            int tbin = wireDigisItr->getTimeBin();
            if (mo_EventDisplay) mo_EventDisplay->SetBinContent(nLayer+3,wg-1, tbin+1);
            if (CheckLayerALCT)
            {
              NumberOfLayersWithHitsInALCT = NumberOfLayersWithHitsInALCT + 1;
              CheckLayerALCT = false;
            }
            // if (tbins.size() > 2) std::cout << eTag << cscTag << " " << wg << " " << tbins.size() << std::endl;
            for (uint32_t n=0; n < tbins.size(); n++)
            {
              tbin = tbins[n];
              if (isMEvalid(cscME, Form("ALCTTime_Ly%d", nLayer), mo)) mo->Fill(wg-1, tbin);
              if (isMEvalid(cscME, Form("ALCTTime_Ly%d_Profile", nLayer), mo)) mo->Fill(wg-1, tbin);
              if (mo_AFEB_RawHits_TimeBins) mo_AFEB_RawHits_TimeBins->Fill(tbin);
              if (cid.endcap() == 1)
              {
                if (mo_CSC_Plus_endcap_AFEB_RawHits_Time) mo_CSC_Plus_endcap_AFEB_RawHits_Time->Fill(tbin);
              }
              if (cid.endcap() == 2)
              {
                if (mo_CSC_Minus_endcap_AFEB_RawHits_Time) mo_CSC_Minus_endcap_AFEB_RawHits_Time->Fill(tbin);
              }

            }
            if (isMEvalid(cscME, Form("ALCT_Ly%d_Rate", nLayer), mo))
            {
              mo->Fill(wg-1);
              int number_wg = (int)(mo->GetBinContent(wg));
              Double_t Number_of_entries_ALCT = mo->getObject()->GetEntries();
              if (isMEvalid(cscME, Form("ALCT_Ly%d_Efficiency", nLayer), mo))
              {
                mo->SetBinContent(wg,((float)number_wg));
                if ((Double_t)(DMBEvents) > 0.0)
                {
                  mo->getObject()->SetNormFactor(100.0*Number_of_entries_ALCT/(Double_t)(DMBEvents));
                }
                else
                {
                  mo->getObject()->SetNormFactor(100.0);
                }
                mo->SetEntries((int)DMBEvents);
              }
            }
            NumberOfWireGroupsWithHitsInALCT = NumberOfWireGroupsWithHitsInALCT + 1;
          }
        }

        // Fill Summary Anode Raw Hits Timing Plots
        if (mo_AFEB_RawHits_TimeBins)
        {
          double rawhits_time_mean = mo_AFEB_RawHits_TimeBins->getObject()->GetMean();
          double rawhits_time_rms = mo_AFEB_RawHits_TimeBins->getObject()->GetRMS();
          if ( CSCtype && CSCposition && mo_CSC_AFEB_RawHits_Time_mean)
          {
            mo_CSC_AFEB_RawHits_Time_mean->SetBinContent(CSCposition, CSCtype+1, rawhits_time_mean);
          }
          if ( CSCtype && CSCposition && mo_CSC_AFEB_RawHits_Time_rms)
          {
            mo_CSC_AFEB_RawHits_Time_rms->SetBinContent(CSCposition, CSCtype+1 ,rawhits_time_rms);
          }
        }

      }
      else
      {
        if (debug) LOG4CPLUS_ERROR(logger_,eTag << cscTag <<
                                     " Can not unpack Anode Data");

      }
      if (isMEvalid(cscME, "ALCT_Number_Of_Layers_With_Hits", mo))
      {
        mo->Fill(NumberOfLayersWithHitsInALCT);
        EmuMonitoringObject* mo1 = 0;
        if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_ALCT_Planes_with_Hits", mo1))
          mo1->SetBinContent(CSCposition, CSCtype+1, mo->getObject()->GetMean());
      }
      if (isMEvalid(cscME, "ALCT_Number_Of_WireGroups_With_Hits", mo)) mo->Fill(NumberOfWireGroupsWithHitsInALCT);
    }
    else
    {
      if (debug) LOG4CPLUS_ERROR(logger_,eTag << cscTag <<
                                   " Can not unpack ALCT Header or/and Trailer");
    }
  }
  else
  {
    //  ALCT not found

    if (isMEvalid(cscME, "ALCT_Number_Rate", mo))
    {
      mo->Fill(0);
      int nALCT = (int)mo->GetBinContent(1);
      if (isMEvalid(cscME, "ALCT_Number_Efficiency", mo)) mo->SetBinContent(1, (float)(nALCT)/(float)(DMBEvents)*100.0);
    }

    if ((alct_dav  > 0) && (isMEvalid(cscME, "DMB_FEB_Unpacked_vs_DAV", mo)))
    {
      mo->Fill(0.0, 1.0);
    }
  }

  // == ALCT and CLCT coinsidence
  if (data.nclct() && data.nalct())
  {
    CSCALCTHeader* alctHeader = data.alctHeader();

    if (alctHeader)
    {
      vector<CSCALCTDigi> alctsDatasTmp = alctHeader->ALCTDigis();
      vector<CSCALCTDigi> alctsDatas;

      for (uint32_t lct=0; lct<alctsDatasTmp.size(); lct++)
      {
        if (alctsDatasTmp[lct].isValid())
          alctsDatas.push_back(alctsDatasTmp[lct]);
      }

      CSCTMBData* tmbData = data.tmbData();
      if (tmbData)
      {
        CSCTMBHeader* tmbHeader = tmbData->tmbHeader();
        if (tmbHeader)
        {
          if (isMEvalid(cscME, "TMB_BXN_vs_ALCT_BXN", mo)) mo->Fill( ((int)(alctHeader->BXNCount()))%256, ((int)(tmbHeader->BXNCount()))%256 );

          if (isMEvalid(cscME, "TMB_ALCT_BXN_diff", mo))
          {
            int clct_alct_bxn_diff = (int)(alctHeader->BXNCount() - tmbHeader->BXNCount());
            if (clct_alct_bxn_diff < -2048) mo->Fill(clct_alct_bxn_diff + 4096);
            else
            {
              if (clct_alct_bxn_diff > 2048) mo->Fill(clct_alct_bxn_diff - 4096);
              else mo->Fill(clct_alct_bxn_diff);
            }
            mo->SetAxisRange(0.1, 1.1*(1.0+mo->GetBinContent(mo->getObject()->GetMaximumBin())), "Y");
          }

          if (isMEvalid(cscME, "TMB_L1A_vs_ALCT_L1A", mo)) mo->Fill((int)(alctHeader->L1Acc()%256),(int)(tmbHeader->L1ANumber()%256));

          if (isMEvalid(cscME, "TMB_ALCT_L1A_diff", mo))
          {
            int clct_alct_l1a_diff = (int)(tmbHeader->L1ANumber() - alctHeader->L1Acc());
            if (clct_alct_l1a_diff < -2048) mo->Fill(clct_alct_l1a_diff + 4096);
            else
            {
              if (clct_alct_l1a_diff > 2048)  mo->Fill(clct_alct_l1a_diff - 4096);
              else mo->Fill(clct_alct_l1a_diff);
            }
            mo->SetAxisRange(0.1, 1.1*(1.0+mo->GetBinContent(mo->getObject()->GetMaximumBin())), "Y");
          }
        }
        else
        {
          if (debug) LOG4CPLUS_ERROR(logger_,eTag << cscTag <<
                                       " Can not unpack TMB Header");
        }

      }
      else
      {
        if (debug) LOG4CPLUS_ERROR(logger_,eTag << cscTag <<
                                     " Can not unpack TMB Data");
      }
    }
    else
    {
      if (debug) LOG4CPLUS_ERROR(logger_,eTag << cscTag <<
                                   " Can not unpack ALCT Header");
    }
  }

  // == CLCT Found
  if (data.nclct())
  {

    CSCTMBData* tmbData = data.tmbData();
    if (tmbData)
    {

      CSCTMBHeader* tmbHeader = tmbData->tmbHeader();
      CSCTMBTrailer* tmbTrailer = tmbData->tmbTrailer();

      if (tmbHeader && tmbTrailer)
      {

        CSCCLCTData* clctData = data.clctData();

        vector<CSCCLCTDigi> clctsDatasTmp = tmbHeader->CLCTDigis(cid.rawId());
        vector<CSCCLCTDigi> clctsDatas;

        for (uint32_t lct=0; lct<clctsDatasTmp.size(); lct++)
        {
          if (clctsDatasTmp[lct].isValid())
            clctsDatas.push_back(clctsDatasTmp[lct]);
        }

        FEBunpacked = FEBunpacked +1;
        tmb_unpacked = 1;

        if (isMEvalid(cscME, "ALCT_Match_Time", mo))
        {
          mo->Fill(tmbHeader->ALCTMatchTime());

          double alct_match_mean = mo->getObject()->GetMean();
          double alct_match_rms = mo->getObject()->GetRMS();
          EmuMonitoringObject*  mo1 = 0;

          if (cid.endcap() == 1)
          {
            if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Plus_endcap_ALCT_CLCT_Match_Time", mo1))
              mo1->Fill(tmbHeader->ALCTMatchTime());
          }
          if (cid.endcap() == 2)
          {
            if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Minus_endcap_ALCT_CLCT_Match_Time", mo1))
              mo1->Fill(tmbHeader->ALCTMatchTime());
          }
          if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_ALCT_CLCT_Match_mean", mo1))
            mo1->SetBinContent(CSCposition, CSCtype+1, alct_match_mean);
          if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_ALCT_CLCT_Match_rms", mo1))
            mo1->SetBinContent(CSCposition, CSCtype+1, alct_match_rms);
        }

        if (isMEvalid(cscME, "LCT_Match_Status", mo))
        {
          if (tmbHeader->CLCTOnly()) mo->Fill(0.0,0.0);
          if (tmbHeader->ALCTOnly()) mo->Fill(0.0,1.0);
          if (tmbHeader->TMBMatch()) mo->Fill(0.0,2.0);
        }


        if (isMEvalid(cscME, "LCT0_Match_BXN_Difference", mo)) mo->Fill(tmbHeader->Bxn0Diff());
        if (isMEvalid(cscME, "LCT1_Match_BXN_Difference", mo)) mo->Fill(tmbHeader->Bxn1Diff());

        if ((tmb_dav  > 0) && (isMEvalid(cscME, "DMB_FEB_Unpacked_vs_DAV", mo)))
        {
          mo->Fill(1.0, 0.0);
        }

        //          Set number of CLCT-events to forth bin
        if (isMEvalid(cscME, "CSC_Rate", mo))
        {
          mo->Fill(3);
          uint32_t CLCTEvent = (uint32_t)mo->GetBinContent(4);
          trigCnts["CLCT"] = CLCTEvent;
          if (isMEvalid(cscME, "CSC_Efficiency", mo))
          {
            if (nEvents > 0)
            {
              mo->SetBinContent(2,((float)CLCTEvent/(float)(DMBEvents)*100.0));
              mo->getObject()->SetEntries(DMBEvents);
            }
          }
        }

        if (isMEvalid(cscME, "CLCT_L1A", mo)) mo->Fill(tmbHeader->L1ANumber());

        // -- Use 6-bit L1A
        if (isMEvalid(cscME, "CLCT_DMB_L1A_diff", mo))
        {
          int clct_dmb_l1a_diff = (int)((tmbHeader->L1ANumber()%64)-dmbHeader->l1a()%64);
          if (clct_dmb_l1a_diff != 0) L1A_out_of_sync = true;
          if (clct_dmb_l1a_diff < -32) mo->Fill(clct_dmb_l1a_diff + 64);
          else
          {
            if (clct_dmb_l1a_diff >= 32)  mo->Fill(clct_dmb_l1a_diff - 64);
            else mo->Fill(clct_dmb_l1a_diff);
          }
          mo->SetAxisRange(0.1, 1.1*(1.0+mo->GetBinContent(mo->getObject()->GetMaximumBin())), "Y");
        }

        if (isMEvalid(cscME, "DMB_L1A_vs_CLCT_L1A", mo)) mo->Fill(tmbHeader->L1ANumber()%256,dmbHeader->l1a());

        if (isMEvalid(cscME, "CLCT_DMB_BXN_diff", mo))
        {
          int clct_dmb_bxn_diff = (int)(tmbHeader->BXNCount()%64-dmbHeader->bxn12()%64);
          if (clct_dmb_bxn_diff < -32) mo->Fill(clct_dmb_bxn_diff + 64);
          else
          {
            if (clct_dmb_bxn_diff >= 32)  mo->Fill(clct_dmb_bxn_diff - 64);
            else mo->Fill(clct_dmb_bxn_diff);
          }
          mo->SetAxisRange(0.1, 1.1*(1.0+mo->GetBinContent(mo->getObject()->GetMaximumBin())), "Y");
        }

        if (isMEvalid(cscME, "CLCT_BXN", mo)) mo->Fill((int)(tmbHeader->BXNCount()));

        if (isMEvalid(cscME, "CLCT_BXN_vs_DMB_BXN", mo)) mo->Fill(tmbHeader->BXNCount()%256,dmbHeader->bxn12()%256);

        if (isMEvalid(cscME, "CLCT_Number_Rate", mo))
        {
          mo->Fill(clctsDatas.size());
          int nCLCT = (int)mo->GetBinContent((int)(clctsDatas.size()+1));
          if (isMEvalid(cscME, "CLCT_Number", mo)) mo->SetBinContent((int)(clctsDatas.size()+1), (float)(nCLCT)/(float)(DMBEvents)*100.0);
        }


        if (clctsDatas.size()==1)
        {
          if (isMEvalid(cscME, "CLCT0_Clssification", mo))
          {
            if (clctsDatas[0].getStripType())         mo->Fill(0.0);
            else                                      mo->Fill(1.0);
          }
        }

        if (clctsDatas.size()==2)
        {
          if (isMEvalid(cscME, "CLCT1_vs_CLCT0_Key_Strip", mo)) mo->Fill(clctsDatas[0].getKeyStrip(),clctsDatas[1].getKeyStrip());
          if (isMEvalid(cscME, "CLCT0_CLCT1_Clssification", mo))
          {
            if ( clctsDatas[0].getStripType() &&  clctsDatas[1].getStripType())       mo->Fill(0.0);
            if ( clctsDatas[0].getStripType() && !clctsDatas[1].getStripType())       mo->Fill(1.0);
            if (!clctsDatas[0].getStripType() &&  clctsDatas[1].getStripType())       mo->Fill(2.0);
            if (!clctsDatas[0].getStripType() &&  !clctsDatas[1].getStripType())      mo->Fill(3.0);
          }
        }

        if (isMEvalid(cscME, "TMB_Word_Count", mo)) mo->Fill((int)(tmbTrailer->wordCount()));
        LOG4CPLUS_DEBUG(logger_, "TMB Trailer Word Count = "
                        << dec << (int)tmbTrailer->wordCount());


        EmuMonitoringObject*  mo_CSC_Plus_endcap_CLCT0_dTime = 0;
        isMEvalid(nodeME, "CSC_Plus_endcap_CLCT0_dTime", mo_CSC_Plus_endcap_CLCT0_dTime);
        EmuMonitoringObject*  mo_CSC_Minus_endcap_CLCT0_dTime = 0;
        isMEvalid(nodeME, "CSC_Minus_endcap_CLCT0_dTime", mo_CSC_Minus_endcap_CLCT0_dTime);
        EmuMonitoringObject*  mo_CSC_CLCT0_BXN_mean = 0;
        isMEvalid(nodeME, "CSC_CLCT0_BXN_mean", mo_CSC_CLCT0_BXN_mean);
        EmuMonitoringObject*  mo_CSC_CLCT0_BXN_rms = 0;
        isMEvalid(nodeME, "CSC_CLCT0_BXN_rms", mo_CSC_CLCT0_BXN_rms);

        for (uint32_t lct=0; lct<clctsDatas.size(); lct++)
        {


          if (isMEvalid(cscME,  Form("CLCT%d_BXN", lct), mo)) mo ->Fill(clctsDatas[lct].getFullBX() %64);


          int clct_dtime = clctsDatas[lct].getFullBX() - tmbHeader->BXNCount();

          if (clct_dtime > 0)
          {
            clct_dtime -= 3564;

          }

          if (isMEvalid(cscME,  Form("CLCT%d_dTime", lct), mo))
          {
            int dTime = clct_dtime;

            mo->Fill(dTime);
            mo->SetAxisRange(0.1, 1.1*(1.0+mo->GetBinContent(mo->getObject()->GetMaximumBin())), "Y");
            double dTime_mean = mo->getObject()->GetMean();
            double dTime_rms = mo->getObject()->GetRMS();


            // == For CLCT0 Fill Summary dTime Histograms
            if (lct == 0)
            {
              if (cid.endcap() == 1)
              {
                if (mo_CSC_Plus_endcap_CLCT0_dTime) mo_CSC_Plus_endcap_CLCT0_dTime->Fill(dTime);
              }
              if (cid.endcap() == 2)
              {
                if (mo_CSC_Minus_endcap_CLCT0_dTime) mo_CSC_Minus_endcap_CLCT0_dTime->Fill(dTime);
              }

              if ( CSCtype && CSCposition && mo_CSC_CLCT0_BXN_mean)
              {
                mo_CSC_CLCT0_BXN_mean->SetBinContent(CSCposition, CSCtype+1, dTime_mean);
              }
              if ( CSCtype && CSCposition && mo_CSC_CLCT0_BXN_rms)
              {
                mo_CSC_CLCT0_BXN_rms->SetBinContent(CSCposition, CSCtype+1 ,dTime_rms);
              }
            }

          }


          LOG4CPLUS_DEBUG(logger_, "LCT:" << lct << " Type:" << clctsDatas[lct].getStripType()
                          << " Strip:" << clctsDatas[lct].getKeyStrip());
          if (clctsDatas[lct].getStripType())   // HalfStrip Type
          {
            if (isMEvalid(cscME,  Form("CLCT%d_KeyHalfStrip", lct), mo)) mo->Fill(clctsDatas[lct].getKeyStrip());

            if (isMEvalid(cscME,  Form("CLCT%d_dTime_vs_Half_Strip", lct), mo))
            {
              mo->Fill((int)(clctsDatas[lct].getKeyStrip()), clct_dtime);
            }

            if (isMEvalid(cscME,  Form("CLCT%d_dTime_Profile", lct), mo))
            {
              mo->Fill((int)(clctsDatas[lct].getKeyStrip()), clct_dtime);
            }

            if (isMEvalid(cscME,  Form("CLCT%d_Half_Strip_Pattern", lct), mo))
            {
              int pattern_clct = clctsDatas[lct].getPattern();
              double tbin = -1;
              switch (pattern_clct)
              {
              case 0:
                tbin=0.;
                break;
              case 1:
                tbin=1.;
                break;
              case 2:
                tbin=2.;
                break;
              case 3:
                tbin=10.;
                break;
              case 4:
                tbin=3.;
                break;
              case 5:
                tbin=9.;
                break;
              case 6:
                tbin=4.;
                break;
              case 7:
                tbin=8.;
                break;
              case 8:
                tbin=5.;
                break;
              case 9:
                tbin=7.;
                break;
              case 10:
                tbin=6.;
                break;
              }
              if (tbin >= 0) mo->Fill(clctsDatas[lct].getKeyStrip(), tbin);
              EmuMonitoringObject* mo1 = 0;
              if (isMEvalid(cscME,  Form("CLCT%d_Half_Strip_Pattern_Distr", lct), mo1))
                mo1->Fill(tbin);

              /*  // 3-bit CLCT pattern field
                if(pattern_clct == 1) mo->Fill(clctsDatas[lct].getKeyStrip(), 7.0);
                if(pattern_clct == 3) mo->Fill(clctsDatas[lct].getKeyStrip(), 6.0);
                if(pattern_clct == 5) mo->Fill(clctsDatas[lct].getKeyStrip(), 5.0);
                if(pattern_clct == 7) mo->Fill(clctsDatas[lct].getKeyStrip(), 4.0);
                if(pattern_clct == 6) mo->Fill(clctsDatas[lct].getKeyStrip(), 3.0);
                if(pattern_clct == 4) mo->Fill(clctsDatas[lct].getKeyStrip(), 2.0);
                if(pattern_clct == 2) mo->Fill(clctsDatas[lct].getKeyStrip(), 1.0);
                if(pattern_clct == 0) mo->Fill(clctsDatas[lct].getKeyStrip(), 0.0);
              */
            }

            if (isMEvalid(cscME,  Form("CLCT%d_Half_Strip_Quality", lct), mo))
              mo->Fill((int)(clctsDatas[lct].getKeyStrip()),(int)(clctsDatas[lct].getQuality()));


            if (mo_EventDisplay)
            {
              mo_EventDisplay->SetBinContent(10, clctsDatas[lct].getKeyStrip(), clct_dtime);
              mo_EventDisplay->SetBinContent(11, clctsDatas[lct].getKeyStrip(), clctsDatas[lct].getQuality());
            }


            if (isMEvalid(cscME,  Form("CLCT%d_Half_Strip_Quality_Distr", lct), mo))
            {
              mo->Fill((int)(clctsDatas[lct].getQuality()));
              if (lct == 0)
              {
                EmuMonitoringObject* mo1 = 0;
                if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_CLCT0_Quality", mo1))
                  mo1->SetBinContent(CSCposition, CSCtype+1, mo->getObject()->GetMean());
              }
            }


            if (isMEvalid(cscME,  Form("CLCT%d_Half_Strip_Quality_Profile", lct), mo))
              mo->Fill((int)(clctsDatas[lct].getKeyStrip()), (int)(clctsDatas[lct].getQuality()));

          }
          else   // DiStrip Type
          {

            if (isMEvalid(cscME,  Form("CLCT%d_KeyDiStrip", lct), mo)) mo->Fill(clctsDatas[lct].getKeyStrip());

            if (lct == 0) clct_kewdistrip = clctsDatas[lct].getKeyStrip();

            if (isMEvalid(cscME,  Form("CLCT%d_dTime_vs_DiStrip", lct), mo))
            {
              mo->Fill((int)(clctsDatas[lct].getKeyStrip()), clct_dtime);
            }

            if (isMEvalid(cscME,  Form("CLCT%d_DiStrip_Pattern", lct), mo))
            {
              int pattern_clct = (int)((clctsDatas[lct].getPattern()>>1)&0x3);
              if (pattern_clct == 1) mo->Fill(clctsDatas[lct].getKeyStrip(), 7.0);
              if (pattern_clct == 3) mo->Fill(clctsDatas[lct].getKeyStrip(), 6.0);
              if (pattern_clct == 5) mo->Fill(clctsDatas[lct].getKeyStrip(), 5.0);
              if (pattern_clct == 7) mo->Fill(clctsDatas[lct].getKeyStrip(), 4.0);
              if (pattern_clct == 6) mo->Fill(clctsDatas[lct].getKeyStrip(), 3.0);
              if (pattern_clct == 4) mo->Fill(clctsDatas[lct].getKeyStrip(), 2.0);
              if (pattern_clct == 2) mo->Fill(clctsDatas[lct].getKeyStrip(), 1.0);
              if (pattern_clct == 0) mo->Fill(clctsDatas[lct].getKeyStrip(), 0.0);
            }

            if (isMEvalid(cscME,  Form("CLCT%d_DiStrip_Quality", lct), mo))
              mo->Fill((int)(clctsDatas[lct].getKeyStrip()),(int)(clctsDatas[lct].getQuality()));

            if (isMEvalid(cscME,  Form("CLCT%d_DiStrip_Quality_Profile", lct), mo))
              mo->Fill((int)(clctsDatas[lct].getKeyStrip()), (int)(clctsDatas[lct].getQuality()));
          }

        }

        int N_CFEBs=5;

        int NumberOfLayersWithHitsInCLCT = 0;
        int NumberOfHalfStripsWithHitsInCLCT = 0;
        if (clctData && clctData->check())
        {
          EmuMonitoringObject* mo_CFEB_Comparators_TimeSamples  = 0;
          isMEvalid(cscME, "CFEB_Comparators_TimeSamples" , mo_CFEB_Comparators_TimeSamples);

          EmuMonitoringObject*  mo_CSC_Plus_endcap_CFEB_Comparators_Time = 0;
          isMEvalid(nodeME, "CSC_Plus_endcap_CFEB_Comparators_Time", mo_CSC_Plus_endcap_CFEB_Comparators_Time);
          EmuMonitoringObject*  mo_CSC_Minus_endcap_CFEB_Comparators_Time = 0;
          isMEvalid(nodeME, "CSC_Minus_endcap_CFEB_Comparators_Time", mo_CSC_Minus_endcap_CFEB_Comparators_Time);
          EmuMonitoringObject*  mo_CSC_CFEB_Comparators_Time_mean = 0;
          isMEvalid(nodeME, "CSC_CFEB_Comparators_Time_mean", mo_CSC_CFEB_Comparators_Time_mean);
          EmuMonitoringObject*  mo_CSC_CFEB_Comparators_Time_rms = 0;
          isMEvalid(nodeME, "CSC_CFEB_Comparators_Time_rms", mo_CSC_CFEB_Comparators_Time_rms);
          for (int nCFEB = 0; nCFEB < N_CFEBs; ++nCFEB)
          {
            for (int nLayer=1; nLayer<=6; nLayer++)
            {
              // std::cout << "ly" <<  nLayer << " ";
              int hstrip_previous    = -1;
              int tbin_clct_previous = -1;
              bool CheckLayerCLCT = true;
              vector<CSCComparatorDigi> compOutData = clctData->comparatorDigis(nLayer, nCFEB);


              for (vector<CSCComparatorDigi>:: iterator compOutDataItr = compOutData.begin(); compOutDataItr != compOutData.end(); ++compOutDataItr)
              {
                // =VB= Fix to get correct half-strip
                int hstrip = 2*(compOutDataItr->getStrip()-1)+compOutDataItr->getComparator();

                vector<int> tbins_clct = compOutDataItr->getTimeBinsOn();
                int tbin_clct = (int)compOutDataItr->getTimeBin();


                if (mo_EventDisplay) mo_EventDisplay->SetBinContent(nLayer+11, hstrip, tbin_clct+1);

                if (CheckLayerCLCT)
                {
                  NumberOfLayersWithHitsInCLCT = NumberOfLayersWithHitsInCLCT + 1;
                  CheckLayerCLCT = false;
                }

                for (uint32_t n=0; n < tbins_clct.size(); n++)
                {

                  tbin_clct = tbins_clct[n];
                  if (hstrip != hstrip_previous || (tbin_clct != tbin_clct_previous + 1 && tbin_clct != tbin_clct_previous - 1) )
                  {
                    if (isMEvalid(cscME,  Form("CLCTTime_Ly%d", nLayer), mo)) mo->Fill(hstrip, tbin_clct);

                    if (mo_CFEB_Comparators_TimeSamples) mo_CFEB_Comparators_TimeSamples->Fill(tbin_clct);
                    if (cid.endcap() == 1)
                    {
                      if (mo_CSC_Plus_endcap_CFEB_Comparators_Time) mo_CSC_Plus_endcap_CFEB_Comparators_Time->Fill(tbin_clct);
                    }
                    if (cid.endcap() == 2)
                    {
                      if (mo_CSC_Minus_endcap_CFEB_Comparators_Time) mo_CSC_Minus_endcap_CFEB_Comparators_Time->Fill(tbin_clct);
                    }

                    if (isMEvalid(cscME,  Form("CLCTTime_Ly%d_Profile", nLayer), mo)) mo->Fill(hstrip, tbin_clct);

                    if (isMEvalid(cscME,  Form("CLCT_Ly%d_Rate", nLayer), mo))
                    {
                      mo->Fill(hstrip);

                      double number_hstrip = mo->GetBinContent(hstrip+1);
                      double Number_of_entries_CLCT = mo->getObject()->GetEntries();

                      if (isMEvalid(cscME,  Form("CLCT_Ly%d_Efficiency", nLayer), mo))
                      {
                        mo->SetBinContent(hstrip+1,number_hstrip);
                        if (DMBEvents > 0)
                        {
                          double norm = (100.0*Number_of_entries_CLCT)/((double)(DMBEvents));
                          mo->getObject()->SetNormFactor(norm);
                        }
                        else
                        {
                          mo->getObject()->SetNormFactor(100.0);
                        }

                        mo->getObject()->SetEntries(DMBEvents);
                      }
                    }
                  }

                  if (hstrip != hstrip_previous)
                  {
                    NumberOfHalfStripsWithHitsInCLCT = NumberOfHalfStripsWithHitsInCLCT + 1;
                  }
                  hstrip_previous    = hstrip;
                  tbin_clct_previous = tbin_clct;
                }
              }
            }

          }
          if (mo_CFEB_Comparators_TimeSamples)
          {
            double comps_time_mean = mo_CFEB_Comparators_TimeSamples->getObject()->GetMean();
            double comps_time_rms = mo_CFEB_Comparators_TimeSamples->getObject()->GetRMS();
            if ( CSCtype && CSCposition && mo_CSC_CFEB_Comparators_Time_mean)
            {
              mo_CSC_CFEB_Comparators_Time_mean->SetBinContent(CSCposition, CSCtype+1, comps_time_mean);
            }
            if ( CSCtype && CSCposition && mo_CSC_CFEB_Comparators_Time_rms)
            {
              mo_CSC_CFEB_Comparators_Time_rms->SetBinContent(CSCposition, CSCtype+1 ,comps_time_rms);
            }
          }
        }
        else
        {
          if (debug) LOG4CPLUS_ERROR(logger_,eTag << cscTag <<
                                       " Can not unpack CLCT Data");
        }
        if (isMEvalid(cscME, "CLCT_Number_Of_Layers_With_Hits", mo))
        {
          mo->Fill(NumberOfLayersWithHitsInCLCT);
          EmuMonitoringObject* mo1 = 0;
          if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_CLCT_Planes_with_Hits", mo1))
            mo1->SetBinContent(CSCposition, CSCtype+1, mo->getObject()->GetMean());
        }
        if (isMEvalid(cscME, "CLCT_Number_Of_HalfStrips_With_Hits", mo))
          mo->Fill(NumberOfHalfStripsWithHitsInCLCT);
      }
      else
      {
        if (debug) LOG4CPLUS_ERROR(logger_,eTag << cscTag <<
                                     " Can not unpack TMB Header or/and Trailer");
      }
    }
    else
    {
      if (debug) LOG4CPLUS_ERROR(logger_,eTag << cscTag <<
                                   " Can not unpack TMB Data");
    }

  }
  else
  {
    //  CLCT not found

    if (isMEvalid(cscME, "CLCT_Number_Rate", mo))
    {
      mo->Fill(0);
      int nCLCT = (int)mo->GetBinContent(1);
      if (isMEvalid(cscME, "CLCT_Number", mo)) mo->SetBinContent(1, (float)(nCLCT)/(float)(DMBEvents)*100.0);
    }
    if ((tmb_dav  > 0) && (isMEvalid(cscME, "DMB_FEB_Unpacked_vs_DAV", mo)))
    {
      mo->Fill(1.0, 1.0);
    }
  }

  // == CFEB found

  int NumberOfUnpackedCFEBs = 0;
  int N_CFEBs=5, N_Samples=16, N_Layers = 6, N_Strips = 16;
  int ADC = 0, OutOffRange, Threshold = 30;
  // bool DebugCFEB = false;
  int Pedestal[5][6][16];
  std::pair<int,int> CellPeak[5][6][16];
  memset(CellPeak, 0, sizeof(CellPeak));
  float PedestalError[5][6][16];
  bool CheckCFEB = true;
  float Clus_Sum_Charge;
  int TrigTime, L1APhase, UnpackedTrigTime, LCTPhase, SCA_BLK, NmbTimeSamples;
  int  FreeCells, LCT_Pipe_Empty, LCT_Pipe_Full, LCT_Pipe_Count, L1_Pipe_Empty, L1_Pipe_Full, Buffer_Count;

  bool CheckThresholdStripInTheLayer[6][80];
  for (int i=0; i<6; i++)
  {
    for (int j=0; j <80; j++) CheckThresholdStripInTheLayer[i][j] = true;
  }


  bool CheckOutOffRangeStripInTheLayer[6][80];
  for (int i=0; i<6; i++)
  {
    for (int j=0; j<80; j++) CheckOutOffRangeStripInTheLayer[i][j] = true;
  }

  float cscdata[N_CFEBs<<4][N_Samples][N_Layers];
  memset(cscdata, 0, sizeof(cscdata));

  char hbuf[255];
  memset(hbuf, 0, sizeof(hbuf));



  EmuMonitoringObject* mo_CFEB_SCA_CellPeak_Time  = 0;
  isMEvalid(cscME, "CFEB_SCA_CellPeak_Time" , mo_CFEB_SCA_CellPeak_Time);

  EmuMonitoringObject*  mo_CSC_Plus_endcap_CFEB_SCA_CellPeak_Time = 0;
  isMEvalid(nodeME, "CSC_Plus_endcap_CFEB_SCA_CellPeak_Time", mo_CSC_Plus_endcap_CFEB_SCA_CellPeak_Time);
  EmuMonitoringObject*  mo_CSC_Minus_endcap_CFEB_SCA_CellPeak_Time = 0;
  isMEvalid(nodeME, "CSC_Minus_endcap_CFEB_SCA_CellPeak_Time", mo_CSC_Minus_endcap_CFEB_SCA_CellPeak_Time);
  EmuMonitoringObject*  mo_CSC_CFEB_SCA_CellPeak_Time_mean = 0;
  isMEvalid(nodeME, "CSC_CFEB_SCA_CellPeak_Time_mean", mo_CSC_CFEB_SCA_CellPeak_Time_mean);
  EmuMonitoringObject*  mo_CSC_CFEB_SCA_CellPeak_Time_rms = 0;
  isMEvalid(nodeME, "CSC_CFEB_SCA_CellPeak_Time_rms", mo_CSC_CFEB_SCA_CellPeak_Time_rms);
  for (int nCFEB = 0; nCFEB < N_CFEBs; ++nCFEB)
  {
    if (data.cfebData(nCFEB) !=0)
    {
      if (!data.cfebData(nCFEB)->check()) continue;
      FEBunpacked     = FEBunpacked +1;     // Increment number of unpacked FED
      NumberOfUnpackedCFEBs = NumberOfUnpackedCFEBs + 1;  // Increment number of unpaked CFEB
      cfeb_unpacked = 1;
      if (CheckCFEB == true)
      {
        if (isMEvalid(cscME, "CSC_Rate", mo))
        {
          mo->Fill(4);
          uint32_t CFEBEvent = (uint32_t)mo->GetBinContent(5);
          trigCnts["CFEB"] = CFEBEvent;
          if (isMEvalid(cscME, "CSC_Efficiency", mo))
          {
            if (nEvents > 0)
            {
              mo->SetBinContent(3, ((float)CFEBEvent/(float)(DMBEvents)*100.0));
              mo->getObject()->SetEntries((int)DMBEvents);
            }
          }
        }

        if ((cfeb_dav2  > 0) && (isMEvalid(cscME, "DMB_FEB_Unpacked_vs_DAV", mo)))
        {
          mo->Fill(2.0, 0.0);
        }
        CheckCFEB = false;
      }

      NmbTimeSamples= (data.cfebData(nCFEB))->nTimeSamples();
      LOG4CPLUS_DEBUG(logger_, "nEvents = " << nEvents);
      LOG4CPLUS_DEBUG(logger_, "Chamber ID = "<< cscTag << " Crate ID = "<< crateID
                      << " DMB ID = " << dmbID
                      << "nCFEB =" << nCFEB);

      EmuMonitoringObject* mo_CFEB_SCA_Block_Occupancy = 0;
      isMEvalid(cscME, Form("CFEB%d_SCA_Block_Occupancy", nCFEB), mo_CFEB_SCA_Block_Occupancy);
      EmuMonitoringObject*  mo_CFEB_Free_SCA_Cells = 0;
      isMEvalid(cscME, Form("CFEB%d_Free_SCA_Cells", nCFEB), mo_CFEB_Free_SCA_Cells);
      EmuMonitoringObject* mo_CFEB_SCA_Blocks_Locked_by_LCTs;
      isMEvalid(cscME, Form("CFEB%d_SCA_Blocks_Locked_by_LCTs", nCFEB), mo_CFEB_SCA_Blocks_Locked_by_LCTs);
      EmuMonitoringObject* mo_CFEB_SCA_Blocks_Locked_by_LCTxL1;
      isMEvalid(cscME, Form("CFEB%d_SCA_Blocks_Locked_by_LCTxL1", nCFEB), mo_CFEB_SCA_Blocks_Locked_by_LCTxL1);
      EmuMonitoringObject* mo_CFEB_DMB_L1A_diff = 0;
      isMEvalid(cscME, Form("CFEB%d_DMB_L1A_diff", nCFEB), mo_CFEB_DMB_L1A_diff);

      for (int nLayer = 1; nLayer <= N_Layers; ++nLayer)
      {
        EmuMonitoringObject * mo_CFEB_Out_Off_Range_Strips = 0;
        isMEvalid(cscME, Form("CFEB_Out_Off_Range_Strips_Ly%d", nLayer), mo_CFEB_Out_Off_Range_Strips);
        EmuMonitoringObject * mo_CFEB_Active_Samples_vs_Strip = 0;
        isMEvalid(cscME, Form("CFEB_Active_Samples_vs_Strip_Ly%d", nLayer), mo_CFEB_Active_Samples_vs_Strip);
        EmuMonitoringObject * mo_CFEB_Active_Samples_vs_Strip_Profile = 0;
        isMEvalid(cscME, Form("CFEB_Active_Samples_vs_Strip_Ly%d_Profile", nLayer), mo_CFEB_Active_Samples_vs_Strip_Profile);
        EmuMonitoringObject * mo_CFEB_ActiveStrips = 0;
        isMEvalid(cscME, Form("CFEB_ActiveStrips_Ly%d", nLayer), mo_CFEB_ActiveStrips);
        EmuMonitoringObject * mo_CFEB_SCA_Cell_Peak = 0;
        isMEvalid(cscME, Form("CFEB_SCA_Cell_Peak_Ly_%d", nLayer), mo_CFEB_SCA_Cell_Peak);

        EmuMonitoringObject * mo_CFEB_Pedestal_withEMV_Sample = 0;
        isMEvalid(cscME, Form("CFEB_Pedestal_withEMV_Sample_01_Ly%d", nLayer), mo_CFEB_Pedestal_withEMV_Sample);
        EmuMonitoringObject * mo_CFEB_Pedestal_withRMS_Sample = 0;
        isMEvalid(cscME, Form("CFEB_Pedestal_withRMS_Sample_01_Ly%d", nLayer), mo_CFEB_Pedestal_withRMS_Sample);
        EmuMonitoringObject * mo_CFEB_PedestalRMS_Sample = 0;
        isMEvalid(cscME, Form("CFEB_PedestalRMS_Sample_01_Ly%d", nLayer), mo_CFEB_PedestalRMS_Sample);

        for (int nSample = 0; nSample < NmbTimeSamples; ++nSample)
        {
          if (timeSlice(data, nCFEB, nSample) == 0)
          {
            if (debug) LOG4CPLUS_WARN(logger_, "CFEB" << nCFEB << " nSample: " << nSample << " - B-Word");
            continue;
          }
          /* else
            {
                    std::cout << "CFEB" << nCFEB << " nSample: " << nSample
                      << " CRC: " << timeSlice[nCFEB][nSample]->get_crc()
                      << " calcCRC: " << timeSlice[nCFEB][nSample]->calcCRC() << std::endl;

            } */

          if (mo_CFEB_DMB_L1A_diff && !fCloseL1As )
          {
            int cfeb_dmb_l1a_diff = (int)((timeSlice(data, nCFEB, nSample)->get_L1A_number())-dmbHeader->l1a()%64);
            if (cfeb_dmb_l1a_diff != 0)
            {
              L1A_out_of_sync = true;
            }
            if (cfeb_dmb_l1a_diff < -32) mo->Fill(cfeb_dmb_l1a_diff + 64);
            else
            {
              if (cfeb_dmb_l1a_diff >= 32) mo->Fill(cfeb_dmb_l1a_diff - 64);
              else mo_CFEB_DMB_L1A_diff->Fill(cfeb_dmb_l1a_diff);
            }
            mo_CFEB_DMB_L1A_diff->SetAxisRange(0.1, 1.1*(1.0
                                               + mo_CFEB_DMB_L1A_diff->GetBinContent(mo_CFEB_DMB_L1A_diff->getObject()->GetMaximumBin())), "Y");
          }


          TrigTime = (int)(timeSlice(data, nCFEB, nSample)->scaControllerWord(nLayer).trig_time);

          FreeCells = timeSlice(data, nCFEB, nSample)->get_n_free_sca_blocks();
          LCT_Pipe_Empty = timeSlice(data, nCFEB, nSample)->get_lctpipe_empty();
          LCT_Pipe_Full = timeSlice(data, nCFEB, nSample)->get_lctpipe_full();
          LCT_Pipe_Count = timeSlice(data, nCFEB, nSample)->get_lctpipe_count();
          L1_Pipe_Empty = timeSlice(data, nCFEB, nSample)->get_l1pipe_empty();
          L1_Pipe_Full = timeSlice(data, nCFEB, nSample)->get_l1pipe_full();
          Buffer_Count = timeSlice(data, nCFEB, nSample)->get_buffer_count();

          SCA_BLK  = (int)(timeSlice(data, nCFEB, nSample)->scaControllerWord(nLayer).sca_blk);


          // SCA Block Occupancy Histograms
          if (mo_CFEB_SCA_Block_Occupancy) mo_CFEB_SCA_Block_Occupancy->Fill(SCA_BLK);

          // Free SCA Cells
          if (mo_CFEB_Free_SCA_Cells)
          {
            if (timeSlice(data, nCFEB, nSample)->scaControllerWord(nLayer).sca_full == 1) mo_CFEB_Free_SCA_Cells->Fill(-1);
            mo_CFEB_Free_SCA_Cells->Fill(FreeCells);
          }

          // Number of SCA Blocks Locked by LCTs
          if (mo_CFEB_SCA_Blocks_Locked_by_LCTs)
          {
            if (LCT_Pipe_Empty == 1) mo_CFEB_SCA_Blocks_Locked_by_LCTs->Fill(-0.5);
            if (LCT_Pipe_Full == 1) mo_CFEB_SCA_Blocks_Locked_by_LCTs->Fill(16.5);
            mo_CFEB_SCA_Blocks_Locked_by_LCTs->Fill(LCT_Pipe_Count);
          }

          // Number of SCA Blocks Locked by LCTxL1
          if (mo_CFEB_SCA_Blocks_Locked_by_LCTxL1)
          {
            if (L1_Pipe_Empty == 1) mo_CFEB_SCA_Blocks_Locked_by_LCTxL1->Fill(-0.5);
            if (L1_Pipe_Full == 1) mo_CFEB_SCA_Blocks_Locked_by_LCTxL1->Fill(31.5);
            mo_CFEB_SCA_Blocks_Locked_by_LCTxL1->Fill(Buffer_Count);
          }

          if (nSample == 0 && nLayer == 1)
          {
            TrigTime = (int)(timeSlice(data, nCFEB, nSample)->scaControllerWord(nLayer).trig_time);
            int k=1;
            while (((TrigTime >> (k-1)) & 0x1) != 1 && k<=8)
            {
              k = k +1;
            }
            L1APhase = (int)((timeSlice(data, nCFEB, nSample)->scaControllerWord(nLayer).l1a_phase) & 0x1);
            UnpackedTrigTime = ((k<<1)&0xE)+L1APhase;

            if (isMEvalid(cscME, Form("CFEB%d_L1A_Sync_Time", nCFEB), mo))
              mo->Fill((int)UnpackedTrigTime);
            LCTPhase = (int)((timeSlice(data, nCFEB, nSample)->scaControllerWord(nLayer).lct_phase)&0x1);

            if (isMEvalid(cscME, Form("CFEB%d_LCT_PHASE_vs_L1A_PHASE", nCFEB), mo))
              mo->Fill(LCTPhase, L1APhase);

            LOG4CPLUS_DEBUG(logger_, "L1APhase " << L1APhase << " UnpackedTrigTime " << UnpackedTrigTime);

            if (isMEvalid(cscME, Form("CFEB%d_L1A_Sync_Time_vs_DMB", nCFEB), mo))
              mo->Fill((int)(dmbHeader->dmbCfebSync()), (int)UnpackedTrigTime);

            if (isMEvalid(cscME, Form("CFEB%d_L1A_Sync_Time_DMB_diff", nCFEB), mo))
            {
              int cfeb_dmb_L1A_sync_time = (int)(dmbHeader->dmbCfebSync()) - (int)UnpackedTrigTime;
              if (cfeb_dmb_L1A_sync_time < -8) mo->Fill(cfeb_dmb_L1A_sync_time+16);
              else
              {
                if (cfeb_dmb_L1A_sync_time >= 8)    mo->Fill(cfeb_dmb_L1A_sync_time-16);
                else                              mo->Fill(cfeb_dmb_L1A_sync_time);
              }
              mo->SetAxisRange(0.1, 1.1*(1.0+mo->GetBinContent(mo->getObject()->GetMaximumBin())), "Y");
            }

          }


          for (int nStrip = 1; nStrip <= N_Strips; ++nStrip)
          {
            ADC = (int) ((timeSample(data, nCFEB, nSample, nLayer, nStrip)->adcCounts) & 0xFFF);
            LOG4CPLUS_DEBUG(logger_, " nStrip="<< dec << nStrip << " ADC=" << hex << ADC);
            OutOffRange = (int) ((timeSample(data, nCFEB, nSample, nLayer, nStrip)->adcOverflow) & 0x1);

            if (nSample == 0)   // nSample == 0
            {
              CellPeak[nCFEB][nLayer-1][nStrip-1] = std::make_pair(nSample,ADC);
              Pedestal[nCFEB][nLayer-1][nStrip-1] = ADC;
            }


            if (OutOffRange == 1 && CheckOutOffRangeStripInTheLayer[nLayer-1][(nCFEB<<4)+nStrip-1] == true)
            {
              if ( mo_CFEB_Out_Off_Range_Strips)
                mo_CFEB_Out_Off_Range_Strips->Fill((int)((nCFEB<<4)+nStrip-1));
              CheckOutOffRangeStripInTheLayer[nLayer-1][(nCFEB<<4)+nStrip-1] = false;
            }
            if (ADC - Pedestal[nCFEB][nLayer-1][nStrip-1] > Threshold && OutOffRange != 1)
            {


              if (mo_CFEB_Active_Samples_vs_Strip)
                mo_CFEB_Active_Samples_vs_Strip->Fill((int)((nCFEB<<4)+nStrip-1), nSample);

              if (mo_CFEB_Active_Samples_vs_Strip_Profile)
                mo_CFEB_Active_Samples_vs_Strip_Profile->Fill((int)((nCFEB<<4)+nStrip-1), nSample);

              if (CheckThresholdStripInTheLayer[nLayer-1][(nCFEB<<4)+nStrip-1] == true)
              {
                if (mo_CFEB_ActiveStrips)
                  mo_CFEB_ActiveStrips->Fill((int)((nCFEB<<4)+nStrip-1));
                CheckThresholdStripInTheLayer[nLayer-1][(nCFEB<<4)+nStrip-1] = false;
              }

              if (ADC - Pedestal[nCFEB][nLayer-1][nStrip-1] > Threshold)
              {
                LOG4CPLUS_DEBUG(logger_, "Layer="<<nLayer<<" Strip="<<(nCFEB<<4)+nStrip<<" Time="<<nSample
                                << " ADC-PEDEST = "<<ADC - Pedestal[nCFEB][nLayer-1][nStrip-1]);
                cscdata[(nCFEB<<4)+nStrip-1][nSample][nLayer-1] = ADC - Pedestal[nCFEB][nLayer-1][nStrip-1];
              }

              if (ADC >  CellPeak[nCFEB][nLayer-1][nStrip-1].second)
              {
                CellPeak[nCFEB][nLayer-1][nStrip-1].first = nSample;
                CellPeak[nCFEB][nLayer-1][nStrip-1].second = ADC;
              }
            }

            if (nSample == 1)
            {
              int channel_threshold = 40;
              if (abs(ADC - Pedestal[nCFEB][nLayer-1][nStrip-1]) < channel_threshold)
              {
                if (mo_CFEB_Pedestal_withEMV_Sample)
                  mo_CFEB_Pedestal_withEMV_Sample->Fill((int)((nCFEB<<4)+nStrip-1), Pedestal[nCFEB][nLayer-1][nStrip-1]);

                if (mo_CFEB_Pedestal_withRMS_Sample)
                {
                  mo_CFEB_Pedestal_withRMS_Sample->Fill((int)((nCFEB<<4)+nStrip-1), Pedestal[nCFEB][nLayer-1][nStrip-1]);
                  PedestalError[nCFEB][nLayer-1][nStrip-1] = mo_CFEB_Pedestal_withRMS_Sample->getObject()->GetBinError((nCFEB<<4)+nStrip);

                  if (mo_CFEB_PedestalRMS_Sample)
                  {
                    mo_CFEB_PedestalRMS_Sample->SetBinContent((nCFEB<<4)+nStrip-1,PedestalError[nCFEB][nLayer-1][nStrip-1]);
                    mo_CFEB_PedestalRMS_Sample->getObject()->SetBinError((nCFEB<<4)+nStrip-1,0.00000000001);
                  }
                }
              }


              Pedestal[nCFEB][nLayer-1][nStrip-1] += ADC;
              Pedestal[nCFEB][nLayer-1][nStrip-1] /= 2;
              LOG4CPLUS_DEBUG(logger_, " nStrip="<< dec << nStrip
                              << " Pedestal=" << hex << Pedestal[nCFEB][nLayer-1][nStrip-1]);
            }
          }
        }

        for (int nStrip = 1; nStrip <= N_Strips; ++nStrip)
        {
          if (mo_CFEB_SCA_Cell_Peak && CellPeak[nCFEB][nLayer-1][nStrip-1].first)
          {
            mo_CFEB_SCA_Cell_Peak->Fill((int)((nCFEB<<4)+nStrip-1), CellPeak[nCFEB][nLayer-1][nStrip-1].first);
            if (mo_CFEB_SCA_CellPeak_Time)
              mo_CFEB_SCA_CellPeak_Time->Fill(CellPeak[nCFEB][nLayer-1][nStrip-1].first);

            if (mo_EventDisplay)
            {
              int peak_sample = CellPeak[nCFEB][nLayer-1][nStrip-1].first;
              int peak_adc = CellPeak[nCFEB][nLayer-1][nStrip-1].second;
              int pedestal = Pedestal[nCFEB][nLayer-1][nStrip-1];
              int peak_sca_charge = peak_adc - pedestal;

              if (peak_adc - pedestal > Threshold)
              {
                if (peak_sample >=1)
                {
                  peak_sca_charge += ((timeSample(data, nCFEB, peak_sample-1, nLayer, nStrip)->adcCounts)&0xFFF)-pedestal;
                }
                if (peak_sample < NmbTimeSamples-1)
                {
                  peak_sca_charge += ((timeSample(data, nCFEB, peak_sample+1, nLayer, nStrip)->adcCounts)&0xFFF)-pedestal;
                }
                mo_EventDisplay->SetBinContent(nLayer+17, (nCFEB<<4)+nStrip-1, peak_sca_charge);
              }
            }

            if (cid.endcap() == 1)
            {
              if (mo_CSC_Plus_endcap_CFEB_SCA_CellPeak_Time)
                mo_CSC_Plus_endcap_CFEB_SCA_CellPeak_Time->Fill(CellPeak[nCFEB][nLayer-1][nStrip-1].first);
            }
            if (cid.endcap() == 2)
            {
              if (mo_CSC_Minus_endcap_CFEB_SCA_CellPeak_Time)
                mo_CSC_Minus_endcap_CFEB_SCA_CellPeak_Time->Fill(CellPeak[nCFEB][nLayer-1][nStrip-1].first);
            }

          }
        }
      }
    }
  }

  // == Fill Summary CFEB Raw Hits Timing Plots
  if (mo_CFEB_SCA_CellPeak_Time)
  {

    double cellpeak_time_mean = mo_CFEB_SCA_CellPeak_Time->getObject()->GetMean();
    double cellpeak_time_rms = mo_CFEB_SCA_CellPeak_Time->getObject()->GetRMS();

    if ( CSCtype && CSCposition && mo_CSC_CFEB_SCA_CellPeak_Time_mean)
    {
      mo_CSC_CFEB_SCA_CellPeak_Time_mean->SetBinContent(CSCposition, CSCtype+1, cellpeak_time_mean);
    }

    if ( CSCtype && CSCposition && mo_CSC_CFEB_SCA_CellPeak_Time_rms)
    {
      mo_CSC_CFEB_SCA_CellPeak_Time_rms->SetBinContent(CSCposition, CSCtype+1 ,cellpeak_time_rms);
    }
  }

  float Cathodes[N_CFEBs*N_Strips*N_Samples*N_Layers];
  for (int i=0; i<N_Layers; ++i)
  {
    for (int j=0; j<N_CFEBs*N_Strips; ++j)
    {
      for (int k=0; k<N_Samples; ++k)
      {
        Cathodes[i*N_CFEBs*N_Strips*N_Samples + N_CFEBs*N_Strips*k + j] = cscdata[j][k][i];
      }
    }
  }

  vector<CSCStripCluster> Clus;
  Clus.clear();
  CSCStripClusterFinder ClusterFinder(N_Layers, N_Samples, N_CFEBs, N_Strips);

  for (int nLayer=1; nLayer<=N_Layers; ++nLayer)
  {

    ClusterFinder.DoAction(nLayer-1, Cathodes);
    Clus = ClusterFinder.getClusters();
    LOG4CPLUS_DEBUG(logger_, "***  CATHODE PART  DEBUG: Layer="<<nLayer
                    <<"  Number of Clusters="<<Clus.size()<<"      ***");

    if (isMEvalid(cscME, Form("CFEB_Number_of_Clusters_Ly_%d", nLayer), mo))
    {
      if (Clus.size() >= 0)  mo->Fill(Clus.size());
    }

    for (uint32_t u=0; u<Clus.size(); u++)
    {
      Clus_Sum_Charge = 0.0;
      for (uint32_t k=0; k<Clus[u].ClusterPulseMapHeight.size(); k++)
      {
        LOG4CPLUS_DEBUG(logger_, "Strip: " << Clus[u].ClusterPulseMapHeight[k].channel_+1);

        for (int n=Clus[u].LFTBNDTime; n < Clus[u].IRTBNDTime; n++)
        {
          Clus_Sum_Charge = Clus_Sum_Charge + Clus[u].ClusterPulseMapHeight[k].height_[n];
        }
      }

      // Clusters Charge Histograms
      if (isMEvalid(cscME, Form("CFEB_Clusters_Charge_Ly_%d", nLayer), mo))
        mo->Fill(Clus_Sum_Charge);

      // Width of Clusters Histograms
      if (isMEvalid(cscME, Form("CFEB_Width_of_Clusters_Ly_%d", nLayer), mo))
        mo->Fill(Clus[u].IRTBNDStrip - Clus[u].LFTBNDStrip+1);

      // Cluster Duration Histograms
      if (isMEvalid(cscME, Form("CFEB_Cluster_Duration_Ly_%d", nLayer), mo))
        mo->Fill(Clus[u].IRTBNDTime - Clus[u].LFTBNDTime+1);

    }

    Clus.clear();

  }


  // Fill Histogram for Different Combinations of FEBs Unpacked vs DAV
  if (isMEvalid(cscME, "DMB_FEB_Combinations_Unpacked_vs_DAV", mo))
  {
    float feb_combination_unpacked = -1.0;
    if (alct_unpacked == 0 && tmb_unpacked == 0 && cfeb_unpacked == 0) feb_combination_unpacked = 0.0;
    if (alct_unpacked >  0 && tmb_unpacked == 0 && cfeb_unpacked == 0) feb_combination_unpacked = 1.0;
    if (alct_unpacked == 0 && tmb_unpacked >  0 && cfeb_unpacked == 0) feb_combination_unpacked = 2.0;
    if (alct_unpacked == 0 && tmb_unpacked == 0 && cfeb_unpacked >  0) feb_combination_unpacked = 3.0;
    if (alct_unpacked >  0 && tmb_unpacked >  0 && cfeb_unpacked == 0) feb_combination_unpacked = 4.0;
    if (alct_unpacked >  0 && tmb_unpacked == 0 && cfeb_unpacked >  0) feb_combination_unpacked = 5.0;
    if (alct_unpacked == 0 && tmb_unpacked >  0 && cfeb_unpacked >  0) feb_combination_unpacked = 6.0;
    if (alct_unpacked >  0 && tmb_unpacked >  0 && cfeb_unpacked >  0) feb_combination_unpacked = 7.0;
    mo->Fill(feb_combination_dav, feb_combination_unpacked);
  }

  if ((clct_kewdistrip > -1 && alct_keywg > -1) && (isMEvalid(cscME, "CLCT0_KeyDiStrip_vs_ALCT0_KeyWiregroup", mo)))
  {
    mo->Fill(alct_keywg, clct_kewdistrip);
  }

  if (L1A_out_of_sync && CSCtype && CSCposition && isMEvalid(nodeME, "CSC_L1A_out_of_sync", mo))
  {
    mo->Fill(CSCposition, CSCtype);
  }

  if (L1A_out_of_sync && isMEvalid(nodeME, "DMB_L1A_out_of_sync", mo))
  {
    mo->Fill(crateID, dmbID);
  }

}



