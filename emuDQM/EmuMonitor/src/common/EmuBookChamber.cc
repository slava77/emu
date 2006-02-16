#include "EmuLocalPlotter.h"

//	Booking of chamber's histogram
map<string, TH1*> EmuLocalPlotter::book_chamber(int ChamberID) {

	int id = ChamberID;
	int CrateID = (int)((id>>4) & 0xFF);
	int DMBID = (int)(id & 0xF);

	map<int, map<string,TH1*> >::iterator h_itr = histos.find(ChamberID);
      	if (h_itr == histos.end() || (histos.size()==0)) {
	  if(debug_printout) {
	    cout << "D**EmuFillChamber> #"
		 << "> ch" << CrateID << ":" << DMBID << ">";
	    cout << " List of Histos for chamber not found" << endl;
	  }
	  if(fill_histo) {
	    if(debug_printout) {
	      cout << "D**EmuFillChamber> #"
		   << "> ch" << CrateID << ":" << DMBID << ">";
	      cout << " Creating of list of Histos for the chamber ..." << endl;
	    }
	  }
	} else { return histos[ChamberID];}


	string IDTextShort = Form("%.3d_%.2d_",CrateID,DMBID);
	string IDTextLong = Form(" Crate ID = %d. DMB ID = %d",CrateID,DMBID);

	string dir = Form("CSC_%.3d_%.2d",CrateID,DMBID);
	string path_to_folder = "";

	string LabelBinNumber[32], bit;
		for(int i = 0; i<32; i++){
			for(int j = 4; j >= 0; j--) {
				bit = Form("%d",(i>>j)&0x1);
				LabelBinNumber[i] = LabelBinNumber[i] + bit;
			}
		}

	string hname, htitle;
	map<string, TPad*> pad;
	map<string, TH1*> h;

	string cnvname,cnvtitle;
	map<string, ConsumerCanvas*> cnv;

	stringstream stname, sttitle;
	Char_t str[200];

//CSC
	if(debug_printout) 	cout << "D**EmuBookChamber> New CSC Canvases are booking ..." << endl;
	if(folders) 		path_to_folder = "";

//KK additional information for each particular chamber
	cnvtitle = "CSC: Data Format Errors and Warnings.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),2,2);
		hname = Form("hist/h%dBinCheck_ErrorStat_Table",id);
		h[hname] = new TH2F(hname.c_str(), "DDU Data Format Errors Table", 1, 0, 1, 19, 0, 19);
		h[hname]->GetXaxis()->SetBinLabel(1, " ");
		for(int i=5; i<24; i++) {
			h[hname]->GetYaxis()->SetBinLabel(i-4, bin_checker.errName(i));
		}
		h[hname]->SetOption("coltext");
		h[hname]->SetXTitle("Number of events");
		pad[hname] = new TPad("pad1", "pad1",0.005,0.3,0.5,0.945);
		pad[hname]->SetLeftMargin(0.5);
		pad[hname]->SetRightMargin(0.0);
		pad[hname]->SetFillColor(0);
		pad[hname]->SetGridy(1);
		cnv[cnvname]->cd();
		pad[hname]->Draw();
		pad[hname]->cd();
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()),  TString(dir.c_str()), 0,  h[hname]);

		hname = Form("hist/h%dBinCheck_ErrorStat_Frecuency",id);
		h[hname] = new TH1F(hname.c_str(), "DDU Data Format Errors Frequency", 19, 0, 19);
		for(int i=5; i<24; i++) {
			h[hname]->GetXaxis()->SetBinLabel(i-4, bin_checker.errName(i));
		}
		h[hname]->SetYTitle("Frequency (%)");
		h[hname]->SetFillColor(THistoFillColor);
		h[hname]->SetOption("texthbar1");
		pad[hname] = new TPad("pad2", "pad2",0.505,0.3,0.995,0.945);
		pad[hname]->SetLeftMargin(0.0);
		pad[hname]->SetRightMargin(0.05);
		pad[hname]->SetFillColor(0);
		cnv[cnvname]->cd();
		pad[hname]->Draw();
		pad[hname]->cd();
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()),  TString(dir.c_str()), 0,  h[hname]);

		hname = Form("hist/h%dBinCheck_WarningStat_Table",id);
		h[hname] = new TH2F(hname.c_str(), "DDU Data Format Warnings Table", 1, 0, 1, 1, 0, 1);
		h[hname]->SetXTitle("Number of events");
		for (int i=1; i<2/*bin_checker.nWARNINGS*/; i++) {
			h[hname]->GetYaxis()->SetBinLabel(i, bin_checker.wrnName(i));
		}
		h[hname]->SetOption("coltext");
		pad[hname] = new TPad("pad3", "pad3",0.005,0,0.5,0.295);
		pad[hname]->SetLeftMargin(0.5);
		pad[hname]->SetRightMargin(0.0);
		pad[hname]->SetFillColor(0);
		pad[hname]->SetGridy(1);
		cnv[cnvname]->cd();
		pad[hname]->Draw();
		pad[hname]->cd();
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dBinCheck_WarningStat_Frequency",id);
		h[hname] = new TH2F(hname.c_str(), "DDU Data Format Warnings Frecuency", 1, 0, 1, 1/*bin_checker.nWARNINGS*/, 0, 1/*bin_checker.nWARNINGS*/);
		for (int i=1; i<2/*bin_checker.nWARNINGS*/; i++) {
			h[hname]->GetXaxis()->SetBinLabel(i, bin_checker.wrnName(i));
		}
		h[hname]->SetYTitle("Frequency (%)");
		h[hname]->SetOption("texthbar1");
		pad[hname] = new TPad("pad3", "pad3",0.505,0,0.995,0.295);
		pad[hname]->SetLeftMargin(0.0);
		pad[hname]->SetRightMargin(0.05);
		pad[hname]->SetFillColor(0);
		cnv[cnvname]->cd();
		pad[hname]->Draw();
		pad[hname]->cd();
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()), 0,cnv[cnvname]);
//KK end

	cnvtitle = "CSC: Data Block Finding Efficiency.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),1,1);
		hname = Form("hist/h%d_CSC_Efficiency", id);
		h[hname] = new TH1F(TString(hname.c_str()), "", 4, 0, 4); // KK 5->4
		h[hname]->SetYTitle("Efficiency (% of total number of events)");
		h[hname]->SetMinimum(0.0);
		///h[hname]->GetXaxis()->SetBinLabel(1,"DDU Events"); // KK
		h[hname]->GetXaxis()->SetBinLabel(1,"DMB Found");     // KK 2->1
		h[hname]->GetXaxis()->SetBinLabel(2,"ALCT Found");    // KK 3->2
		h[hname]->GetXaxis()->SetBinLabel(3,"TMB Found");     // KK 4->3
		h[hname]->GetXaxis()->SetBinLabel(4,"CFEB Found");    // KK 5->4
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("histtext");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%d_CSC_Rate",id);
		h[hname] = new TH1F(TString(hname.c_str()), "", 5, 0, 5);
		h[hname]->SetYTitle("Efficiency");
		h[hname]->GetXaxis()->SetBinLabel(1,"DDU Events");
		h[hname]->GetXaxis()->SetBinLabel(2,"DMB Found");
		h[hname]->GetXaxis()->SetBinLabel(3,"ALCT Found");
		h[hname]->GetXaxis()->SetBinLabel(4,"CLCT Found");
		h[hname]->GetXaxis()->SetBinLabel(5,"CFEB Found");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("histtext");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

