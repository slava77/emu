#include "EmuPlotter.h"

// == Prints four 16-bits words in Hex 
void printb(unsigned short* buf)
{
        for (int i=0; i<4; i++)
        cout << " " << setw(4)<< setfill('0') << hex << buf[i];
        cout << dec << std::endl;
};

void EmuPlotter::processEvent(const char * data, int32_t evtSize, uint32_t errorStat, int32_t nodeNumber)
{
  //	LOG4CPLUS_INFO(logger_ , "processing event data");
 

  int node = 0; // Set EMU root folder
  //   int node = nodeNumber;
  //  std::string nodeTag = Form("EMU_%d",node); // == This emuMonitor node number

  std::string nodeTag = "EMU";
  std::map<std::string, ME_List >::iterator itr;
  EmuMonitoringObject *mo = NULL;  // == pointer to MonitoringObject
  unpackedDMBcount = 0; 
 
  nEvents++;
  eTag=Form("Evt# %d: ", nEvents); 
/*
  // == Check and book global node specific histos
  if (MEs.size() == 0 || ((itr = MEs.find(nodeTag)) == MEs.end())) {
    LOG4CPLUS_WARN(logger_, eTag << "List of MEs for " << nodeTag << " not found. Booking...");
    fBusy = true;
    MEs[nodeTag] = bookCommon(node);
    MECanvases[nodeTag] = bookCommonCanvases(node);
    // printMECollection(MEs[nodeTag]);
    fBusy = false;
  }
*/	
  ME_List& nodeME = MEs[nodeTag]; // === Global histos specific for this emuMonitor node

  

  // if (isMEvalid(nodeME, "Buffer_Size", mo)) mo->Fill(evtSize);

  // ==     Check DDU Readout Error Status
  /*
    if (isMEvalid(nodeME, "Readout_Errors", mo)) { 
    if(errorStat != 0) {
    LOG4CPLUS_WARN(logger_,eTag << "Non-zero Readout Error Status is observed: 0x" << std::hex << errorStat << " mask 0x" << dduCheckMask);
    for (int i=0; i<16; i++) if ((errorStat>>i) & 0x1) mo->Fill(0.,i);
    }
    else {
    LOG4CPLUS_DEBUG(logger_,eTag << "Readout Error Status is OK: 0x" << std::hex << errorStat);
    }
    }
  */
  if (isMEvalid(nodeME, "All_Readout_Errors", mo)) {
    if(errorStat != 0) {      
      LOG4CPLUS_WARN(logger_,eTag << "Readout Errors: 0x" << std::hex << errorStat << " mask 0x" << dduCheckMask);
      mo->Fill(nodeNumber,1);
      for (int i=0; i<16; i++) if ((errorStat>>i) & 0x1) mo->Fill(nodeNumber,i+2);
    } else {
      mo->Fill(nodeNumber,0);
    }
  }



  //	Binary check of the buffer
  uint32_t BinaryErrorStatus = 0, BinaryWarningStatus = 0;
  LOG4CPLUS_DEBUG(logger_,eTag << "Start binary checking of buffer...");
  const uint16_t *tmp = reinterpret_cast<const uint16_t *>(data);
  bin_checker.setMask(binCheckMask);
  if( bin_checker.check(tmp,evtSize/sizeof(short)) < 0 ){
    //   No ddu trailer found - force checker to summarize errors by adding artificial trailer
    const uint16_t dduTrailer[4] = { 0x8000, 0x8000, 0xFFFF, 0x8000 };
    tmp = dduTrailer; bin_checker.check(tmp,uint32_t(4));
  }


  BinaryErrorStatus   = bin_checker.errors();
  BinaryWarningStatus = bin_checker.warnings();
  LOG4CPLUS_DEBUG(logger_,nodeTag <<" Done");

  if (isMEvalid(nodeME, "All_DDUs_Format_Errors", mo)) {
    std::vector<int> DDUs = bin_checker.listOfDDUs();
    for (std::vector<int>::iterator ddu_itr = DDUs.begin(); ddu_itr != DDUs.end(); ++ddu_itr) {
      if (*ddu_itr != 0xFFF) {
        long errs = bin_checker.errorsForDDU(*ddu_itr);
        int dduID = (*ddu_itr)&0xFF;

	std::string dduTag = Form("DDU_%02d",dduID);

        if (MEs.size() == 0 || ((itr = MEs.find(dduTag)) == MEs.end())) {
	  LOG4CPLUS_WARN(logger_, eTag << "List of MEs for " << dduTag << " not found. Booking...");
	  fBusy = true;
	  MEs[dduTag] = bookMEs("DDU", dduTag);
	  MECanvases[dduTag] = bookMECanvases("DDU",dduTag, Form(" DDU = %02d", dduID));
	  // printMECollection(MEs[dduTag]);
	  fBusy = false;
	  L1ANumbers[dduID] = 0;
	  fFirstEvent = true;
  	}


        if (errs != 0) {
          for(int i=0; i<bin_checker.nERRORS; i++) { // run over all errors
            if ((errs>>i) & 0x1 ) mo->Fill(dduID,i+1);
          }
        } else {
          // mo->Fill(dduID,0);
        }
      }
    }

    /* Temporary tweak for cases when there were no DDU errors  */
    if (bin_checker.errors() == 0) {
      int dduID = bin_checker.dduSourceID() & 0xFF;
      std::string dduTag = Form("DDU_%02d",dduID);

      if (MEs.size() == 0 || ((itr = MEs.find(dduTag)) == MEs.end())) {
	LOG4CPLUS_WARN(logger_, eTag << "List of MEs for " << dduTag << " not found. Booking...");
	fBusy = true;
	MEs[dduTag] = bookMEs("DDU",dduTag);
	MECanvases[dduTag] = bookMECanvases("DDU",dduTag, Form(" DDU = %02d", dduID));
	// printMECollection(MEs[dduTag]);
	fBusy = false;
	L1ANumbers[dduID] = 0;
	fFirstEvent = true;
      }
      mo->Fill(dduID,0);
    }

  }


  if(BinaryErrorStatus != 0) {
    LOG4CPLUS_WARN(logger_,eTag << "Format Errors DDU level: 0x" << std::hex << BinaryErrorStatus << " mask: 0x" << binCheckMask << std::dec << " evtSize:"<<  evtSize);
    /*
      if (isMEvalid(nodeME, "BinaryChecker_Errors", mo)) {
      for(int i=0; i<bin_checker.nERRORS; i++) { // run over all errors
      if( bin_checker.error(i) ) mo->Fill(0.,i);
      }
      }
    */

  }
  else {
    LOG4CPLUS_DEBUG(logger_,eTag << "Format Errors Status is OK: 0x" << std::hex << BinaryErrorStatus);
  }


  if(BinaryWarningStatus != 0) {
    LOG4CPLUS_WARN(logger_,eTag << "Format Warnings DDU level: 0x" 
		   << std::hex << BinaryWarningStatus)
      /*
	if (isMEvalid(nodeME, "BinaryChecker_Warnings", mo)) {
	for(int i=0; i<bin_checker.nWARNINGS; i++) { // run over all warnings
	if( bin_checker.warning(i) ) mo->Fill(0.,i);
	}
	}
      */

      }
  else {
    LOG4CPLUS_DEBUG(logger_,eTag << "Format Warnings Status is OK: 0x" << std::hex << BinaryWarningStatus);

  }

  //	Accept or deny event
  bool EventDenied = false;
  //	Accept or deny event according to DDU Readout Error and dduCheckMask
  if (((uint32_t)errorStat & dduCheckMask) > 0) {
    LOG4CPLUS_WARN(logger_,eTag << "Skipped because of DDU Readout Error");
    EventDenied = true;
  }

  // if ((BinaryErrorStatus & binCheckMask)>0) {
  if ((BinaryErrorStatus & dduBinCheckMask)>0) {
    LOG4CPLUS_WARN(logger_,eTag << "Skipped because of DDU Format Error");
    EventDenied = true;
  }

  if ((BinaryErrorStatus != 0) || (BinaryWarningStatus != 0)) {
    nBadEvents++;
//    fillChamberBinCheck(node, EventDenied);
  }

  fillChamberBinCheck(node, EventDenied);

  if(EventDenied) return;
  else LOG4CPLUS_DEBUG(logger_,eTag << "is accepted");

  nGoodEvents++;


  // CSCDDUEventData::setDebug(true);
  int dduID = 0;
  CSCDDUEventData dduData((uint16_t *) data, &bin_checker);
  // CSCDDUEventData dduData((uint16_t *) data);
  
 
  CSCDDUHeader dduHeader  = dduData.header();
/*
  if (!dduHeader.check()) {
         LOG4CPLUS_WARN(logger_,eTag << "Skipped because of DDU Header check failed.");
         return;
  }
*/
  // printb(dduHeader.data());
  // printb(dduHeader.data()+4);

  CSCDDUTrailer dduTrailer = dduData.trailer();
  if (!dduTrailer.check()) {
	 LOG4CPLUS_WARN(logger_,eTag << "Skipped because of DDU Trailer check failed.");
	 return;
  }


  dduID = dduHeader.source_id()&0xFF; // Only 8bits are significant; format of DDU id is Dxx



  if (isMEvalid(nodeME, "All_DDUs_in_Readout", mo)) {
    mo->Fill(dduID);
  }



  std::string dduTag = Form("DDU_%02d",dduID);

  if (MEs.size() == 0 || ((itr = MEs.find(dduTag)) == MEs.end())) {
    LOG4CPLUS_WARN(logger_, eTag << "List of MEs for " << dduTag << " not found. Booking...");
    fBusy = true;
    MEs[dduTag] = bookMEs("DDU",dduTag);
    MECanvases[dduTag] = bookMECanvases("DDU",dduTag, Form(" DDU = %02d", dduID));
    // printMECollection(MEs[dduTag]);
    fBusy = false;
    L1ANumbers[dduID] = 0;
    //  L1ANumbers[dduID] = (int)(dduHeader.lvl1num());
    fFirstEvent = true;
  }

  ME_List& dduME = MEs[dduTag];

  LOG4CPLUS_DEBUG(logger_,eTag << "Start unpacking " << dduTag);

  if (isMEvalid(dduME, "Buffer_Size", mo)) mo->Fill(evtSize);
  // ==     DDU word counter
  int trl_word_count = 0;
  trl_word_count = dduTrailer.wordcount();
  if (isMEvalid(dduME, "Word_Count", mo)) mo->Fill(trl_word_count );
  LOG4CPLUS_DEBUG(logger_,dduTag << " Trailer Word (64 bits) Count = " << std::dec << trl_word_count);
  if (trl_word_count > 0) { 
    if (isMEvalid(nodeME, "All_DDUs_Event_Size", mo)) {
      mo->Fill(dduID, log10((double)trl_word_count) );
    }
  }
  if (isMEvalid(nodeME, "All_DDUs_Average_Event_Size", mo)) {
    mo->Fill(dduID, trl_word_count );
  }

  fCloseL1As = dduTrailer.reserved() & 0x1; // Get status if Close L1As bit
  if (fCloseL1As) LOG4CPLUS_DEBUG(logger_,eTag << " Close L1As bit is set");

  // ==     DDU Header bunch crossing number (BXN)
  BXN=dduHeader.bxnum();
  // LOG4CPLUS_WARN(logger_,dduTag << " DDU Header BXN Number = " << std::dec << BXN);
  if (isMEvalid(dduME, "BXN", mo)) mo->Fill(BXN);

  // ==     L1A number from DDU Header
  int L1ANumber_previous_event = L1ANumbers[dduID];
  L1ANumbers[dduID] = (int)(dduHeader.lvl1num());
  L1ANumber = L1ANumbers[dduID];
  LOG4CPLUS_DEBUG(logger_,dduTag << " Header L1A Number = " << std::dec << L1ANumber);
  int L1A_inc = L1ANumber - L1ANumber_previous_event;
  if (!fFirstEvent) {
    if (isMEvalid(dduME, "L1A_Increment", mo)) mo->Fill(L1A_inc);
  
    if (isMEvalid(nodeME, "All_DDUs_L1A_Increment", mo)) {
      if      (L1A_inc > 100000){ L1A_inc = 19;}
      else if (L1A_inc > 30000) { L1A_inc = 18;}
      else if (L1A_inc > 10000) { L1A_inc = 17;}
      else if (L1A_inc > 3000)  { L1A_inc = 16;}
      else if (L1A_inc > 1000)  { L1A_inc = 15;}
      else if (L1A_inc > 300)   { L1A_inc = 14;}
      else if (L1A_inc > 100)   { L1A_inc = 13;}
      else if (L1A_inc > 30)    { L1A_inc = 12;}
      else if (L1A_inc > 10)    { L1A_inc = 11;}
      mo->Fill(dduID, L1A_inc);
    }
  }

  // ==     Occupancy and number of DMB (CSC) with Data available (DAV) in header of particular DDU
  int dmb_dav_header      = 0;
  int dmb_dav_header_cnt  = 0;

  int ddu_connected_inputs= 0;
  int ddu_connected_inputs_cnt = 0;

  int csc_error_state     = 0;
  int csc_warning_state   = 0;

  //  ==    Number of active DMB (CSC) in header of particular DDU
  int dmb_active_header   = 0;

  dmb_dav_header     = dduHeader.dmb_dav();
  dmb_active_header  = (int)(dduHeader.ncsc()&0xF);
  csc_error_state    = dduTrailer.dmb_full()&0x7FFF; // Only 15 inputs for DDU
  csc_warning_state  = dduTrailer.dmb_warn()&0x7FFF; // Only 15 inputs for DDU
  ddu_connected_inputs=dduHeader.live_cscs();


  LOG4CPLUS_DEBUG(logger_,dduTag << " Header DMB DAV = 0x" << std::hex << dmb_dav_header);
  LOG4CPLUS_DEBUG(logger_,dduTag << " Header Number of Active DMB = " << std::dec << dmb_active_header);


  double freq = 0;
  for (int i=0; i<15; ++i) {
    if ((dmb_dav_header>>i) & 0x1) {
      dmb_dav_header_cnt++;      
      if (isMEvalid(dduME, "DMB_DAV_Header_Occupancy_Rate", mo)) {
	mo->Fill(i+1);
	freq = (100.0*mo->GetBinContent(i+1))/nEvents;
        if (isMEvalid(dduME, "DMB_DAV_Header_Occupancy", mo)) mo->SetBinContent(i+1,freq);
      }
      if (isMEvalid(nodeME, "All_DDUs_Inputs_with_Data", mo)) {
        mo->Fill(dduID, i);
      }
    }

    if( (ddu_connected_inputs>>i) & 0x1 ){
      ddu_connected_inputs_cnt++;
      if (isMEvalid(dduME, "DMB_Connected_Inputs_Rate", mo)) {
	mo->Fill(i+1);
	freq = (100.0*mo->GetBinContent(i+1))/nEvents;
	if (isMEvalid(dduME, "DMB_Connected_Inputs", mo)) mo->SetBinContent(i+1, freq);
      }
      if (isMEvalid(nodeME, "All_DDUs_Live_Inputs", mo)) {
        mo->Fill(dduID, i);
      }
    }

    if( (csc_error_state>>i) & 0x1 ){
      if (isMEvalid(dduME, "CSC_Errors_Rate", mo)) {
	mo->Fill(i+1);
	freq = (100.0*mo->GetBinContent(i+1))/nEvents;
	if (isMEvalid(dduME, "CSC_Errors", mo)) mo->SetBinContent(i+1, freq);
      }
      if (isMEvalid(nodeME, "All_DDUs_Inputs_Errors", mo)) {
        mo->Fill(dduID, i+2);
      }

    }
	
    if( (csc_warning_state>>i) & 0x1 ){
      if (isMEvalid(dduME, "CSC_Warnings_Rate", mo)) {
	mo->Fill(i+1);
	freq = (100.0*mo->GetBinContent(i+1))/nEvents;
	if (isMEvalid(dduME,"CSC_Warnings", mo)) mo->SetBinContent(i+1, freq);
      }
      if (isMEvalid(nodeME, "All_DDUs_Inputs_Warnings", mo)) {
        mo->Fill(dduID, i+2);
      }
    }
  }

  if (isMEvalid(nodeME, "All_DDUs_Average_Live_Inputs", mo)) {
    mo->Fill(dduID, ddu_connected_inputs_cnt);
  }

  if (isMEvalid(nodeME, "All_DDUs_Average_Inputs_with_Data", mo)) {
    mo->Fill(dduID, dmb_dav_header_cnt);
  }

  if (isMEvalid(nodeME, "All_DDUs_Inputs_Errors", mo)) {
    if (csc_error_state>0) { mo->Fill(dduID, 1);} // Any Input
    else { mo->Fill(dduID, 0);} // No errors
  }

  if (isMEvalid(nodeME, "All_DDUs_Inputs_Warnings", mo)) {
    if (csc_warning_state>0) { mo->Fill(dduID, 1);} // Any Input
    else { mo->Fill(dduID, 0);} // No warnings
  }

  if (isMEvalid(dduME,"DMB_DAV_Header_Occupancy",mo)) mo->SetEntries(nEvents);

  if (isMEvalid(dduME, "DMB_Connected_Inputs", mo)) mo->SetEntries(nEvents);
  if (isMEvalid(dduME, "CSC_Errors", mo)) mo->SetEntries(nEvents);
  if (isMEvalid(dduME, "CSC_Warnings", mo)) mo->SetEntries(nEvents);

  if (isMEvalid(dduME, "DMB_Active_Header_Count", mo)) mo->Fill(dmb_active_header);
  if (isMEvalid(dduME, "DMB_DAV_Header_Count_vs_DMB_Active_Header_Count", mo)) mo->Fill(dmb_active_header,dmb_dav_header_cnt);


  // ==     Check binary Error status at DDU Trailer
  uint32_t trl_errorstat = dduTrailer.errorstat();
  if (dmb_dav_header_cnt==0) trl_errorstat &= ~0x20000000; // Ignore No Good DMB CRC bit of no DMB is present
  LOG4CPLUS_DEBUG(logger_,dduTag << " Trailer Error Status = 0x" << std::hex << trl_errorstat);
  for (int i=0; i<32; i++) {
    if ((trl_errorstat>>i) & 0x1) {
      if (isMEvalid(dduME,"Trailer_ErrorStat_Rate", mo)) { 
	mo->Fill(i);
	double freq = (100.0*mo->GetBinContent(i+1))/nEvents;
	if (isMEvalid(dduME, "Trailer_ErrorStat_Frequency", mo)) mo->SetBinContent(i+1, freq);
      }
      if (isMEvalid(dduME, "Trailer_ErrorStat_Table", mo)) mo->Fill(0.,i);
    }
  }
  if (isMEvalid(nodeME, "All_DDUs_Trailer_Errors", mo)) {
    if (trl_errorstat) {
      mo->Fill(dduID,1); // Any Error
      for (int i=0; i<32; i++) {	
	if ((trl_errorstat>>i) & 0x1) {
	  mo->Fill(dduID,i+2);
	}
      }
    } else {
      mo->Fill(dduID,0); // No Errors
    }
  }
	
  if (isMEvalid(dduME,"Trailer_ErrorStat_Table", mo)) mo->SetEntries(nEvents);
  if (isMEvalid(dduME,"Trailer_ErrorStat_Frequency", mo)) mo->SetEntries(nEvents);

  //      Unpack all founded CSC
  std::vector<CSCEventData> chamberDatas;
  chamberDatas.clear();
  chamberDatas = dduData.cscData();


/*
  std::map<short,std::map<short,unsigned long> > ddus =  bin_checker.DMB_ptrOffsets();
  std::map<short,std::map<short,unsigned long> >::iterator ddu_itr;

  for ( ddu_itr=ddus.begin(); ddu_itr != ddus.end(); ++ddu_itr) {
//	int dduID=(ddu_itr->first&0xFF);
	std::string dduTag = Form("DDU_%d",(ddu_itr->first&0xFF));
	std::map<short,unsigned long> & cscs = ddu_itr->second;
	std::map<short,unsigned long>::iterator csc_itr;
	for (csc_itr=cscs.begin(); csc_itr != cscs.end(); ++csc_itr) {
		short cscid = csc_itr->first;
                std::string cscTag(Form("CSC_%03d_%02d", (cscid>>4) & 0xFF , cscid & 0xF));
		unsigned long offset = csc_itr->second; 
		long errors = bin_checker.errorsForChamber(cscid);
		if ((errors & binCheckMask) > 0 ) {
			LOG4CPLUS_WARN(logger_,eTag  // << " offset: " << offset
                << "Format Errors " << cscTag << ": 0x" << hex << errors << " Skipped CSC Unpacking");
			continue;
		} else {
		// LOG4CPLUS_WARN(logger_,eTag << cscTag << " offset: " << offset);
		}
		unsigned short* pos = (uint16_t *)(data)+offset;
		chamberDatas.push_back(CSCEventData(pos));
		
	}
	
  }
*/
  int nCSCs = chamberDatas.size();

/*
  if (nCSCs != dduHeader.ncsc()) {
    LOG4CPLUS_WARN(logger_,eTag << dduTag << " Mismatch between number of unpacked CSCs:" << chamberDatas.size() <<" and reported CSCs from DDU Header:" << dduHeader.ncsc() );
    // == Current trick to maximize number of unpacked CSCs.
    // == Unpacker gives up after screwed chamber.
    // == So we need to exclude it from the list by reducing chamberDatas vector size
//    nCSCs-=1;
//    return;
  }
*/
  // return;

  for(int i=0; i< nCSCs; i++) {
    nCSCEvents++;
    unpackedDMBcount++;
    processChamber(chamberDatas[i], node, dduID);
  }

  /*

  for(std::vector<CSCEventData>::iterator chamberDataItr = chamberDatas.begin(); chamberDataItr != chamberDatas.end(); ++chamberDataItr) {
  nCSCEvents++;
  unpackedDMBcount++;
  processChamber(*chamberDataItr, node, dduID);
  }
  */

  if (isMEvalid(dduME,"DMB_unpacked_vs_DAV",mo)) mo->Fill(dmb_active_header, unpackedDMBcount);

  fFirstEvent = false;

}


