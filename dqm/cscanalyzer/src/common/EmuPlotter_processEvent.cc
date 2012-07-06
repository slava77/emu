#include "emu/dqm/cscanalyzer/EmuPlotter.h"

/**
 * @fn void EmuPlotter::processEvent(const char * data, int32_t evtSize, uint32_t errorStat, int32_t nodeNumber)
 * @brief Process DDU event data buffer
 *  @param data - pointer to DDU event data buffer
 *  @param evtSize - size of event buffer in bytes
 *  @param errorStat - readout errors status from the data reader (file or Spy mode)
 *  @param nodeNumber - optional calling module node number (EmuMonitor instance in online, 0 in offline mode)
 */
void EmuPlotter::processEvent(const char * data, int32_t evtSize, uint32_t errorStat, int32_t nodeNumber, int32_t nBlocks)
{

  appBSem_.take();

  int node = 0; // Set EMU root folder

  std::string nodeTag     = "EMU";
  std::map<std::string, ME_List >::iterator itr;
  EmuMonitoringObject *mo   = NULL;  ///< pointer to MonitoringObject
  // EmuMonitoringObject *mo1   = NULL;  ///< pointer to 2nd MonitoringObject
  unpackedDMBcount    = 0;

  nEvents++;
  eTag=Form("Evt# %d: ", nEvents);

  fInterestingEvent   = false; ///< flag to mark selected event for saving into file

  ///**  Check and book global node specific histos
  if (MEs.size() == 0 || ((itr = MEs.find(nodeTag)) == MEs.end()))
  {
    LOG4CPLUS_WARN(logger_, eTag << "List of MEs for " << nodeTag << " not found. Booking...");
    MEs["EMU"] = bookMEs("EMU","EMU_Summary");
    MECanvases["EMU"] = bookMECanvases("EMU","EMU");
  }

  ME_List& nodeME = MEs[nodeTag]; ///< Global histos specific for this emuMonitor node

  // if (isMEvalid(nodeME, "Buffer_Size", mo)) mo->Fill(evtSize);

  if (isMEvalid(nodeME, "All_Readout_Data_Blocks", mo))
  {
    mo->Fill(nodeNumber, nBlocks);
  }

  ///**  Check DDU Readout Error Status
  if (isMEvalid(nodeME, "All_Readout_Errors", mo))
  {
    if (errorStat != 0)
    {
      if (debug) LOG4CPLUS_WARN(logger_,eTag << "Readout Errors: 0x" << std::hex << errorStat << " mask 0x" << dduCheckMask);
      mo->Fill(nodeNumber,1);
      for (int i=0; i<16; i++) if ((errorStat>>i) & 0x1) mo->Fill(nodeNumber,i+2);
    }
  }

  ///**  Accept or skip event according to DDU Readout Error and dduCheckMask
  if ((errorStat & dduCheckMask) > 0)
  {
    if (debug) LOG4CPLUS_WARN(logger_,eTag << "Skipped because of DDU Readout Error");
    appBSem_.give();
    return;
  }



  ///** Binary check of the buffer
  uint32_t BinaryErrorStatus = 0, BinaryWarningStatus = 0;
  LOG4CPLUS_DEBUG(logger_,eTag << "Start binary checking of buffer...");
  const uint16_t *tmp = reinterpret_cast<const uint16_t *>(data);
  bin_checker.setMask(binCheckMask);
  if ( bin_checker.check(tmp,evtSize/sizeof(short)) < 0 )
  {
    ///**  No ddu trailer found - force checker to summarize errors by adding artificial trailer
    const uint16_t dduTrailer[4] = { 0x8000, 0x8000, 0xFFFF, 0x8000 };
    tmp = dduTrailer;
    bin_checker.check(tmp,uint32_t(4));
  }


  BinaryErrorStatus   = bin_checker.errors();
  BinaryWarningStatus = bin_checker.warnings();
  LOG4CPLUS_DEBUG(logger_,nodeTag <<" Done");

  if (isMEvalid(nodeME, "All_DDUs_Format_Errors", mo))
  {
    std::vector<DDUIdType> DDUs = bin_checker.listOfDDUs();
    for (std::vector<DDUIdType>::iterator ddu_itr = DDUs.begin(); ddu_itr != DDUs.end(); ++ddu_itr)
    {
      long errs = bin_checker.errorsForDDU(*ddu_itr);
      int dduID = (*ddu_itr)&0xFF;

      ///** Trying to remap unknown DDU ID to online node number, to handle missing DDU Header errors
      if (*ddu_itr == 0xFFF)  dduID = nodeNumber;

      ///** Fix for b904 TF DDU. remap ID 760 (248) to 1
      if (dduID == 248) dduID = 1;

      std::string dduTag = Form("DDU_%02d",dduID);

      if (MEs.size() == 0 || ((itr = MEs.find(dduTag)) == MEs.end()))
      {
        LOG4CPLUS_WARN(logger_, eTag << "List of MEs for " << dduTag << " not found. Booking...");
        MEs[dduTag] = bookMEs("DDU", dduTag);
        MECanvases[dduTag] = bookMECanvases("DDU",dduTag, Form(" DDU = %02d", dduID));
        L1ANumbers[dduID] = 0;
        fFirstEvent = true;
      }

      if (errs != 0)
      {
        for (int i=0; i<bin_checker.nERRORS; i++)   // run over all errors
        {
          if ((errs>>i) & 0x1 )
          {
            mo->Fill(dduID,i+1);
          }
        }
      }
    }

    ///** Handle cases when there were no DDU errors
    if (bin_checker.errors() == 0)
    {
      int dduID = bin_checker.dduSourceID() & 0xFF;
      ///** Fix for b904 TF DDU. remap ID 760 (248) to 1
      if (dduID == 248) dduID = 1;
      std::string dduTag = Form("DDU_%02d",dduID);

      if (MEs.size() == 0 || ((itr = MEs.find(dduTag)) == MEs.end()))
      {
        LOG4CPLUS_WARN(logger_, eTag << "List of MEs for " << dduTag << " not found. Booking...");
        MEs[dduTag] = bookMEs("DDU",dduTag);
        MECanvases[dduTag] = bookMECanvases("DDU",dduTag, Form(" DDU = %02d", dduID));
        L1ANumbers[dduID] = 0;
        fFirstEvent = true;
      }
      mo->Fill(dduID,0);
    }

  }

  if (BinaryErrorStatus != 0)
  {
    if (debug) LOG4CPLUS_WARN(logger_,eTag << "Format Errors DDU level: 0x"
                                << std::hex << BinaryErrorStatus << " mask: 0x" << binCheckMask << std::dec << " evtSize:"<<  evtSize);
    ///** TODO: Interesting event selection policy
    fInterestingEvent = true;
  }
  else
  {
    LOG4CPLUS_DEBUG(logger_,eTag << "Format Errors Status is OK: 0x"
                    << std::hex << BinaryErrorStatus);
  }


  if (BinaryWarningStatus != 0)
  {
    if (debug) LOG4CPLUS_WARN(logger_,eTag << "Format Warnings DDU level: 0x"
                                << std::hex << BinaryWarningStatus);

    ///** (Ugly) Examiner warnings are reported on Readout Buffer Errors histogram
    //    to handle cases, when because of incomplete DDU header we can not identify DDU ID
    //    so we will rely on nodeNumber in online mode
    if (isMEvalid(nodeME, "All_Readout_Errors", mo))
    {
      mo->Fill(nodeNumber,1);
      if ((BinaryWarningStatus & 0x1) > 0) mo->Fill(nodeNumber,11); // Extra words between DDU Trailer and Header
      if ((BinaryWarningStatus & 0x2) > 0) mo->Fill(nodeNumber,12); // Incomplete DDU Header

    }
    ///** TODO: Interesting event selection policy
    fInterestingEvent = true;
  }
  else
  {
    LOG4CPLUS_DEBUG(logger_,eTag << "Format Warnings Status is OK: 0x"
                    << std::hex << BinaryWarningStatus);

    ///** No Readout Errors detected
    if (isMEvalid(nodeME, "All_Readout_Errors", mo) && (errorStat == 0) ) mo->Fill(nodeNumber,0);

  }



  bool EventDenied = false; ///< Accept or deny event flag

  /*
  //  Accept or deny event according to DDU Readout Error and dduCheckMask
    if (((uint32_t)errorStat & dduCheckMask) > 0)
      {
        if (debug) LOG4CPLUS_WARN(logger_,eTag << "Skipped because of DDU Readout Error");
        EventDenied = true;
      }
  */

  ///** Skip event if not masked binary errors or  binary warning is Incomplete DDU Header
  if (((BinaryErrorStatus & dduBinCheckMask)>0) || ((BinaryWarningStatus & 0x2) > 0) )
  {
    if (debug) LOG4CPLUS_WARN(logger_,eTag << "Skipped because of DDU Format Error");
    EventDenied = true;
  }

  if ((BinaryErrorStatus != 0) || (BinaryWarningStatus != 0))
  {
    nBadEvents++;
  }

  fillChamberBinCheck(node, EventDenied);

  if (EventDenied)
  {
    appBSem_.give();
    return;
  }

  nGoodEvents++;


// CSCDDUEventData::setDebug(true);

  int dduID = 0;
  CSCDDUEventData dduData((uint16_t *) data, &bin_checker);


  CSCDDUHeader dduHeader  = dduData.header();

  CSCDDUTrailer dduTrailer = dduData.trailer();
  if (!dduTrailer.check())
  {
    if (debug) LOG4CPLUS_WARN(logger_,eTag << "Skipped because of DDU Trailer check failed.");
    appBSem_.give();
    return;
  }

  dduID = dduHeader.source_id()&0xFF; /// Only 8bits are significant; format of DDU id is Dxx

  if (debug)
  {
    if ( (nodeNumber != 0) && (dduID != 248) && (dduID != nodeNumber) )
      LOG4CPLUS_WARN(logger_, eTag << "DDU ID " << dduID << " mismatch with EmuMonitor instance number" << nodeNumber);
  }


  ///** Fix for b904 TF DDU. remap ID 760 (248) to 1
  if (dduID == 248) dduID = 1;

  if (isMEvalid(nodeME, "All_DDUs_in_Readout", mo))
  {
    mo->Fill(dduID);
  }

  ///** Check and report if DDU ID is greater than 36
  if (dduID > 36)
  {
    LOG4CPLUS_WARN(logger_, "DDU ID" << dduID << " value is outside of normal ID range");
  }

  std::string dduTag = Form("DDU_%02d",dduID);

  if (MEs.size() == 0 || ((itr = MEs.find(dduTag)) == MEs.end()))
  {
    LOG4CPLUS_WARN(logger_, eTag << "List of MEs for " << dduTag << " not found. Booking...");
    MEs[dduTag] = bookMEs("DDU",dduTag);
    MECanvases[dduTag] = bookMECanvases("DDU",dduTag, Form(" DDU = %02d", dduID));
    L1ANumbers[dduID] = 0;
    fFirstEvent = true;
  }

  ME_List& dduME = MEs[dduTag];

  LOG4CPLUS_DEBUG(logger_, eTag << "Start unpacking " << dduTag);

  if (isMEvalid(dduME, "Buffer_Size", mo)) mo->Fill(evtSize);

  ///** DDU word count from trailer
  int trl_word_count = dduTrailer.wordcount();
  if (isMEvalid(dduME, "Word_Count", mo)) mo->Fill(trl_word_count );
  LOG4CPLUS_DEBUG(logger_, dduTag << " Trailer Word (64 bits) Count = " << std::dec << trl_word_count);

  if (trl_word_count > 0)
  {
    if (isMEvalid(nodeME, "All_DDUs_Event_Size", mo))
    {
      mo->Fill(dduID, log10((double)trl_word_count) );
    }
  }
  if (isMEvalid(nodeME, "All_DDUs_Average_Event_Size", mo))
  {
    mo->Fill(dduID, trl_word_count );
  }

  fCloseL1As = dduTrailer.reserved() & 0x1; // Get status if Close L1As bit
  if (fCloseL1As) LOG4CPLUS_DEBUG(logger_,eTag << " Close L1As bit is set");

  ///** DDU Header bunch crossing number (BXN)
  BXN=dduHeader.bxnum();
  if (isMEvalid(nodeME, "All_DDUs_BXNs", mo)) mo->Fill(BXN);
  if (isMEvalid(dduME, "BXN", mo)) mo->Fill(BXN);

  ///** L1A number from DDU Header
  int L1ANumber_previous_event = L1ANumbers[dduID];
  L1ANumbers[dduID] = (int)(dduHeader.lvl1num());
  L1ANumber = L1ANumbers[dduID];
  LOG4CPLUS_DEBUG(logger_,dduTag << " Header L1A Number = " << std::dec << L1ANumber);
  int L1A_inc = L1ANumber - L1ANumber_previous_event;

  ///** Handle 24-bit L1A roll-over maximum value case
  if ( L1A_inc < 0 ) L1A_inc = 0xFFFFFF + L1A_inc;

  if (fNotFirstEvent[dduID])
  {
    if (isMEvalid(dduME, "L1A_Increment", mo)) mo->Fill(L1A_inc);

    if (isMEvalid(nodeME, "All_DDUs_L1A_Increment", mo))
    {
      if      (L1A_inc > 100000)
      {
        L1A_inc = 19;
      }
      else if (L1A_inc > 30000)
      {
        L1A_inc = 18;
      }
      else if (L1A_inc > 10000)
      {
        L1A_inc = 17;
      }
      else if (L1A_inc > 3000)
      {
        L1A_inc = 16;
      }
      else if (L1A_inc > 1000)
      {
        L1A_inc = 15;
      }
      else if (L1A_inc > 300)
      {
        L1A_inc = 14;
      }
      else if (L1A_inc > 100)
      {
        L1A_inc = 13;
      }
      else if (L1A_inc > 30)
      {
        L1A_inc = 12;
      }
      else if (L1A_inc > 10)
      {
        L1A_inc = 11;
      }
      mo->Fill(dduID, L1A_inc);
    }
  }

  ///** Occupancy and number of DMB (CSC) with Data available (DAV) in header of particular DDU
  int dmb_dav_header      = 0;
  int dmb_dav_header_cnt  = 0;


  int ddu_connected_inputs= 0;
  int ddu_connected_inputs_cnt = 0;

  int csc_error_state     = 0;
  int csc_warning_state   = 0;

  int dmb_fifo_full     = 0; // DDU Input DMB FIFO Full

  ///**Number of active DMB (CSC) in header of particular DDU
  int dmb_active_header   = 0;

  dmb_dav_header     = dduHeader.dmb_dav();
  dmb_fifo_full      = dduHeader.dmb_full();
  dmb_active_header  = (int)(dduHeader.ncsc()&0xF);
  csc_error_state    = dduTrailer.dmb_full()&0x7FFF; // Only 15 inputs for DDU
  csc_warning_state  = dduTrailer.dmb_warn()&0x7FFF; // Only 15 inputs for DDU
  ddu_connected_inputs = dduHeader.live_cscs();


  LOG4CPLUS_DEBUG(logger_,dduTag << " Header DMB DAV = 0x" << std::hex << dmb_dav_header);
  LOG4CPLUS_DEBUG(logger_,dduTag << " Header Number of Active DMB = " << std::dec << dmb_active_header);


  double freq = 0;
  for (int i=0; i<15; ++i)
  {

    if ((dmb_dav_header>>i) & 0x1) dmb_dav_header_cnt++;
    if (isMEvalid(dduME, "DMB_DAV_Header_Occupancy_Rate", mo))
    {
      if ((dmb_dav_header>>i) & 0x1) mo->Fill(i+1);
      freq = (100.0*mo->GetBinContent(i+1))/nEvents;
      if (isMEvalid(dduME, "DMB_DAV_Header_Occupancy", mo)) mo->SetBinContent(i+1,freq);
    }
    if ( ((dmb_dav_header>>i) & 0x1) && isMEvalid(nodeME, "All_DDUs_Inputs_with_Data", mo))
    {
      mo->Fill(dduID, i);
    }


    if ( (ddu_connected_inputs>>i) & 0x1 ) ddu_connected_inputs_cnt++;
    if (isMEvalid(dduME, "DMB_Connected_Inputs_Rate", mo))
    {
      if ( (ddu_connected_inputs>>i) & 0x1 ) mo->Fill(i+1);
      freq = (100.0*mo->GetBinContent(i+1))/nEvents;
      if (isMEvalid(dduME, "DMB_Connected_Inputs", mo)) mo->SetBinContent(i+1, freq);
    }
    if ( ((ddu_connected_inputs>>i) & 0x1 ) && isMEvalid(nodeME, "All_DDUs_Live_Inputs", mo))
    {
      mo->Fill(dduID, i);
    }


    if (isMEvalid(dduME, "CSC_Errors_Rate", mo))
    {
      if ( (csc_error_state>>i) & 0x1 ) mo->Fill(i+1);
      freq = (100.0*mo->GetBinContent(i+1))/nEvents;
      if (isMEvalid(dduME, "CSC_Errors", mo)) mo->SetBinContent(i+1, freq);
    }
    if ( ((csc_error_state>>i) & 0x1) && isMEvalid(nodeME, "All_DDUs_Inputs_Errors", mo))
    {
      mo->Fill(dduID, i+2);
    }


    if (isMEvalid(dduME, "CSC_Warnings_Rate", mo))
    {
      if ( (csc_warning_state>>i) & 0x1 ) mo->Fill(i+1);
      freq = (100.0*mo->GetBinContent(i+1))/nEvents;
      if (isMEvalid(dduME,"CSC_Warnings", mo)) mo->SetBinContent(i+1, freq);
    }
    if ( ((csc_warning_state>>i) & 0x1 ) && isMEvalid(nodeME, "All_DDUs_Inputs_Warnings", mo))
    {
      mo->Fill(dduID, i+2);
    }


    if (isMEvalid(dduME, "CSC_DMB_FIFO_Full_Rate", mo))
    {
      if ( (dmb_fifo_full>>i) & 0x1 ) mo->Fill(i+1);
      freq = (100.0*mo->GetBinContent(i+1))/nEvents;
      if (isMEvalid(dduME, "CSC_DMB_FIFO_Full", mo)) mo->SetBinContent(i+1, freq);
    }
    if ( ((dmb_fifo_full>>i) & 0x1 ) && isMEvalid(nodeME, "All_DDUs_DMB_FIFO_Full", mo))
    {
      mo->Fill(dduID, i+2);
    }

  }

  if (isMEvalid(nodeME, "All_DDUs_Average_Live_Inputs", mo))
  {
    mo->Fill(dduID, ddu_connected_inputs_cnt);
  }

  if (isMEvalid(nodeME, "All_DDUs_Average_Inputs_with_Data", mo))
  {
    mo->Fill(dduID, dmb_dav_header_cnt);
  }

  if (isMEvalid(nodeME, "All_DDUs_Inputs_Errors", mo))
  {
    if (csc_error_state>0)
    {
      mo->Fill(dduID, 1);    // Any Input
    }
    else
    {
      mo->Fill(dduID, 0);    // No errors
    }
  }

  if (isMEvalid(nodeME, "All_DDUs_Inputs_Warnings", mo))
  {
    if (csc_warning_state>0)
    {
      mo->Fill(dduID, 1);    // Any Input
    }
    else
    {
      mo->Fill(dduID, 0);    // No warnings
    }
  }

  if (isMEvalid(nodeME, "All_DDUs_DMB_FIFO_Full", mo))
  {
    if (dmb_fifo_full>0)
    {
      mo->Fill(dduID, 1);    // Any Input
    }
    else
    {
      mo->Fill(dduID, 0);    // No warnings
    }
  }

  if (isMEvalid(dduME,"DMB_DAV_Header_Occupancy",mo)) mo->SetEntries(nEvents);

  if (isMEvalid(dduME, "DMB_Connected_Inputs", mo)) mo->SetEntries(nEvents);
  if (isMEvalid(dduME, "CSC_Errors", mo)) mo->SetEntries(nEvents);
  if (isMEvalid(dduME, "CSC_Warnings", mo)) mo->SetEntries(nEvents);

  if (isMEvalid(dduME, "DMB_Active_Header_Count", mo)) mo->Fill(dmb_active_header);
  if (isMEvalid(dduME, "DMB_DAV_Header_Count_vs_DMB_Active_Header_Count", mo)) mo->Fill(dmb_active_header,dmb_dav_header_cnt);


  ///** Check DDU Output Path status in DDU Header
  uint32_t ddu_output_path_status = dduHeader.output_path_status();
  LOG4CPLUS_DEBUG(logger_,dduTag << " Output Path Status = 0x" << std::hex << ddu_output_path_status);
  if (isMEvalid(nodeME, "All_DDUs_Output_Path_Status", mo))
  {

    EmuMonitoringObject* mo_DDU_Output_Path_Status_Rate  = 0;
    isMEvalid(dduME, "Output_Path_Status_Rate" , mo_DDU_Output_Path_Status_Rate);
    EmuMonitoringObject* mo_DDU_Output_Path_Status_Frequency  = 0;
    isMEvalid(dduME, "Output_Path_Status_Frequency" , mo_DDU_Output_Path_Status_Frequency);
    EmuMonitoringObject* mo_DDU_Output_Path_Status_Table  = 0;
    isMEvalid(dduME, "Output_Path_Status_Table" , mo_DDU_Output_Path_Status_Table);

    if (ddu_output_path_status)
    {
      mo->Fill(dduID,1); // Any Error
      for (int i=0; i<16; i++)
      {

        if ((ddu_output_path_status>>i) & 0x1)
        {
          mo->Fill(dduID,i+2); // Fill Summary Histo

          // Fill per DDU histos
          if (mo_DDU_Output_Path_Status_Rate)  mo_DDU_Output_Path_Status_Rate->Fill(i);
          if (mo_DDU_Output_Path_Status_Table) mo_DDU_Output_Path_Status_Table->Fill(0.,i);
        }

        if (mo_DDU_Output_Path_Status_Rate)
        {
          double freq = (100.0*mo_DDU_Output_Path_Status_Rate->GetBinContent(i+1))/nEvents;
          if (mo_DDU_Output_Path_Status_Frequency) mo_DDU_Output_Path_Status_Frequency->SetBinContent(i+1, freq);
        }
      }
    }
    else
    {
      mo->Fill(dduID,0); // No Errors
    }

  }

  if (isMEvalid(dduME,"Output_Path_Status_Table", mo)) mo->SetEntries(nEvents);
  if (isMEvalid(dduME,"Output_Path_Status_Frequency", mo)) mo->SetEntries(nEvents);

  ///** Check binary Error status at DDU Trailer
  uint32_t trl_errorstat = dduTrailer.errorstat();
  if (dmb_dav_header_cnt==0) trl_errorstat &= ~0x20000000; // Ignore No Good DMB CRC bit of no DMB is present
  LOG4CPLUS_DEBUG(logger_,dduTag << " Trailer Error Status = 0x" << std::hex << trl_errorstat);

  if (isMEvalid(nodeME, "All_DDUs_Trailer_Errors", mo))
  {

    EmuMonitoringObject* mo_DDU_Trailer_ErrorStat_Rate  = 0;
    isMEvalid(dduME, "Trailer_ErrorStat_Rate" , mo_DDU_Trailer_ErrorStat_Rate);
    EmuMonitoringObject* mo_DDU_Trailer_ErrorStat_Frequency  = 0;
    isMEvalid(dduME, "Trailer_ErrorStat_Frequency" , mo_DDU_Trailer_ErrorStat_Frequency);
    EmuMonitoringObject* mo_DDU_Trailer_ErrorStat_Table  = 0;
    isMEvalid(dduME, "Trailer_ErrorStat_Table" , mo_DDU_Trailer_ErrorStat_Table);

    if (trl_errorstat)
    {
      mo->Fill(dduID,1); // Any Error
      for (int i=0; i<32; i++)
      {
        if ((trl_errorstat>>i) & 0x1)
        {
          mo->Fill(dduID,i+2); // Fill Summary Histo

          // Fill per DDU histos
          if (mo_DDU_Trailer_ErrorStat_Rate)  mo_DDU_Trailer_ErrorStat_Rate->Fill(i);
          if (mo_DDU_Trailer_ErrorStat_Table) mo_DDU_Trailer_ErrorStat_Table->Fill(0.,i);

        }
        if (mo_DDU_Trailer_ErrorStat_Rate)
        {
          double freq = (100.0*mo_DDU_Trailer_ErrorStat_Rate->GetBinContent(i+1))/nEvents;
          if (mo_DDU_Trailer_ErrorStat_Frequency) mo_DDU_Trailer_ErrorStat_Frequency->SetBinContent(i+1, freq);
        }
      }
    }
    else
    {
      mo->Fill(dduID,0); // No Errors
    }

  }

  if (isMEvalid(dduME,"Trailer_ErrorStat_Table", mo)) mo->SetEntries(nEvents);
  if (isMEvalid(dduME,"Trailer_ErrorStat_Frequency", mo)) mo->SetEntries(nEvents);

  ///** Unpack all founded CSC
  std::vector<CSCEventData> chamberDatas;
  chamberDatas.clear();
  chamberDatas = dduData.cscData();

  int nCSCs = chamberDatas.size();

  for (int i=0; i< nCSCs; i++)
  {
    nCSCEvents++;
    unpackedDMBcount++;
    processChamber(chamberDatas[i], node, dduID);
  }

  if (isMEvalid(dduME,"DMB_unpacked_vs_DAV",mo)) mo->Fill(dmb_active_header, unpackedDMBcount);

  fFirstEvent = false;
  /** First event per DDU **/
  fNotFirstEvent[dduID] = true;

  appBSem_.give();

}