//DMBs
	if(debug_printout) 	cout << "D**EmuBookChamber> New DMB Canvases are booking ..." << endl;
	if(folders) 		path_to_folder = "DMB/";

	cnvtitle = "DMB: FEBs DAV and Unpacked.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),1,2);
		hname = Form("hist/h%dDMB_FEB_DAV",id);
		h[hname] = new TH1F(TString(hname.c_str()), "Boards DAV Statistics", 8,  0 , 8);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("Number of FEBs (ALCT+CLCT+CFEBs) DAV");
		for(int i = 1; i<=8; i++) h[hname]->GetXaxis()->SetBinLabel(i,Form("%d",i-1));
		h[hname]->SetYTitle("Number of Events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dDMB_FEB_unpacked_vs_DAV", id);
		h[hname] = new TH2F(TString(hname.c_str()), "DMB Unpacked FEBs vs FEBs DAV", 8,  0 , 8, 8, 0, 8);
		h[hname]->SetXTitle("Number of FEBs DAV");
		for(int i = 1; i<=8; i++) h[hname]->GetXaxis()->SetBinLabel(i,Form("%d",i-1));
		h[hname]->SetYTitle("Number of FEBs Unpacked");
		for(int i = 1; i<=8; i++) h[hname]->GetXaxis()->SetBinLabel(i,Form("%d",i-1));
		h[hname]->SetOption("textcolz");
		cnv[cnvname]->cd(2,1,1);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "DMB: CFEBs DAV and Active.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),2,2);
		hname = Form("hist/h%dDMB_CFEB_Active_vs_DAV", id);
		h[hname] = new TH2F(TString(hname.c_str()), "CFEB_Active vs CFEB_DAV combinations", 32, 0, 32, 32,  0 , 32);
		h[hname]->SetXTitle("DMB CFEB DAV");
		for(int i = 0; i < 32; i++) h[hname]->GetXaxis()->SetBinLabel(i+1,LabelBinNumber[i].c_str());
		h[hname]->SetYTitle("DMB CFEB Active");
		for(int i = 0; i < 32; i++) h[hname]->GetYaxis()->SetBinLabel(i+1,LabelBinNumber[i].c_str());
		h[hname]->SetOption("colz");
		h[hname]->LabelsOption("v","X");
		h[hname]->SetTitleOffset(1.35,"X");
		h[hname]->SetTitleOffset(1.35,"Y");
		cnv[cnvname]->cd(1,1,1);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dDMB_CFEB_Active", id);
		h[hname] = new TH1F(TString(hname.c_str()), "Active CFEBs combinations as reported by TMB", 32,  0 , 32);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("DMB CFEB Active");
		for(int i = 0; i < 32; i++) h[hname]->GetXaxis()->SetBinLabel(i+1,LabelBinNumber[i].c_str());
		h[hname]->SetYTitle("Number of Events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hbar");
		h[hname]->SetStats(false);
		h[hname]->SetTitleOffset(1.35,"X");
		cnv[cnvname]->cd(2);
		h[hname]->Draw();
//KK
                TPaveText *pt110 = new TPaveText(0.23,0.72,0.31,0.81,"brNDC");
                pt110->SetTextAlign(12);
                pt110->AddText("n/a");
                pt110->Draw();
