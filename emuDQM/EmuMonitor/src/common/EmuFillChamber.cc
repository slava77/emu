#include "EmuLocalPlotter.h"
#include "CSCStripClusterFinder.h"
#include <TF1.h>

//	Filling of chamber's histogram
void EmuLocalPlotter::fill(const CSCEventData& data, int dduID=0) {
	stringstream stname;
	string hname;
	int i;
	int chamb = 0;
	int FEBunpacked = 0;
	int alct_unpacked = 0;
	int tmb_unpacked  = 0;
	int cfeb_unpacked = 0;
	
	int alct_keywg = -1;
	int clct_kewdistrip = -1;

	if (&data==0) {
		if(error_printout) {
			LOG4CPLUS_DEBUG(logger_, "E**EmuFillChamber> #" << dec << nEvents << " Zero pointer. DMB data are not available for unpacking"); //KK is->are
		}
		return;
	} else {
		if(debug_printout) {
			LOG4CPLUS_DEBUG(logger_, "D**EmuFillChamber> #" << dec << nEvents << " Nonzero pointer. DMB data are available for unpacking"); //KK is->are
		}
	}

//	DMB Found
//	Unpacking of DMB Header and trailer
	CSCDMBHeader  dmbHeader;
	CSCDMBTrailer dmbTrailer;
	if(unpack_dmb) {
		if(debug_printout) {
			LOG4CPLUS_DEBUG(logger_,"D**EmuFillChamber> #" << dec << nEvents << "> Unpacking of DMB Header and Trailer ... ");
		}
		dmbHeader  = data.dmbHeader();
		dmbTrailer = data.dmbTrailer();
		if(debug_printout) LOG4CPLUS_DEBUG(logger_, "Done.");
	}

//	Unpacking of Chamber Identification number
	int crateID	= 0xFF;
	int dmbID	= 0xF;
	int ChamberID	= 0xFFF;
	
	if(unpack_data) {
		if(debug_printout) {
			LOG4CPLUS_DEBUG(logger_, "D**EmuFillChamber> #" << dec << nEvents << "> Unpacking of Chamber ID ... ");
		}
		crateID	  = dmbHeader.crateID();
		dmbID     = dmbHeader.dmbID();
		ChamberID = (((crateID) << 4) + dmbID) & 0xFFF;

		if(debug_printout) {
			LOG4CPLUS_DEBUG(logger_, "Done");
			LOG4CPLUS_DEBUG(logger_, "D**EmuFillChamber> #" << dec << nEvents << "> Chamber ID = "<< ChamberID << " Crate ID = "<< crateID << " DMB ID = " << dmbID);
		}
	}
	nDMBEvents[ChamberID]++;

	string CSCTag(Form("_CSC_%03d_%02d_", crateID, dmbID));
//	Creating list of histograms for the particular chamber
	map<int, map<string,TH1*> >::iterator h_itr = histos.find(ChamberID);
	if (h_itr == histos.end() || (histos.size()==0)) {
		if(debug_printout) {
			LOG4CPLUS_DEBUG(logger_,"D**EmuFillChamber> #" << dec << nEvents << "> ch" << crateID << ":" << dmbID << ">" << " List of Histos for chamber not found");
		}
		if(fill_histo) {
			if(debug_printout) {
				LOG4CPLUS_DEBUG(logger_, "D**EmuFillChamber> #" << dec << nEvents << "> ch" << crateID << ":" << dmbID << ">" 
				<< " Creating of list of Histos for the chamber ...");
			}
			histos[ChamberID] = book_chamber(ChamberID);
			if(debug_printout) {
				LOG4CPLUS_DEBUG(logger_, "D**EmuFillChamber> #" << dec << nEvents << "> ch" << crateID << ":" << dmbID << "> Done.");
    			}
  		}
	}
	
	map<string, TH1*> h_gen = histos[0];
	map<string, TH1*> h = histos[ChamberID];

//	Efficiency of the chamber
	float DMBEff = float(nDMBEvents[ChamberID])/float(nEvents);
	if(DMBEff > 1.0) {
		if(debug_printout) LOG4CPLUS_DEBUG(logger_, "+++debug> ERROR: Chamber (ID=" << ChamberID  << ") has efficiency " << DMBEff << " which is greater than 1");
	}
	
//	Unpacking L1A number from DMB header
	int dmbHeaderL1A 	= 0;
	int dmb_ddu_l1a_diff 	= 0;
	if(unpack_data) {
    		dmbHeaderL1A = dmbHeader.l1a();
//		Calculation difference between L1A numbers from DDU and DMB
		dmb_ddu_l1a_diff = (int)(dmbHeaderL1A-(int)(L1ANumber&0xFF));
		if(debug_printout) LOG4CPLUS_DEBUG(logger_, "+++debug> DMB(ID=" << ChamberID  << ") L1A = " << dmbHeaderL1A << " : DMB L1A - DDU L1A = " << dmb_ddu_l1a_diff);
	}
	
	if(fill_histo) {
		hname = Form("hist/h%sDMB_L1A_Distrib", CSCTag.c_str());
		h[hname]->Fill(dmbHeaderL1A);

		hname = Form("hist/h%sDMB_DDU_L1A_diff", CSCTag.c_str());
		if(dmb_ddu_l1a_diff < -128) {
			h[hname]->Fill(dmb_ddu_l1a_diff + 256);
		} else {
			if(dmb_ddu_l1a_diff > 128)  h[hname]->Fill(dmb_ddu_l1a_diff - 256);
			else h[hname]->Fill(dmb_ddu_l1a_diff);
		}
		h[hname]->SetAxisRange(0.1, 1.1*(1.0+h[hname]->GetBinContent(h[hname]->GetMaximumBin())), "Y");

		hname = Form("hist/h%sDMB_L1A_vs_DDU_L1A", CSCTag.c_str());
		h[hname]->Fill((int)(L1ANumber&0xFF), (int)dmbHeaderL1A);
	}
	
//KK
	if(check_bin_error){
		map<int,long> checkerErrors = bin_checker.errorsDetailed();
		map<int,long>::const_iterator chamber = checkerErrors.begin();
		while( chamber != checkerErrors.end() ){
			string cscname(Form("_CSC_%d_%d_", (chamber->first>>4) & 0xFF, chamber->first & 0xF)); 
			hname = Form("hist/h%sBinCheck_ErrorStat_Table", cscname.c_str());
			for(int bit=5; bit<19; bit++){
///KK 				it should be:
///				for(int bit=5; bit<24; bit++){
///				if( bit==19 || bit==20 ) continue;
///				But I don't know where booking is now.
				if( chamber->second & (1<<bit) ) {
					if( histos[chamber->first].find(hname) != histos[chamber->first].end() ) {
						histos[chamber->first][hname]->Fill(0.,bit-5);
					} else {
						LOG4CPLUS_DEBUG(logger_," Error: unknown histogram "<<hname);
					}
				}
			}
			chamber++;
		}
		
		map<int,long> checkerWarnings  = bin_checker.warningsDetailed();
		chamber = checkerWarnings.begin();
		while( chamber != checkerWarnings.end() ){
			string cscname(Form("_CSC_%d_%d_", (chamber->first>>4) & 0xFF, chamber->first & 0xF));
			hname = Form("hist/h%sBinCheck_WarningStat_Table", cscname.c_str());
			for(int bit=1; bit<2; bit++) {
	  			if( chamber->second & (1<<bit) ) {
	    				if( histos[chamber->first].find(hname) != histos[chamber->first].end() ) {
						histos[chamber->first][hname]->Fill(0.,bit-1);
					} else {
						LOG4CPLUS_DEBUG(logger_," Error: unknown histogram "<<hname);
					}
				}
			}
			chamber++;
		}
	}
//KK end

//	Unpacking BXN number from DMB header
	int dmbHeaderBXN 	= 0;
	int dmb_ddu_bxn_diff	= 0;
	if(unpack_data) {
		dmbHeaderBXN = dmbHeader.bxn();
//		Calculation difference between BXN numbers from DDU and DMB
		dmb_ddu_bxn_diff = (int)(dmbHeaderBXN-(int)(BXN&0x7F));
		if(debug_printout) LOG4CPLUS_DEBUG(logger_, "+++debug> DMB(ID=" << ChamberID  << ") BXN = " << dmbHeaderBXN << " : DMB BXN - DDU BXN = " << dmb_ddu_bxn_diff);
	}
	if(fill_histo) {
		hname = Form("hist/h%sDMB_BXN_Distrib", CSCTag.c_str());
		h[hname]->Fill((int)(dmbHeader.bxn()));

		hname = Form("hist/h%sDMB_DDU_BXN_diff", CSCTag.c_str());
		if(dmb_ddu_bxn_diff < -64) {
			h[hname]->Fill(dmb_ddu_bxn_diff + 128);
		} else {
			if(dmb_ddu_bxn_diff > 64)   h[hname]->Fill(dmb_ddu_bxn_diff - 128);
			else h[hname]->Fill(dmb_ddu_bxn_diff);
		}
		h[hname]->SetAxisRange(0.1, 1.1*(1.0+h[hname]->GetBinContent(h[hname]->GetMaximumBin())), "Y");

		hname = Form("hist/h%sDMB_BXN_vs_DDU_BXN",CSCTag.c_str());
		h[hname]->Fill((int)(BXN), (int)dmbHeaderBXN);
	}

//	Unpacking CFEB information from DMB header
	int cfeb_dav	  = 0;
	int cfeb_dav_num  = 0;
	int cfeb_movlp	  = 0;
	int dmb_cfeb_sync = 0;
	if(unpack_data) {
		cfeb_dav = (int)dmbHeader.cfebAvailable();
		for (i=0; i<5; i++) cfeb_dav_num = cfeb_dav_num + (int)((cfeb_dav>>i) & 0x1);
		cfeb_movlp    = (int)dmbHeader.cfebMovlp();
		dmb_cfeb_sync = (int)dmbHeader.dmbCfebSync();
	}

	if(fill_histo) {
		hname = Form("hist/h%sDMB_CFEB_DAV", CSCTag.c_str());
		h[hname]->Fill(cfeb_dav);

		hname = Form("hist/h%sDMB_CFEB_DAV_multiplicity", CSCTag.c_str());
		h[hname]->Fill(cfeb_dav_num);

		hname = Form("hist/h%sDMB_CFEB_MOVLP", CSCTag.c_str());
		h[hname]->Fill(cfeb_movlp);

		hname = Form("hist/h%sDMB_CFEB_Sync", CSCTag.c_str());
		h[hname]->Fill(dmb_cfeb_sync);
	}

	h_gen["hist/hCSC_Unpacked"]->Fill(crateID,dmbID);
	h_gen["hist/hCSC_Unpacked"]->SetEntries(nEvents);

	hname = Form("hist/h%sDMB_CFEB_Active", CSCTag.c_str());
//	h[hname]->Fill((dmbTrailer.header_1a>>5)&0x1F); //KK
	h[hname]->Fill(dmbHeader.cfebActive()); //KK

	hname = Form("hist/h%sDMB_CFEB_Active_vs_DAV", CSCTag.c_str());
//	h[hname]->Fill(dmbHeader.cfebAvailable(),(int)((dmbTrailer.header_1a>>5)&0x1F)); //KK
	h[hname]->Fill(dmbHeader.cfebAvailable(),dmbHeader.cfebActive()); //KK

	hname = Form("hist/h%sDMB_L1_Pipe", CSCTag.c_str());
	h[hname]->Fill(dmbTrailer.dmb_l1pipe);

	hname = Form("hist/h%sDMB_FIFO_stats", CSCTag.c_str());
	if (dmbTrailer.tmb_empty  == 1) h[hname]->Fill(1.0, 0.0); //KK
	if (dmbTrailer.tmb_half   == 0) h[hname]->Fill(1.0, 1.0);
	if (dmbTrailer.tmb_full   == 1) h[hname]->Fill(1.0, 2.0); //KK
	if (dmbTrailer.alct_empty == 1) h[hname]->Fill(0.0, 0.0);
	if (dmbTrailer.alct_half  == 0) h[hname]->Fill(0.0, 1.0);
	if (dmbTrailer.alct_full  == 1) h[hname]->Fill(0.0, 2.0); //KK 0->1
	for (int i=0; i<5; i++) {
		if ((int)((dmbTrailer.cfeb_empty>>i)&0x1) == 1) h[hname]->Fill(i+2,0.0);
		if ((int)((dmbTrailer.cfeb_half >>i)&0x1) == 0) h[hname]->Fill(i+2,1);
		if ((int)((dmbTrailer.cfeb_full >>i)&0x1) == 1) h[hname]->Fill(i+2,2);
	}
	h[hname]->SetEntries((int)(float)nDMBEvents[ChamberID]);

	hname = Form("hist/h%sDMB_FEB_Timeouts", CSCTag.c_str());
	if ((dmbTrailer.tmb_timeout==0) && (dmbTrailer.alct_timeout==0) && (dmbTrailer.cfeb_starttimeout==0) && (dmbTrailer.cfeb_endtimeout==0)) {
		h[hname]->Fill(0.0);
	}
	if (dmbTrailer.alct_timeout)    h[hname]->Fill(1);
	if (dmbTrailer.tmb_timeout)     h[hname]->Fill(2);
	if (dmbTrailer.alct_endtimeout) h[hname]->Fill(8); // KK
	if (dmbTrailer.tmb_endtimeout)  h[hname]->Fill(9); // KK
	for (i=0; i<5; i++) {
		if ((dmbTrailer.cfeb_starttimeout>>i) & 0x1) h[hname]->Fill(i+3);
		if ((dmbTrailer.cfeb_endtimeout>>i) & 0x1) h[hname]->Fill(i+10); // KK 8->10
	}
	h[hname]->SetEntries((int)(float)nDMBEvents[ChamberID]);

//	Get FEBs Data Available Info
	int alct_dav  = dmbHeader.nalct();
	int tmb_dav   = dmbHeader.nclct();
	int cfeb_dav2 = 0;
	for (int i=0; i<5; i++) cfeb_dav2 = cfeb_dav2 + (int)((dmbHeader.cfebAvailable()>>i) & 0x1);
	
//	Fill Hisogram for FEB DAV Efficiency
	if(alct_dav  > 0) {
		hname = Form("hist/h%sDMB_FEB_DAV_Rate",CSCTag.c_str());
		h[hname]->Fill(0.0);
		float alct_dav_number   = h[hname]->GetBinContent(1);
		hname = Form("hist/h%sDMB_FEB_DAV_Efficiency", CSCTag.c_str());
		h[hname]->SetBinContent(1, ((float)alct_dav_number/(float)(nDMBEvents[ChamberID])*100.0));
		h[hname]->SetEntries(nDMBEvents[ChamberID]);
		
		hname = Form("hist/h%sCSC_Rate",CSCTag.c_str());
		h[hname]->Fill(0.0);
		hname = Form("hist/h%sCSC_Efficiency", CSCTag.c_str());
		h[hname]->SetBinContent(1, ((float)alct_dav_number/(float)(nDMBEvents[ChamberID])*100.0));
		h[hname]->SetEntries(nDMBEvents[ChamberID]);
	}
	if(tmb_dav   > 0) {
		hname = Form("hist/h%sDMB_FEB_DAV_Rate",CSCTag.c_str());
		h[hname]->Fill(1.0);
		float tmb_dav_number    = h[hname]->GetBinContent(2);
		hname = Form("hist/h%sDMB_FEB_DAV_Efficiency", CSCTag.c_str());
		h[hname]->SetBinContent(2, ((float)tmb_dav_number/(float)(nDMBEvents[ChamberID])*100.0));
		h[hname]->SetEntries(nDMBEvents[ChamberID]);
		
		hname = Form("hist/h%sCSC_Rate",CSCTag.c_str());
		h[hname]->Fill(1.0);
		hname = Form("hist/h%sCSC_Efficiency", CSCTag.c_str());
		h[hname]->SetBinContent(2, ((float)tmb_dav_number/(float)(nDMBEvents[ChamberID])*100.0));
		h[hname]->SetEntries(nDMBEvents[ChamberID]);
	}
	if(cfeb_dav2 > 0) {
		hname = Form("hist/h%sDMB_FEB_DAV_Rate",CSCTag.c_str());
		h[hname]->Fill(2.0);
		float cfeb_dav2_number = h[hname]->GetBinContent(3);
		hname = Form("hist/h%sDMB_FEB_DAV_Efficiency", CSCTag.c_str());
		h[hname]->SetBinContent(3, ((float)cfeb_dav2_number/(float)(nDMBEvents[ChamberID])*100.0));
		h[hname]->SetEntries(nDMBEvents[ChamberID]);
		
		hname = Form("hist/h%sCSC_Rate",CSCTag.c_str());
		h[hname]->Fill(2.0);
		hname = Form("hist/h%sCSC_Efficiency", CSCTag.c_str());
		h[hname]->SetBinContent(3, ((float)cfeb_dav2_number/(float)(nDMBEvents[ChamberID])*100.0));
		h[hname]->SetEntries(nDMBEvents[ChamberID]);
	}

//	Fill Hisogram for Different Combinations of FEB DAV Efficiency	
	float feb_combination_dav = -1.0;
	if(alct_dav == 0 && tmb_dav == 0 && cfeb_dav2 == 0) feb_combination_dav = 0.0;
	if(alct_dav >  0 && tmb_dav == 0 && cfeb_dav2 == 0) feb_combination_dav = 1.0;
	if(alct_dav == 0 && tmb_dav >  0 && cfeb_dav2 == 0) feb_combination_dav = 2.0;
	if(alct_dav == 0 && tmb_dav == 0 && cfeb_dav2 >  0) feb_combination_dav = 3.0;
	if(alct_dav >  0 && tmb_dav >  0 && cfeb_dav2 == 0) feb_combination_dav = 4.0;
	if(alct_dav >  0 && tmb_dav == 0 && cfeb_dav2 >  0) feb_combination_dav = 5.0;
	if(alct_dav == 0 && tmb_dav >  0 && cfeb_dav2 >  0) feb_combination_dav = 6.0;
	if(alct_dav >  0 && tmb_dav >  0 && cfeb_dav2 >  0) feb_combination_dav = 7.0;
	hname = Form("hist/h%sDMB_FEB_Combinations_DAV_Rate",CSCTag.c_str());
	h[hname]->Fill(feb_combination_dav);
	float feb_combination_dav_number = h[hname]->GetBinContent((int)(feb_combination_dav+1.0));
	hname = Form("hist/h%sDMB_FEB_Combinations_DAV_Efficiency",CSCTag.c_str());
	h[hname]->SetBinContent((int)(feb_combination_dav+1.0), ((float)feb_combination_dav_number/(float)(nDMBEvents[ChamberID])*100.0));
	h[hname]->SetEntries(nDMBEvents[ChamberID]);

//***************************
//	ALCT Found
//***************************
	if (data.nalct()) {
		CSCALCTHeader alctHeader = data.alctHeader();
		CSCALCTTrailer alctTrailer = data.alctTrailer();
		CSCAnodeData alctData = data.alctData();
/*
//KK
//	vector<L1MuCSCAnodeLCT> alctsDatas = alctHeader.ALCTs();
	vector<L1MuCSCAnodeLCT> alctsDatas;
	alctsDatas.push_back( L1MuCSCAnodeLCT(alctHeader.alct0Word()) );
	alctsDatas.push_back( L1MuCSCAnodeLCT(alctHeader.alct1Word()) );
//KK end
*/
		vector<CSCALCTDigi> alctsDatasTmp = alctHeader.ALCTDigis();
		vector<CSCALCTDigi> alctsDatas;

		for (int lct=0; lct<alctsDatasTmp.size(); lct++) {
			if (alctsDatasTmp[lct].isValid()) alctsDatas.push_back(alctsDatasTmp[lct]);
		}

		FEBunpacked = FEBunpacked +1;
		alct_unpacked = 1;
		
		hname = Form("hist/h%sDMB_FEB_Unpacked_vs_DAV", CSCTag.c_str());
		if(alct_dav  > 0) {
			h[hname]->Fill(0.0, 0.0);
		}
		
		hname = Form("hist/h%sALCT_L1A", CSCTag.c_str());
		h[hname]->Fill((int)(alctHeader.L1Acc()));

		hname = Form("hist/h%sALCT_DMB_L1A_diff", CSCTag.c_str());
		int alct_dmb_l1a_diff = (int)((dmbHeader.l1a()&0xF)-alctHeader.L1Acc());
		if(alct_dmb_l1a_diff < -8) {
			h[hname]->Fill(alct_dmb_l1a_diff + 16);
		} else {
			if(alct_dmb_l1a_diff > 8) h[hname]->Fill(alct_dmb_l1a_diff - 16);
			else                      h[hname]->Fill(alct_dmb_l1a_diff);
		}
		h[hname]->SetAxisRange(0.1, 1.1*(1.0+h[hname]->GetBinContent(h[hname]->GetMaximumBin())), "Y");

		hname = Form("hist/h%sDMB_L1A_vs_ALCT_L1A", CSCTag.c_str());
		h[hname]->Fill(alctHeader.L1Acc(),dmbHeader.l1a());

		hname = Form("hist/h%sALCT_DMB_BXN_diff", CSCTag.c_str());
		int alct_dmb_bxn_diff = (int)(dmbHeader.bxn()-(alctHeader.BXNCount()&0x7F));
		if(alct_dmb_bxn_diff < -64) {
			h[hname]->Fill(alct_dmb_bxn_diff + 128);
		} else {
			if(alct_dmb_bxn_diff > 64) h[hname]->Fill(alct_dmb_bxn_diff - 128);
 			else                       h[hname]->Fill(alct_dmb_bxn_diff);
		}
		h[hname]->SetAxisRange(0.1, 1.1*(1.0+h[hname]->GetBinContent(h[hname]->GetMaximumBin())), "Y");

		hname = Form("hist/h%sALCT_BXN", CSCTag.c_str());
		h[hname]->Fill(alctHeader.BXNCount());

		hname = Form("hist/h%sALCT_BXN_vs_DMB_BXN", CSCTag.c_str());
		h[hname]->Fill((int)((alctHeader.BXNCount())), (int)(dmbHeader.bxn()));

		hname = Form("hist/h%sALCT_Number_Rate", CSCTag.c_str());
		h[hname]->Fill(alctsDatas.size());
		int nALCT = (int)h[hname]->GetBinContent((int)(alctsDatas.size()+1));

		hname = Form("hist/h%sALCT_Number_Efficiency", CSCTag.c_str());
		h[hname]->SetBinContent((int)(alctsDatas.size()+1), (float)(nALCT)/(float)((float)nDMBEvents[ChamberID])*100.0);

		hname = Form("hist/h%sALCT_Word_Count", CSCTag.c_str());
		h[hname]->Fill((int)(alctTrailer.wordCount()));
		if(debug_printout) LOG4CPLUS_DEBUG(logger_, "+++debug>  ALCT Trailer Word Count = " << dec << (int)alctTrailer.wordCount());

		if (alctsDatas.size()==2) {
			hname = Form("hist/h%sALCT1_vs_ALCT0_KeyWG", CSCTag.c_str());
 			h[hname]->Fill(alctsDatas[0].getKeyWG(),alctsDatas[1].getKeyWG());
		}
		

		for (int lct=0; lct<alctsDatas.size(); lct++) {
			hname = Form("hist/h%sALCT%d_KeyWG", CSCTag.c_str(), lct);
			h[hname]->Fill(alctsDatas[lct].getKeyWG());
			if(lct == 0) alct_keywg  = alctsDatas[lct].getKeyWG();

			int alct_dtime = (int)(alctsDatas[lct].getBX()-(alctHeader.BXNCount()&0x1F));
			hname = Form("hist/h%sALCT%d_dTime", CSCTag.c_str(), lct);
			if(alct_dtime < -16) {
				h[hname]->Fill(alct_dtime + 32);
			} else {
				if(alct_dtime > 16)  h[hname]->Fill(alct_dtime - 32);
				else h[hname]->Fill(alct_dtime);
			}
			h[hname]->SetAxisRange(0.1, 1.1*(1.0+h[hname]->GetBinContent(h[hname]->GetMaximumBin())), "Y");

			hname = Form("hist/h%sALCT%d_dTime_vs_KeyWG", CSCTag.c_str(), lct);
			if(alct_dtime < -16) {
				h[hname]->Fill(alctsDatas[lct].getKeyWG(), alct_dtime + 32);
			} else {
				if(alct_dtime > 16)  	h[hname]->Fill(alctsDatas[lct].getKeyWG(), alct_dtime - 32);
				else 			h[hname]->Fill(alctsDatas[lct].getKeyWG(), alct_dtime);
			}

			hname = Form("hist/h%sALCT%d_dTime_Profile", CSCTag.c_str(), lct);
			if(alct_dtime < -16) {
				h[hname]->Fill(alctsDatas[lct].getKeyWG(), alct_dtime + 32);
			} else {
				if(alct_dtime > 16) h[hname]->Fill(alctsDatas[lct].getKeyWG(), alct_dtime - 32);
				else                h[hname]->Fill(alctsDatas[lct].getKeyWG(), alct_dtime);
			}

			hname = Form("hist/h%sALCT%d_BXN", CSCTag.c_str(), lct);
			h[hname]->Fill(alctsDatas[lct].getBX());

			hname = Form("hist/h%sALCT%d_Quality", CSCTag.c_str(), lct);
			h[hname]->Fill(alctsDatas[lct].getKeyWG(), alctsDatas[lct].getQuality() );

			hname = Form("hist/h%sALCT%d_Quality_Profile", CSCTag.c_str(), lct);
			h[hname]->Fill(alctsDatas[lct].getKeyWG(), alctsDatas[lct].getQuality() );

 			hname = Form("hist/h%sALCT%d_Pattern", CSCTag.c_str(), lct);
			int pattern = (alctsDatas[lct].getAccelerator()<<1) + alctsDatas[lct].getCollisionB();
			int keywg = alctsDatas[lct].getKeyWG();
			h[hname]->Fill(keywg, pattern );
		}
		

		int NumberOfLayersWithHitsInALCT = 0;
		int NumberOfWireGroupsWithHitsInALCT = 0;
		for (int nLayer=1; nLayer<=6; nLayer++) {
			int wg_previous   = -1;
			int tbin_previous = -1;
			bool CheckLayerALCT = true;
			vector<CSCWireDigi> wireDigis = alctData.wireDigis(nLayer);
			for (vector<CSCWireDigi>:: iterator wireDigisItr = wireDigis.begin(); wireDigisItr != wireDigis.end(); ++wireDigisItr) {
				int wg = wireDigisItr->getWireGroup();
				int tbin = wireDigisItr->getBeamCrossingTag();
				if(CheckLayerALCT) {
					NumberOfLayersWithHitsInALCT = NumberOfLayersWithHitsInALCT + 1;
					CheckLayerALCT = false;
				}
				if(wg != wg_previous || (tbin != tbin_previous + 1 && tbin != tbin_previous - 1) ) {
					hname = Form("hist/h%sALCTTime_Ly%d", CSCTag.c_str(), nLayer);
					h[hname]->Fill(wg, tbin);

					hname = Form("hist/h%sALCTTime_Ly%d_Profile", CSCTag.c_str(), nLayer);
					h[hname]->Fill(wg, tbin);

					hname = Form("hist/h%sALCT_Ly%d_Rate", CSCTag.c_str(), nLayer);
					h[hname]->Fill(wg);
					int number_wg = (int)(h[hname]->GetBinContent(wg+1));
					Double_t Number_of_entries_ALCT = h[hname]->GetEntries();

					hname = Form("hist/h%sALCT_Ly%d_Efficiency", CSCTag.c_str(), nLayer);
					h[hname]->SetBinContent(wg+1,((float)number_wg));
					if((Double_t)(nDMBEvents[ChamberID]) > 0.0) {
						h[hname]->SetNormFactor(100.0*Number_of_entries_ALCT/(Double_t)(nDMBEvents[ChamberID]));
					} else {
						h[hname]->SetNormFactor(100.0);
					}
					h[hname]->SetEntries(nDMBEvents[ChamberID]);
				}
				if(wg != wg_previous) {
					NumberOfWireGroupsWithHitsInALCT = NumberOfWireGroupsWithHitsInALCT + 1;
				}
				wg_previous   = wg;
				tbin_previous = tbin;
			}
		}
		hname = Form("hist/h%sALCT_Number_Of_Layers_With_Hits", CSCTag.c_str());
		h[hname]->Fill(NumberOfLayersWithHitsInALCT);
		hname = Form("hist/h%sALCT_Number_Of_WireGroups_With_Hits", CSCTag.c_str());
		h[hname]->Fill(NumberOfWireGroupsWithHitsInALCT);
	} else {
//		ALCT not found
		hname= Form("hist/h%sALCT_Number_Rate", CSCTag.c_str());
		h[hname]->Fill(0);
		int nALCT = (int)h[hname]->GetBinContent(1);
		hname = Form("hist/h%sALCT_Number_Efficiency", CSCTag.c_str());
		h[hname]->SetBinContent(1, (float)(nALCT)/(float)((float)nDMBEvents[ChamberID])*100.0);
		
		hname = Form("hist/h%sDMB_FEB_Unpacked_vs_DAV", CSCTag.c_str());
		if(alct_dav  > 0) {
			h[hname]->Fill(0.0, 1.0);
		}
	}

//	ALCT and CLCT coinsidence
  if(data.nclct() && data.nalct()) {
    CSCALCTHeader alctHeader = data.alctHeader();
    CSCAnodeData alctData = data.alctData();
    /* !!! CMSSW
    //KK
    //vector<L1MuCSCAnodeLCT> alctsDatas = alctHeader.ALCTs();
    vector<L1MuCSCAnodeLCT> alctsDatas;
    alctsDatas.push_back( L1MuCSCAnodeLCT(alctHeader.alct0Word()) );
    alctsDatas.push_back( L1MuCSCAnodeLCT(alctHeader.alct1Word()) );
    //KK end
    */
    vector<CSCALCTDigi> alctsDatasTmp = alctHeader.ALCTDigis();
    vector<CSCALCTDigi> alctsDatas;

    for (int lct=0; lct<alctsDatasTmp.size(); lct++) {
        if (alctsDatasTmp[lct].isValid())
        alctsDatas.push_back(alctsDatasTmp[lct]);
    }


    CSCTMBData tmbData = data.tmbData();
    CSCTMBHeader tmbHeader = tmbData.tmbHeader();

    /*
      vector<L1MuCSCCathodeLCT> clctsDatas;// = tmbHeader.CLCTs();
      L1MuCSCCathodeLCT clct0(tmbHeader.clct0Word());
      clctsDatas.push_back(clct0);
      L1MuCSCCathodeLCT clct1(tmbHeader.clct1Word());
      clctsDatas.push_back(clct1);
    */
    // vector<CSCCLCTDigi> clctsDatas = tmbHeader.CLCTDigis();
    /*
      CSCCLCTDigi clct0(tmbHeader.clct0Word());
      if (clct0.isValid())
      clctsDatas.push_back(clct0);
      CSCCLCTDigi clct1(tmbHeader.clct1Word());
      if (clct1.isValid())
      clctsDatas.push_back(clct1);

      CSCCLCTData clctData = data.clctData();
    */

    hname = Form("hist/h%sTMB_BXN_vs_ALCT_BXN", CSCTag.c_str());
    h[hname]->Fill((int)((alctHeader.BXNCount())),(int)(tmbHeader.BXNCount()));

    hname = Form("hist/h%sTMB_ALCT_BXN_diff", CSCTag.c_str());
    int clct_alct_bxn_diff = (int)(alctHeader.BXNCount() - tmbHeader.BXNCount());
    if(clct_alct_bxn_diff < -512) h[hname]->Fill(clct_alct_bxn_diff + 1024);
    else {
      if(clct_alct_bxn_diff > 512)  h[hname]->Fill(clct_alct_bxn_diff - 1024);
      else h[hname]->Fill(clct_alct_bxn_diff);
    }
    h[hname]->SetAxisRange(0.1, 1.1*(1.0+h[hname]->GetBinContent(h[hname]->GetMaximumBin())), "Y");

    hname = Form("hist/h%sTMB_L1A_vs_ALCT_L1A", CSCTag.c_str());
    h[hname]->Fill((int)(alctHeader.L1Acc()),(int)(tmbHeader.L1ANumber()));

    hname = Form("hist/h%sTMB_ALCT_L1A_diff", CSCTag.c_str());
    int clct_alct_l1a_diff = (int)(tmbHeader.L1ANumber() - alctHeader.L1Acc());
    if(clct_alct_l1a_diff < -8) h[hname]->Fill(clct_alct_l1a_diff + 16);
    else {
      if(clct_alct_l1a_diff > 8)  h[hname]->Fill(clct_alct_l1a_diff - 16);
      else h[hname]->Fill(clct_alct_l1a_diff);
    }
    h[hname]->SetAxisRange(0.1, 1.1*(1.0+h[hname]->GetBinContent(h[hname]->GetMaximumBin())), "Y");
  }

  //CLCT Found
  if (data.nclct()) {
    CSCTMBData tmbData = data.tmbData();
    CSCTMBHeader tmbHeader = tmbData.tmbHeader();
    CSCTMBTrailer tmbTrailer = tmbData.tmbTrailer();
    /* !!! CMSSW
       vector<L1MuCSCCathodeLCT> clctsDatas;// = tmbHeader.CLCTs();
       L1MuCSCCathodeLCT clct0(tmbHeader.clct0Word());
       if (clct0.isValid())
       clctsDatas.push_back(clct0);
       L1MuCSCCathodeLCT clct1(tmbHeader.clct1Word());
       if (clct1.isValid())
       clctsDatas.push_back(clct1);
    */

    vector<CSCCLCTDigi> clctsDatasTmp = tmbHeader.CLCTDigis();
    vector<CSCCLCTDigi> clctsDatas;

    for (int lct=0; lct<clctsDatasTmp.size(); lct++) {    
	if (clctsDatasTmp[lct].isValid())
	clctsDatas.push_back(clctsDatasTmp[lct]);
    }
		
    /*
      CSCCLCTDigi clct0(tmbHeader.clct0Word());
      if (clct0.isValid())
      clctsDatas.push_back(clct0);
      CSCCLCTDigi clct1(tmbHeader.clct1Word());
      if (clct1.isValid())
      clctsDatas.push_back(clct1);
    */

    CSCCLCTData clctData = data.clctData();

    FEBunpacked = FEBunpacked +1;
    tmb_unpacked = 1;

    hname = Form("hist/h%sALCT_Match_Time", CSCTag.c_str());
    h[hname]->Fill(tmbHeader.ALCTMatchTime());

    hname = Form("hist/h%sLCT_Match_Status", CSCTag.c_str());
    if (tmbHeader.CLCTOnly()) h[hname]->Fill(0.0,0.0);
    if (tmbHeader.ALCTOnly()) h[hname]->Fill(0.0,1.0);
    if (tmbHeader.TMBMatch()) h[hname]->Fill(0.0,2.0);

    hname = Form("hist/h%sLCT0_Match_BXN_Difference", CSCTag.c_str());
    h[hname]->Fill(tmbHeader.Bxn0Diff());


    hname = Form("hist/h%sLCT1_Match_BXN_Difference", CSCTag.c_str());
    h[hname]->Fill(tmbHeader.Bxn1Diff());

		hname = Form("hist/h%sDMB_FEB_Unpacked_vs_DAV", CSCTag.c_str());
		if(tmb_dav  > 0) {
			h[hname]->Fill(1.0, 0.0);
		}

/*     hname = Form("hist/h%sDMB_FEB_DAV_Rate", CSCTag.c_str());
    //		Set number of CLCT-events to forth bin
    h[hname]->Fill(1);

    float CLCTEvent = h[hname]->GetBinContent(2);
    hname = Form("hist/h%sDMB_FEB_DAV_Efficiency", CSCTag.c_str());
    if(nEvents > 0) {
//KK
//      h[hname]->SetBinContent(4,((float)CLCTEvent/(float)(nEvents)*100.0));
      h[hname]->SetBinContent(2,((float)CLCTEvent/(float)(nDMBEvents[ChamberID])*100.0));
//KKend
      h[hname]->SetEntries(nEvents);
    } */

    hname = Form("hist/h%sCLCT_L1A", CSCTag.c_str());
    h[hname]->Fill(tmbHeader.L1ANumber());

    hname = Form("hist/h%sCLCT_DMB_L1A_diff", CSCTag.c_str());
    int clct_dmb_l1a_diff = (int)((dmbHeader.l1a()&0xF)-tmbHeader.L1ANumber());
    if(clct_dmb_l1a_diff < -8) h[hname]->Fill(clct_dmb_l1a_diff + 16);
    else {
      if(clct_dmb_l1a_diff > 8)  h[hname]->Fill(clct_dmb_l1a_diff - 16);
      else h[hname]->Fill(clct_dmb_l1a_diff);
    }
    h[hname]->SetAxisRange(0.1, 1.1*(1.0+h[hname]->GetBinContent(h[hname]->GetMaximumBin())), "Y");

    hname = Form("hist/h%sDMB_L1A_vs_CLCT_L1A", CSCTag.c_str());
    h[hname]->Fill(tmbHeader.L1ANumber(),dmbHeader.l1a());

    hname = Form("hist/h%sCLCT_DMB_BXN_diff", CSCTag.c_str());
    int clct_dmb_bxn_diff = (int)(dmbHeader.bxn()-(tmbHeader.BXNCount()&0x7F));
    if(clct_dmb_bxn_diff < -64) h[hname]->Fill(clct_dmb_bxn_diff + 128);
    else {
      if(clct_dmb_bxn_diff > 64)  h[hname]->Fill(clct_dmb_bxn_diff - 128);
      else h[hname]->Fill(clct_dmb_bxn_diff);
    }
    h[hname]->SetAxisRange(0.1, 1.1*(1.0+h[hname]->GetBinContent(h[hname]->GetMaximumBin())), "Y");

    hname = Form("hist/h%sCLCT_BXN", CSCTag.c_str());
    h[hname]->Fill((int)(tmbHeader.BXNCount()));

    hname = Form("hist/h%sCLCT_BXN_vs_DMB_BXN", CSCTag.c_str());
    h[hname]->Fill(tmbHeader.BXNCount(),dmbHeader.bxn());


    hname = Form("hist/h%sCLCT_Number_Rate", CSCTag.c_str());
    h[hname]->Fill(clctsDatas.size());
    int nCLCT = (int)h[hname]->GetBinContent((int)(clctsDatas.size()+1));

    hname = Form("hist/h%sCLCT_Number", CSCTag.c_str());
    h[hname]->SetBinContent((int)(clctsDatas.size()+1), (float)(nCLCT)/(float)((float)nDMBEvents[ChamberID])*100.0);

    if (clctsDatas.size()==2) {
      hname = Form("hist/h%sCLCT1_vs_CLCT0_Key_Strip", CSCTag.c_str());
      h[hname]->Fill(clctsDatas[0].getKeyStrip(),clctsDatas[1].getKeyStrip());
    }

    if (clctsDatas.size()==1) {
      hname = Form("hist/h%sCLCT0_Clssification", CSCTag.c_str());
      if (clctsDatas[0].getStripType()) 	h[hname]->Fill(0.0);
      else					h[hname]->Fill(1.0);
    }

    if (clctsDatas.size()==2) {
      hname = Form("hist/h%sCLCT0_CLCT1_Clssification", CSCTag.c_str());
      if ( clctsDatas[0].getStripType() &&  clctsDatas[1].getStripType()) 	h[hname]->Fill(0.0);
      if ( clctsDatas[0].getStripType() && !clctsDatas[1].getStripType()) 	h[hname]->Fill(1.0);
      if (!clctsDatas[0].getStripType() &&  clctsDatas[1].getStripType()) 	h[hname]->Fill(2.0);
      if (!clctsDatas[0].getStripType() &&  !clctsDatas[1].getStripType()) 	h[hname]->Fill(3.0);
    }


    hname = Form("hist/h%sTMB_Word_Count", CSCTag.c_str());
    h[hname]->Fill((int)(tmbTrailer.wordCount()));
    if(debug_printout) LOG4CPLUS_DEBUG(logger_, "+++debug>  TMB Trailer Word Count = " 
	<< dec << (int)tmbTrailer.wordCount());


    for (int lct=0; lct<clctsDatas.size(); lct++) {

      hname = Form("hist/h%sCLCT%d_BXN", CSCTag.c_str(), lct);
      h[hname]->Fill(clctsDatas[lct].getBX());

      hname = Form("hist/h%sCLCT%d_dTime", CSCTag.c_str(), lct);
      int clct_dtime = (int)(clctsDatas[lct].getBX()-(tmbHeader.BXNCount()&0x3));
      if(clct_dtime < -2) h[hname]->Fill(clct_dtime + 4);
      else {
	if(clct_dtime > 2)  h[hname]->Fill(clct_dtime - 4);
	else h[hname]->Fill(clct_dtime);
      }
      h[hname]->SetAxisRange(0.1, 1.1*(1.0+h[hname]->GetBinContent(h[hname]->GetMaximumBin())), "Y");
      //			LOG4CPLUS_DEBUG(logger_, "CLCT BX = " << clctsDatas[lct].getBX() << " TMB BX = " << tmbHeader.BXNCount() << " 03 = " << (int)(tmbHeader.BXNCount()&0x3));
      //			LOG4CPLUS_DEBUG(logger_, "diff = " << clctsDatas[lct].getBX()-(tmbHeader.BXNCount()&0x3));

      if(debug_printout) LOG4CPLUS_DEBUG(logger_, "+++debug> LCT:" << lct << " Type:" << clctsDatas[lct].getStripType() 
	<< " Strip:" << clctsDatas[lct].getKeyStrip());
      if (clctsDatas[lct].getStripType()) {
	hname = Form("hist/h%sCLCT%d_KeyHalfStrip", CSCTag.c_str(), lct);
	h[hname]->Fill(clctsDatas[lct].getKeyStrip());

	hname = Form("hist/h%sCLCT%d_dTime_vs_Half_Strip", CSCTag.c_str(), lct);
	if(clct_dtime < -2) h[hname]->Fill((int)(clctsDatas[lct].getKeyStrip()), clct_dtime + 4);
	else {
	  if(clct_dtime > 2)  	h[hname]->Fill((int)(clctsDatas[lct].getKeyStrip()), clct_dtime - 4);
	  else 			h[hname]->Fill((int)(clctsDatas[lct].getKeyStrip()), clct_dtime);
	}

	hname = Form("hist/h%sCLCT%d_Half_Strip_Pattern", CSCTag.c_str(), lct);
	int pattern_clct = (int)((clctsDatas[lct].getPattern()>>1)&0x3);
	//				pattern_clct = Number of patterns in CLCT
	//				Last (left) bit is bend. Positive bend = 1, negative bend = 0
	if(pattern_clct == 1) h[hname]->Fill(clctsDatas[lct].getKeyStrip(), 7.0);
	if(pattern_clct == 3) h[hname]->Fill(clctsDatas[lct].getKeyStrip(), 6.0);
	if(pattern_clct == 5) h[hname]->Fill(clctsDatas[lct].getKeyStrip(), 5.0);
	if(pattern_clct == 7) h[hname]->Fill(clctsDatas[lct].getKeyStrip(), 4.0);
	if(pattern_clct == 6) h[hname]->Fill(clctsDatas[lct].getKeyStrip(), 3.0);
	if(pattern_clct == 4) h[hname]->Fill(clctsDatas[lct].getKeyStrip(), 2.0);
	if(pattern_clct == 2) h[hname]->Fill(clctsDatas[lct].getKeyStrip(), 1.0);
	if(pattern_clct == 0) h[hname]->Fill(clctsDatas[lct].getKeyStrip(), 0.0);

	hname = Form("hist/h%sCLCT%d_Half_Strip_Quality", CSCTag.c_str(), lct);
	h[hname]->Fill((int)(clctsDatas[lct].getKeyStrip()),(int)(clctsDatas[lct].getQuality()));

	hname = Form("hist/h%sCLCT%d_Half_Strip_Quality_Profile", CSCTag.c_str(), lct);
	h[hname]->Fill((int)(clctsDatas[lct].getKeyStrip()), (int)(clctsDatas[lct].getQuality()));
      } else {
	hname = Form("hist/h%sCLCT%d_KeyDiStrip", CSCTag.c_str(), lct);
	h[hname]->Fill(clctsDatas[lct].getKeyStrip());
	
	if(lct == 0) clct_kewdistrip = clctsDatas[lct].getKeyStrip();
	
	hname = Form("hist/h%sCLCT%d_dTime_vs_DiStrip", CSCTag.c_str(), lct);
	if(clct_dtime < -2) h[hname]->Fill((int)(clctsDatas[lct].getKeyStrip()), clct_dtime + 4);
	else {
	  if(clct_dtime > 2)  	h[hname]->Fill((int)(clctsDatas[lct].getKeyStrip()), clct_dtime - 4);
	  else 			h[hname]->Fill((int)(clctsDatas[lct].getKeyStrip()), clct_dtime);
	}

	hname = Form("hist/h%sCLCT%d_DiStrip_Pattern", CSCTag.c_str(), lct);
	int pattern_clct = (int)((clctsDatas[lct].getPattern()>>1)&0x3);
	//				pattern_clct = Number of patterns in CLCT
	//				Last (left) bit is bend. Positive bend = 1, negative bend = 0
	if(pattern_clct == 1) h[hname]->Fill(clctsDatas[lct].getKeyStrip(), 7.0);
	if(pattern_clct == 3) h[hname]->Fill(clctsDatas[lct].getKeyStrip(), 6.0);
	if(pattern_clct == 5) h[hname]->Fill(clctsDatas[lct].getKeyStrip(), 5.0);
	if(pattern_clct == 7) h[hname]->Fill(clctsDatas[lct].getKeyStrip(), 4.0);
	if(pattern_clct == 6) h[hname]->Fill(clctsDatas[lct].getKeyStrip(), 3.0);
	if(pattern_clct == 4) h[hname]->Fill(clctsDatas[lct].getKeyStrip(), 2.0);
	if(pattern_clct == 2) h[hname]->Fill(clctsDatas[lct].getKeyStrip(), 1.0);
	if(pattern_clct == 0) h[hname]->Fill(clctsDatas[lct].getKeyStrip(), 0.0);

	hname = Form("hist/h%sCLCT%d_DiStrip_Quality", CSCTag.c_str(), lct);
	h[hname]->Fill((int)(clctsDatas[lct].getKeyStrip()),(int)(clctsDatas[lct].getQuality()));

	hname = Form("hist/h%sCLCT%d_DiStrip_Quality_Profile", CSCTag.c_str(), lct);
	h[hname]->Fill((int)(clctsDatas[lct].getKeyStrip()), (int)(clctsDatas[lct].getQuality()));
      }
    }
    int NumberOfLayersWithHitsInCLCT = 0;
    int NumberOfHalfStripsWithHitsInCLCT = 0;
    if (clctData.check()) {
      for (int nLayer=1; nLayer<=6; nLayer++) {
	int hstrip_previous    = -1;
	int tbin_clct_previous = -1;
	bool CheckLayerCLCT = true;
	vector<CSCComparatorDigi> compOutData = clctData.comparatorDigis(nLayer);
	for (vector<CSCComparatorDigi>:: iterator compOutDataItr = compOutData.begin(); compOutDataItr != compOutData.end(); ++compOutDataItr) {
		// =VB= Fix to get right hafstrip
	  int hstrip = 2*compOutDataItr->getStrip()+compOutDataItr->getComparator();
	  int tbin_clct = (int)compOutDataItr->getTimeBin();
	  if(CheckLayerCLCT) {
	    NumberOfLayersWithHitsInCLCT = NumberOfLayersWithHitsInCLCT + 1;
	    CheckLayerCLCT = false;
	  }
	  if(hstrip != hstrip_previous || (tbin_clct != tbin_clct_previous + 1 && tbin_clct != tbin_clct_previous - 1) ) {
	    hname = Form("hist/h%sCLCTTime_Ly%d", CSCTag.c_str(), nLayer);
	    h[hname]->Fill(hstrip, tbin_clct);

	    hname = Form("hist/h%sCLCTTime_Ly%d_Profile", CSCTag.c_str(), nLayer);
	    h[hname]->Fill(hstrip, tbin_clct);

	    hname = Form("hist/h%sCLCT_Ly%d_Rate", CSCTag.c_str(), nLayer);
	    h[hname]->Fill(hstrip);

	    int number_hstrip = (int)(h[hname]->GetBinContent(hstrip+1));
	    Double_t Number_of_entries_CLCT = h[hname]->GetEntries();
	    
	    hname = Form("hist/h%sCLCT_Ly%d_Efficiency",CSCTag.c_str(), nLayer);
	    h[hname]->SetBinContent(hstrip+1,(float)number_hstrip);
	    if((Double_t)(nDMBEvents[ChamberID]) > 0.0) {
		h[hname]->SetNormFactor(100.0*Number_of_entries_CLCT/(Double_t)(nDMBEvents[ChamberID]));
	    } else {
	    	h[hname]->SetNormFactor(100.0);
	    }
	    h[hname]->SetEntries(nDMBEvents[ChamberID]);
	  }
	  if(hstrip != hstrip_previous) {
	    NumberOfHalfStripsWithHitsInCLCT = NumberOfHalfStripsWithHitsInCLCT + 1;
	  }
	  hstrip_previous    = hstrip;
	  tbin_clct_previous = tbin_clct;
	}
      }
    }
    hname = Form("hist/h%sCLCT_Number_Of_Layers_With_Hits", CSCTag.c_str());
    h[hname]->Fill(NumberOfLayersWithHitsInCLCT);
    hname = Form("hist/h%sCLCT_Number_Of_HalfStrips_With_Hits", CSCTag.c_str());
    h[hname]->Fill(NumberOfHalfStripsWithHitsInCLCT);

  } else {
    //	CLCT not found
    hname = Form("hist/h%sCLCT_Number_Rate", CSCTag.c_str());
    h[hname]->Fill(0);
    int nCLCT = (int)h[hname]->GetBinContent(1);
    hname = Form("hist/h%sCLCT_Number", CSCTag.c_str());
    h[hname]->SetBinContent(1, (float)(nCLCT)/(float)((float)nDMBEvents[ChamberID])*100.0);
    
		hname = Form("hist/h%sDMB_FEB_Unpacked_vs_DAV", CSCTag.c_str());
		if(tmb_dav  > 0) {
			h[hname]->Fill(1.0, 1.0);
		}
  }
		

  //	CFEB found
  int NumberOfUnpackedCFEBs = 0;
  int N_CFEBs=5, N_Samples=16, N_Layers = 6, N_Strips = 16;
  int ADC = 0, OutOffRange, Threshold = 30;
  bool DebugCFEB = false;
  CSCCFEBData * cfebData[5];
  CSCCFEBTimeSlice *  timeSlice[5][16];
  CSCCFEBDataWord * timeSample[5][16][6][16];
  int Pedestal[5][6][16];
  float PedestalError[5][6][16];
  CSCCFEBSCAControllerWord scaControllerWord[5][16][6];
  bool CheckCFEB = true;
  //--------------B
  float Clus_Sum_Charge;
  int TrigTime, L1APhase, UnpackedTrigTime, LCTPhase, SCA_BLK, NmbTimeSamples, NmbCell, SCA_Nmb_FC;
  int  FreeCells, LCT_Pipe_Empty, LCT_Pipe_Full, LCT_Pipe_Count, L1_Pipe_Empty, L1_Pipe_Full, Buffer_Count;
  //--------------E

  bool CheckThresholdStripInTheLayer[6][80];
  for(int i=1; i<=6; ++i) {
    for(int j = 1; j <= 80; ++j) CheckThresholdStripInTheLayer[i][j] = true;
  }

  bool CheckOutOffRangeStripInTheLayer[6][80];
  for(int i=1; i<=6; ++i) {
    for(int j = 1; j <= 80; ++j) CheckOutOffRangeStripInTheLayer[i][j] = true;
  }

  //--------------B
  float cscdata[N_CFEBs*16][N_Samples][N_Layers];
  int TrigTimeData[N_CFEBs*16][N_Samples][N_Layers];
  int SCABlockData[N_CFEBs*16][N_Samples][N_Layers];
  for(int i=0; i<N_Layers; ++i) {
    for(int j = 0; j < N_CFEBs*16; ++j) {
      for(int k = 0; k < N_Samples; ++k) {
	cscdata[j][k][i] = 0.0;
	TrigTimeData[j][k][i] = 0;
	SCABlockData[j][k][i] = 0;
      }
    }
  }
  //--------------E

  for(int nCFEB = 0; nCFEB < N_CFEBs; ++nCFEB) {
    cfebData[nCFEB] = data.cfebData(nCFEB);
    if (cfebData[nCFEB] !=0) {
      //			CFEB Found
      FEBunpacked = FEBunpacked +1; // Increment number of unpacked FED
      NumberOfUnpackedCFEBs = NumberOfUnpackedCFEBs + 1; // Increment number of unpaked CFEB
      cfeb_unpacked = 1;
      if(CheckCFEB == true){
		
		hname = Form("hist/h%sDMB_FEB_Unpacked_vs_DAV", CSCTag.c_str());
		if(cfeb_dav2  > 0) {
			h[hname]->Fill(2.0, 0.0);
		}
		
/* 	hname = Form("hist/h%sDMB_FEB_DAV_Rate", CSCTag.c_str());
	h[hname]->Fill(2);

	float CFEBEvent = h[hname]->GetBinContent(3);
	hname = Form("hist/h%sDMB_FEB_DAV_Efficiency", CSCTag.c_str());
	if(nEvents > 0) {
//KK
//	  h[hname]->SetBinContent(5, ((float)CFEBEvent/(float)(nEvents)*100.0));
          h[hname]->SetBinContent(3, ((float)CFEBEvent/(float)(nDMBEvents[ChamberID])*100.0));
//KKend
	  h[hname]->SetEntries(nEvents);
	} */
	CheckCFEB = false;
      }
      //-------------B
      NmbTimeSamples= (cfebData[nCFEB])->nTimeSamples();
      //-------------E
      if(DebugCFEB) {
	LOG4CPLUS_DEBUG(logger_, "nEvents = " << nEvents);
	LOG4CPLUS_DEBUG(logger_, "Chamber ID = "<< CSCTag.c_str() << " Crate ID = "<< crateID 
		<< " DMB ID = " << dmbID
		<< "nCFEB =" << nCFEB);
      }
      for(int nSample = 0; nSample < (cfebData[nCFEB])->nTimeSamples(); ++nSample) {
	timeSlice[nCFEB][nSample] = (CSCCFEBTimeSlice * )((cfebData[nCFEB])->timeSlice(nSample));
	if (timeSlice[nCFEB][nSample] == 0) {
	  //					if (debug) LOG4CPLUS_DEBUG(logger_, "+++debug> nCFEB" << nCFEB << " nSample: " << nSample << " - B-Word");
	  continue;
	}
	if(DebugCFEB) LOG4CPLUS_DEBUG(logger_, " nSample = " << nSample);
	for(int nLayer = 1; nLayer <= N_Layers; ++nLayer) {
	  scaControllerWord[nCFEB][nSample][nLayer] = (timeSlice[nCFEB][nSample])->scaControllerWord(nLayer);
	  TrigTime = (int)(scaControllerWord[nCFEB][nSample][nLayer]).trig_time;
	  //--------------B
          FreeCells = (timeSlice[nCFEB][nSample])->get_n_free_sca_blocks();
          LCT_Pipe_Empty = (timeSlice[nCFEB][nSample])->get_lctpipe_empty();
          LCT_Pipe_Full = (timeSlice[nCFEB][nSample])->get_lctpipe_full();
          LCT_Pipe_Count = (timeSlice[nCFEB][nSample])->get_lctpipe_count();
          L1_Pipe_Empty = (timeSlice[nCFEB][nSample])->get_l1pipe_empty();
          L1_Pipe_Full = (timeSlice[nCFEB][nSample])->get_l1pipe_full();
//          L1_Pipe_Count = (timeSlice[nCFEB][nSample])->get_L1A_number();
          Buffer_Count = (timeSlice[nCFEB][nSample])->get_buffer_count();

	  SCA_BLK  = (int)(scaControllerWord[nCFEB][nSample][nLayer]).sca_blk;
	  //					LOG4CPLUS_DEBUG(logger_, "SCA BLOCK: Chamber="<<ChamberID<<" CFEB="<<nCFEB+1<<" TRIGTIME="<<TrigTime<<" TimeSlice="<<nSample+1<<" Layer="<<nLayer<<" SCA_BLK="<<SCA_BLK);
	  for(int nStrip = 0; nStrip < N_Strips; ++nStrip) {
	    SCABlockData[nCFEB*16+nStrip][nSample][nLayer-1] = SCA_BLK;
	    //						if(res<=1) TrigTimeData[nCFEB*16+nStrip][nSample][nLayer-1] = TrigTime;
	  }
	  //					LOG4CPLUS_DEBUG(logger_,"*********"<<" TRIGTIME="<<TrigTime<<" BIT COUNT="<<bit_count);

	  //					SCA Block Occupancy Histograms
	  hname = Form("hist/h%sCFEB%d_SCA_Block_Occupancy", CSCTag.c_str(), nCFEB);
	  h[hname]->Fill(SCA_BLK);

          //                                    Free SCA Cells
          hname = Form("hist/h%sCFEB%d_Free_SCA_Cells", CSCTag.c_str(), nCFEB);
          if (scaControllerWord[nCFEB][nSample][nLayer].sca_full == 1) h[hname]->Fill(-1);
          h[hname]->Fill(FreeCells);

          //                                    Number of SCA Blocks Locked by LCTs
          hname = Form("hist/h%sCFEB%d_SCA_Blocks_Locked_by_LCTs", CSCTag.c_str(), nCFEB);
          if (LCT_Pipe_Empty == 1) h[hname]->Fill(-0.5);
          if (LCT_Pipe_Full == 1) h[hname]->Fill(16.5);
          h[hname]->Fill(LCT_Pipe_Count);

          //                                    Number of SCA Blocks Locked by LCTxL1
          hname = Form("hist/h%sCFEB%d_SCA_Blocks_Locked_by_LCTxL1", CSCTag.c_str(), nCFEB);
          if (L1_Pipe_Empty == 1) h[hname]->Fill(-0.5);
          if (L1_Pipe_Full == 1) h[hname]->Fill(31.5);
          h[hname]->Fill(Buffer_Count);


	  //--------------E
	  //					if(debug) LOG4CPLUS_DEBUG(logger_, "+++debug> nCFEB " << nCFEB << " nSample " << nSample << " nLayer " << nLayer << " TrigTime " << TrigTime);
	  if(nSample == 0 && nLayer == 1) {
	    TrigTime = (int)(scaControllerWord[nCFEB][nSample][nLayer]).trig_time;
	    int k=1;
	    while (((TrigTime >> (k-1)) & 0x1) != 1 && k<=8) {
	      k = k +1;
	    }
	    L1APhase = (int)(((scaControllerWord[nCFEB][nSample][nLayer]).l1a_phase)&0x1);
	    UnpackedTrigTime = ((k<<1)&0xE)+L1APhase;

	    hname = Form("hist/h%sCFEB%d_L1A_Sync_Time", CSCTag.c_str(), nCFEB);
	    h[hname]->Fill((int)UnpackedTrigTime);
	    LCTPhase = (int)(((scaControllerWord[nCFEB][nSample][nLayer]).lct_phase)&0x1);

	    hname = Form("hist/h%sCFEB%d_LCT_PHASE_vs_L1A_PHASE", CSCTag.c_str(), nCFEB);
	    h[hname]->Fill(LCTPhase, L1APhase);

	    if(debug_printout) LOG4CPLUS_DEBUG(logger_, "+++debug> L1APhase " << L1APhase << " UnpackedTrigTime " << UnpackedTrigTime);

	    hname = Form("hist/h%sCFEB%d_L1A_Sync_Time_vs_DMB", CSCTag.c_str(), nCFEB);
	    h[hname]->Fill((int)(dmbHeader.dmbCfebSync()), (int)UnpackedTrigTime);

	    hname = Form("hist/h%sCFEB%d_L1A_Sync_Time_DMB_diff", CSCTag.c_str(), nCFEB);
	    int cfeb_dmb_L1A_sync_time = (int)(dmbHeader.dmbCfebSync()) - (int)UnpackedTrigTime;
	    if(cfeb_dmb_L1A_sync_time < -8) h[hname]->Fill(cfeb_dmb_L1A_sync_time+16);
	    else {
	      if(cfeb_dmb_L1A_sync_time > 8) 	h[hname]->Fill(cfeb_dmb_L1A_sync_time-16);
	      else 				h[hname]->Fill(cfeb_dmb_L1A_sync_time);
	    }
	    h[hname]->SetAxisRange(0.1, 1.1*(1.0+h[hname]->GetBinContent(h[hname]->GetMaximumBin())), "Y");

	  }
	  if(DebugCFEB) LOG4CPLUS_DEBUG(logger_, " nLayer = " << nLayer);
	  for(int nStrip = 1; nStrip <= N_Strips; ++nStrip) {
	    timeSample[nCFEB][nSample][nLayer][nStrip]=(data.cfebData(nCFEB)->timeSlice(nSample))->timeSample(nLayer,nStrip);
	    ADC = (int) ((timeSample[nCFEB][nSample][nLayer][nStrip]->adcCounts)&0xFFF);
	    if(DebugCFEB) LOG4CPLUS_DEBUG(logger_, " nStrip="<< dec << nStrip << " ADC=" << hex << ADC);
	    OutOffRange = (int) ((timeSample[nCFEB][nSample][nLayer][nStrip]->adcOverflow)&0x1);
	
	    if(nSample == 0) { // nSample == 0
	
	      Pedestal[nCFEB][nLayer][nStrip] = ADC;
	      if(DebugCFEB) LOG4CPLUS_DEBUG(logger_, " nStrip="<< dec << nStrip 
		<< " Pedestal=" << hex << Pedestal[nCFEB][nLayer][nStrip]);
//--------------B
/*
              hname = Form("hist/h%sCFEB_PedestalRMS_Sample_01_Ly%d_Strip%d", CSCTag.c_str(), nLayer, nStrip);
              h[hname]->Fill(Pedestal[nCFEB][nLayer][nStrip]);
              int num_ent = h[hname]->GetEntries();
              if ((num_ent % 100) == 0){
                  h[hname]->Fit("gaus","Q");
                  TF1 *gaus_f =  h[hname]->GetFunction("gaus");
                  hname = Form("hist/h%sCFEB_PedestalRMS_Sample_01_Ly%d", CSCTag.c_str(), nLayer);
                  h[hname]->SetBinContent(nCFEB*16+nStrip,gaus_f->GetParameter(2));
                  h[hname]->SetBinError(nCFEB*16+nStrip,0.00000000001);
              }
*/
//--------------E             
/*            int ADC_tmp = (int) ((timeSample[nCFEB][nSample+1][nLayer][nStrip]->adcCounts)&0xFFF); 
            hname = Form("hist/h%sCFEB_PedestalRMS_Sample_01_Ly%d",CSCTag.c_str(),nLayer);
            if (abs(ADC-ADC_tmp) < 40){
	        h[hname]->SetBinContent(nCFEB*16+nStrip,PedestalError[nCFEB][nLayer][nStrip]);
	        h[hname]->SetBinError(nCFEB*16+nStrip,0.00000000001);
	      }
*/	      
	     }
	    
	    if(OutOffRange == 1 && CheckOutOffRangeStripInTheLayer[nLayer][nCFEB*16+nStrip] == true) {
	      hname = Form("hist/h%sCFEB_Out_Off_Range_Strips_Ly%d", CSCTag.c_str(), nLayer);
	      h[hname]->Fill((int)(nCFEB*16+nStrip));
	      CheckOutOffRangeStripInTheLayer[nLayer][nCFEB*16+nStrip] = false;
	    }
	    if(ADC - Pedestal[nCFEB][nLayer][nStrip] > Threshold && OutOffRange != 1) {
	      hname = Form("hist/h%sCFEB_Active_Samples_vs_Strip_Ly%d", CSCTag.c_str(), nLayer);
	      h[hname]->Fill((int)(nCFEB*16+nStrip), nSample);
	      hname = Form("hist/h%sCFEB_Active_Samples_vs_Strip_Ly%d_Profile", CSCTag.c_str(), nLayer);

	      h[hname]->Fill((int)(nCFEB*16+nStrip), nSample);
	      if(CheckThresholdStripInTheLayer[nLayer][nCFEB*16+nStrip] == true) {
		hname = Form("hist/h%sCFEB_ActiveStrips_Ly%d", CSCTag.c_str(), nLayer);
		h[hname]->Fill((int)(nCFEB*16+nStrip));
		CheckThresholdStripInTheLayer[nLayer][nCFEB*16+nStrip] = false;
	      }
//--------------B
	      if(ADC - Pedestal[nCFEB][nLayer][nStrip] > Threshold) {
	         if(DebugCFEB) {
		    LOG4CPLUS_DEBUG(logger_, "Layer="<<nLayer<<" Strip="<<nCFEB*16+nStrip<<" Time="<<nSample
			<< " ADC-PEDEST = "<<ADC - Pedestal[nCFEB][nLayer][nStrip]);
	         }
	         cscdata[nCFEB*16+nStrip-1][nSample][nLayer-1] = ADC - Pedestal[nCFEB][nLayer][nStrip];
	      }
//--------------E
	    }
//--------------B
	    if(nSample == 1) {
	       int channel_threshold = 40;
	       if (abs(ADC - Pedestal[nCFEB][nLayer][nStrip]) < channel_threshold){
  	           hname = Form("hist/h%sCFEB_Pedestal(withEMV)_Sample_01_Ly%d", CSCTag.c_str(), nLayer);
	           h[hname]->Fill((int)(nCFEB*16+nStrip), Pedestal[nCFEB][nLayer][nStrip]);
	           
	           hname = Form("hist/h%sCFEB_Pedestal(withRMS)_Sample_01_Ly%d", CSCTag.c_str(), nLayer);
	           h[hname]->Fill((int)(nCFEB*16+nStrip), Pedestal[nCFEB][nLayer][nStrip]);
	           
	           PedestalError[nCFEB][nLayer][nStrip] = h[hname]->GetBinError(nCFEB*16+nStrip);
	           
                   hname = Form("hist/h%sCFEB_PedestalRMS_Sample_01_Ly%d",CSCTag.c_str(),nLayer);
	           h[hname]->SetBinContent(nCFEB*16+nStrip,PedestalError[nCFEB][nLayer][nStrip]);
	           h[hname]->SetBinError(nCFEB*16+nStrip,0.00000000001);	       
	       }
	    }
//--------------E
	  }
	}                            
      }
    }
  }

  //--------------B
  float Cathodes[N_CFEBs*N_Strips*N_Samples*N_Layers];
  for(int i=0; i<N_Layers; ++i) {
    for(int j=0; j<N_CFEBs*N_Strips; ++j) {
      for(int k=0; k<N_Samples; ++k) {
	Cathodes[i*N_CFEBs*N_Strips*N_Samples + N_CFEBs*N_Strips*k + j] = cscdata[j][k][i];
      }
    }
  }

  vector<CSCStripCluster> Clus;
  Clus.clear();

  for(int nLayer=1; nLayer<=N_Layers; ++nLayer) {
    CSCStripClusterFinder *ClusterFinder = new CSCStripClusterFinder(N_Layers, N_Samples, N_CFEBs, N_Strips);
    ClusterFinder->DoAction(nLayer-1, Cathodes);
    Clus = ClusterFinder->getClusters();

    for(int j=0; j<N_CFEBs*N_Strips; j++){
      int SCAbase=SCABlockData[j][0][nLayer-1];
      int SCAcount=0;
      for(int k=0; k<NmbTimeSamples; k++){
	int SCA=SCABlockData[j][k][nLayer-1];
	if(SCA==SCAbase) SCAcount++;
      }
      int TmpTrigTime=NmbTimeSamples+1-SCAcount;
      for(int k=0;k<SCAcount;k++){
	TrigTimeData[j][k][nLayer-1]=TmpTrigTime;
      }
    }

    if(DebugCFEB) LOG4CPLUS_DEBUG(logger_, "***  CATHODE PART  DEBUG: Layer="<<nLayer
	<<"  Number of Clusters="<<Clus.size()<<"      ***");
    //		Number of Clusters Histograms
    hname = Form("hist/h%sCFEB_Number_of_Clusters_Ly_%d", CSCTag.c_str(), nLayer);
    if(Clus.size() >= 0) h[hname]->Fill(Clus.size());
    for(unsigned int u=0;u<Clus.size();u++){
	/*
      if(DebugCFEB) LOG4CPLUS_DEBUG(logger_, "Chamber: "<< ChamberID  << " Cluster: " << u+1
		<< " Number of local Maximums " <<  Clus[u].localMax.size());
      for(unsigned int t=0;t<Clus[u].localMax.size();t++){
	int iS=Clus[u].localMax[t].Strip;
	int jT=Clus[u].localMax[t].Time;
	//				Peak SCA Cell Histograms
	hname = Form("hist/h%sCFEB_SCA_Cell_Peak_Ly_%d", CSCTag.c_str(), nLayer);
	int SCA = SCABlockData[iS][jT][nLayer-1];
	int TmpTrigTime = TrigTimeData[iS][jT][nLayer-1];
	//				LOG4CPLUS_DEBUG(logger_,"TmpTrigTime(max)="<<TmpTrigTime<<" Layer="<<nLayer);
	if(TmpTrigTime>=0) {
	  NmbCell = (SCA-1)*NmbTimeSamples+TmpTrigTime+jT;
	  if(TmpTrigTime==0) NmbCell++;
	  h[hname]->Fill(iS+1,NmbCell);
	}

	if(DebugCFEB) {
	  for(unsigned int k=0;k<Clus[u].ClusterPulseMapHeight.size();k++){
	    if(Clus[u].ClusterPulseMapHeight[k].channel_==iS) {
	      LOG4CPLUS_DEBUG(logger_, "Local Max: " << t+1 << " Strip: " << iS+1 << " Time: " << jT+1
	      	<< " Height: " << Clus[u].ClusterPulseMapHeight[k].height_[jT]);
	    }
	  }
	}
      }
*/	
      Clus_Sum_Charge = 0.0;
      for(unsigned int k=0;k<Clus[u].ClusterPulseMapHeight.size();k++) {
	if(DebugCFEB) LOG4CPLUS_DEBUG(logger_, "Strip: " << Clus[u].ClusterPulseMapHeight[k].channel_+1);
	//				Strip Occupancy Histograms
	//				hname = Form("Chamber_%d_Strip_Occupancy_Ly_%d", ChamberID, nLayer);
	//				h[hname]->Fill(Clus[u].ClusterPulseMapHeight[k].channel_+1);


	for(unsigned int n=Clus[u].LFTBNDTime; n < Clus[u].IRTBNDTime; n++){
	  Clus_Sum_Charge = Clus_Sum_Charge + Clus[u].ClusterPulseMapHeight[k].height_[n];
	/*
	  //					SCA Cell Occupancy Histograms
	  hname = Form("hist/h%sCFEB_SCA_Cell_Occupancy_Ly_%d", CSCTag.c_str(), nLayer);
	  int SCA = SCABlockData[Clus[u].ClusterPulseMapHeight[k].channel_][n][nLayer-1];
	  int TmpTrigTime = TrigTimeData[Clus[u].ClusterPulseMapHeight[k].channel_][n][nLayer-1];
	  if(TmpTrigTime>=0) {
	    //						LOG4CPLUS_DEBUG(logger_, "TmpTrigTime(cluster)="<<TmpTrigTime<<" Layer="<<nLayer);
	    NmbCell = (SCA-1)*NmbTimeSamples+TmpTrigTime+n;
	    if(TmpTrigTime==0) NmbCell++;
	    h[hname]->Fill(Clus[u].ClusterPulseMapHeight[k].channel_+1, NmbCell);
	  }
	*/
	}
      }
	
      //			Clusters Charge Histograms
      hname = Form("hist/h%sCFEB_Clusters_Charge_Ly_%d", CSCTag.c_str(), nLayer);
      h[hname]->Fill(Clus_Sum_Charge);

      //			Width of Clusters Histograms
      hname = Form("hist/h%sCFEB_Width_of_Clusters_Ly_%d", CSCTag.c_str(), nLayer);
      h[hname]->Fill(Clus[u].IRTBNDStrip - Clus[u].LFTBNDStrip+1);

      //                        Cluster Duration Histograms
      hname = Form("hist/h%sCFEB_Cluster_Duration_Ly_%d",  CSCTag.c_str(), nLayer);
      h[hname]->Fill(Clus[u].IRTBNDTime - Clus[u].LFTBNDTime+1);

    }
    Clus.clear();
    delete ClusterFinder;
  }
  //--------------E

//	Fill Hisogram for Different Combinations of FEBs Unpacked vs DAV	
	float feb_combination_unpacked = -1.0;
	if(alct_unpacked == 0 && tmb_unpacked == 0 && cfeb_unpacked == 0) feb_combination_unpacked = 0.0;
	if(alct_unpacked >  0 && tmb_unpacked == 0 && cfeb_unpacked == 0) feb_combination_unpacked = 1.0;
	if(alct_unpacked == 0 && tmb_unpacked >  0 && cfeb_unpacked == 0) feb_combination_unpacked = 2.0;
	if(alct_unpacked == 0 && tmb_unpacked == 0 && cfeb_unpacked >  0) feb_combination_unpacked = 3.0;
	if(alct_unpacked >  0 && tmb_unpacked >  0 && cfeb_unpacked == 0) feb_combination_unpacked = 4.0;
	if(alct_unpacked >  0 && tmb_unpacked == 0 && cfeb_unpacked >  0) feb_combination_unpacked = 5.0;
	if(alct_unpacked == 0 && tmb_unpacked >  0 && cfeb_unpacked >  0) feb_combination_unpacked = 6.0;
	if(alct_unpacked >  0 && tmb_unpacked >  0 && cfeb_unpacked >  0) feb_combination_unpacked = 7.0;
	hname = Form("hist/h%sDMB_FEB_Combinations_Unpacked_vs_DAV",CSCTag.c_str());
	h[hname]->Fill(feb_combination_dav, feb_combination_unpacked);
	
	if(clct_kewdistrip > -1 && alct_keywg > -1) {
		hname = Form("hist/h%sCLCT0_KeyDiStrip_vs_ALCT0_KeyWiregroup", CSCTag.c_str());
		h[hname]->Fill(alct_keywg, clct_kewdistrip);
	}
	
}