void EmuPlotter::fillChamberBinCheck(int32_t node, bool isEventDenied)
{

  std::string nodeTag = "EMU";
  ME_List nodeME = MEs[nodeTag];
  EmuMonitoringObject* mo = NULL;
  EmuMonitoringObject* mo1 = NULL;
  EmuMonitoringObject* mo2 = NULL;
  EmuMonitoringObject* mof = NULL;

  // == Check and fill CSC Data Flow
  std::map<CSCIdType, ExaminerStatusType> payloads = bin_checker.payloadDetailed();
  for (std::map<CSCIdType, ExaminerStatusType>::const_iterator chamber=payloads.begin(); chamber!=payloads.end(); chamber++)
  {
    int CrateID = (chamber->first>>4) & 0xFF;
    int DMBSlot = chamber->first & 0xF;
    int dduID = bin_checker.dduSourceID() & 0xFF;
    ///** Fix for b904 TF DDU. remap ID 760 (248) to 1
    if (dduID == 248) dduID = 1;

    std::string cscTag(Form("CSC_%03d_%02d", CrateID, DMBSlot));

    if (CrateID == 255)
    {
      continue;
    }

    if ( (CrateID > 60) || (DMBSlot > 10) || (CrateID <= 0) || (DMBSlot <= 0) )
    {
      if (debug) LOG4CPLUS_WARN(logger_, eTag << "Invalid CSC: " << cscTag << ". Skipping");
      if (isMEvalid(nodeME, "All_DDUs_Format_Errors", mo))
      {
        mo->Fill(dduID, 30);
      }
      continue;
    }

    int iendcap = -1;
    int istation = -1;
    // TODO: Add actual Map conversion
    uint32_t id = cscMapping.chamber(iendcap, istation, CrateID, DMBSlot, -1);
    if (id==0)
    {
      if (debug) LOG4CPLUS_ERROR(logger_, eTag << "Can not find map entry for CSC: " << cscTag << ". Skipping");
      if (isMEvalid(nodeME, "All_DDUs_Format_Errors", mo))
      {
        mo->Fill(dduID, 30);
      }
      continue;
    }
    CSCDetId cid( id );


    int CSCtype = 0;
    int CSCposition = 0;
    std::string cscName = getCSCFromMap(CrateID, DMBSlot, CSCtype, CSCposition );


    if ( fCheckMapping &&  (dduID != cscMapping.dduId(cid)) )
    {
      if (debug) LOG4CPLUS_ERROR(logger_, eTag << "DDU->CSC Mapping inconsistency: Read DDU ID" << dduID <<  ", but for detected " << cscName << " it expects DDU ID" <<  cscMapping.dduId(cid) << ". Skipping");

      if (isMEvalid(nodeME, "All_DDUs_Format_Errors", mo))
      {
        mo->Fill(dduID, 30);
      }
      continue;
    }


    std::map<std::string, ME_List >::iterator h_itr = MEs.find(cscTag);
    if (h_itr == MEs.end() || (MEs.size()==0))
    {
      LOG4CPLUS_WARN(logger_, eTag <<
                     "List of MEs for " << cscTag <<  " not found. Booking...");
      LOG4CPLUS_DEBUG(logger_,
                      "Booking Histos for " << cscTag);
      MEs[cscTag] = bookMEs("CSC",cscTag);
      MECanvases[cscTag] = bookMECanvases("CSC", cscTag, Form(" Crate ID = %02d. DMB ID = %02d", CrateID, DMBSlot));
      cscCounters[cscTag] = bookCounters();
    }
    ME_List& cscME = MEs[cscTag];

    // == Update counters
    nDMBEvents[cscTag]++;
    CSCCounters& trigCnts = cscCounters[cscTag];
    trigCnts["DMB"] = nDMBEvents[cscTag];


    long DMBEvents= nDMBEvents[cscTag];

    if (isMEvalid(nodeME, "DMB_Reporting", mo))
    {
      mo->Fill(CrateID, DMBSlot);
    }

    if ( CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Reporting", mo))
    {
      mo->Fill(CSCposition, CSCtype);
    }


    //      Get FEBs Data Available Info
    long payload = chamber->second;
    int cfeb_dav = (payload>>7) & 0x1F;
    int cfeb_active = payload & 0x1F;
    int alct_dav = (payload>>5) & 0x1;
    int tmb_dav = (payload>>6) & 0x1;
    int cfeb_dav_num=0;

    if (alct_dav==0)
    {
      if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_wo_ALCT", mo))
      {
        mo->Fill(CSCposition, CSCtype);
      }

      if (isMEvalid(nodeME, "DMB_wo_ALCT", mo))
      {
        mo->Fill(CrateID,DMBSlot);
      }
    }

    if (tmb_dav==0)
    {
      if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_wo_CLCT", mo))
      {
        mo->Fill(CSCposition, CSCtype);
      }

      if (isMEvalid(nodeME, "DMB_wo_CLCT", mo))
      {
        mo->Fill(CrateID,DMBSlot);
      }
    }

    if (cfeb_dav==0)
    {
      if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_wo_CFEB", mo))
      {
        mo->Fill(CSCposition, CSCtype);
      }

      if (isMEvalid(nodeME, "DMB_wo_CFEB", mo))
      {
        mo->Fill(CrateID,DMBSlot);
      }
    }

    if (isMEvalid(cscME, "Actual_DMB_CFEB_DAV_Rate", mo)
        && isMEvalid(cscME, "Actual_DMB_CFEB_DAV_Frequency", mof))
    {

      if (isMEvalid(cscME, "DMB_CFEB_DAV_Unpacking_Inefficiency", mo1)
          && isMEvalid(cscME, "DMB_CFEB_DAV", mo2))
      {
        for (int i=1; i<=5; i++)
        {
          double actual_dav_num = mo->GetBinContent(i);
          double unpacked_dav_num = mo2->GetBinContent(i);
          if (actual_dav_num)
          {
            mo1->SetBinContent(i,1, 100.*(1-unpacked_dav_num/actual_dav_num));
          }
          mo1->SetEntries((int)DMBEvents);
        }
      }

      for (int i=0; i<5; i++)
      {
        int cfeb_present = (cfeb_dav>>i) & 0x1;
        cfeb_dav_num += cfeb_present;
        if (cfeb_present)
        {
          mo->Fill(i);
        }
        float cfeb_entries = mo->GetBinContent(i+1);
        mof->SetBinContent(i+1, ((float)cfeb_entries/(float)(DMBEvents)*100.0));
      }

      mof->SetEntries((int)DMBEvents);

    }

    if (isMEvalid(cscME, "Actual_DMB_CFEB_DAV_multiplicity_Rate", mo)
        && isMEvalid(cscME, "Actual_DMB_CFEB_DAV_multiplicity_Frequency", mof))
    {

      for (int i=1; i<7; i++)
      {
        float cfeb_entries =  mo->GetBinContent(i);
        mof->SetBinContent(i, ((float)cfeb_entries/(float)(DMBEvents)*100.0));
      }

      mof->SetEntries((int)DMBEvents);

      if (isMEvalid(cscME, "DMB_CFEB_DAV_multiplicity_Unpacking_Inefficiency", mo1)
          && isMEvalid(cscME, "DMB_CFEB_DAV_multiplicity", mo2))
      {
        for (int i=1; i<7; i++)
        {
          float actual_dav_num = mo->GetBinContent(i);
          float unpacked_dav_num = mo2->GetBinContent(i);
          if (actual_dav_num)
          {
            mo1->SetBinContent(i,1, 100.*(1-unpacked_dav_num/actual_dav_num));
          }
          mo1->SetEntries((int)DMBEvents);
        }
      }

      mo->Fill(cfeb_dav_num);

    }


    if (isMEvalid(cscME, "DMB_CFEB_Active_vs_DAV", mo)) mo->Fill(cfeb_dav,cfeb_active);

    // == Fill Histogram for FEB DAV Efficiency
    if (isMEvalid(cscME, "Actual_DMB_FEB_DAV_Rate", mo))
    {
      if (isMEvalid(cscME, "Actual_DMB_FEB_DAV_Frequency", mo1))
      {
        for (int i=1; i<4; i++)
        {
          float dav_num = mo->GetBinContent(i);
          mo1->SetBinContent(i, ((float)dav_num/(float)(DMBEvents)*100.0));
        }
        mo1->SetEntries((int)DMBEvents);

        if (isMEvalid(cscME, "DMB_FEB_DAV_Unpacking_Inefficiency", mof)
            && isMEvalid(cscME, "DMB_FEB_DAV_Rate", mo2))
        {
          for (int i=1; i<4; i++)
          {
            float actual_dav_num = mo->GetBinContent(i);
            float unpacked_dav_num = mo2->GetBinContent(i);
            if (actual_dav_num)
            {
              mof->SetBinContent(i,1, 100.*(1-unpacked_dav_num/actual_dav_num));
            }
            mof->SetEntries((int)DMBEvents);
            mof->getObject()->SetMaximum(100.0);
          }
        }
      }

      if (alct_dav  > 0)
      {
        mo->Fill(0.0);
      }
      if (tmb_dav  > 0)
      {
        mo->Fill(1.0);
      }
      if (cfeb_dav > 0)
      {
        mo->Fill(2.0);
      }
    }


    float feb_combination_dav = -1.0;
    // == Fill Histogram for Different Combinations of FEB DAV Efficiency
    if (isMEvalid(cscME, "Actual_DMB_FEB_Combinations_DAV_Rate", mo))
    {
      if (alct_dav == 0 && tmb_dav == 0 && cfeb_dav == 0) feb_combination_dav = 0.0; // Nothing
      if (alct_dav >  0 && tmb_dav == 0 && cfeb_dav == 0) feb_combination_dav = 1.0; // ALCT Only
      if (alct_dav == 0 && tmb_dav >  0 && cfeb_dav == 0) feb_combination_dav = 2.0; // TMB Only
      if (alct_dav == 0 && tmb_dav == 0 && cfeb_dav >  0) feb_combination_dav = 3.0; // CFEB Only
      if (alct_dav == 0 && tmb_dav >  0 && cfeb_dav >  0) feb_combination_dav = 4.0; // TMB+CFEB
      if (alct_dav >  0 && tmb_dav >  0 && cfeb_dav == 0) feb_combination_dav = 5.0; // ALCT+TMB
      if (alct_dav >  0 && tmb_dav == 0 && cfeb_dav >  0) feb_combination_dav = 6.0; // ALCT+CFEB
      if (alct_dav >  0 && tmb_dav >  0 && cfeb_dav >  0) feb_combination_dav = 7.0; // ALCT+TMB+CFEB

      if (isMEvalid(cscME, "Actual_DMB_FEB_Combinations_DAV_Frequency",mo1))
      {
        for (int i=1; i<9; i++)
        {
          float feb_combination_dav_number = mo->GetBinContent(i);
          mo1->SetBinContent(i, ((float)feb_combination_dav_number/(float)(DMBEvents)*100.0));
        }
        mo1->SetEntries(DMBEvents);

        if (isMEvalid(cscME, "DMB_FEB_Combinations_DAV_Unpacking_Inefficiency", mof)
            && isMEvalid(cscME, "DMB_FEB_Combinations_DAV_Rate", mo2))
        {
          for (int i=1; i<9; i++)
          {
            float actual_dav_num = mo->GetBinContent(i);
            float unpacked_dav_num = mo2->GetBinContent(i);
            if (actual_dav_num)
            {
              mof->SetBinContent(i,1, 100.*(1-unpacked_dav_num/actual_dav_num));
            }
            mof->SetEntries((int)DMBEvents);
            mof->getObject()->SetMaximum(100.0);
          }
        }

      }
      mo->Fill(feb_combination_dav);
    }


  }


  // == Check and fill CSC Data Flow Problems
  std::map<CSCIdType, ExaminerStatusType> statuses = bin_checker.statusDetailed();
  for (std::map<CSCIdType, ExaminerStatusType>::const_iterator chamber=statuses.begin(); chamber!=statuses.end(); chamber++)
  {
    int CrateID = (chamber->first>>4) & 0xFF;
    int DMBSlot = chamber->first & 0xF;
    std::string cscTag(Form("CSC_%03d_%02d", CrateID, DMBSlot));

    if (CrateID == 255) continue;

    if ( (CrateID > 60) || (DMBSlot > 10) || (CrateID <= 0) || (DMBSlot <= 0) )
    {
      if (debug) LOG4CPLUS_WARN(logger_, eTag << "Invalid CSC: " << cscTag << ". Skipping");
      continue;
    }

    std::map<std::string, ME_List >::iterator h_itr = MEs.find(cscTag);
    if ( h_itr == MEs.end() || (MEs.size() == 0) )
    {

      LOG4CPLUS_WARN(logger_, eTag <<
                     "List of MEs for " << cscTag <<  " not found. Booking...");
      LOG4CPLUS_DEBUG(logger_,
                      "Booking Histos for " << cscTag);


      MEs[cscTag]     = bookMEs("CSC",cscTag);
      MECanvases[cscTag]  = bookMECanvases("CSC", cscTag, Form(" Crate ID = %02d. DMB ID = %02d", CrateID, DMBSlot));
      cscCounters[cscTag]   = bookCounters();

    }

    ME_List& cscME = MEs[cscTag];

    int CSCtype     = 0;
    int CSCposition   = 0;
    getCSCFromMap(CrateID, DMBSlot, CSCtype, CSCposition );

    if (isMEvalid(cscME, "BinCheck_DataFlow_Problems_Table", mo))
    {
      for (int bit=0; bit<bin_checker.nSTATUSES; bit++)
        if ( chamber->second & (1<<bit) )
        {
          mo->Fill(0.,bit);
        }
      mo->SetEntries(nDMBEvents[cscTag]);
    }


    int anyInputFull = chamber->second & 0x3F;
    if (anyInputFull)
    {
      if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_DMB_input_fifo_full", mo))
      {
        mo->Fill(CSCposition, CSCtype);
      }
      if (isMEvalid(nodeME, "DMB_input_fifo_full", mo))
      {
        mo->Fill(CrateID, DMBSlot);
      }
    }


    int anyInputTO = (chamber->second >> 7) & 0x3FFF;
    if (anyInputTO)
    {
      if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_DMB_input_timeout", mo))
      {
        mo->Fill(CSCposition, CSCtype);
      }
      if (isMEvalid(nodeME, "DMB_input_timeout", mo))
      {
        mo->Fill(CrateID, DMBSlot);
      }
    }


    // == CFEB B-Word
    if (chamber->second & (1<<22))
    {

      if (isMEvalid(nodeME, "DMB_Format_Warnings", mo))
      {
        mo->Fill(CrateID, DMBSlot);
      }

      if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Format_Warnings", mo))
      {
        mo->Fill(CSCposition, CSCtype);
      }

    }
  }


  // == Check and fill CSC Format Errors
  std::map<CSCIdType, ExaminerStatusType> checkerErrors = bin_checker.errorsDetailed();
  for (std::map<CSCIdType, ExaminerStatusType>::const_iterator chamber=checkerErrors.begin(); chamber!=checkerErrors.end(); chamber++)
  {
    int CrateID = (chamber->first>>4) & 0xFF;
    int DMBSlot = chamber->first & 0xF;

    std::string cscTag(Form("CSC_%03d_%02d", CrateID , DMBSlot));
    std::map<std::string, ME_List >::iterator h_itr = MEs.find(cscTag);

    if ((CrateID == 255) ||
        (chamber->second & 0x80)) continue;    // = Skip chamber detection if DMB header is missing (Error code 6)

    if ( (CrateID > 60) || (DMBSlot > 10) || (CrateID <= 0) || (DMBSlot <= 0) )
    {
      if (debug) LOG4CPLUS_WARN(logger_, eTag << "Invalid CSC: " << cscTag << ". Skipping");
      continue;
    }

    if (h_itr == MEs.end() || (MEs.size()==0))
    {
      LOG4CPLUS_WARN(logger_, eTag <<
                     "List of MEs for " << cscTag <<  " not found. Booking...");
      LOG4CPLUS_DEBUG(logger_,
                      "Booking Histos for " << cscTag);
      MEs[cscTag] = bookMEs("CSC", cscTag);
      MECanvases[cscTag] = bookMECanvases("CSC", cscTag, Form(" Crate ID = %02d. DMB ID = %02d", CrateID, DMBSlot));
      cscCounters[cscTag] = bookCounters();

      // printMECollection(MEs[cscTag]);
    }
    ME_List& cscME = MEs[cscTag];

    if ((chamber->second & binCheckMask) != 0)
    {
      //  nDMBEvents[cscTag]++;
      CSCCounters& trigCnts = cscCounters[cscTag];
      trigCnts["BAD"]++;
    }

    bool isCSCError = false;

    if (isMEvalid(cscME, "BinCheck_ErrorStat_Table", mo))
    {
      for (int bit=5; bit<24; bit++)
        if ( chamber->second & (1<<bit) )
        {
          isCSCError = true;
          mo->Fill(0.,bit-5);

        }
      mo->SetEntries(nDMBEvents[cscTag]);
    }

    if (isCSCError)
    {

      if (debug) LOG4CPLUS_WARN(logger_,eTag << "Format Errors "<< cscTag << ": 0x" << std::hex << chamber->second);

      if (isMEvalid(nodeME, "DMB_Format_Errors", mo))
      {
        mo->Fill(CrateID, DMBSlot);
      }

      if (!isEventDenied  && isMEvalid(nodeME, "DMB_Unpacked_with_errors", mo))
      {
        mo->Fill(CrateID, DMBSlot);
      }

      int CSCtype   = 0;
      int CSCposition = 0;
      getCSCFromMap(CrateID, DMBSlot, CSCtype, CSCposition );
      if ( CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Format_Errors", mo))
      {
        mo->Fill(CSCposition, CSCtype);
      }

      if (!isEventDenied  && CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Unpacked_with_errors", mo))
      {
        mo->Fill(CSCposition, CSCtype);
      }
    }

  }
}

void EmuPlotter::updateCSCHistos()
{
  std::map<std::string, ME_List>::iterator itr;
  for (itr = MEs.begin(); itr != MEs.end(); ++itr)
  {
    if (itr->first.find("CSC_") != std::string::npos)
    {
      updateCSCFractionHistos(itr->first);
    }
  }

}

void EmuPlotter::updateCSCFractionHistos(std::string cscTag)
{
  std::vector<std::string> csc_list;
  csc_list.clear();
  if (cscTag == "")
  {
    csc_list = getListOfFolders("CSC");
  }
  else
  {
    csc_list.push_back(cscTag);
  }

  for (std::vector<std::string>::iterator itr = csc_list.begin(); itr != csc_list.end(); ++itr)
  {
    std::string cscName = *itr;
    EmuMonitoringObject *mo = NULL;
    EmuMonitoringObject *mof = NULL;
    ME_List& cscME = MEs[cscName];


//    if (nDMBEvents[cscName] != 0)
    {
      if (isMEvalid(cscME, "BinCheck_DataFlow_Problems_Table", mo)
          && isMEvalid(cscME, "BinCheck_DataFlow_Problems_Frequency", mof))
      {
        double entries = mo->getObject()->GetEntries();
        if (entries > 0) {
          mof->getObject()->Reset();
          mof->getObject()->Add(mo->getObject());
          // mof->getObject()->Scale(1./(nDMBEvents[cscName]));
          mof->getObject()->Scale(1./entries);
          mof->getObject()->SetMaximum(1.);
          mof->SetEntries(entries);
          // mof->SetEntries(nDMBEvents[cscName]);
          // mo->SetEntries(nDMBEvents[cscName]);
        }

      }

      if (isMEvalid(cscME, "BinCheck_ErrorStat_Table", mo)
          && isMEvalid(cscME, "BinCheck_Errors_Frequency", mof))
      {
        double entries = mo->getObject()->GetEntries();
        if (entries > 0) {
          mof->getObject()->Reset();
          mof->getObject()->Add(mo->getObject());
          // mof->getObject()->Scale(1./(nDMBEvents[cscName]));
          mof->getObject()->Scale(1./entries);
          mof->getObject()->SetMaximum(1.);
          mof->SetEntries(entries);
          // mof->SetEntries(nDMBEvents[cscName]);
          // mo->SetEntries(nDMBEvents[cscName]);
        }
      }
    }
  }
}

/*
  After the histograms have been collected this function
  updates fraction histograms.
*/
void EmuPlotter::updateFractionHistos()
{
  std::string nodeTag = "EMU";
  ME_List nodeME; // === Global histos
  nodeME = MEs[nodeTag];

  // ************************************
  // Calculating Fractions
  // ************************************

  calcFractionHisto(nodeME, "DMB_Format_Errors_Fract", "DMB_Reporting", "DMB_Format_Errors");
  calcFractionHisto(nodeME, "CSC_Format_Errors_Fract", "CSC_Reporting", "CSC_Format_Errors");
  calcFractionHisto(nodeME, "DMB_Format_Warnings_Fract", "DMB_Reporting", "DMB_Format_Warnings");
  calcFractionHisto(nodeME, "CSC_Format_Warnings_Fract", "CSC_Reporting", "CSC_Format_Warnings");
  calcFractionHisto(nodeME, "DMB_Unpacked_Fract", "DMB_Reporting", "DMB_Unpacked");
  calcFractionHisto(nodeME, "CSC_Unpacked_Fract", "CSC_Reporting", "CSC_Unpacked");
  calcFractionHisto(nodeME, "DMB_wo_ALCT_Fract", "DMB_Reporting", "DMB_wo_ALCT");
  calcFractionHisto(nodeME, "CSC_wo_ALCT_Fract", "CSC_Reporting", "CSC_wo_ALCT");
  calcFractionHisto(nodeME, "DMB_wo_CLCT_Fract", "DMB_Reporting", "DMB_wo_CLCT");
  calcFractionHisto(nodeME, "CSC_wo_CLCT_Fract", "CSC_Reporting", "CSC_wo_CLCT");
  calcFractionHisto(nodeME, "DMB_wo_CFEB_Fract", "DMB_Reporting", "DMB_wo_CFEB");
  calcFractionHisto(nodeME, "CSC_wo_CFEB_Fract", "CSC_Reporting", "CSC_wo_CFEB");
  calcFractionHisto(nodeME, "CSC_DMB_input_fifo_full_Fract", "CSC_Reporting", "CSC_DMB_input_fifo_full");
  calcFractionHisto(nodeME, "DMB_input_fifo_full_Fract", "DMB_Reporting", "DMB_input_fifo_full");
  calcFractionHisto(nodeME, "CSC_DMB_input_timeout_Fract", "CSC_Reporting", "CSC_DMB_input_timeout");
  calcFractionHisto(nodeME, "DMB_input_timeout_Fract", "DMB_Reporting", "DMB_input_timeout");
  calcFractionHisto(nodeME, "CSC_L1A_out_of_sync_Fract", "CSC_Reporting", "CSC_L1A_out_of_sync");
  calcFractionHisto(nodeME, "DMB_L1A_out_of_sync_Fract", "DMB_Reporting", "DMB_L1A_out_of_sync");

}

/*
  After the histograms have been collected this function
  updates detector summary efficiency histograms.
*/
void EmuPlotter::updateEfficiencyHistos()
{
  std::string nodeTag = "EMU";
  ME_List nodeME; // === Global histos
  nodeME = MEs[nodeTag];

  EmuMonitoringObject *mo = NULL;
  EmuMonitoringObject *mo1 = NULL;
  //
  // Set detector information
  //

  summary.Reset();

  if (isMEvalid(nodeME, "CSC_Reporting", mo))
  {

    TH2* rep = dynamic_cast<TH2*>(mo->getObject());
    summary.ReadReportingChambers((const TH2*&)rep, 1.0);

    if (isMEvalid(nodeME, "CSC_Format_Errors", mo1))
    {
      TH2* err = dynamic_cast<TH2*>(mo1->getObject());
      summary.ReadErrorChambers((const TH2*&)rep, (const TH2*&)err, cscdqm::FORMAT_ERR, 0.1, 5.0);
    }

    if (isMEvalid(nodeME, "CSC_L1A_out_of_sync", mo1))
    {
      TH2* err = dynamic_cast<TH2*>(mo1->getObject());
      summary.ReadErrorChambers((const TH2*&)rep, (const TH2*&)err, cscdqm::L1SYNC_ERR, 0.1, 5.0);
    }

    if (isMEvalid(nodeME, "CSC_DMB_input_fifo_full", mo1))
    {
      TH2* err = dynamic_cast<TH2*>(mo1->getObject());
      summary.ReadErrorChambers((const TH2*&)rep, (const TH2*&)err, cscdqm::FIFOFULL_ERR, 0.1, 5.0);
    }

    if (isMEvalid(nodeME, "CSC_DMB_input_timeout", mo1))
    {
      TH2* err = dynamic_cast<TH2*>(mo1->getObject());
      summary.ReadErrorChambers((const TH2*&)rep, (const TH2*&)err, cscdqm::INPUTTO_ERR, 0.1, 5.0);
    }

  }

  //
  // Write summary information
  //


  if (isMEvalid(nodeME, "Physics_ME1", mo))
  {
    TH2* tmp = dynamic_cast<TH2*>(mo->getObject());
    summary.Write(tmp, 1);
  }

  if (isMEvalid(nodeME, "Physics_ME2", mo))
  {
    TH2* tmp = dynamic_cast<TH2*>(mo->getObject());
    summary.Write(tmp, 2);
  }

  if (isMEvalid(nodeME, "Physics_ME3", mo))
  {
    TH2* tmp = dynamic_cast<TH2*>(mo->getObject());
    summary.Write(tmp, 3);
  }

  if (isMEvalid(nodeME, "Physics_ME4", mo))
  {
    TH2* tmp = dynamic_cast<TH2*>(mo->getObject());
    summary.Write(tmp, 4);
  }


  if (isMEvalid(nodeME, "Physics_EMU", mo))
  {
    TH2* tmp=dynamic_cast<TH2*>(mo->getObject());
    summary.WriteMap(tmp);
  }

}

/*
  Calculate fractional histogram:
  MEs - a list where to take histograms from
  resultHistoName - name of the resulting histogram (to be computed)
  setHistoName - the whole set histogram (data)
  subSetHistoName - the subset of the whole set (setHistoName)

  resultHistoName is being computed by dividing subSetHistoName from setHistoName, i.e.
  fraction of errors = errors / records
*/
void EmuPlotter::calcFractionHisto(
  ME_List MEs,
  std::string resultHistoName,
  std::string setHistoName,
  std::string subSetHistoName)
{

  EmuMonitoringObject *mo = NULL;
  EmuMonitoringObject *mo1 = NULL;
  EmuMonitoringObject *mo2 = NULL;

  if (isMEvalid(MEs, resultHistoName, mo)
      && isMEvalid(MEs, setHistoName, mo2)
      && isMEvalid(MEs, subSetHistoName, mo1))
  {
    mo->getObject()->Reset();
    mo->getObject()->Divide(mo1->getObject(), mo2->getObject());
    mo->getObject()->SetMaximum(1.);
  }

}