//KK end
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dDMB_CFEB_DAV", id);
		h[hname] = new TH1F(TString(hname.c_str()), "CFEBs combinations reporting DAV", 32,  0 , 32);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("DMB CFEB DAV");
		for(int i = 0; i < 32; i++) h[hname]->GetXaxis()->SetBinLabel(i+1,LabelBinNumber[i].c_str());
		h[hname]->SetYTitle("Number of Events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		h[hname]->LabelsOption("v","X");
		h[hname]->SetStats(false);
		h[hname]->SetTitleOffset(1.35,"X");
		cnv[cnvname]->cd(3);
		h[hname]->Draw();
//KK
                TPaveText *pt111 = new TPaveText(0.23,0.72,0.31,0.81,"brNDC");
                pt111->SetTextAlign(12);
                pt111->AddText("n/a");
                pt111->Draw();
//KK end
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dDMB_CFEB_DAV_multiplicity", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Number of CFEBs reporting DAV per event", 6,  0 , 6);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetYTitle("Number of Events");
		h[hname]->SetXTitle("Number of CFEBs Reporting DAV");
		for(int i = 1; i<=6; i++) h[hname]->GetXaxis()->SetBinLabel(i,Form("%d",i-1));
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(4);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "DMB: CFEB Multiple Overlaps.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),1,1);
		hname = Form("hist/h%dDMB_CFEB_MOVLP", id);
		h[hname] = new TH1F(TString(hname.c_str()), "", 32, 0, 32);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("CFEBs");
		for(int i = 0; i < 32; i++) h[hname]->GetXaxis()->SetBinLabel(i+1,LabelBinNumber[i].c_str());
		h[hname]->SetYTitle("Number of Events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("texthist");
		h[hname]->LabelsOption("v","X");
		h[hname]->SetTitleOffset(1.35,"X");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

	cnvtitle = "DMB: DMB-CFEB-SYNC BXN Counter.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),1,1);
		hname = Form("hist/h%dDMB_CFEB_Sync", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Counter of BXNs since last SyncReset to L1A", 16, 0, 16);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetYTitle("Number of Events");
		h[hname]->SetXTitle("BXN Number");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		gStyle->SetOptStat("eo");
		consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);
	consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

	cnvtitle = "DMB: FEB Status (Timeouts, FIFO, L1 pipe).";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),2,2);
		hname = Form("hist/h%dDMB_FEB_Timeouts", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "DMB FEB Timeouts", 15,  0 , 15);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetYTitle("Number of Events");
		h[hname]->GetXaxis()->SetBinLabel(1,"No Timeouts");
		h[hname]->GetXaxis()->SetBinLabel(2,"ALCT StartTimeout");
		h[hname]->GetXaxis()->SetBinLabel(3,"TMB StartTimeout");
		h[hname]->GetXaxis()->SetBinLabel(4,"CFEB1 StartTimeout");
		h[hname]->GetXaxis()->SetBinLabel(5,"CFEB2 StartTimeout");
		h[hname]->GetXaxis()->SetBinLabel(6,"CFEB3 StartTimeout");
		h[hname]->GetXaxis()->SetBinLabel(7,"CFEB4 StartTimeout");
		h[hname]->GetXaxis()->SetBinLabel(8,"CFEB5 StartTimeout");
		h[hname]->GetXaxis()->SetBinLabel(9,"ALCT EndTimeout");
		h[hname]->GetXaxis()->SetBinLabel(10,"TMB EndTimeout");
		h[hname]->GetXaxis()->SetBinLabel(11,"CFEB1 EndTimeout");
		h[hname]->GetXaxis()->SetBinLabel(12,"CFEB2 EndTimeout");
		h[hname]->GetXaxis()->SetBinLabel(13,"CFEB3 EndTimeout");
		h[hname]->GetXaxis()->SetBinLabel(14,"CFEB4 EndTimeout");
		h[hname]->GetXaxis()->SetBinLabel(15,"CFEB5 EndTimeout");
		h[hname]->LabelsOption("d","X");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("texthist");
		cnv[cnvname]->cd(1,1);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		cnv[cnvname]->cd(2);

		hname = Form("hist/h%dDMB_L1_Pipe", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Number of L1A requests piped in DMB for readout", 288, 0, 288);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("Number of L1A");
		h[hname]->SetYTitle("Number of Events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(3);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dDMB_FIFO_stats", ChamberID);
		h[hname] = new TH2F(TString(hname.c_str()), "FEDs FIFO Status", 7,  0 , 7, 3, 0, 3);
		h[hname]->SetXTitle("FEDs");
		h[hname]->GetXaxis()->SetBinLabel(1,"ALCT");
		h[hname]->GetXaxis()->SetBinLabel(2,"TMB");
		for (int i=1; i<=5; i++) h[hname]->GetXaxis()->SetBinLabel(i+2,Form("CFEB%d",i));
		h[hname]->SetYTitle("FIFO States");
		h[hname]->GetYaxis()->SetBinLabel(1,"EMPTY");
		h[hname]->GetYaxis()->SetBinLabel(2,"HALF");
		h[hname]->GetYaxis()->SetBinLabel(3,"FULL");
		h[hname]->SetOption("textcolz");
		cnv[cnvname]->cd(4,1,1);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		TPaveText *pt10 = new TPaveText(0.23,0.72,0.31,0.81,"brNDC");
		pt10->SetTextAlign(12);
		pt10->AddText("n/a");
		//pt10->Draw(); //KK
		TPaveText *pt11 = new TPaveText(0.23,0.2,0.31,0.29,"brNDC");
		pt11->SetTextAlign(12);
		pt11->AddText("n/a");
		//pt11->Draw(); //KK
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

//ALCTs
	if(debug_printout) 	cout << "D**EmuBookChamber> New ALCT Canvases are booking ..." << endl;
	if(folders) 		path_to_folder = "ALCT/";

	cnvtitle = "ALCT: Number of Words in ALCT.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),1,1);
		hname = Form("hist/h%dALCT_Word_Count", ChamberID);
                h[hname] = new TH1F(TString(hname.c_str()), "", 1024, 0, 1024);
		h[hname]->SetMinimum(0.0);
                h[hname]->SetXTitle("Number of Words in ALCT (from ALCT Trailer)");
                h[hname]->SetYTitle("Number of events");
                h[hname]->SetFillColor(48);
                h[hname]->SetOption("hist");
                cnv[cnvname]->cd(1);
                h[hname]->Draw();
		gStyle->SetOptStat("emo");
                consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
        consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "ALCT: Anode Hit Occupancy per Chamber.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),2,1);
		hname = Form("hist/h%dALCT_Number_Of_Layers_With_Hits", ChamberID);
                h[hname] = new TH1F(TString(hname.c_str()), "Number of Layers with Hits", 7, 0, 7);
		h[hname]->SetMinimum(0.0);
                h[hname]->SetXTitle("Number of Layers");
                h[hname]->SetYTitle("Number of events");
                h[hname]->SetFillColor(48);
                h[hname]->SetOption("texthist");
                cnv[cnvname]->cd(1);
                h[hname]->Draw();
		gStyle->SetOptStat("emo");
                consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dALCT_Number_Of_WireGroups_With_Hits", ChamberID);
                h[hname] = new TH1F(TString(hname.c_str()), "Total Number of Wire Groups with Hits", 672, 1, 673);
		h[hname]->SetMinimum(0.0);
                h[hname]->SetXTitle("Number of Wire Groups");
                h[hname]->SetYTitle("Number of events");
                h[hname]->SetFillColor(48);
                h[hname]->SetOption("hist");
                cnv[cnvname]->cd(2);
		gPad->SetLogx();
                h[hname]->Draw();
		gStyle->SetOptStat("emo");
                consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
        consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "ALCT: Anode Hit Occupancy per Wire Group.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nLayer=1; nLayer<=6; nLayer++) {
		hname = Form("hist/h%dALCT_Ly%d_Rate", ChamberID, nLayer);
		h[hname] = new TH1F(TString(hname.c_str()), Form("Layer = %d", nLayer), 112, 0, 112);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("Anode Wiregroup");
                h[hname]->SetYTitle("Number of events");
                h[hname]->SetFillColor(48);
                h[hname]->SetOption("hist");

		hname = Form("hist/h%dALCT_Ly%d_Efficiency", ChamberID, nLayer);
		h[hname] = new TH1F(TString(hname.c_str()), Form("Layer = %d", nLayer), 112, 0, 112);
		h[hname]->SetXTitle("Anode Wiregroup");
                h[hname]->SetYTitle("Occupancy (% of total DMB events)");
                h[hname]->SetFillColor(48);
                h[hname]->SetOption("hist");
		cnv[cnvname]->cd(nLayer);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	}
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "ALCT: Raw Hit Time Bin Occupancy.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nLayer=1; nLayer<=6; nLayer++) {
		hname = Form("hist/h%dALCTTime_Ly%d", ChamberID, nLayer);
		h[hname] = new TH2F(TString(hname.c_str()), Form("Layer = %d", nLayer), 112, 0, 112, 32, 0, 32);
		h[hname]->SetXTitle("Anode Wiregroup");
                h[hname]->SetYTitle("Time Bin");
		h[hname]->SetOption("col");
		cnv[cnvname]->cd(nLayer);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	}
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "ALCT: Raw Hit Time Bin Average Occupancy.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nLayer=1; nLayer<=6; nLayer++) {
		hname = Form("hist/h%dALCTTime_Ly%d_Profile", ChamberID, nLayer);
		h[hname] = new TProfile(TString(hname.c_str()), Form("Layer = %d", nLayer), 112, 0, 112, 0, 32, "g");
		h[hname]->SetXTitle("Anode Wiregroup");
                h[hname]->SetYTitle("Average Time Bin");
		h[hname]->SetOption("col");
		h[hname]->SetLineWidth(TProfileLineWidth);
		h[hname]->SetMarkerColor(TProfileMarkerColor);
		h[hname]->SetMarkerStyle(TProfileMarkerStyle);
		h[hname]->SetMarkerSize(TProfileMarkerSize);
		h[hname]->SetOption(TProfileDrawOption);
		cnv[cnvname]->cd(nLayer);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	}
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "ALCT: ALCTs Found";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),2,1);
		hname = Form("hist/h%dALCT_Number_Rate", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Number of ALCTs", 3, 0, 3 );
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dALCT_Number_Efficiency", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Number of ALCTs", 3, 0, 3 );
		h[hname]->SetXTitle("Number of ALCTs");
		for(int i = 1; i<=3; i++) h[hname]->GetXaxis()->SetBinLabel(i,Form("%d",i-1));
		h[hname]->SetYTitle("Efficiency (% of total DMB events)");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("texthist");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dALCT1_vs_ALCT0_KeyWG", ChamberID);
		h[hname] = new TH2F(TString(hname.c_str()), "ALCT1 & ALCT0 Correlation", 112, 0, 112, 112, 0, 112);
		h[hname]->SetXTitle("ALCT0 Key Wiregroup");
		h[hname]->SetYTitle("ALCT1 Key Wiregroup");
		h[hname]->SetOption("col");
		cnv[cnvname]->cd(2);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	for(Int_t lct = 0; lct<=1; lct++) {
		cnvtitle = Form("ALCT: ALCT%d Key Wiregroups, Patterns and Quality.",lct);
		cnvname  = path_to_folder + IDTextShort + cnvtitle;
		cnvtitle = cnvtitle + IDTextLong;
		cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),2,2);
			hname = Form("hist/h%dALCT%d_KeyWG", ChamberID, lct);
			h[hname] = new TH1F(TString(hname.c_str()), Form("ALCT%d Key Wiregroup", lct), 112, 0, 112);
			h[hname]->SetMinimum(0.0);
			h[hname]->SetXTitle("Key Wiregroup");
			h[hname]->SetYTitle("Number of events");
			h[hname]->SetFillColor(48);
			h[hname]->SetOption("hist");
			cnv[cnvname]->cd(1);
			h[hname]->Draw();
			gStyle->SetOptStat("em");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

			hname = Form("hist/h%dALCT%d_Quality", ChamberID, lct);
			h[hname] = new TH2F(TString(hname.c_str()), Form("ALCT%d Quality vs Key Wiregroup", lct), 112, 0, 112, 4, 0, 4);
			h[hname]->SetXTitle("Key Wiregroup");
			h[hname]->GetYaxis()->SetBinLabel(1, "Q=0");
			h[hname]->GetYaxis()->SetBinLabel(2, "Q=1");
			h[hname]->GetYaxis()->SetBinLabel(3, "Q=2");
			h[hname]->GetYaxis()->SetBinLabel(4, "Q=3");
			h[hname]->SetOption("col");
			cnv[cnvname]->cd(2,0,1);
			h[hname]->Draw();
			gStyle->SetOptStat("e");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

			hname = Form("hist/h%dALCT%d_Pattern", ChamberID, lct);
			h[hname] = new TH2F(TString(hname.c_str()), Form("ALCT%d Patterns vs Key Wiregroup", lct), 112, 0, 112, 4, 0, 4);
			h[hname]->SetXTitle("Key Wiregroup");
			h[hname]->GetYaxis()->SetBinLabel(1, "Collision A");
			h[hname]->GetYaxis()->SetBinLabel(2, "Collision B");
			h[hname]->GetYaxis()->SetBinLabel(3, "Acelerator");
			h[hname]->GetYaxis()->SetBinLabel(4, "N/A");
			h[hname]->SetOption("col");
			cnv[cnvname]->cd(3,0,1);
			h[hname]->Draw();
			gStyle->SetOptStat("e");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

			hname = Form("hist/h%dALCT%d_Quality_Profile", ChamberID, lct);
			h[hname] = new TProfile(TString(hname.c_str()), Form("ALCT%d Average Quality", lct), 112, 0, 112, 0, 4, "g");
			h[hname]->SetMaximum(4.0);
			h[hname]->SetXTitle("Key Wiregroup");
			h[hname]->SetYTitle("Average Quality");
			h[hname]->SetLineWidth(TProfileLineWidth);
			h[hname]->SetMarkerColor(TProfileMarkerColor);
			h[hname]->SetMarkerStyle(TProfileMarkerStyle);
			h[hname]->SetMarkerSize(TProfileMarkerSize);
			h[hname]->SetOption(TProfileDrawOption);
			cnv[cnvname]->cd(4,0,1);
			h[hname]->Draw();
			gStyle->SetOptStat("e");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
		consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

		cnvtitle = Form("ALCT: ALCT%d_BXN and ALCT_L1A_BXN Synchronization.",lct);
		cnvname  = path_to_folder + IDTextShort + cnvtitle;
		cnvtitle = cnvtitle + IDTextLong;
		cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),2,2);
			hname = Form("hist/h%dALCT%d_BXN", ChamberID, lct);
			h[hname] = new TH1F(TString(hname.c_str()), Form("ALCT%d BXN", lct), 32, 0.0, 32.0);
			h[hname]->SetMinimum(0.0);
			h[hname]->SetXTitle(Form("ALCT%d BXN", lct));
			h[hname]->SetYTitle("Number of events");
			h[hname]->SetFillColor(48);
			h[hname]->SetOption("hist");
			cnv[cnvname]->cd(1);
			h[hname]->Draw();
			gStyle->SetOptStat("eo");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

			hname = Form("hist/h%dALCT%d_dTime_vs_KeyWG", ChamberID, lct);
			h[hname] = new TH2F(TString(hname.c_str()), Form("(ALCT%d BXN - ALCT_L1A BXN) vs Key Wiregroup", lct), 112, 0.0, 112.0, 32, -16.0, 16.0);
			h[hname]->SetXTitle("Key Wiregroup");
			h[hname]->SetYTitle(Form("ALCT%d BXN - ALCT_L1A BXN", lct));
			h[hname]->SetOption("col");
			cnv[cnvname]->cd(2);
			h[hname]->Draw();
			gStyle->SetOptStat("e");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

			hname = Form("hist/h%dALCT%d_dTime", ChamberID, lct);
			h[hname] = new TH1F(TString(hname.c_str()), Form("ALCT%d BXN - ALCT_L1A BXN difference", lct), 32, -16.0, 16.0);
			h[hname]->SetXTitle(Form("ALCT%d BXN - ALCT_L1A BXN", lct));
			h[hname]->SetYTitle("Number of events");
			h[hname]->SetFillColor(48);
			h[hname]->SetOption("hist");
			cnv[cnvname]->cd(3);
			gPad->SetLogy();
			h[hname]->SetAxisRange(0.1, 1.1, "Y");
			h[hname]->Draw();
			gStyle->SetOptStat("emuo");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

			hname = Form("hist/h%dALCT%d_dTime_Profile", ChamberID, lct);
			h[hname] = new TProfile(TString(hname.c_str()), Form("Average (ALCT%d BXN - L1A BXN) vs Key Wiregroup", lct), 112, 0.0, 112.0, -16.0, 16.0, "g");
			h[hname]->SetXTitle("Key Wiregroup");
			h[hname]->SetYTitle(Form("Average (ALCT%d BXN - L1A BXN)", lct));
			h[hname]->SetLineWidth(TProfileLineWidth);
			h[hname]->SetMarkerColor(TProfileMarkerColor);
			h[hname]->SetMarkerStyle(TProfileMarkerStyle);
			h[hname]->SetMarkerSize(TProfileMarkerSize);
			h[hname]->SetOption(TProfileDrawOption);
			cnv[cnvname]->cd(4);
			h[hname]->Draw();
			gStyle->SetOptStat("e");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
		consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);
	}