void EmuPlotter::fillChamberBinCheck(int32_t node, bool isEventDenied) {

  std::string nodeTag = "EMU";
  ME_List nodeME = MEs[nodeTag];
  EmuMonitoringObject* mo = NULL;
  EmuMonitoringObject* mo1 = NULL;
  EmuMonitoringObject* mo2 = NULL;
  EmuMonitoringObject* mof = NULL;
  
  // === Check and fill CSC Data Flow 
  std::map<int,long> payloads = bin_checker.payloadDetailed();
  for(std::map<int,long>::const_iterator chamber=payloads.begin(); chamber!=payloads.end(); chamber++)
    {
      int CrateID = (chamber->first>>4) & 0xFF;
      int DMBSlot = chamber->first & 0xF;
      std::string cscTag(Form("CSC_%03d_%02d", CrateID, DMBSlot));

      if (CrateID ==255) {continue;}

      std::map<std::string, ME_List >::iterator h_itr = MEs.find(cscTag);
      if (h_itr == MEs.end() || (MEs.size()==0)) {
	LOG4CPLUS_WARN(logger_, eTag << 
		       "List of MEs for " << cscTag <<  " not found. Booking...");
	LOG4CPLUS_DEBUG(logger_,
			"Booking Histos for " << cscTag);
	fBusy = true;
	MEs[cscTag] = bookMEs("CSC",cscTag);
	MECanvases[cscTag] = bookMECanvases("CSC", cscTag, Form(" Crate ID = %02d. DMB ID = %02d", CrateID, DMBSlot));
	cscCounters[cscTag] = bookCounters();
	// printMECollection(MEs[cscTag]);
	fBusy = false;
      }
      ME_List& cscME = MEs[cscTag];

      // === Update counters
      nDMBEvents[cscTag]++;
      CSCCounters& trigCnts = cscCounters[cscTag];
      trigCnts["DMB"] = nDMBEvents[cscTag];


      long DMBEvents= nDMBEvents[cscTag];

      if (isMEvalid(nodeME, "DMB_Reporting", mo)) {
	mo->Fill(CrateID, DMBSlot);
      }

      int CSCtype   = 0;
      int CSCposition = 0;
      getCSCFromMap(CrateID, DMBSlot, CSCtype, CSCposition );
      if ( CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Reporting", mo)) {
	mo->Fill(CSCposition, CSCtype);
      }


      //      Get FEBs Data Available Info
      long payload = chamber->second;
      int cfeb_dav = (payload>>7) & 0x1F;
      int cfeb_active = payload & 0x1F;
      int alct_dav = (payload>>5) & 0x1;
      int tmb_dav = (payload>>6) & 0x1; 
      int cfeb_dav_num=0;
      
      if (alct_dav==0) {
	if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_wo_ALCT", mo)){
	  mo->Fill(CSCposition, CSCtype);
	}

	if (isMEvalid(nodeME, "DMB_wo_ALCT", mo)) {
	  mo->Fill(CrateID,DMBSlot);
	}
      }
     
      if (tmb_dav==0) {
	if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_wo_CLCT", mo)){
	  mo->Fill(CSCposition, CSCtype);
	}

	if (isMEvalid(nodeME, "DMB_wo_CLCT", mo)) {
	  mo->Fill(CrateID,DMBSlot);
	}
      }

      if (cfeb_dav==0) {
	if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_wo_CFEB", mo)){
	  mo->Fill(CSCposition, CSCtype);
	}

	if (isMEvalid(nodeME, "DMB_wo_CFEB", mo)) {
	  mo->Fill(CrateID,DMBSlot);
	}
      }
      
      if (isMEvalid(cscME, "Actual_DMB_CFEB_DAV_Rate", mo)
	  && isMEvalid(cscME, "Actual_DMB_CFEB_DAV_Frequency", mof)) {
	/*
	for (int i=0; i<5;i++) {
	  int cfeb_present = (cfeb_dav>>i) & 0x1;
	  cfeb_dav_num += cfeb_present;
	  if (cfeb_present) {
	    mo->Fill(i);
	  }
	  float cfeb_entries = mo->GetBinContent(i+1);
	  mof->SetBinContent(i+1, ((float)cfeb_entries/(float)(DMBEvents)*100.0));
	}
	mof->SetEntries((int)DMBEvents);
	*/
	if (isMEvalid(cscME, "DMB_CFEB_DAV_Unpacking_Inefficiency", mo1)
	    && isMEvalid(cscME, "DMB_CFEB_DAV", mo2)) {	   
	  for (int i=1; i<=5; i++) {
	    double actual_dav_num = mo->GetBinContent(i);
	    double unpacked_dav_num = mo2->GetBinContent(i);
	    if (actual_dav_num){
	      mo1->SetBinContent(i,1, 100.*(1-unpacked_dav_num/actual_dav_num));
	    }				   
	    mo1->SetEntries((int)DMBEvents);
	    //	    mo1->getObject()->SetMinimum(0);
	    //mo1->getObject()->SetMaximum(100.0);
	  }
	}	
	for (int i=0; i<5;i++) {
	  int cfeb_present = (cfeb_dav>>i) & 0x1;
	  cfeb_dav_num += cfeb_present;
	  if (cfeb_present) {
	    mo->Fill(i);
	  }
	  float cfeb_entries = mo->GetBinContent(i+1);
	  mof->SetBinContent(i+1, ((float)cfeb_entries/(float)(DMBEvents)*100.0));
	}
	mof->SetEntries((int)DMBEvents);

      }
      
      if (isMEvalid(cscME, "Actual_DMB_CFEB_DAV_multiplicity_Rate", mo)
	  && isMEvalid(cscME, "Actual_DMB_CFEB_DAV_multiplicity_Frequency", mof)) {
	// mo->Fill(cfeb_dav_num);
	for (int i=1; i<7; i++) {
	  float cfeb_entries =  mo->GetBinContent(i);
	  mof->SetBinContent(i, ((float)cfeb_entries/(float)(DMBEvents)*100.0));
	}
	mof->SetEntries((int)DMBEvents);

	if (isMEvalid(cscME, "DMB_CFEB_DAV_multiplicity_Unpacking_Inefficiency", mo1)
	    && isMEvalid(cscME, "DMB_CFEB_DAV_multiplicity", mo2)) {	   
	  for (int i=1; i<7; i++) {
	    float actual_dav_num = mo->GetBinContent(i);
	    float unpacked_dav_num = mo2->GetBinContent(i);
	    if (actual_dav_num){
	      mo1->SetBinContent(i,1, 100.*(1-unpacked_dav_num/actual_dav_num));
	    }				   
	    mo1->SetEntries((int)DMBEvents);
	    // mo1->getObject()->SetMaximum(100.0);
	  }
	}	
	mo->Fill(cfeb_dav_num);
      }

	  
	
      

      if (isMEvalid(cscME, "DMB_CFEB_Active_vs_DAV", mo)) mo->Fill(cfeb_dav,cfeb_active);

      //      Fill Histogram for FEB DAV Efficiency
      if (isMEvalid(cscME, "Actual_DMB_FEB_DAV_Rate", mo)) {
	if (isMEvalid(cscME, "Actual_DMB_FEB_DAV_Frequency", mo1)) {
	  for (int i=1; i<4; i++) {
	    float dav_num = mo->GetBinContent(i);
	    mo1->SetBinContent(i, ((float)dav_num/(float)(DMBEvents)*100.0));
	  }
	  mo1->SetEntries((int)DMBEvents);

	  if (isMEvalid(cscME, "DMB_FEB_DAV_Unpacking_Inefficiency", mof)
     	      && isMEvalid(cscME, "DMB_FEB_DAV_Rate", mo2)) {	   
	    for (int i=1; i<4; i++) {
	      float actual_dav_num = mo->GetBinContent(i);
	      float unpacked_dav_num = mo2->GetBinContent(i);
	      if (actual_dav_num){
		mof->SetBinContent(i,1, 100.*(1-unpacked_dav_num/actual_dav_num));
	      }				   
	      mof->SetEntries((int)DMBEvents);
	      mof->getObject()->SetMaximum(100.0);
	    }
	  }	  
	}

	if (alct_dav  > 0) {
	  mo->Fill(0.0);
	}
	if (tmb_dav  > 0) {
	  mo->Fill(1.0);
	}
	if (cfeb_dav > 0) {
	  mo->Fill(2.0);
	}
      }
      

      float feb_combination_dav = -1.0;
      //      Fill Histogram for Different Combinations of FEB DAV Efficiency
      if (isMEvalid(cscME, "Actual_DMB_FEB_Combinations_DAV_Rate", mo)) {
	if(alct_dav == 0 && tmb_dav == 0 && cfeb_dav == 0) feb_combination_dav = 0.0; // Nothing
	if(alct_dav >  0 && tmb_dav == 0 && cfeb_dav == 0) feb_combination_dav = 1.0; // ALCT Only
	if(alct_dav == 0 && tmb_dav >  0 && cfeb_dav == 0) feb_combination_dav = 2.0; // TMB Only
	if(alct_dav == 0 && tmb_dav == 0 && cfeb_dav >  0) feb_combination_dav = 3.0; // CFEB Only
	if(alct_dav == 0 && tmb_dav >  0 && cfeb_dav >  0) feb_combination_dav = 4.0; // TMB+CFEB
	if(alct_dav >  0 && tmb_dav >  0 && cfeb_dav == 0) feb_combination_dav = 5.0; // ALCT+TMB
	if(alct_dav >  0 && tmb_dav == 0 && cfeb_dav >  0) feb_combination_dav = 6.0; // ALCT+CFEB
	if(alct_dav >  0 && tmb_dav >  0 && cfeb_dav >  0) feb_combination_dav = 7.0; // ALCT+TMB+CFEB
	//	mo->Fill(feb_combination_dav);
	if (isMEvalid(cscME, "Actual_DMB_FEB_Combinations_DAV_Frequency",mo1)) {
	  for (int i=1; i<9; i++) {
	    float feb_combination_dav_number = mo->GetBinContent(i);
	    mo1->SetBinContent(i, ((float)feb_combination_dav_number/(float)(DMBEvents)*100.0));
	  }
	  mo1->SetEntries(DMBEvents);
	  
	  if (isMEvalid(cscME, "DMB_FEB_Combinations_DAV_Unpacking_Inefficiency", mof)
     	      && isMEvalid(cscME, "DMB_FEB_Combinations_DAV_Rate", mo2)) {	   
	    for (int i=1; i<9; i++) {
	      float actual_dav_num = mo->GetBinContent(i);
	      float unpacked_dav_num = mo2->GetBinContent(i);
	      if (actual_dav_num){
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


  // === Check and fill CSC Data Flow Problems
  std::map<int,long> statuses = bin_checker.statusDetailed();
  for(std::map<int,long>::const_iterator chamber=statuses.begin(); chamber!=statuses.end(); chamber++)
    {
      int CrateID = (chamber->first>>4) & 0xFF;
      int DMBSlot = chamber->first & 0xF;
      std::string cscTag(Form("CSC_%03d_%02d", CrateID, DMBSlot));

      if (CrateID ==255) {continue;}

      std::map<std::string, ME_List >::iterator h_itr = MEs.find(cscTag);
      if (h_itr == MEs.end() || (MEs.size()==0)) {
	LOG4CPLUS_WARN(logger_, eTag << 
		       "List of MEs for " << cscTag <<  " not found. Booking...");
	LOG4CPLUS_DEBUG(logger_,
			"Booking Histos for " << cscTag);
	fBusy = true;
	MEs[cscTag] = bookMEs("CSC",cscTag);
	MECanvases[cscTag] = bookMECanvases("CSC", cscTag, Form(" Crate ID = %02d. DMB ID = %02d", CrateID, DMBSlot));
	cscCounters[cscTag] = bookCounters();
	// printMECollection(MEs[cscTag]);
	fBusy = false;
      }
      ME_List& cscME = MEs[cscTag];
  
      int CSCtype   = 0;
      int CSCposition = 0;
      getCSCFromMap(CrateID, DMBSlot, CSCtype, CSCposition );

      if (isMEvalid(cscME, "BinCheck_DataFlow_Problems_Table", mo)) {
	for(int bit=0; bit<bin_checker.nSTATUSES; bit++)
	  if( chamber->second & (1<<bit) ) {
	    mo->Fill(0.,bit);
	  }
	mo->SetEntries(nDMBEvents[cscTag]);
      }

      
      int anyInputFull = chamber->second & 0x3F;
      if(anyInputFull){
	if(CSCtype && CSCposition && isMEvalid(nodeME, "CSC_DMB_input_fifo_full", mo)){
	  mo->Fill(CSCposition, CSCtype);
	}
	if (isMEvalid(nodeME, "DMB_input_fifo_full", mo)) {
	  mo->Fill(CrateID, DMBSlot);
	}
      }


      int anyInputTO = (chamber->second >> 7) & 0x3FFF;
      if(anyInputTO){
	if(CSCtype && CSCposition && isMEvalid(nodeME, "CSC_DMB_input_timeout", mo)){
	  mo->Fill(CSCposition, CSCtype);
	}
	if (isMEvalid(nodeME, "DMB_input_timeout", mo)) {
	  mo->Fill(CrateID, DMBSlot);
	}
      }

      
      // === CFEB B-Word
      if (chamber->second & (1<<22)) {
	// LOG4CPLUS_WARN(logger_,eTag << cscTag << " CFEB B-Words found ");

	if (isMEvalid(nodeME, "DMB_Format_Warnings", mo)) {
	  mo->Fill(CrateID, DMBSlot);
	}
  
	if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Format_Warnings", mo)) {
	  mo->Fill(CSCposition, CSCtype);
	}
	
      }
    }


  // === Check and fill CSC Format Errors 
  std::map<int,long> checkerErrors = bin_checker.errorsDetailed();
  for(std::map<int,long>::const_iterator chamber=checkerErrors.begin(); chamber!=checkerErrors.end(); chamber++)
    {
      int CrateID = (chamber->first>>4) & 0xFF;
      int DMBSlot = chamber->first & 0xF;

      std::string cscTag(Form("CSC_%03d_%02d", CrateID , DMBSlot));
      std::map<std::string, ME_List >::iterator h_itr = MEs.find(cscTag);

      if ((CrateID ==255) || 
	  (chamber->second & 0x80)) {continue;} // = Skip chamber detection if DMB header is missing (Error code 6)

      if (CrateID>60 || DMBSlot>10) {
	LOG4CPLUS_WARN(logger_, eTag << "Invalid CSC: " << cscTag << ". Skipping");
	continue;
      }
 
      if (h_itr == MEs.end() || (MEs.size()==0)) {
	LOG4CPLUS_WARN(logger_, eTag << 
		       "List of MEs for " << cscTag <<  " not found. Booking...");
	LOG4CPLUS_DEBUG(logger_,
			"Booking Histos for " << cscTag);
	fBusy = true;
	MEs[cscTag] = bookMEs("CSC", cscTag);
	MECanvases[cscTag] = bookMECanvases("CSC", cscTag, Form(" Crate ID = %02d. DMB ID = %02d", CrateID, DMBSlot));
	cscCounters[cscTag] = bookCounters();

	// printMECollection(MEs[cscTag]);
	fBusy = false;
      }
      ME_List& cscME = MEs[cscTag];

      if ((chamber->second & binCheckMask) != 0) {
	//	nDMBEvents[cscTag]++;	
	CSCCounters& trigCnts = cscCounters[cscTag];
	trigCnts["BAD"]++; 
      }

      bool isCSCError = false;

      if (isMEvalid(cscME, "BinCheck_ErrorStat_Table", mo)) {
	for(int bit=5; bit<24; bit++)
	  if( chamber->second & (1<<bit) ) {
	    isCSCError = true;
	    mo->Fill(0.,bit-5);
	   
	  }
	mo->SetEntries(nDMBEvents[cscTag]);
      }

      if (isCSCError) {

	LOG4CPLUS_WARN(logger_,eTag << "Format Errors "<< cscTag << ": 0x" << std::hex << chamber->second);

	if (isMEvalid(nodeME, "DMB_Format_Errors", mo)) {
	  mo->Fill(CrateID, DMBSlot);
	}

	if (!isEventDenied  && isMEvalid(nodeME, "DMB_Unpacked_with_errors", mo)) {
	  mo->Fill(CrateID, DMBSlot);
	}

	int CSCtype   = 0;
	int CSCposition = 0;
	getCSCFromMap(CrateID, DMBSlot, CSCtype, CSCposition );
	if ( CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Format_Errors", mo)) {
	  mo->Fill(CSCposition, CSCtype);
	}

	if (!isEventDenied  && CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Unpacked_with_errors", mo)) {
	  mo->Fill(CSCposition, CSCtype);
	}
      }

    }

  /*
  // === Check and fill CSC Format Warnings
  std::map<int,long> checkerWarnings  = bin_checker.warningsDetailed();
  for(std::map<int,long>::const_iterator chamber=checkerWarnings.begin(); chamber!=checkerWarnings.end(); chamber++)
  {
  int ChamberID     = chamber->first;
  int CrateID = (chamber->first>>4) & 0xFF;
  int DMBSlot = chamber->first & 0xF;

  std::string cscTag(Form("CSC_%03d_%02d", CrateID, DMBSlot));

  if (CrateID ==255) {continue;}

  std::map<std::string, ME_List >::iterator h_itr = MEs.find(cscTag);
  if (h_itr == MEs.end() || (MEs.size()==0)) {
  LOG4CPLUS_WARN(logger_, eTag << 
  "List of Histos for " << cscTag <<  " not found. Booking...");
  LOG4CPLUS_DEBUG(logger_,
  "Booking Histos for " << cscTag);
  fBusy = true;
  MEs[cscTag] = bookChamber(ChamberID);
  MECanvases[cscTag] = bookChamberCanvases(ChamberID);
  printMECollection(MEs[cscTag]);
  fBusy = false;
  }
  ME_List& cscME = MEs[cscTag];

  bool isCSCWarning = false;

  if (isMEvalid(cscME, "BinCheck_WarningStat_Table", mo)
  && isMEvalid(cscME, "BinCheck_WarningStat_Frequency", mof)) {
  for(int bit=1; bit<2; bit++)
  if( chamber->second & (1<<bit) ) {
  isCSCWarning = true;
  mo->Fill(0.,bit-1);
  float freq = (mo->GetBinContent(1,bit))/nDMBEvents[cscTag];
  mof->SetBinContent(1,bit, freq);
  }
  mo->SetEntries(nDMBEvents[cscTag]);
  mof->SetEntries(nDMBEvents[cscTag]);
  }

  if (isCSCWarning) {
  LOG4CPLUS_WARN(logger_,eTag << "Format Warnings "<< cscTag << ": 0x" << std::hex << chamber->second);

  if (isMEvalid(nodeME, "DMB_Format_Warnings", mo)) {
  mo->Fill(CrateID, DMBSlot);
  }
  
  int CSCtype   = 0;
  int CSCposition = 0;
  getCSCFromMap(CrateID, DMBSlot, CSCtype, CSCposition );
  if (CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Format_Warnings", mo)) {
  mo->Fill(CSCposition, CSCtype);
  }
	
  if (!isEventDenied && CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Unpacked_with_warnings", mo)) {
  mo->Fill(CSCposition, CSCtype);
  }
	
  }
  }
  */
}

void EmuPlotter::updateCSCHistos()
{
   std::map<std::string, ME_List>::iterator itr;
   for (itr = MEs.begin(); itr != MEs.end(); ++itr) {
     if (itr->first.find("CSC_") != std::string::npos) {
       updateCSCFractionHistos(itr->first);
     }
   }

}

void EmuPlotter::updateCSCFractionHistos(std::string cscTag)
{

  EmuMonitoringObject *mo = NULL;
  EmuMonitoringObject *mof = NULL;
  ME_List& cscME = MEs[cscTag];

  if (isMEvalid(cscME, "BinCheck_DataFlow_Problems_Table", mo)
      && isMEvalid(cscME, "BinCheck_DataFlow_Problems_Frequency", mof)) {
    mof->getObject()->Reset();
    mof->getObject()->Add(mo->getObject());
    mof->getObject()->Scale(1./(nDMBEvents[cscTag]));
    mof->getObject()->SetMaximum(1.);
    mof->SetEntries(nDMBEvents[cscTag]);
    mo->SetEntries(nDMBEvents[cscTag]);

  }

  if (isMEvalid(cscME, "BinCheck_ErrorStat_Table", mo)
      && isMEvalid(cscME, "BinCheck_Errors_Frequency", mof)) {
    mof->getObject()->Reset();
    mof->getObject()->Add(mo->getObject());
    mof->getObject()->Scale(1./(nDMBEvents[cscTag]));
    mof->getObject()->SetMaximum(1.);
    mof->SetEntries(nDMBEvents[cscTag]);
    mo->SetEntries(nDMBEvents[cscTag]);
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

  //  LOG4CPLUS_WARN(logger_, "Update Fraction Histograms");

  /*
  EmuMonitoringObject *mo = NULL;
  EmuMonitoringObject *mo1 = NULL;
  EmuMonitoringObject *mo2 = NULL;
  EmuMonitoringObject *mo3 = NULL;
  */

  // ************************************
  // Collecting all the reporting DMBs and CSCs
  // ************************************
/*
  if (isMEvalid(nodeME, "DMB_Reporting", mo)
      && isMEvalid(nodeME, "DMB_Format_Errors", mo1)
      && isMEvalid(nodeME, "DMB_Unpacked", mo2))
    {
      mo->getObject()->Add(mo1->getObject(), mo2->getObject());
      if (isMEvalid(nodeME, "DMB_Unpacked_with_errors", mo3)) {
	mo->getObject()->Add(mo3->getObject(), -1);
      }
    }

  if (isMEvalid(nodeME, "CSC_Reporting", mo)
      && isMEvalid(nodeME, "CSC_Format_Errors", mo1)
      && isMEvalid(nodeME, "CSC_Unpacked", mo2))
    {
      mo->getObject()->Add(mo1->getObject(), mo2->getObject());
      if (isMEvalid(nodeME, "CSC_Unpacked_with_errors", mo3)) {
	mo->getObject()->Add(mo3->getObject(), -1);
      }
    }
*/
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
  
/*
  if (isMEvalid(nodeME, "DMB_Format_Warnings_Fract", mo)
      && isMEvalid(nodeME, "DMB_Format_Warnings", mo1)
      && isMEvalid(nodeME, "DMB_Unpacked", mo2))
    {
      TH1* tmp=dynamic_cast<TH1*>(mo2->getObject()->Clone());
      tmp->Add(mo1->getObject());


      mo->getObject()->Divide(mo1->getObject(), tmp);
      delete tmp;
    }

  if (isMEvalid(nodeME, "CSC_Format_Warnings_Fract", mo)
      && isMEvalid(nodeME, "CSC_Format_Warnings", mo1)
      && isMEvalid(nodeME, "CSC_Unpacked", mo2))
    {
      TH1* tmp=dynamic_cast<TH1*>(mo2->getObject()->Clone());
      tmp->Add(mo1->getObject());
      if (isMEvalid(nodeME, "CSC_Unpacked_with_warnings", mo3)) {
	tmp->Add(mo3->getObject(), -1);
      }
      mo->getObject()->Divide(mo1->getObject(), tmp);
      delete tmp;
    }
*/

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

  if (isMEvalid(nodeME, "CSC_Reporting", mo)) {

    TH2* rep = dynamic_cast<TH2*>(mo->getObject());
    summary.ReadReportingChambers(rep, 1.0);

    if (isMEvalid(nodeME, "CSC_Format_Errors", mo1)) {
      TH2* err = dynamic_cast<TH2*>(mo1->getObject());
      summary.ReadErrorChambers(rep, err, cscdqm::FORMAT_ERR, 0.1, 5.0);
    }

    if (isMEvalid(nodeME, "CSC_L1A_out_of_sync", mo1)) {
      TH2* err = dynamic_cast<TH2*>(mo1->getObject());
      summary.ReadErrorChambers(rep, err, cscdqm::L1SYNC_ERR, 0.1, 5.0);
    }

    if (isMEvalid(nodeME, "CSC_DMB_input_fifo_full", mo1)) {
      TH2* err = dynamic_cast<TH2*>(mo1->getObject());
      summary.ReadErrorChambers(rep, err, cscdqm::FIFOFULL_ERR, 0.1, 5.0);
    }

    if (isMEvalid(nodeME, "CSC_DMB_input_timeout", mo1)) {
      TH2* err = dynamic_cast<TH2*>(mo1->getObject());
      summary.ReadErrorChambers(rep, err, cscdqm::INPUTTO_ERR, 0.1, 5.0);
    }

  }

  //
  // Write summary information
  //


  if (isMEvalid(nodeME, "Physics_ME1", mo)) {
    TH2* tmp = dynamic_cast<TH2*>(mo->getObject());
    summary.Write(tmp, 1);
  }

  if (isMEvalid(nodeME, "Physics_ME2", mo)) {
    TH2* tmp = dynamic_cast<TH2*>(mo->getObject());
    summary.Write(tmp, 2);
  }

  if (isMEvalid(nodeME, "Physics_ME3", mo)) {
    TH2* tmp = dynamic_cast<TH2*>(mo->getObject());
    summary.Write(tmp, 3);
  }

  if (isMEvalid(nodeME, "Physics_ME4", mo)) {
    TH2* tmp = dynamic_cast<TH2*>(mo->getObject());
    summary.Write(tmp, 4);
  }


  if (isMEvalid(nodeME, "Physics_EMU", mo)) {

    TH2* tmp=dynamic_cast<TH2*>(mo->getObject());
    // float rs = 
    summary.WriteMap(tmp);
    // float he = summary.GetEfficiencyHW();
    // TString title = Form("EMU Status: Physics Efficiency %.2f", rs);
    // tmp->SetTitle(title);

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
				   std::string subSetHistoName){

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

