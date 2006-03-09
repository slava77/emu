#include "EmuLocalPlotter.h"

//	Filling of common histograms
void EmuLocalPlotter::fill(unsigned char * data, int dataLength, unsigned short errorStat) {

	map<string, TH1*> h = histos[0];
	if (fSaveHistos) save(HistoFile.c_str());

	if(fill_histo) h["hist/hDDU_Buffer_Size"]->Fill(dataLength);
	nEvents = nEvents +1;
	if(debug_printout) cout << "D**EmuFillCommon> event #" << dec << nEvents << "> BEGINNING OF EVENT :-) Buffer size = " << dec << dataLength << endl;

//************CHECK ERRORS IN EVENT START**************************************************************************************************
//	Check DDU Readout Error Status
	if(debug_printout) cout << "D**EmuFillCommon> event #" << dec << nEvents << "> Start error checking" << endl;
	if(errorStat != 0) {
		if(error_printout) {
			cout << "E**EmuFillCommon> event #" << dec << nEvents << "> Nonzero DDU Readout Error Status is observed: 0x" << hex << errorStat << " (";
			for (int i=0; i<16; i++) cout << ((errorStat>>i) & 0x1);
			cout << ")" << endl;
        	}
		if(fill_histo) {
			if(error_printout) cout << "E**EmuFillCommon> event #" << dec << nEvents << "> Filling histogram of DDU Readout Error Status" << endl;
			for (int i=0; i<16; i++) if ((errorStat>>i) & 0x1) h["hist/hDDU_Readout_Errors"]->Fill(0.,i);
	       	}
	}
	else {
		if(debug_printout) cout << "D**EmuFillCommon> event #" << dec << nEvents << "> DDU Readout Error Status is OK: 0x" << hex << errorStat << endl;
	}

//	Binary check of the buffer
	unsigned long BinaryErrorStatus = 0, BinaryWarningStatus = 0;
	if(check_bin_error){
//KK
		if(debug_printout) cout << "D**EmuFillCommon> event #" << dec << nEvents << "> Start binary checking of buffer...";
		const unsigned short *tmp = reinterpret_cast<const unsigned short *>(data);
		if( bin_checker.check(tmp,dataLength/sizeof(short)) < 0 ){
//			No ddu trailer found - force checker to summarize errors by adding artificial trailer
			const unsigned short dduTrailer[4] = { 0x8000, 0x8000, 0xFFFF, 0x8000 };
			tmp = dduTrailer; bin_checker.check(tmp,long(4));
		}
//KK end
		BinaryErrorStatus   = bin_checker.errors();
		BinaryWarningStatus = bin_checker.warnings();
		if(debug_printout) cout << "Done" << endl;
		if(BinaryErrorStatus != 0) {
			if(error_printout) {
				cout << "E**EmuFillCommon> event #" << dec << nEvents << "> Nonzero Binary ERROR Status is observed: 0x" << hex << BinaryErrorStatus << " (";
				for (int i=0; i<bin_checker.nERRORS; i++)  cout<<((BinaryErrorStatus>>i) & 0x1);
				cout << ")" << endl;
        		}
			if(fill_histo) {
				if(error_printout) cout << "E**EmuFillCommon> event #" << dec << nEvents << "> Filling histogram of Binary Error Status" << endl;
				for(int i=0; i<bin_checker.nERRORS; i++) { // run over all errors
					if( bin_checker.error(i) ) h["hist/hDDUBinCheck_Errors"]->Fill(0.,i);
				}
	       		}
		}
		else {
			if(debug_printout) cout << "D**EmuFillCommon> event #" << dec << nEvents << "> DDU Binary Error Status is OK: 0x" << hex << BinaryErrorStatus << endl;
		}
		if(BinaryWarningStatus != 0) {
			if(error_printout) {
				cout << "W**EmuFillCommon> event #" << dec << nEvents << "> Nonzero Binary Warning Status is observed: 0x" << hex << BinaryWarningStatus << " (";
				for (int i=0; i<bin_checker.nWARNINGS; i++)  cout<<((BinaryWarningStatus>>i) & 0x1);
				cout << ")" << endl;
        		}
			if(fill_histo) {
				if(error_printout) cout << "W**EmuFillCommon> event #" << dec << nEvents << "> Filling histogram of Binary Warning Status" << endl;
				for(int i=0; i<bin_checker.nWARNINGS; i++) { // run over all warnings
					if( bin_checker.warning(i) ) h["hist/hDDUBinCheck_Warnings"]->Fill(0.,i);
				}
	       		}
		}
		else {
			if(debug_printout) cout << "D**EmuFillCommon> event #" << dec <<nEvents<< "> DDU Binary Warning Status is OK: 0x" << hex << BinaryWarningStatus << endl;
		}
	}
	else if(debug_printout) cout << "D**EmuFillCommon> event #" << dec << nEvents << "> Binary checking skipped";

//	if any error
	if( BinaryErrorStatus != 0 ) {
		if(fill_histo) h["hist/hDDU_Data_Format_Check_vs_nEvents"]->Fill(nEvents,2.0);
	} else {
		if(fill_histo) h["hist/hDDU_Data_Format_Check_vs_nEvents"]->Fill(nEvents,0.0);
	}
//	if any warnings
	if( BinaryWarningStatus != 0 ) {
		if(fill_histo) h["hist/hDDU_Data_Format_Check_vs_nEvents"]->Fill(nEvents,1.0);
	}
	h["hist/hDDU_Data_Format_Check_vs_nEvents"]->SetAxisRange(0, nEvents, "X");
	if(debug_printout) cout << "D**EmuFillCommon> event #" << dec << nEvents << "> Error checking has been done" << endl;

//	Accept or deny event
	bool EventDenied = false;
//	Accept or deny event according to DDU Readout Error and dduCheckMask
	if (((unsigned long)errorStat & dduCheckMask) > 0) {
		if (error_printout) cout << "E**EmuFillCommon> event #" << dec << nEvents << "> Event skiped because of DDU Readout Error" << endl;
		EventDenied = true;
	}
//	Accept or deny event according to Binary Error and binCheckMask
	if ((BinaryErrorStatus & binCheckMask)>0) {
		if (error_printout) cout << "E**EmuFillCommon> event #" << dec << nEvents << "> Event skiped because of Binary Error" << endl;
		EventDenied = true;
	}
	if(EventDenied) return;
	else		if(debug_printout) cout << "D**EmuFillCommon> event #" << dec << nEvents << "> Event accepted" << endl;
//************CHECK ERRORS IN EVENT END**************************************************************************************************

//************START DDU UNPACKING********************************************************************************************************
	// CSCDDUTrailer::setDDU2004((ddu2004)?true:false);
        // CSCDMBHeader::setDDU2004((ddu2004)?true:false);
	// CSCDDUEventData::setDebug(true);
	CSCDDUEventData dduData((unsigned short *) data);
	
	CSCDDUHeader dduHeader;
	CSCDDUTrailer dduTrailer;

	if(unpack_ddu) {
		dduHeader  = dduData.header();
		dduTrailer = dduData.trailer();
		if(debug_printout) cout << "D**EmuFillCommon> event #" << dec << nEvents << "> Start DDU unpacking" << endl;
	}
	else    if(debug_printout) cout << "D**EmuFillCommon> event #" << dec << nEvents << "> DDU unpacking is skiped" << endl;

//	Check binary Error status at DDU Trailer
	unsigned int trl_errorstat = 0x0;
	if(unpack_ddu) {
		trl_errorstat     = dduTrailer.errorstat();
	}
	if(debug_printout) {
		cout << "D**EmuFillCommon> event #" << dec << nEvents << "> DDU Trailer Error Status = 0x" << hex << trl_errorstat << endl;
	}
	if(fill_histo) {
		for (int i=0; i<32; i++) {
			if ((trl_errorstat>>i) & 0x1) {
				h["hist/hDDU_Trailer_ErrorStat_Rate"]->Fill(i);
				h["hist/hDDU_Trailer_ErrorStat_Occupancy"]->SetBinContent(i+1, 100.0*(h["hist/hDDU_Trailer_ErrorStat_Rate"]->GetBinContent(i+1))/nEvents);
				h["hist/hDDU_Trailer_ErrorStat_Table"]->Fill(0.,i);
				h["hist/hDDU_Trailer_ErrorStat_vs_nEvents"]->Fill(nEvents, i);
			}
		}
	h["hist/hDDU_Trailer_ErrorStat_Table"]->SetEntries(nEvents);
	h["hist/hDDU_Trailer_ErrorStat_Occupancy"]->SetEntries(nEvents);
	h["hist/hDDU_Trailer_ErrorStat_vs_nEvents"]->SetEntries(nEvents);
	h["hist/hDDU_Trailer_ErrorStat_vs_nEvents"]->SetAxisRange(0, nEvents, "X");
	}

//	DDU word counter
	int trl_word_count = 0;
	if(unpack_ddu) {
		trl_word_count = dduTrailer.wordcount();
	}
	if(fill_histo) 	h["hist/hDDU_Word_Count"]->Fill(trl_word_count );
	if(debug_printout)  cout << "D**EmuFillCommon> event #" << dec << nEvents << "> DDU Trailer Word (64 bits) Count = " << dec << trl_word_count << endl;

//	DDU Header banch crossing number (BXN)
	if(unpack_ddu) {
		BXN=dduHeader.bxnum();
	}
	if(debug_printout)  cout << "D**EmuFillCommon> event #" << dec << nEvents << "> DDU Header BXN Number = " << dec << BXN << endl;
	if(fill_histo) h["hist/hDDU_BXN"]->Fill(BXN);

//	L1A number from DDU Header
	int L1ANumber_previous_event = L1ANumber;
	if(unpack_ddu) {
		L1ANumber = (int)(dduHeader.lvl1num());
	}
	if(debug_printout)  cout << "D**EmuFillCommon> event #" << dec << nEvents << "> DDU Header L1A Number = " << dec << L1ANumber << endl;
	if(fill_histo) {
		h["hist/hDDU_L1A_Increment"]->Fill(L1ANumber - L1ANumber_previous_event);
		if(L1ANumber - L1ANumber_previous_event == 0) {
			h["hist/hDDU_L1A_Increment_vs_nEvents"]->Fill((int)(nEvents), 0.0);
		}
		if(L1ANumber - L1ANumber_previous_event == 1) {
			h["hist/hDDU_L1A_Increment_vs_nEvents"]->Fill((int)(nEvents), 1.0);
		}
		if(L1ANumber - L1ANumber_previous_event > 1) {
			h["hist/hDDU_L1A_Increment_vs_nEvents"]->Fill((int)(nEvents), 2.0);
		}
		h["hist/hDDU_L1A_Increment_vs_nEvents"]->SetAxisRange(0, nEvents, "X");
	}

//	Occupancy and number of DMB (CSC) with Data available (DAV) in header of particular DDU
	int dmb_dav_header      = 0;
	int dmb_dav_header_cnt  = 0;
//KK
	int ddu_connected_inputs= 0;
	int csc_error_state     = 0;
	int csc_warning_state   = 0;
//KK end

//	Number of active DMB (CSC) in header of particular DDU
	int dmb_active_header   = 0;
	if(unpack_ddu) {
		dmb_dav_header     = dduHeader.dmb_dav();
		dmb_active_header  = (int)(dduHeader.ncsc()&0xF);
		csc_error_state    = dduTrailer.dmb_full();
		csc_warning_state  = dduTrailer.dmb_warn();
		ddu_connected_inputs=dduHeader.live_cscs();
	}
	if(debug_printout) {
		if(debug_printout)  cout << "D**EmuFillCommon> event #" << dec << nEvents << "> DDU Header DMB DAV = 0x" << hex << dmb_dav_header << endl;
		if(debug_printout)  cout << "D**EmuFillCommon> event #" << dec << nEvents << "> DDU Header Number of Active DMB = " << dec << dmb_active_header << endl;
	}
	if(fill_histo) {
		for (int i=0; i<16; ++i) {
			if ((dmb_dav_header>>i) & 0x1) {
				dmb_dav_header_cnt = dmb_dav_header_cnt + 1;
				h["hist/hDDU_DMB_DAV_Header_Occupancy_Rate"]->Fill(i);
				h["hist/hDDU_DMB_DAV_Header_Occupancy"]->SetBinContent(i+1, 100.0*(h["hist/hDDU_DMB_DAV_Header_Occupancy_Rate"]->GetBinContent(i+1))/nEvents);
			}
//KK
			if( (ddu_connected_inputs>>i) & 0x1 ){
				h["hist/hDDU_DMB_Connected_Inputs_Rate"]->Fill(i);
				h["hist/hDDU_DMB_Connected_Inputs"]->SetBinContent(i+1, 100.0*(h["hist/hDDU_DMB_Connected_Inputs_Rate"]->GetBinContent(i+1))/nEvents);

			}
			if( (csc_error_state>>i) & 0x1 ){
                                h["hist/hDDU_CSC_Errors_Rate"]->Fill(i);
                                h["hist/hDDU_CSC_Errors"]->SetBinContent(i+1, 100.0*(h["hist/hDDU_CSC_Errors_Rate"]->GetBinContent(i+1))/nEvents);
			}
			if( (csc_warning_state>>i) & 0x1 ){
                                h["hist/hDDU_CSC_Warnings_Rate"]->Fill(i);
                                h["hist/hDDU_CSC_Warnings"]->SetBinContent(i+1, 100.0*(h["hist/hDDU_CSC_Warnings_Rate"]->GetBinContent(i+1))/nEvents);
			}
//KK end
		}
		h["hist/hDDU_DMB_DAV_Header_Occupancy"]->SetEntries(nEvents);
//KK
		h["hist/hDDU_DMB_Connected_Inputs"]->SetEntries(nEvents);
		h["hist/hDDU_CSC_Errors"]->SetEntries(nEvents);
		h["hist/hDDU_CSC_Warnings"]->SetEntries(nEvents);
//KK end
		h["hist/hDDU_DMB_Active_Header_Count"]->Fill(dmb_active_header);
		h["hist/hDDU_DMB_DAV_Header_Count_vs_DMB_Active_Header_Count"]->Fill(dmb_active_header,dmb_dav_header_cnt);
	}

//	Unpack all founded CSC
	vector<CSCEventData> chamberDatas;
	if(unpack_ddu) {
		chamberDatas = dduData.cscData();
	}
//	Unpack DMB for each particular CSC
	int unpacked_dmb_cnt = 0;
	if(unpack_dmb) {
		for(vector<CSCEventData>::iterator chamberDataItr = chamberDatas.begin(); chamberDataItr != chamberDatas.end(); ++chamberDataItr) {
			unpacked_dmb_cnt=unpacked_dmb_cnt+1;
			if(debug_printout) {
				cout << "D**EmuFillCommon> event #" << dec << nEvents << "> Found DMB " << dec << unpacked_dmb_cnt << ". Run unpacking procedure..." << endl;
			}
			fill(*chamberDataItr);
			if(debug_printout) {
				cout << "D**EmuFillCommon> event #" << dec << nEvents << "> Unpacking procedure for DMB " << dec << unpacked_dmb_cnt << " finished" << endl;
			}
		}
	}
	if(debug_printout) cout << "D**EmuFillCommon> event #" << dec << nEvents << "> Total number of unpacked DMB = " << dec << unpacked_dmb_cnt << endl;

	if(fill_histo) {
		h["hist/hDDU_DMB_unpacked_vs_DAV"]->Fill(dmb_active_header, unpacked_dmb_cnt);
		if(dmb_active_header == unpacked_dmb_cnt) {
			h["hist/hDDU_Unpacking_Match_vs_nEvents"]->Fill(nEvents, 0.0);
		}
		else {
			h["hist/hDDU_Unpacking_Match_vs_nEvents"]->Fill(nEvents, 1.0);
		}
		h["hist/hDDU_Unpacking_Match_vs_nEvents"]->SetAxisRange(0, nEvents, "X");
	}
	if(debug_printout) cout << "D**EmuFillCommon> event #" << dec << nEvents << "> END OF EVENT :-(" << endl;
}