//TMB
	if(debug_printout) 	cout << "D**EmuBookChamber> New TMB Canvases are booking ..." << endl;
	if(folders) 		path_to_folder = "TMB/";

	cnvtitle = "TMB: Number of Words in TMB.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),1,1);
		hname = Form("hist/h%dTMB_Word_Count", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "", 2048, 0, 2048);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("Number of Words in TMB (from TMB Trailer)");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		gStyle->SetOptStat("emo");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "TMB: ALCT - CLCT Time MatchingSynchronization.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),2,2);
		hname = Form("hist/h%dALCT_Match_Time", ChamberID);
                h[hname] = new TH1F(TString(hname.c_str()), "Location of ALCT in CLCT match window", 16, 0, 16);
		h[hname]->SetMinimum(0.0);
                h[hname]->SetXTitle("ALCT location in CLCT-tagged window");
                h[hname]->SetYTitle("Number of events");
                h[hname]->SetFillColor(48);
                h[hname]->SetOption("hist");
                cnv[cnvname]->cd(1);
                h[hname]->Draw();
		gStyle->SetOptStat("e");
                consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dLCT_Match_Status", ChamberID);
                h[hname] = new TH2F(TString(hname.c_str()), "ALCT-CLCT match status", 1, 0, 1, 3, 0, 3);
                h[hname]->GetXaxis()->SetBinLabel(1,"Number of events");
                h[hname]->GetYaxis()->SetBinLabel(1,"CLCT only");
                h[hname]->GetYaxis()->SetBinLabel(2,"ALCT only");
                h[hname]->GetYaxis()->SetBinLabel(3,"Matched LCT");
                h[hname]->SetOption("coltext");
                cnv[cnvname]->cd(2);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dLCT0_Match_BXN_Difference", ChamberID);
                h[hname] = new TH1F(TString(hname.c_str()), "ALCT-CLCT BXN Difference for Matched LCT0", 4, 0, 4);
		h[hname]->SetMinimum(0.0);
                h[hname]->SetXTitle("BXN difference");
                h[hname]->SetYTitle("Number of events");
                h[hname]->SetFillColor(48);
                h[hname]->SetOption("hist");
                cnv[cnvname]->cd(3);
                h[hname]->Draw();
		gStyle->SetOptStat("e");
                consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dLCT1_Match_BXN_Difference", ChamberID);
                h[hname] = new TH1F(TString(hname.c_str()), "ALCT-CLCT BXN Difference for Matched LCT1", 4, 0, 4);
		h[hname]->SetMinimum(0.0);
                h[hname]->SetXTitle("BXN difference");
                h[hname]->SetYTitle("Number of events");
                h[hname]->SetFillColor(48);
                h[hname]->SetOption("hist");
                cnv[cnvname]->cd(4);
                h[hname]->Draw();
		gStyle->SetOptStat("e");
                consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

