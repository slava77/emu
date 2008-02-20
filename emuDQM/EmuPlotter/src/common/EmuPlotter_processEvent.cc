#include "EmuPlotter.h"


void EmuPlotter::processEvent(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
{
  //	LOG4CPLUS_INFO(logger_ , "processing event data");
  
  int node = 0; // Set EMU root folder
  //   int node = nodeNumber;
  //  std::string nodeTag = Form("EMU_%d",node); // == This emuMonitor node number

  std::string nodeTag = "EMU";
  std::map<std::string, ME_List >::iterator itr;
  ME_List nodeME; // === Global histos specific for this emuMonitor node
  EmuMonitoringObject *mo = NULL;  // == pointer to MonitoringObject
  unpackedDMBcount = 0; 
  

  // == Check and book global node specific histos
  if (MEs.size() == 0 || ((itr = MEs.find(nodeTag)) == MEs.end())) {
    LOG4CPLUS_WARN(logger_, "List of MEs for " << nodeTag << " not found. Booking...");
    fBusy = true;
    MEs[nodeTag] = bookCommon(node);
    MECanvases[nodeTag] = bookCommonCanvases(node);
    printMECollection(MEs[nodeTag]);
    fBusy = false;
  }
	
  nodeME = MEs[nodeTag];
  
  nEvents++;
  eTag=Form("Evt# %d: ", nEvents);

  if (isMEvalid(nodeME, "Buffer_Size", mo)) mo->Fill(dataSize);

  // ==     Check DDU Readout Error Status
  if (isMEvalid(nodeME, "Readout_Errors", mo)) { 
    if(errorStat != 0) {
      LOG4CPLUS_WARN(logger_,eTag << "Non-zero Readout Error Status is observed: 0x" << std::hex << errorStat << " mask 0x" << dduCheckMask);
      for (int i=0; i<16; i++) if ((errorStat>>i) & 0x1) mo->Fill(0.,i);
    }
    else {
      LOG4CPLUS_DEBUG(logger_,eTag << "Readout Error Status is OK: 0x" << std::hex << errorStat);
    }
  }

  if (isMEvalid(nodeME, "All_Readout_Errors", mo)) {
    if(errorStat != 0) {      
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
  if( bin_checker.check(tmp,dataSize/sizeof(short)) < 0 ){
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

	std::string dduTag = Form("DDU_%d",dduID);

        if (MEs.size() == 0 || ((itr = MEs.find(dduTag)) == MEs.end())) {
	  LOG4CPLUS_WARN(logger_, "List of MEs for " << dduTag << " not found. Booking...");
	  fBusy = true;
	  MEs[dduTag] = bookDDU(dduID);
	  MECanvases[dduTag] = bookDDUCanvases(dduID);
	  printMECollection(MEs[dduTag]);
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
      std::string dduTag = Form("DDU_%d",dduID);

      if (MEs.size() == 0 || ((itr = MEs.find(dduTag)) == MEs.end())) {
	LOG4CPLUS_WARN(logger_, "List of MEs for " << dduTag << " not found. Booking...");
	fBusy = true;
	MEs[dduTag] = bookDDU(dduID);
	MECanvases[dduTag] = bookDDUCanvases(dduID);
	printMECollection(MEs[dduTag]);
	fBusy = false;
	L1ANumbers[dduID] = 0;
	fFirstEvent = true;
      }
      mo->Fill(dduID,0);
    }

  }


  if(BinaryErrorStatus != 0) {
    LOG4CPLUS_WARN(logger_,eTag << "Nonzero Binary Errors Status is observed: 0x" << std::hex << BinaryErrorStatus << " mask: 0x" << binCheckMask);

    if (isMEvalid(nodeME, "BinaryChecker_Errors", mo)) {
      for(int i=0; i<bin_checker.nERRORS; i++) { // run over all errors
	if( bin_checker.error(i) ) mo->Fill(0.,i);
      }
    }

  }
  else {
    LOG4CPLUS_DEBUG(logger_,eTag << "Binary Error Status is OK: 0x" << std::hex << BinaryErrorStatus);
  }


  if(BinaryWarningStatus != 0) {
    LOG4CPLUS_WARN(logger_,eTag << "Nonzero Binary Warnings Status is observed: 0x" 
		   << std::hex << BinaryWarningStatus)
      if (isMEvalid(nodeME, "BinaryChecker_Warnings", mo)) {
	for(int i=0; i<bin_checker.nWARNINGS; i++) { // run over all warnings
	  if( bin_checker.warning(i) ) mo->Fill(0.,i);
	}
      }

  }
  else {
    LOG4CPLUS_DEBUG(logger_,eTag << "Binary Warnings Status is OK: 0x" << std::hex << BinaryWarningStatus);

  }

  //	Accept or deny event
  bool EventDenied = false;
  //	Accept or deny event according to DDU Readout Error and dduCheckMask
  if (((uint32_t)errorStat & dduCheckMask) > 0) {
    LOG4CPLUS_WARN(logger_,eTag << "Skipped because of DDU Readout Error");
    EventDenied = true;
  }

  if ((BinaryErrorStatus & binCheckMask)>0) {
    LOG4CPLUS_WARN(logger_,eTag << "Skipped because of Binary Error");
    EventDenied = true;
  }

  if ((BinaryErrorStatus != 0) || (BinaryWarningStatus != 0)) {
    nBadEvents++;
    fillChamberBinCheck(node, EventDenied);
  }


  if(EventDenied) return;
  else LOG4CPLUS_DEBUG(logger_,eTag << "is accepted");

  nGoodEvents++;


  // CSCDDUEventData::setDebug(true);
  int dduID = 0;
  CSCDDUEventData dduData((uint16_t *) data);


  CSCDDUHeader dduHeader  = dduData.header();
  CSCDDUTrailer dduTrailer = dduData.trailer();
  
  dduID = dduHeader.source_id()&0xFF; // Only 8bits are significant; format of DDU id is Dxx
  
  if (isMEvalid(nodeME, "All_DDUs_in_Readout", mo)) {
    mo->Fill(dduID);
  }



  std::string dduTag = Form("DDU_%d",dduID);

  if (MEs.size() == 0 || ((itr = MEs.find(dduTag)) == MEs.end())) {
    LOG4CPLUS_WARN(logger_, "List of MEs for " << dduTag << " not found. Booking...");
    fBusy = true;
    MEs[dduTag] = bookDDU(dduID);
    MECanvases[dduTag] = bookDDUCanvases(dduID);
    printMECollection(MEs[dduTag]);
    fBusy = false;
    L1ANumbers[dduID] = 0;
    //  L1ANumbers[dduID] = (int)(dduHeader.lvl1num());
    fFirstEvent = true;
  }

  ME_List& dduME = MEs[dduTag];

  LOG4CPLUS_DEBUG(logger_,"Start unpacking " << dduTag);

  if (isMEvalid(dduME, "Buffer_Size", mo)) mo->Fill(dataSize);
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

  // ==     DDU Header banch crossing number (BXN)
  BXN=dduHeader.bxnum();
  LOG4CPLUS_DEBUG(logger_,dduTag << " DDU Header BXN Number = " << std::dec << BXN);
  if (isMEvalid(dduME, "BXN", mo)) mo->Fill((double)BXN);

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
  chamberDatas = dduData.cscData();


  for(std::vector<CSCEventData>::iterator chamberDataItr = chamberDatas.begin(); chamberDataItr != chamberDatas.end(); ++chamberDataItr) {
    nCSCEvents++;
    unpackedDMBcount++;
    //  LOG4CPLUS_DEBUG(logger_,
    //		    "Found DMB " << std::dec << unpackedDMBcount  << ". Run unpacking procedure...");
    processChamber(*chamberDataItr, node, dduID);
    //  LOG4CPLUS_DEBUG(logger_,
    //		    "Unpacking procedure for DMB " << std::dec << unpackedDMBcount << " finished");
  }
  // LOG4CPLUS_DEBUG(logger_,
  //		  "Total number of unpacked DMB = " << std::dec << unpackedDMBcount);

  
  if (isMEvalid(dduME,"DMB_unpacked_vs_DAV",mo)) mo->Fill(dmb_active_header, unpackedDMBcount);

/*
  // Calculate ratio histograms every 200 events
  if (nEvents%200==0) {

  EmuMonitoringObject *mo1 = NULL;
  EmuMonitoringObject *mo2 = NULL;
  if (isMEvalid(nodeME, "DMB_Format_Errors_Fract", mo) 
      && isMEvalid(nodeME, "DMB_Format_Errors", mo1) 
      && isMEvalid(nodeME, "DMB_Unpacked", mo2)) 
    {
       // mo->getObject()->Divide(mo1->getObject(), mo2->getObject());
	
	MonitorElement* tmp=dynamic_cast<MonitorElement*>(mo2->getObject()->Clone());
        tmp->Add(mo1->getObject());
        mo->getObject()->Divide(mo1->getObject(), tmp);
        delete tmp;
	
	
    }

  if (isMEvalid(nodeME, "CSC_Format_Errors_Fract", mo)
      && isMEvalid(nodeME, "CSC_Format_Errors", mo1)
      && isMEvalid(nodeME, "CSC_Unpacked", mo2))
    {
       // mo->getObject()->Divide(mo1->getObject(), mo2->getObject());
	
	MonitorElement* tmp=dynamic_cast<MonitorElement*>(mo2->getObject()->Clone());
        tmp->Add(mo1->getObject());
        mo->getObject()->Divide(mo1->getObject(), tmp);
        delete tmp;
	

    }

  if (isMEvalid(nodeME, "CSC_Format_Warnings_Fract", mo)
      && isMEvalid(nodeME, "CSC_Format_Warnings", mo1)
      && isMEvalid(nodeME, "CSC_Unpacked", mo2))
    {
      // mo->getObject()->Divide(mo1->getObject(), mo2->getObject());
	
	MonitorElement* tmp=dynamic_cast<MonitorElement*>(mo2->getObject()->Clone());
        tmp->Add(mo1->getObject());
        mo->getObject()->Divide(mo1->getObject(), tmp);
        delete tmp;
	

    }

  }
 */
  //LOG4CPLUS_DEBUG(logger_,
  //		  "END OF EVENT :-(");
  fFirstEvent = false;

}


void EmuPlotter::fillChamberBinCheck(int32_t node, bool isEventDenied) {

  std::string nodeTag = "EMU";
  ME_List nodeME = MEs[nodeTag];
  EmuMonitoringObject* mo = NULL;
  EmuMonitoringObject* mof = NULL;

  //  if(check_bin_error){
  std::map<int,long> checkerErrors = bin_checker.errorsDetailed();
  std::map<int,long>::const_iterator chamber = checkerErrors.begin();
  while( chamber != checkerErrors.end() ){
    int ChamberID     = chamber->first;
    int CrateID = (chamber->first>>4) & 0xFF;
    int DMBSlot = chamber->first & 0xF;
    std::string cscTag(Form("CSC_%03d_%02d", CrateID , DMBSlot));
    std::map<std::string, ME_List >::iterator h_itr = MEs.find(cscTag);
    if ((CrateID ==255) || 
        (chamber->second & 0x80)) { chamber++; continue;} // = Skip chamber detection if DMB header is missing (Error code 6)
    if (h_itr == MEs.end() || (MEs.size()==0)) {
      LOG4CPLUS_WARN(logger_,
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
    if ( (bin_checker.errors() & binCheckMask) != 0) {
      nDMBEvents[cscTag]++;      
    }

    bool isCSCError = false;
    if (isMEvalid(cscME, "BinCheck_ErrorStat_Table", mo)
	&& isMEvalid(cscME, "BinCheck_ErrorStat_Frequency", mof)) {
      for(int bit=5; bit<24; bit++)
	if( chamber->second & (1<<bit) ) {
	  isCSCError = true;
	  mo->Fill(0.,bit-5);
	   
	  double freq = (100.0*mo->GetBinContent(1,bit-4))/nDMBEvents[cscTag];
	  mof->SetBinContent(bit-4, freq);
	}
      mo->SetEntries(nDMBEvents[cscTag]);
      mof->SetEntries(nDMBEvents[cscTag]);
    }

    if (isCSCError && isMEvalid(nodeME, "DMB_Format_Errors", mo)) {
      mo->Fill(CrateID, DMBSlot);
    }

    if (!isEventDenied && isCSCError && isMEvalid(nodeME, "DMB_Unpacked_with_Errors", mo)) {
      mo->Fill(CrateID, DMBSlot);
    }


    int CSCtype   = 0;
    int CSCposition = 0;
    getCSCFromMap(CrateID, DMBSlot, CSCtype, CSCposition );
    if (isCSCError && CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Format_Errors", mo)) {
      mo->Fill(CSCposition, CSCtype);
    }

    if (!isEventDenied && isCSCError && CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Unpacked_with_errors", mo)) {
      mo->Fill(CSCposition, CSCtype);
    }

    chamber++;
  }

  std::map<int,long> checkerWarnings  = bin_checker.warningsDetailed();
  chamber = checkerWarnings.begin();
  while( chamber != checkerWarnings.end() ){
    int ChamberID     = chamber->first;
    int CrateID = (chamber->first>>4) & 0xFF;
    int DMBSlot = chamber->first & 0xF;
    std::string cscTag(Form("CSC_%03d_%02d", CrateID, DMBSlot));
    if (CrateID ==255) {chamber++; continue;}
    std::map<std::string, ME_List >::iterator h_itr = MEs.find(cscTag);
    if (h_itr == MEs.end() || (MEs.size()==0)) {
      LOG4CPLUS_WARN(logger_,
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
	  double freq = (100.0*mo->GetBinContent(1,bit))/nDMBEvents[cscTag];
	  mof->SetBinContent(bit, freq);
	}
      mo->SetEntries(nDMBEvents[cscTag]);
      mof->SetEntries(nDMBEvents[cscTag]);
    }

    if (isCSCWarning && isMEvalid(nodeME, "DMB_Format_Warnings", mo)) {
      mo->Fill(CrateID, DMBSlot);
    }
  
    int CSCtype   = 0;
    int CSCposition = 0;
    getCSCFromMap(CrateID, DMBSlot, CSCtype, CSCposition );
    if (isCSCWarning && CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Format_Warnings", mo)) {
      mo->Fill(CSCposition, CSCtype);
      //      mo->SetEntries(nBadEvents);
    }

    if (!isEventDenied && isCSCWarning && CSCtype && CSCposition && isMEvalid(nodeME, "CSC_Unpacked_with_warnings", mo)) {
      mo->Fill(CSCposition, CSCtype);
      //      mo->SetEntries(nBadEvents);
    }


    chamber++;
  }
  // }
}
		    
void EmuPlotter::updateFractionHistos()
{

  std::string nodeTag = "EMU";
  ME_List nodeME; // === Global histos
  nodeME = MEs[nodeTag];

//  LOG4CPLUS_WARN(logger_, "Update Fraction Histograms");

  EmuMonitoringObject *mo = NULL;
  EmuMonitoringObject *mo1 = NULL;
  EmuMonitoringObject *mo2 = NULL;
  EmuMonitoringObject *mo3 = NULL;
  if (isMEvalid(nodeME, "DMB_Format_Errors_Fract", mo)
      && isMEvalid(nodeME, "DMB_Format_Errors", mo1)
      && isMEvalid(nodeME, "DMB_Unpacked", mo2))
    {
       // mo->getObject()->Divide(mo1->getObject(), mo2->getObject());
        
        MonitorElement* tmp=dynamic_cast<MonitorElement*>(mo2->getObject()->Clone());
        tmp->Add(mo1->getObject());
        if (isMEvalid(nodeME, "DMB_Unpacked_with_errors", mo3)) {
		tmp->Add(mo3->getObject(), -1);
        }
	
        mo->getObject()->Divide(mo1->getObject(), tmp);
        delete tmp;


    }

  if (isMEvalid(nodeME, "CSC_Format_Errors_Fract", mo)
      && isMEvalid(nodeME, "CSC_Format_Errors", mo1)
      && isMEvalid(nodeME, "CSC_Unpacked", mo2))
    {
       // mo->getObject()->Divide(mo1->getObject(), mo2->getObject());

        MonitorElement* tmp=dynamic_cast<MonitorElement*>(mo2->getObject()->Clone());
        tmp->Add(mo1->getObject());
	if (isMEvalid(nodeME, "CSC_Unpacked_with_errors", mo3)) {
                tmp->Add(mo3->getObject(), -1);
        }
        mo->getObject()->Divide(mo1->getObject(), tmp);
        delete tmp;


    }

  if (isMEvalid(nodeME, "CSC_Format_Warnings_Fract", mo)
      && isMEvalid(nodeME, "CSC_Format_Warnings", mo1)
      && isMEvalid(nodeME, "CSC_Unpacked", mo2))
    {
      // mo->getObject()->Divide(mo1->getObject(), mo2->getObject());

        MonitorElement* tmp=dynamic_cast<MonitorElement*>(mo2->getObject()->Clone());
        tmp->Add(mo1->getObject());
	if (isMEvalid(nodeME, "CSC_Unpacked_with_warnings", mo3)) {
                tmp->Add(mo3->getObject(), -1);
        }
        mo->getObject()->Divide(mo1->getObject(), tmp);
        delete tmp;


    }


} 
