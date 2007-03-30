#include "EmuPlotter.h"

void EmuPlotter::processEvent(const char * data, int32_t dataSize, uint32_t errorStat, int32_t nodeNumber)
{
  //	LOG4CPLUS_INFO(logger_ , "processing event data");
  
  string nodeTag = Form("EMU_%d",nodeNumber); // == This emuMonitor node number
  std::map<std::string, ME_List >::iterator itr;
  ME_List nodeME; // === Global histos specific for this emuMonitor node
  EmuMonitoringObject *mo = NULL;  // == pointer to MonitoringObject
  unpackedDMBcount = 0; 
  

  // == Check and book global node specific histos
  if (MEs.size() == 0 || ((itr = MEs.find(nodeTag)) == MEs.end())) {
    LOG4CPLUS_INFO(logger_, " List of MEs for " << nodeTag << " not found. Booking...")
      fBusy = true;
      MEs[nodeTag] = bookCommon(nodeNumber);
      MECanvases[nodeTag] = bookCommonCanvases(nodeNumber);
      printMECollection(MEs[nodeTag]);
      fBusy = false;
  }
	
  nodeME = MEs[nodeTag];
  
  // dduME = &commonMEfactory;
  nEvents = nEvents+1;

  if (isMEvalid(nodeME, "Buffer_Size", mo)) mo->Fill(dataSize);

  // ==     Check DDU Readout Error Status
  if (isMEvalid(nodeME, "Readout_Errors", mo)) { 
    if(errorStat != 0) {
      LOG4CPLUS_WARN(logger_,nodeTag << " Non-zero Readout Error Status is observed: 0x" << std::hex << errorStat << " mask 0x" << dduCheckMask);
      for (int i=0; i<16; i++) if ((errorStat>>i) & 0x1) mo->Fill(0.,i);
    }
    else {
      LOG4CPLUS_INFO(logger_,nodeTag << " Readout Error Status is OK: 0x" << std::hex << errorStat);
    }
  }



  //	Binary check of the buffer
  uint32_t BinaryErrorStatus = 0, BinaryWarningStatus = 0;
  // if(check_bin_error){
  LOG4CPLUS_INFO(logger_,nodeTag << " Start binary checking of buffer...");
  const uint16_t *tmp = reinterpret_cast<const uint16_t *>(data);
  if( bin_checker.check(tmp,dataSize/sizeof(short)) < 0 ){
    //   No ddu trailer found - force checker to summarize errors by adding artificial trailer
    const uint16_t dduTrailer[4] = { 0x8000, 0x8000, 0xFFFF, 0x8000 };
    tmp = dduTrailer; bin_checker.check(tmp,uint32_t(4));
  }

  BinaryErrorStatus   = bin_checker.errors();
  BinaryWarningStatus = bin_checker.warnings();
  LOG4CPLUS_INFO(logger_,nodeTag <<" Done");
  if(BinaryErrorStatus != 0) {

    LOG4CPLUS_WARN(logger_,nodeTag << " Nonzero Binary Errors Status is observed: 0x" << std::hex << BinaryErrorStatus << " mask: 0x" << binCheckMask);

    if (isMEvalid(nodeME, "BinaryChecker_Errors", mo)) {
      for(int i=0; i<bin_checker.nERRORS; i++) { // run over all errors
	if( bin_checker.error(i) ) mo->Fill(0.,i);
      }
    }

  }
  else {
    LOG4CPLUS_INFO(logger_,nodeTag << " Binary Error Status is OK: 0x" << hex << BinaryErrorStatus);
  }


  if(BinaryWarningStatus != 0) {
    LOG4CPLUS_WARN(logger_,nodeTag << " Nonzero Binary Warnings Status is observed: 0x" 
		   << hex << BinaryWarningStatus)
      if (isMEvalid(nodeME, "BinaryChecker_Warnings", mo)) {
	for(int i=0; i<bin_checker.nWARNINGS; i++) { // run over all warnings
	  if( bin_checker.warning(i) ) mo->Fill(0.,i);
	}
      }

  }
  else {
    LOG4CPLUS_INFO(logger_,nodeTag << " Binary Warnings Status is OK: 0x" << hex << BinaryWarningStatus);

  }

  //  }
  // else LOG4CPLUS_INFO(logger_,dduTag << " Binary checking skipped");

  //	if any error
  if (isMEvalid(nodeME, "Data_Format_Check_vs_nEvents", mo)) {
    if( BinaryErrorStatus != 0 ) {
      mo->Fill(nEvents,2.0);
    } else {
      mo->Fill(nEvents,0.0);
    }
 
    //	if any warnings
    if( BinaryWarningStatus != 0 ) {
      mo->Fill(nEvents,1.0);
    }
    mo->SetAxisRange(0, nEvents, "X");
    LOG4CPLUS_INFO(logger_,nodeTag << " Error checking has been done");
  }

  //	Accept or deny event
  bool EventDenied = false;
  //	Accept or deny event according to DDU Readout Error and dduCheckMask
  if (((uint32_t)errorStat & dduCheckMask) > 0) {
    LOG4CPLUS_WARN(logger_,nodeTag << "Event skiped because of DDU Readout Error");
    EventDenied = true;
  }
  //	Accept or deny event according to Binary Error and binCheckMask
  if ((BinaryErrorStatus != 0) || (BinaryWarningStatus != 0)) {
    fillChamberBinCheck();
  }    
  if ((BinaryErrorStatus & binCheckMask)>0) {
    LOG4CPLUS_WARN(logger_,nodeTag << " Event skiped because of Binary Error");
    EventDenied = true;
  }
  if(EventDenied) return;
  else LOG4CPLUS_INFO(logger_,nodeTag << " Event is accepted");




  // CSCDDUEventData::setDebug(true);
  int dduID = 0;
  CSCDDUEventData dduData((uint16_t *) data);

  CSCDDUHeader dduHeader  = dduData.header();
  CSCDDUTrailer dduTrailer = dduData.trailer();
  
  dduID = dduHeader.source_id();
  
  if (isMEvalid(nodeME, "Source_ID", mo)) mo->Fill(dduID);



  string dduTag = Form("DDU_%d",dduID);

  if (MEs.size() == 0 || ((itr = MEs.find(dduTag)) == MEs.end())) {
    LOG4CPLUS_INFO(logger_, " List of MEs for " << dduTag << " not found. Booking...")
      fBusy = true;
      MEs[dduTag] = bookDDU(dduID);
      MECanvases[dduTag] = bookDDUCanvases(dduID);
      printMECollection(MEs[dduTag]);
      fBusy = false;
  }

  ME_List& dduME = MEs[dduTag];



  LOG4CPLUS_INFO(logger_,"Start unpacking " << dduTag);

  // ==     Check binary Error status at DDU Trailer
  uint32_t trl_errorstat = dduTrailer.errorstat();
  LOG4CPLUS_INFO(logger_,dduTag << " Trailer Error Status = 0x" << hex << trl_errorstat);
  for (int i=0; i<32; i++) {
    if ((trl_errorstat>>i) & 0x1) {
      if (isMEvalid(dduME,"Trailer_ErrorStat_Rate", mo)) { 
	mo->Fill(i);
	double freq = (100.0*mo->GetBinContent(i+1))/nEvents;
	if (isMEvalid(dduME, "Trailer_ErrorStat_Frequency", mo)) mo->SetBinContent(i+1, freq);
      }
      if (isMEvalid(dduME, "Trailer_ErrorStat_Table", mo)) mo->Fill(0.,i);
      if (isMEvalid(dduME, "Trailer_ErrorStat_vs_nEvents", mo)) mo->Fill(nEvents, i);
    }
  }
	
  if (isMEvalid(dduME,"Trailer_ErrorStat_Table", mo)) mo->SetEntries(nEvents);
  if (isMEvalid(dduME,"Trailer_ErrorStat_Frequency", mo)) mo->SetEntries(nEvents);
  if (isMEvalid(dduME,"Trailer_ErrorStat_vs_nEvents", mo)) { 
    mo->SetEntries(nEvents);
    mo->SetAxisRange(0, nEvents, "X");
  }


  if (isMEvalid(dduME, "Buffer_Size", mo)) mo->Fill(dataSize);
  // ==     DDU word counter
  int trl_word_count = 0;
  trl_word_count = dduTrailer.wordcount();
  if (isMEvalid(dduME, "Word_Count", mo)) mo->Fill(trl_word_count );
  LOG4CPLUS_INFO(logger_,dduTag << " Trailer Word (64 bits) Count = " << dec << trl_word_count);

  // ==     DDU Header banch crossing number (BXN)
  BXN=dduHeader.bxnum();
  LOG4CPLUS_INFO(logger_,dduTag << " DDU Header BXN Number = " << dec << BXN);
  if (isMEvalid(dduME, "BXN", mo)) mo->Fill((double)BXN);

  // ==     L1A number from DDU Header
  int L1ANumber_previous_event = L1ANumber;
  L1ANumber = (int)(dduHeader.lvl1num());
  LOG4CPLUS_INFO(logger_,dduTag << " Header L1A Number = " << dec << L1ANumber);
  if (isMEvalid(dduME, "L1A_Increment", mo)) dduME["L1A_Increment"]->Fill(L1ANumber - L1ANumber_previous_event);

  if (isMEvalid(dduME, "L1A_Increment_vs_nEvents", mo)) {
    if(L1ANumber - L1ANumber_previous_event == 0) {
      mo->Fill((double)(nEvents), 0.0);
    }
    if(L1ANumber - L1ANumber_previous_event == 1) {
      mo->Fill((double)(nEvents), 1.0);
    }
    if(L1ANumber - L1ANumber_previous_event > 1) {
      mo->Fill((double)(nEvents), 2.0);

    }
    mo->SetAxisRange(0, nEvents, "X");
  }

  // ==     Occupancy and number of DMB (CSC) with Data available (DAV) in header of particular DDU
  int dmb_dav_header      = 0;
  int dmb_dav_header_cnt  = 0;

  int ddu_connected_inputs= 0;
  int csc_error_state     = 0;
  int csc_warning_state   = 0;

  //  ==    Number of active DMB (CSC) in header of particular DDU
  int dmb_active_header   = 0;

  dmb_dav_header     = dduHeader.dmb_dav();
  dmb_active_header  = (int)(dduHeader.ncsc()&0xF);
  csc_error_state    = dduTrailer.dmb_full();
  csc_warning_state  = dduTrailer.dmb_warn();
  ddu_connected_inputs=dduHeader.live_cscs();


  LOG4CPLUS_INFO(logger_,dduTag << " Header DMB DAV = 0x" << hex << dmb_dav_header);
  LOG4CPLUS_INFO(logger_,dduTag << " Header Number of Active DMB = " << dec << dmb_active_header);


  double freq = 0;
  for (int i=0; i<16; ++i) {
    if ((dmb_dav_header>>i) & 0x1) {
      dmb_dav_header_cnt = dmb_dav_header_cnt + 1;      
      if (isMEvalid(dduME, "DMB_DAV_Header_Occupancy_Rate", mo)) {
	mo->Fill(i);
	freq = 100.0*(mo->GetBinContent(i+1))/nEvents;
        if (isMEvalid(dduME, "DMB_DAV_Header_Occupancy", mo)) mo->SetBinContent(i+1,freq);
      }
    }

    if( (ddu_connected_inputs>>i) & 0x1 ){
      if (isMEvalid(dduME, "DMB_Connected_Inputs_Rate", mo)) {
	mo->Fill(i);
	freq = 100.0*(mo->GetBinContent(i+1))/nEvents;
	if (isMEvalid(dduME, "DMB_Connected_Inputs", mo)) mo->SetBinContent(i+1, freq);
      }
    }
    if( (csc_error_state>>i) & 0x1 ){
      if (isMEvalid(dduME, "CSC_Errors_Rate", mo)) {
	mo->Fill(i);
	freq = 100.0*(mo->GetBinContent(i+1))/nEvents;
	if (isMEvalid(dduME, "CSC_Errors", mo)) mo->SetBinContent(i+1, freq);
      }
    }
    if( (csc_warning_state>>i) & 0x1 ){
      if (isMEvalid(dduME, "CSC_Warnings_Rate", mo)) {
	mo->Fill(i);
	freq = 100.0*(mo->GetBinContent(i+1))/nEvents;
	if (isMEvalid(dduME,"CSC_Warnings", mo)) mo->SetBinContent(i+1, freq);
      }
    }

  }
  if (isMEvalid(dduME,"DMB_DAV_Header_Occupancy",mo)) mo->SetEntries(nEvents);

  if (isMEvalid(dduME, "DMB_Connected_Inputs", mo)) mo->SetEntries(nEvents);
  if (isMEvalid(dduME, "CSC_Errors", mo)) mo->SetEntries(nEvents);
  if (isMEvalid(dduME, "CSC_Warnings", mo)) mo->SetEntries(nEvents);

  if (isMEvalid(dduME, "DMB_Active_Header_Count", mo)) mo->Fill(dmb_active_header);
  if (isMEvalid(dduME, "DMB_DAV_Header_Count_vs_DMB_Active_Header_Count", mo)) mo->Fill(dmb_active_header,dmb_dav_header_cnt);

  //      Unpack all founded CSC
  vector<CSCEventData> chamberDatas;
  chamberDatas = dduData.cscData();
  //      Unpack DMB for each particular CSC
  // int unpacked_dmb_cnt = 0;
  for(vector<CSCEventData>::iterator chamberDataItr = chamberDatas.begin(); chamberDataItr != chamberDatas.end(); ++chamberDataItr) {
    unpackedDMBcount++;
    // unpacked_dmb_cnt=unpacked_dmb_cnt+1;
    LOG4CPLUS_INFO(logger_,
		   "Found DMB " << dec << unpackedDMBcount  << ". Run unpacking procedure...");
    processChamber(*chamberDataItr, nodeNumber, dduID);
    LOG4CPLUS_INFO(logger_,
		   "Unpacking procedure for DMB " << dec << unpackedDMBcount << " finished");
  }
  LOG4CPLUS_INFO(logger_,
		 "Total number of unpacked DMB = " << dec << unpackedDMBcount);

  if (isMEvalid(dduME,"DMB_unpacked_vs_DAV",mo)) mo->Fill(dmb_active_header, unpackedDMBcount);
  if (isMEvalid(nodeME,"Unpacking_Match_vs_nEvents", mo)) {
    if(dmb_active_header == unpackedDMBcount) {
      mo->Fill(nEvents, 0.0);
    }
    else {
      mo->Fill(nEvents, 1.0);
    }
    mo->SetAxisRange(0, nEvents, "X");
  }
  LOG4CPLUS_DEBUG(logger_,
		  "END OF EVENT :-(");


}