//TMB - CLCTs
	if(debug_printout) 	cout << "D**EmuBookChamber> New TMB-CLCT Canvases are booking ..." << endl;
	if(folders) 		path_to_folder = "TMB/";

	cnvtitle = "TMB-CLCT: Cathode Comparator Hit Occupancy per Chamber.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),2,1);
		hname = Form("hist/h%dCLCT_Number_Of_Layers_With_Hits", ChamberID);
                h[hname] = new TH1F(TString(hname.c_str()), "Number of Layers with Hits", 7, 0, 7);
		h[hname]->SetMinimum(0.0);
                h[hname]->SetXTitle("Number of Layers");
                h[hname]->SetYTitle("Number of events");
                h[hname]->SetFillColor(48);
                h[hname]->SetOption("texthist");
                cnv[cnvname]->cd(1);
                h[hname]->Draw();
		gStyle->SetOptStat("emo");
                consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dCLCT_Number_Of_HalfStrips_With_Hits", ChamberID);
                h[hname] = new TH1F(TString(hname.c_str()), "Total Number of HalfStrips with Hits", 672, 1, 673);
		h[hname]->SetMinimum(0.0);
                h[hname]->SetXTitle("Number of HalfStrips");
                h[hname]->SetYTitle("Number of events");
                h[hname]->SetFillColor(48);
                h[hname]->SetOption("hist");
                cnv[cnvname]->cd(2);
		gPad->SetLogx();
                h[hname]->Draw();
		gStyle->SetOptStat("emo");
                consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
        consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "TMB-CLCT: Cathode Comparator Hit Occupancy per Half Strip.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nLayer=1; nLayer<=6; nLayer++) {
		hname = Form("hist/h%dCLCT_Ly%d_Rate", ChamberID, nLayer);
		h[hname] = new TH1F(TString(hname.c_str()), Form("Layer = %d",nLayer), 160, 0, 160);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("Cathode HalfStrip");
                h[hname]->SetYTitle("Number of events");
                h[hname]->SetFillColor(48);
                h[hname]->SetOption("hist");

		hname = Form("hist/h%dCLCT_Ly%d_Efficiency",ChamberID, nLayer);
		h[hname] = new TH1F(TString(hname.c_str()), Form("Layer = %d",nLayer), 160, 0, 160);
		h[hname]->SetXTitle("Cathode HalfStrip");
                h[hname]->SetYTitle("Efficiency (% of total DMB events)");
                h[hname]->SetFillColor(48);
                h[hname]->SetOption("hist");
		cnv[cnvname]->cd(nLayer);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	}
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "TMB-CLCT: Comparator Raw Hit Time Bin Occupancy.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nLayer=1; nLayer<=6; nLayer++) {
		hname = Form("hist/h%dCLCTTime_Ly%d", ChamberID, nLayer);
		h[hname] = new TH2F(TString(hname.c_str()), Form("Layer = %d",nLayer), 160, 0, 160, 32, 0, 32);
		h[hname]->SetXTitle("Cathode HalfStrip");
                h[hname]->SetYTitle("Time Bin");
		h[hname]->SetOption("col");
		cnv[cnvname]->cd(nLayer);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	}
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "TMB-CLCT: Comparator Raw Hit Time Bin Average Occupancy.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
        for (int nLayer=1; nLayer<=6; nLayer++) {
		hname = Form("hist/h%dCLCTTime_Ly%d_Profile", ChamberID, nLayer);
		h[hname] = new TProfile(TString(hname.c_str()), Form("Layer = %d",nLayer), 160, 0, 160, 0, 32, "g");
		h[hname]->SetXTitle("Cathode HalfStrip");
                h[hname]->SetYTitle("Average Time Bin");
		h[hname]->SetOption("col");
		h[hname]->SetLineWidth(TProfileLineWidth);
		h[hname]->SetMarkerColor(TProfileMarkerColor);
		h[hname]->SetMarkerStyle(TProfileMarkerStyle);
		h[hname]->SetMarkerSize(TProfileMarkerSize);
		h[hname]->SetOption(TProfileDrawOption);
		cnv[cnvname]->cd(nLayer);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	}
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "TMB-CLCT: CLCTs Found.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),2,2);
		hname = Form("hist/h%dCLCT_Number_Rate", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Number of CLCTs", 3, 0, 3 );
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dCLCT_Number", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Number of CLCTs", 3, 0, 3 );
		h[hname]->SetXTitle("Number of CLCTs");
		for(int i = 1; i<=3; i++) h[hname]->GetXaxis()->SetBinLabel(i,Form("%d",i-1));
		h[hname]->SetYTitle("Efficiency (% of total DMB events)");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("texthist");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dCLCT1_vs_CLCT0_Key_Strip", ChamberID);
		h[hname] = new TH2F(TString(hname.c_str()), "CLCT1 & CLCT0 Correlation", 160, 0, 160, 160, 0, 160);
		h[hname]->SetXTitle("CLCT0 Key Strip");
		h[hname]->SetYTitle("CLCT1 Key Strip");
		h[hname]->SetOption("col");
		cnv[cnvname]->cd(2);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dCLCT0_Clssification", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Single CLCT Classification (CLCT0)", 2, 0, 2);
		h[hname]->SetXTitle("Key Strips");
		h[hname]->GetXaxis()->SetBinLabel(1,"Half");
		h[hname]->GetXaxis()->SetBinLabel(2,"Di");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("texthist");
		cnv[cnvname]->cd(3);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dCLCT0_CLCT1_Clssification", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Double CLCT Classification (CLCT0+CLCT1)", 4, 0, 4);
		h[hname]->SetXTitle("Key Strips Combination");
		h[hname]->GetXaxis()->SetBinLabel(1,"Half+Half");
		h[hname]->GetXaxis()->SetBinLabel(2,"Half+Di");
		h[hname]->GetXaxis()->SetBinLabel(3,"Di+Half");
		h[hname]->GetXaxis()->SetBinLabel(4,"Di+Di");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("texthist");
		cnv[cnvname]->cd(4);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	for(int lct=0; lct<=1; lct++) {
		cnvtitle = Form("TMB-CLCT: CLCT%d Key HalfStrips, Patterns and Quality.",lct);
		cnvname  = path_to_folder + IDTextShort + cnvtitle;
		cnvtitle = cnvtitle + IDTextLong;
		cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),2,2);
			hname = Form("hist/h%dCLCT%d_KeyHalfStrip", ChamberID, lct);
			h[hname] = new TH1F(TString(hname.c_str()), Form("CLCT%d Key Half Strip",lct), 160, 0, 160);
			h[hname]->SetMinimum(0.0);
			h[hname]->SetXTitle("Key Half Strip");
			h[hname]->SetYTitle("Number of events");
			h[hname]->SetFillColor(48);
			h[hname]->SetOption("hist");
			cnv[cnvname]->cd(1);
			h[hname]->Draw();
			gStyle->SetOptStat("em");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

			hname = Form("hist/h%dCLCT%d_Half_Strip_Quality", ChamberID, lct);
			h[hname] = new TH2F(TString(hname.c_str()), Form("CLCT%d Quality vs Key Half Strip",lct), 160, 0, 160, 7, 0, 7);
			h[hname]->SetXTitle("Key Half Strip");
			h[hname]->GetYaxis()->SetBinLabel(1, "Q=0");
			h[hname]->GetYaxis()->SetBinLabel(2, "Q=1");
			h[hname]->GetYaxis()->SetBinLabel(3, "Q=2");
			h[hname]->GetYaxis()->SetBinLabel(4, "Q=3");
			h[hname]->GetYaxis()->SetBinLabel(5, "Q=4");
			h[hname]->GetYaxis()->SetBinLabel(6, "Q=5");
			h[hname]->GetYaxis()->SetBinLabel(7, "Q=6");
			h[hname]->SetOption("col");
			cnv[cnvname]->cd(2,0,1);
			h[hname]->Draw();
			gStyle->SetOptStat("e");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

			hname = Form("hist/h%dCLCT%d_Half_Strip_Pattern", ChamberID, lct);
			h[hname] = new TH2F(TString(hname.c_str()), Form("CLCT%d Patterns vs Key Half Strip",lct), 160, 0, 160, 8, 0, 8);
			h[hname]->SetXTitle("Key Half Strip");
			h[hname]->GetYaxis()->SetBinLabel(1, "P=0");
			h[hname]->GetYaxis()->SetBinLabel(2, "P=2");
			h[hname]->GetYaxis()->SetBinLabel(3, "P=4");
			h[hname]->GetYaxis()->SetBinLabel(4, "P=6");
			h[hname]->GetYaxis()->SetBinLabel(5, "P=7");
			h[hname]->GetYaxis()->SetBinLabel(6, "P=5");
			h[hname]->GetYaxis()->SetBinLabel(7, "P=3");
			h[hname]->GetYaxis()->SetBinLabel(8, "P=1");
			h[hname]->SetOption("col");
			cnv[cnvname]->cd(3,0,1);
			h[hname]->Draw();
			gStyle->SetOptStat("e");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

			hname = Form("hist/h%dCLCT%d_Half_Strip_Quality_Profile", ChamberID, lct);
			h[hname] = new TProfile(TString(hname.c_str()), Form("CLCT%d Average Quality",lct), 160, 0.0, 160.0, 0.0, 7.0, "g");
			h[hname]->SetXTitle("Key Half Strip");
			h[hname]->SetYTitle("Average Quality");
			h[hname]->SetOption("col");
			h[hname]->SetLineWidth(TProfileLineWidth);
			h[hname]->SetMarkerColor(TProfileMarkerColor);
			h[hname]->SetMarkerStyle(TProfileMarkerStyle);
			h[hname]->SetMarkerSize(TProfileMarkerSize);
			h[hname]->SetOption(TProfileDrawOption);
			cnv[cnvname]->cd(4,0,1);
			h[hname]->Draw();
			gStyle->SetOptStat("e");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
		consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

		cnvtitle = Form("TMB-CLCT: CLCT%d Key DiStrips, Patterns and Quality.",lct);
		cnvname  = path_to_folder + IDTextShort + cnvtitle;
		cnvtitle = cnvtitle + IDTextLong;
		cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),2,2);
			hname = Form("hist/h%dCLCT%d_KeyDiStrip", ChamberID, lct);
			h[hname] = new TH1F(TString(hname.c_str()), Form("CLCT%d Key DiStrip",lct), 40, 0, 160);
			h[hname]->SetMinimum(0.0);
			h[hname]->SetXTitle("Key DiStrip");
			h[hname]->SetYTitle("Number of events");
			h[hname]->SetFillColor(48);
			h[hname]->SetOption("hist");
			cnv[cnvname]->cd(1);
			h[hname]->Draw();
			gStyle->SetOptStat("em");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

			hname = Form("hist/h%dCLCT%d_DiStrip_Quality", ChamberID, lct);
			h[hname] = new TH2F(TString(hname.c_str()), Form("CLCT%d Quality vs Key DiStrip",lct), 40, 0, 160, 7, 0, 7);
			h[hname]->SetXTitle("Key DiStrip");
			h[hname]->GetYaxis()->SetBinLabel(1, "Q=0");
			h[hname]->GetYaxis()->SetBinLabel(2, "Q=1");
			h[hname]->GetYaxis()->SetBinLabel(3, "Q=2");
			h[hname]->GetYaxis()->SetBinLabel(4, "Q=3");
			h[hname]->GetYaxis()->SetBinLabel(5, "Q=4");
			h[hname]->GetYaxis()->SetBinLabel(6, "Q=5");
			h[hname]->GetYaxis()->SetBinLabel(7, "Q=6");
			h[hname]->SetOption("col");
			cnv[cnvname]->cd(2,0,1);
			h[hname]->Draw();
			gStyle->SetOptStat("e");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

			hname = Form("hist/h%dCLCT%d_DiStrip_Pattern", ChamberID, lct);
			h[hname] = new TH2F(TString(hname.c_str()), Form("CLCT%d Patterns vs Key DiStrip",lct), 40, 0, 160, 8, 0, 8);
			h[hname]->SetXTitle("Key DiStrip");
			h[hname]->GetYaxis()->SetBinLabel(1, "P=0");
			h[hname]->GetYaxis()->SetBinLabel(2, "P=2");
			h[hname]->GetYaxis()->SetBinLabel(3, "P=4");
			h[hname]->GetYaxis()->SetBinLabel(4, "P=6");
			h[hname]->GetYaxis()->SetBinLabel(5, "P=7");
			h[hname]->GetYaxis()->SetBinLabel(6, "P=5");
			h[hname]->GetYaxis()->SetBinLabel(7, "P=3");
			h[hname]->GetYaxis()->SetBinLabel(8, "P=1");
			h[hname]->SetOption("col");
			cnv[cnvname]->cd(3,0,1);
			h[hname]->Draw();
			gStyle->SetOptStat("e");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

			hname = Form("hist/h%dCLCT%d_DiStrip_Quality_Profile", ChamberID, lct);
			h[hname] = new TProfile(TString(hname.c_str()), Form("CLCT%d Average Quality",lct), 40, 0, 160, 0, 7, "g");
			h[hname]->SetXTitle("Key DiStrip");
			h[hname]->SetYTitle("Average Quality");
			h[hname]->SetOption("col");
			h[hname]->SetLineWidth(TProfileLineWidth);
			h[hname]->SetMarkerColor(TProfileMarkerColor);
			h[hname]->SetMarkerStyle(TProfileMarkerStyle);
			h[hname]->SetMarkerSize(TProfileMarkerSize);
			h[hname]->SetOption(TProfileDrawOption);
			cnv[cnvname]->cd(4,0,1);
			h[hname]->Draw();
			gStyle->SetOptStat("e");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
		consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

		cnvtitle = Form("TMB-CLCT: CLCT%d_BXN and TMB_L1A_BXN Synchronization.",lct);
		cnvname  = path_to_folder + IDTextShort + cnvtitle;
		cnvtitle = cnvtitle + IDTextLong;
		cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),2,2);
			hname = Form("hist/h%dCLCT%d_BXN", ChamberID, lct);
			h[hname] = new TH1F(TString(hname.c_str()), Form("CLCT%d BXN",lct), 4, 0.0, 4.0);
			h[hname]->SetMinimum(0.0);
			h[hname]->SetXTitle(Form("CLCT%d BXN",lct));
			h[hname]->SetYTitle("Number of events");
			h[hname]->SetFillColor(48);
			h[hname]->SetOption("hist");
			cnv[cnvname]->cd(1);
			h[hname]->Draw();
			gStyle->SetOptStat("eo");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

			hname = Form("hist/h%dCLCT%d_dTime_vs_Half_Strip", ChamberID, lct);
			h[hname] = new TH2F(TString(hname.c_str()), Form("(CLCT%d BXN - TMB_L1A BXN) vs Key Half Strip",lct), 160, 0, 160, 8, -4.0, 4.0);
			h[hname]->SetXTitle("Key Half Strip");
			h[hname]->SetYTitle(Form("CLCT%d BXN - TMB_L1A BXN",lct));
			h[hname]->SetOption("col");
			cnv[cnvname]->cd(2);
			h[hname]->Draw();
			gStyle->SetOptStat("e");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

			hname = Form("hist/h%dCLCT%d_dTime", ChamberID, lct);
			h[hname] = new TH1F(TString(hname.c_str()), Form("CLCT%d BXN - TMB_L1A BXN Difference",lct), 8, -4.0, 4.0);
			h[hname]->SetXTitle(Form("CLCT%d BXN - TMB_L1A BXN",lct));
        		h[hname]->SetYTitle("Number of events");
	        	h[hname]->SetFillColor(48);
			h[hname]->SetOption("hist");
			cnv[cnvname]->cd(3);
			gPad->SetLogy();
			h[hname]->SetAxisRange(0.1, 1.1, "Y");
			h[hname]->Draw();
			gStyle->SetOptStat("emuo");
	       		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

			hname = Form("hist/h%dCLCT%d_dTime_vs_DiStrip", ChamberID, lct);
			h[hname] = new TH2F(TString(hname.c_str()), Form("(CLCT%d BXN - TMB_L1A BXN) vs Key DiStrip",lct), 40, 0, 160, 8, -4.0, 4.0);
			h[hname]->SetXTitle("Key DiStrip");
			h[hname]->SetYTitle(Form("CLCT%d BXN - TMB_L1A BXN",lct));
			h[hname]->SetOption("col");
			cnv[cnvname]->cd(4);
			h[hname]->Draw();
			gStyle->SetOptStat("e");
			consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
		consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);
	}

// CFEBs
	if(debug_printout) 	cout << "D**EmuBookChamber> New CFEB Canvases are booking ..." << endl;
	if(folders) 		path_to_folder = "CFEB/";

//	CFEBs by numbers
	cnvtitle = "CFEB: SCA Block Occupancy.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nCFEB=0; nCFEB<5; nCFEB++) {
		hname = Form("hist/h%dCFEB%d_SCA_Block_Occupancy", ChamberID, nCFEB);
		h[hname] = new TH1F(TString(hname.c_str()), Form("CFEB%d",nCFEB), 16, 0.0, 16.0);
		h[hname]->SetXTitle("SCA Block");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		cnv[cnvname]->cd(nCFEB+1);
		h[hname]->Draw();
		gStyle->SetOptStat("em");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
        }
        consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

//	CFEBs by layers
	cnvtitle = "CFEB: SCA Active Strips Occupancy.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nLayer=1; nLayer<=6; nLayer++) {
		hname = Form("hist/h%dCFEB_ActiveStrips_Ly%d", ChamberID, nLayer);
		h[hname] = new TH1F(TString(hname.c_str()), Form("Layer %d", nLayer), 80, 0, 80);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("Strip number");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		cnv[cnvname]->cd(nLayer);
		h[hname]->Draw();
		gStyle->SetOptStat("em");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	}
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "CFEB: SCA Active Time Samples vs Strip Numbers.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nLayer=1; nLayer<=6; nLayer++) {
		hname = Form("hist/h%dCFEB_Active_Samples_vs_Strip_Ly%d", ChamberID, nLayer);
		h[hname] = new TH2F(TString(hname.c_str()), Form("Layer %d", nLayer), 80, 0, 80, 16, 0, 16);
		h[hname]->SetXTitle("Strip number");
		h[hname]->SetYTitle("Active sample number");
		h[hname]->SetOption("col");
		cnv[cnvname]->cd(nLayer);
		h[hname]->Draw();
		gStyle->SetOptStat("em");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	}
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "CFEB: SCA Active Time Samples vs Strip Numbers Profile.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nLayer=1; nLayer<=6; nLayer++) {
		hname = Form("hist/h%dCFEB_Active_Samples_vs_Strip_Ly%d_Profile", ChamberID, nLayer);
		h[hname] = new TProfile(TString(hname.c_str()), Form("Layer %d", nLayer), 80, 0, 80, 0, 16,"g");
		h[hname]->SetXTitle("Strip number");
		h[hname]->SetYTitle("Active sample number");
		h[hname]->SetOption("col");
		h[hname]->SetLineWidth(TProfileLineWidth);
		h[hname]->SetMarkerColor(TProfileMarkerColor);
		h[hname]->SetMarkerStyle(TProfileMarkerStyle);
		h[hname]->SetMarkerSize(TProfileMarkerSize);
		h[hname]->SetOption(TProfileDrawOption);
		cnv[cnvname]->cd(nLayer);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	}
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "CFEB: SCA Cell Occupancy.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nLayer=1; nLayer<=6; nLayer++) {
                hname = Form("hist/h%dCFEB_SCA_Cell_Occupancy_Ly_%d", ChamberID, nLayer);
                h[hname] = new TH2F(TString(hname.c_str()), Form("Layer %d", nLayer), 80, 0.0, 80.0, 96, 0.0, 96.0);
		h[hname]->SetXTitle("Strip number");
		h[hname]->SetYTitle("SCA Cell");
		h[hname]->SetOption("col");
		cnv[cnvname]->cd(nLayer);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	}
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "CFEB: SCA Cell Peak";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nLayer=1; nLayer<=6; nLayer++) {
		hname = Form("hist/h%dCFEB_SCA_Cell_Peak_Ly_%d", ChamberID, nLayer);
		h[hname] = new TH2F(TString(hname.c_str()), Form("Layer %d", nLayer), 80, 0.0, 80.0, 96, 0.0, 96.0);
		h[hname]->SetXTitle("Strip number");
		h[hname]->SetYTitle("SCA Cell");
		h[hname]->SetOption("col");
		cnv[cnvname]->cd(nLayer);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	}
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "CFEB: Pedestals (First Sample).";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nLayer=1; nLayer<=6; nLayer++) {
//		Histograms of CFEB Pedestals with Error on Mean Value: Option = "g"
		hname = Form("hist/h%dCFEB_Pedestal(withEMV)_Sample_01_Ly%d", ChamberID, nLayer);
		h[hname] = new TProfile(TString(hname.c_str()), Form("Layer %d", nLayer), 80, 0, 80, 0, 4096,"g");
		h[hname]->SetXTitle("Strip number");
		h[hname]->SetYTitle("Pedestal Value (in ADC Chanels)");
		h[hname]->SetLineWidth(TProfileLineWidth);
		h[hname]->SetMarkerColor(TProfileMarkerColor);
		h[hname]->SetMarkerStyle(TProfileMarkerStyle);
		h[hname]->SetMarkerSize(TProfileMarkerSize);
		h[hname]->SetOption(TProfileDrawOption);
		cnv[cnvname]->cd(nLayer);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	}
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

        cnvtitle = "CFEB: Pedestals RMS.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nLayer=1; nLayer<=6; nLayer++) {