void EmuPlotter::fillChamberBinCheck() {
  EmuMonitoringObject* mo = NULL;
  //  if(check_bin_error){
  map<int,uint32_t> checkerErrors = bin_checker.errorsDetailed();
  map<int,uint32_t>::const_iterator chamber = checkerErrors.begin();
  while( chamber != checkerErrors.end() ){
    int ChamberID     = chamber->first;
    string cscTag(Form("CSC_%03d_%02d", (chamber->first>>4) & 0xFF, chamber->first & 0xF));
    map<string, ME_List >::iterator h_itr = MEs.find(cscTag);
    if ((((chamber->first>>4) & 0xFF) ==255) || 
        (chamber->second & 0x40)) { chamber++; continue;} // = Skip chamber detection if DMB header is missing (Error code 6)
    if (h_itr == MEs.end() || (MEs.size()==0)) {
      LOG4CPLUS_WARN(logger_,
		     "List of Histos for " << cscTag <<  " not found");
      LOG4CPLUS_INFO(logger_,
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

    if (isMEvalid(cscME, "BinCheck_ErrorStat_Table", mo)) {
      for(int bit=5; bit<19; bit++)
	if( chamber->second & (1<<bit) ) {
	  mo->Fill(0.,bit-5);
	  double freq = (100.0*mo->GetBinContent(bit-4))/nDMBEvents[cscTag];
	  if (isMEvalid(cscME, "BinCheck_ErrorStat_Frequency", mo)) mo->SetBinContent(bit-4, freq);
	}
    }
    chamber++;
  }
  map<int,uint32_t> checkerWarnings  = bin_checker.warningsDetailed();
  chamber = checkerWarnings.begin();
  while( chamber != checkerWarnings.end() ){
    int ChamberID     = chamber->first;
    string cscTag(Form("CSC_%03d_%02d", (chamber->first>>4) & 0xFF, chamber->first & 0xF));
    if (((chamber->first>>4) & 0xFF) ==255) {chamber++; continue;}
    map<string, ME_List >::iterator h_itr = MEs.find(cscTag);
    if (h_itr == MEs.end() || (MEs.size()==0)) {
      LOG4CPLUS_WARN(logger_,
		     "List of Histos for " << cscTag <<  " not found");
      LOG4CPLUS_INFO(logger_,
		     "Booking Histos for " << cscTag);
      fBusy = true;
      MEs[cscTag] = bookChamber(ChamberID);
      MECanvases[cscTag] = bookChamberCanvases(ChamberID);
      printMECollection(MEs[cscTag]);
      fBusy = false;
    }
    ME_List& cscME = MEs[cscTag];
    if (isMEvalid(cscME, "BinCheck_WarningStat_Table", mo)) {
      for(int bit=1; bit<2; bit++)
	if( chamber->second & (1<<bit) ) {
	  mo->Fill(0.,bit-1);
	  double freq = (100.0*mo->GetBinContent(bit))/nDMBEvents[cscTag];
	  if (isMEvalid(cscME, "BinCheck_WarningStat_Frequency", mo)) mo->SetBinContent(bit, freq);
	}
    }
    chamber++;

  }
  // }
}
		     