//		Histograms of CFEB Pedestals with Root Mean Square: Option = "s"
                hname = Form("hist/h%dCFEB_Pedestal(withRMS)_Sample_01_Ly%d", ChamberID, nLayer);
		h[hname] = new TProfile(TString(hname.c_str()), Form("Layer %d", nLayer), 80, 0, 80, 0, 4096,"s");
		h[hname]->SetXTitle("Strip number");
		h[hname]->SetYTitle("Pedestal Value (in ADC Chanels)");

		hname = Form("hist/h%dCFEB_PedestalRMS_Sample_01_Ly%d",ChamberID,nLayer);
                h[hname] = new TH1F(TString(hname.c_str()), Form("Layer %d", nLayer), 80, 0.0, 80.0);
                h[hname]->SetXTitle("Strip number");
                h[hname]->SetYTitle("Pedestal RMS Value (in ADC Chanels)");
                h[hname]->SetFillColor(48);
                cnv[cnvname]->cd(nLayer);
                gStyle->SetOptStat("e");
                h[hname]->Draw();
                consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
        }
        consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "CFEB: Out of ADC Range Strips.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nLayer=1; nLayer<=6; nLayer++) {
		hname = Form("hist/h%dCFEB_Out_Off_Range_Strips_Ly%d", ChamberID, nLayer);
		h[hname] = new TH1F(TString(hname.c_str()), Form("Layer %d", nLayer), 80, 0, 80);
		h[hname]->SetMinimum(0.0);
  		h[hname]->SetXTitle("Strip Number");
		h[hname]->SetYTitle("Number of Events");
		h[hname]->SetFillColor(48);
		cnv[cnvname]->cd(nLayer);
		h[hname]->Draw();
		gStyle->SetOptStat("em");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	}
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

        cnvtitle = "CFEB: Number of Clusters.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nLayer=1; nLayer<=6; nLayer++) {
		hname = Form("hist/h%dCFEB_Number_of_Clusters_Ly_%d", ChamberID, nLayer);
		h[hname] = new TH1F(TString(hname.c_str()), Form("Layer %d", nLayer), 41, 0.0, 41.0);
		h[hname]->SetXTitle("Number of Clusters");
		h[hname]->SetYTitle("Number of Events");
		h[hname]->SetFillColor(48);
		cnv[cnvname]->cd(nLayer);
		h[hname]->Draw();
		gStyle->SetOptStat("em");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	}
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "CFEB: Clusters Width.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nLayer=1; nLayer<=6; nLayer++) {
                hname = Form("hist/h%dCFEB_Width_of_Clusters_Ly_%d", ChamberID, nLayer);
                h[hname] = new TH1F(TString(hname.c_str()), Form("Layer %d", nLayer), 80, 0.0, 80.0);
                h[hname]->SetXTitle("Width of Clusters");
                h[hname]->SetYTitle("Number of Events");
                h[hname]->SetFillColor(48);
                cnv[cnvname]->cd(nLayer);
                h[hname]->Draw();
		gStyle->SetOptStat("em");
                consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
        }
        consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "CFEB: Clusters Charge.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
	for (int nLayer=1; nLayer<=6; nLayer++) {
                hname = Form("hist/h%dCFEB_Clusters_Charge_Ly_%d", ChamberID, nLayer);
                h[hname] = new TH1F(TString(hname.c_str()), Form("Layer %d", nLayer), 100, 80, 12000);
                h[hname]->SetXTitle("Clusters Charge (in ADC Chanels)");
                h[hname]->SetYTitle("Number of Events");
                h[hname]->SetFillColor(48);
                cnv[cnvname]->cd(nLayer);
                h[hname]->Draw();
		gStyle->SetOptStat("em");
                consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
        }
        consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

//SYNC
	if(debug_printout) 	cout << "D**EmuBookChamber> New SYNC Canvases are booking ..." << endl;
	if(folders) 		path_to_folder = "SYNC/";

	cnvtitle = "SYNC: DMB - DDU Synchronization.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
		hname = Form("hist/h%dDMB_L1A_Distrib", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Distribution of DMB L1A Counter", 256, 0.0, 256);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("DMB L1A");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		gStyle->SetOptStat("eo");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dDMB_DDU_L1A_diff", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Difference between DMB and DDU L1A numbers", 256, -128.0, 128.0);
		h[hname]->SetXTitle("DMB L1A - DDU L1A");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(2);
		gPad->SetLogy();
		h[hname]->SetAxisRange(0.1, 1.1, "Y");
		h[hname]->Draw();
		gStyle->SetOptStat("emou");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dDMB_L1A_vs_DDU_L1A", ChamberID);
		h[hname] = new TH2F(TString(hname.c_str()), "DMB L1A vs DDU L1A", 256, 0.0, 256.0, 256, 0.0, 256.0);
		h[hname]->SetXTitle("DDU L1A");
		h[hname]->SetYTitle("DMB L1A");
		h[hname]->SetOption("box");
		h[hname]->SetStats(false);
		cnv[cnvname]->cd(3);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dDMB_BXN_Distrib", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Distribution of DMB BXN Counter", 128,  0.0, 128.0);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("DMB BXN");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(4);
		h[hname]->Draw();
		gStyle->SetOptStat("eo");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dDMB_DDU_BXN_diff", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Difference between DMB and DDU BXN numbers", 128, -64.0, 64.0);
		h[hname]->SetXTitle("DMB BXN - DDU BXN");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(5);
		gPad->SetLogy();
		h[hname]->SetAxisRange(0.1, 1.1, "Y");
		h[hname]->Draw();
		gStyle->SetOptStat("emou");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dDMB_BXN_vs_DDU_BXN",ChamberID);
		h[hname] = new TH2F(TString(hname.c_str()), "DMB BXN vs DDU BXN", 4096, 0 , 4096, 128, 0, 128);
		h[hname]->SetXTitle("DDU BXN");
		h[hname]->SetYTitle("DMB BXN");
		h[hname]->SetOption("box");
		h[hname]->SetStats(false);
		cnv[cnvname]->cd(6);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "SYNC: ALCT - DMB Synchronization.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
		hname = Form("hist/h%dALCT_L1A", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Distribution of ALCT L1A counter", 16, 0.0, 16.0);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("ALCT L1A");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		gStyle->SetOptStat("eo");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dALCT_DMB_L1A_diff", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Difference between ALCT and DMB L1A numbers", 16,  -8 , 8);
		h[hname]->SetXTitle("DMB L1A - ALCT L1A");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(2);
		gPad->SetLogy();
		h[hname]->SetAxisRange(0.1, 1.1, "Y");
		h[hname]->Draw();
		gStyle->SetOptStat("emuo");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dDMB_L1A_vs_ALCT_L1A", ChamberID);
		h[hname] = new TH2F(TString(hname.c_str()), "DMB L1A vs ALCT L1A", 16, 0.0, 16.0, 256, 0.0, 256.0);
		h[hname]->SetXTitle("ALCT L1A");
		h[hname]->SetYTitle("DMB L1A");
		h[hname]->SetOption("box");
		h[hname]->SetStats(false);
		cnv[cnvname]->cd(3);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dALCT_BXN", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Distribution of ALCT BXN counter", 1024,  0 , 1024);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("ALCT BXN");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(4);
		h[hname]->Draw();
		gStyle->SetOptStat("eo");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dALCT_DMB_BXN_diff", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Difference between ALCT and DMB BXN numbers", 128,  -64 , 64);
		h[hname]->SetXTitle("DMB BXN - ALCT BXN");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(5);
		gPad->SetLogy();
		h[hname]->SetAxisRange(0.1, 1.1, "Y");
		h[hname]->Draw();
		gStyle->SetOptStat("emuo");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dALCT_BXN_vs_DMB_BXN", ChamberID);
		h[hname] = new TH2F(TString(hname.c_str()), "DMB BXN vs ALCT BXN Numbers", 1024,  0 , 1024, 128,  0 , 128);
		h[hname]->SetXTitle("ALCT BXN");
		h[hname]->SetYTitle("DMB BXN");
		h[hname]->SetOption("box");
		h[hname]->SetStats(false);
		cnv[cnvname]->cd(6);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "SYNC: TMB - DMB Synchronization.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,2);
		hname = Form("hist/h%dCLCT_L1A", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Distribution of TMB L1A Counter", 18,  0 , 18);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("TMB L1A");
		h[hname]->SetYTitle("Number of Events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		gStyle->SetOptStat("eo");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dCLCT_DMB_L1A_diff", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Difference between TMB and DMB L1A numbers", 16, -8, 8);
		h[hname]->SetXTitle("DMB_L1A-TMB_L1A");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(2);
		gPad->SetLogy();
		h[hname]->SetAxisRange(0.1, 1.1, "Y");
		h[hname]->Draw();
		gStyle->SetOptStat("emuo");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dDMB_L1A_vs_CLCT_L1A", ChamberID);
		h[hname] = new TH2F(TString(hname.c_str()), "DMB L1A vs TMB L1A", 16, 0.0, 16.0, 256, 0.0, 256.0);
		h[hname]->SetXTitle("TMB L1A");
		h[hname]->SetYTitle("DMB L1A");
		h[hname]->SetOption("box");
		h[hname]->SetStats(false);
		cnv[cnvname]->cd(3);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dCLCT_BXN", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Distribution of TMB BXN counter", 1024, 0, 1024);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("TMB BXN");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(4);
		h[hname]->Draw();
		gStyle->SetOptStat("eo");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dCLCT_DMB_BXN_diff", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "Difference between TMB and DMB BXN numbers", 128, -64, 64);
		h[hname]->SetXTitle("DMB BXN - TMB BXN");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(5);
		gPad->SetLogy();
		h[hname]->SetAxisRange(0.1, 1.1, "Y");
		h[hname]->Draw();
		gStyle->SetOptStat("emuo");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dCLCT_BXN_vs_DMB_BXN", ChamberID);
		h[hname] = new TH2F(TString(hname.c_str()), "DMB BXN vs TMB BXN Numbers", 1024,  0 , 1024, 128,  0 , 128);
		h[hname]->SetXTitle("TMB BXN");
		h[hname]->SetYTitle("DMB BXN");
		h[hname]->SetOption("box");
		h[hname]->SetStats(false);
		cnv[cnvname]->cd(6);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "SYNC: TMB - ALCT Syncronization.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),2,2);
		hname = Form("hist/h%dTMB_L1A_vs_ALCT_L1A", ChamberID);
		h[hname] = new TH2F(TString(hname.c_str()), "TMB L1A vs ALCT L1A", 16,  0 , 16, 16,  0 , 16);
		h[hname]->SetXTitle("ALCT L1A");
		h[hname]->SetYTitle("TMB L1A");
		h[hname]->SetOption("box");
		h[hname]->SetStats(false);
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dTMB_ALCT_L1A_diff", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "TMB L1A - ALCT L1A", 16,  -8 , 8);
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetXTitle("TMB_L1A - ALCT_L1A");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(2);
		gPad->SetLogy();
		h[hname]->SetAxisRange(0.1, 1.1, "Y");
		h[hname]->Draw();
		gStyle->SetOptStat("emou");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dTMB_BXN_vs_ALCT_BXN", ChamberID);
		h[hname] = new TH2F(TString(hname.c_str()), "TMB BXN vs ALCT BXN", 1024,  0 , 1024, 1024,  0 , 1024);
		h[hname]->SetXTitle("ALCT BXN");
		h[hname]->SetYTitle("TMB BXN");
		h[hname]->SetOption("box");
		h[hname]->SetStats(false);
		cnv[cnvname]->cd(3);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dTMB_ALCT_BXN_diff", ChamberID);
		h[hname] = new TH1F(TString(hname.c_str()), "TMB BXN - ALCT BXN", 1024,  -512 , 512);
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetXTitle("TMB_BXN - ALCT_BXN");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(4);
		gPad->SetLogy();
		h[hname]->SetAxisRange(0.1, 1.1, "Y");
		h[hname]->Draw();
		gStyle->SetOptStat("emou");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "SYNC: CFEB - DMB Synchronization.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),5,3);
	for (int nCFEB=0; nCFEB<=4; nCFEB++) {
		hname = Form("hist/h%dCFEB%d_L1A_Sync_Time", ChamberID, nCFEB);
		h[hname] = new TH1F(TString(hname.c_str()), Form("CFEB%d L1A Sync BXN", nCFEB), 16, 0, 16);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle(Form("CFEB%d L1A Sync BXN", nCFEB));
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(nCFEB+1);
		h[hname]->Draw();
		gStyle->SetOptStat("eo");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dCFEB%d_L1A_Sync_Time_vs_DMB", ChamberID, nCFEB);
		h[hname] = new TH2F(TString(hname.c_str()), Form("CFEB%d L1A Sync Time vs DMB CFEB Sync Time", nCFEB), 16, 0, 16, 16, 0, 16);
		h[hname]->SetXTitle("DMB L1A_Sync_BXN");
		h[hname]->SetYTitle(Form("CFEB%d L1A Sync BXN", nCFEB));
		h[hname]->SetOption("col");
		cnv[cnvname]->cd(nCFEB+6);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);

		hname = Form("hist/h%dCFEB%d_L1A_Sync_Time_DMB_diff", ChamberID, nCFEB);
		h[hname] = new TH1F(TString(hname.c_str()), Form("CFEB%d L1A Sync Time - DMB CFEB Sync Time", nCFEB), 16, -8.0, 8.0);
		h[hname]->SetXTitle("CFEB - DMB L1A_Sync_BXN");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("hist");
		cnv[cnvname]->cd(nCFEB+11);
		gPad->SetLogy();
		h[hname]->SetAxisRange(0.1, 1.1, "Y");
		h[hname]->Draw();
		gStyle->SetOptStat("emuo");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	}
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

	cnvtitle = "SYNC: CFEB - DMB Phase Synchronization.";
	cnvname  = path_to_folder + IDTextShort + cnvtitle;
	cnvtitle = cnvtitle + IDTextLong;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),5,3);
	for (int nCFEB=0; nCFEB<=4; nCFEB++) {
		hname = Form("hist/h%dCFEB%d_LCT_PHASE_vs_L1A_PHASE", ChamberID, nCFEB);
		h[hname] = new TH2F(TString(hname.c_str()), Form("LCT Phase vs L1A Phase. CFEB%d", nCFEB), 2, 0, 2, 2, 0, 2);
		h[hname]->SetXTitle("LCT Phase");
		for(int i = 1; i<=2; i++) h[hname]->GetXaxis()->SetBinLabel(i,Form("%d",i-1));
		h[hname]->SetYTitle("L1A Phase");
		for(int i = 1; i<=2; i++) h[hname]->GetYaxis()->SetBinLabel(i,Form("%d",i-1));
		h[hname]->SetOption("textcol");
		cnv[cnvname]->cd(nCFEB+1,1,1);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(dir.c_str()), 0, h[hname]);
	}
	consinfo->addObject(TString(cnvname.c_str()),TString(dir.c_str()),0,cnv[cnvname]);

 	fListModified = true;
	canvases[id] = cnv;
	save(HistoFile.c_str());
	return h;
}
