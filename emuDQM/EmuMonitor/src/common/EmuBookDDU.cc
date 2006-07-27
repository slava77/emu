#include "EmuLocalPlotter.h"

//	Booking of common histograms
map<string, TH1*> EmuLocalPlotter::book_ddu(int dduID) {

	string cnvname, cnvtitle;
	map<string, ConsumerCanvas*> cnv = canvases[0];

	string hname, htitle;
	map<string, TPad*> pad;
	map<string, TH1*> h = histos[0];


	string ID = Form("%04d", dduID&0xFFF);
	string dir = Form("DDU_%s/", ID.c_str());
	string path_to_folder = dir;

	string  DDUHardwareErrorName[33];
		DDUHardwareErrorName[0] = "Blank: 00";
		DDUHardwareErrorName[1] = "CFEB CRC Error (test ddu): 01";
		DDUHardwareErrorName[2] = "DMB-DDU L1A Mismatch: 02";
		DDUHardwareErrorName[3] = "Lost/New Fibers: 03";
		DDUHardwareErrorName[4] = "INPUT FIFO Full: 04";
		DDUHardwareErrorName[5] = "Two Single Bit-Vote Errors: 05";
		DDUHardwareErrorName[6] = "Multiple Bit-Vote Error: 06";
		DDUHardwareErrorName[7] = "Timeout Error: 07";
		DDUHardwareErrorName[8] = "Extra Control Words: 08";
		DDUHardwareErrorName[9] = "Missing Control Words: 09";
		DDUHardwareErrorName[10] = "CFEB Lost Samples: 10";
		DDUHardwareErrorName[11] = "Control FPGA Clock-DLL Error: 11";
		DDUHardwareErrorName[12] = "Hardware Bit-Vote Errors: 12";
		DDUHardwareErrorName[13] = "INPUT FIFO Near Full: 13";
		DDUHardwareErrorName[14] = "DDU Single Event Warning: 14";
		DDUHardwareErrorName[15] = "DDU Single Event Error: 15";
		DDUHardwareErrorName[16] = "DDU Critical Error: 16";
		DDUHardwareErrorName[17] = "TMB/ALCT-DDU L1A Mismatch: 17";
		DDUHardwareErrorName[18] = "TMB/ALCT Word Count Error: 18";
		DDUHardwareErrorName[19] = "ALCT Error: 19";
		DDUHardwareErrorName[20] = "TMB Error: 20";
		DDUHardwareErrorName[21] = "S-Link Not Ready: 21";
		DDUHardwareErrorName[22] = "S-Link Full Bit Present: 22";
		DDUHardwareErrorName[23] = "Local DAQ FPGA Clock-DLL Error: 23";
		DDUHardwareErrorName[24] = "One Single Bit-Vote Error: 24";
		DDUHardwareErrorName[25] = "No Live Fibers: 25";
		DDUHardwareErrorName[26] = "Data Stuck in FIFO: 26";
		DDUHardwareErrorName[27] = "L1A-FIFO Full: 27";
		DDUHardwareErrorName[28] = "Wrong First Word: 28";
		DDUHardwareErrorName[29] = "Local DAQ Fiber Error: 29";
		DDUHardwareErrorName[30] = "Local DAQ FIFO Near Full: 30";
		DDUHardwareErrorName[31] = "Local DAQ FIFO Full: 31";
		DDUHardwareErrorName[32] = "DDU Output Constricted: 32";
//DDU
	if(debug_printout) LOG4CPLUS_INFO(logger_, 
		// "D**EmuBookCommon> <<
		"Booking new DDU Canvases");
//	if(folders) 	path_to_folder = "EMU/";

	cnvtitle = "DDU: Data Format Errors and Warnings in Tables";
	cnvname = path_to_folder + cnvtitle;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),3,1);
		hname = Form("hist/hDDU_%s_Readout_Errors", ID.c_str());
                h[hname] = new TH2F(TString(hname.c_str()), "DDU RUI Readout Errors", 1, 0, 1, 16 ,0 ,16);
                h[hname]->GetXaxis()->SetBinLabel(1, "Number of Events");
		h[hname]->GetYaxis()->SetBinLabel(16,"DDU Trailer Missing (2 headers in a row)");
		h[hname]->GetYaxis()->SetBinLabel(15,"DDU Trailer Missing (overflow)");
		h[hname]->GetYaxis()->SetBinLabel(14,"DDU Header Missing");
		h[hname]->GetYaxis()->SetBinLabel(13,"DDU Trailer bit error flag from hardware");
		h[hname]->GetYaxis()->SetBinLabel(12,"Unidentified buffer");
                h[hname]->SetOption("textcolz");
		cnv[cnvname]->cd(1,1,1);
		gPad->SetLeftMargin(0.6);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = Form("hist/hDDU_%s_BinCheck_Errors", ID.c_str());
		h[hname] = new TH2F(TString(hname.c_str()), "DDU Data Format Errors", 1, 0, 1, bin_checker.nERRORS, 0, bin_checker.nERRORS);
        	h[hname]->GetXaxis()->SetBinLabel(1, "Number of events");
        	for (int i=0; i < bin_checker.nERRORS; i++)	h[hname]->GetYaxis()->SetBinLabel(i+1, bin_checker.errorName(i));
		h[hname]->SetOption("textcolz");
		cnv[cnvname]->cd(2,1,1);
		gPad->SetLeftMargin(0.6);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = Form("hist/hDDU_%s_BinCheck_Warnings", ID.c_str());
                h[hname] = new TH2F(TString(hname.c_str()), "DDU Data Format Warnings", 1, 0, 1, bin_checker.nWARNINGS, 0, bin_checker.nWARNINGS);
                h[hname]->GetXaxis()->SetBinLabel(1, "Number of Events");
                for (int i=0; i < bin_checker.nWARNINGS; i++)	h[hname]->GetYaxis()->SetBinLabel(i+1, bin_checker.warningName(i));
                h[hname]->SetOption("textcolz");
		cnv[cnvname]->cd(3,1,1);
		gPad->SetLeftMargin(0.6);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
                consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);
	if(debug_printout) LOG4CPLUS_DEBUG(logger_,
		// "D**EmuBookCommon> ">> 
		"Canvas " << cnvname << " is created");
/*
	cnvtitle = "DDU: Unpacked DMBs";
	cnvname = path_to_folder + cnvtitle;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),1,1);
		hname = "hist/hCSC_Unpacked";
		h[hname] = new TH2F(TString(hname.c_str()), "", 300 ,0 , 300, 16, 0, 16);
		h[hname]->SetXTitle("Crate ID");
		h[hname]->SetYTitle("DMB ID");
		h[hname]->SetOption("textcolz");
		cnv[cnvname]->cd(1,1,1);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);
	if(debug_printout) LOG4CPLUS_DEBUG(logger_,
		// "D**EmuBookCommon> " << 
		"Canvas " << cnvname << " is created");
*/
	cnvtitle = "DDU: L1A and BXN Counters";
	cnvname = path_to_folder + cnvtitle;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),1,2);
		hname = Form("hist/hDDU_%s_BXN", ID.c_str());
		h[hname] = new TH1F(TString(hname.c_str()), "", 4096 ,  0 , 4096);
		h[hname]->SetXTitle("BXN Counter Number");
		h[hname]->SetYTitle("Number of Events");
		h[hname]->SetFillColor(THistoFillColor);
		h[hname]->SetOption("bar1");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = Form("hist/hDDU_%s_L1A_Increment", ID.c_str());
		h[hname] = new TH1F(TString(hname.c_str()), "Incremental change in DDU L1A number since previous event", 100 ,  0 , 100);
		h[hname]->SetXTitle("L1A counter increment");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(THistoFillColor);
		h[hname]->SetOption("bar1");
		cnv[cnvname]->cd(2);
		h[hname]->Draw();
		gStyle->SetOptStat("em");
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);
	if(debug_printout) LOG4CPLUS_DEBUG(logger_,"D**EmuBookCommon> Canvas " << cnvname << " is created");

	cnvtitle = "DDU: Connected and Active Inputs"; //KK +Connected and
	cnvname = path_to_folder + cnvtitle;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),1,2);
//KK
		hname = Form("hist/hDDU_%s_DMB_Connected_Inputs_Rate", ID.c_str());
		h[hname] = new TH1F(TString(hname.c_str()), "hDDU_DMB_Connected_Inputs_Rate", 16 ,  0 , 16);
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = Form("hist/hDDU_%sDMB_Connected_Inputs", ID.c_str());
		h[hname] = new TH1F(TString(hname.c_str()), "DDU Inputs connected to DMBs", 16 ,  0 , 16);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("DDU Inputs");
		for(int i = 1; i<=16; i++) h[hname]->GetXaxis()->SetBinLabel(i,Form("%d",i));
		h[hname]->SetYTitle("Efficiency (% of total number of events)");
		h[hname]->SetFillColor(THistoFillColor);
		h[hname]->SetOption("bar1text");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
//KK end
		hname = Form("hist/hDDU_%s_DMB_DAV_Header_Occupancy_Rate", ID.c_str());
		h[hname] = new TH1F(TString(hname.c_str()), "DMBs reporting DAV (data available) in Header", 16 ,  0 , 16);
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = Form("hist/hDDU_%s_DMB_DAV_Header_Occupancy", ID.c_str());
		h[hname] = new TH1F(TString(hname.c_str()), "DMBs reporting DAV (data available) in Header", 16 ,  0 , 16);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetXTitle("DMB position in DDU");
		for(int i = 1; i<=16; i++) h[hname]->GetXaxis()->SetBinLabel(i,Form("%d",i));
		h[hname]->SetYTitle("Efficiency (% of total number of events)");
		h[hname]->SetFillColor(THistoFillColor);
		h[hname]->SetOption("bar1text");
		cnv[cnvname]->cd(2); //KK 1->2
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);
	if(debug_printout) LOG4CPLUS_DEBUG(logger_,
		// "D**EmuBookCommon> " << 
		"Canvas " << cnvname << " is created");

	cnvtitle = "DDU: DMBs DAV and Unpacked vs DMBs Active";
	cnvname = path_to_folder + cnvtitle;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),1,3);
		hname = Form("hist/hDDU_%s_DMB_Active_Header_Count", ID.c_str());
		h[hname] = new TH1F(TString(hname.c_str()), "Number of active DMBs reporting DAV in Header", 16 ,  0 , 16);
		h[hname]->SetXTitle("Counter of active DMBs from Header");
		for(int i = 1; i<=16; i++) h[hname]->GetXaxis()->SetBinLabel(i,Form("%d",i-1));
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(THistoFillColor);
		h[hname]->SetOption("bar1text");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		gStyle->SetOptStat("em");
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = Form("hist/hDDU_%s_DMB_DAV_Header_Count_vs_DMB_Active_Header_Count", ID.c_str());
		h[hname] = new TH2F(TString(hname.c_str()), "DMB DAV Header Count vs DMB Active Header Count", 16, 0, 16, 16, 0, 16);
		h[hname]->SetXTitle("Counter of active DMBs from Header");
		for(int i = 1; i<=16; i++) h[hname]->GetXaxis()->SetBinLabel(i,Form("%d",i-1));
		h[hname]->SetYTitle("Counter of DAV DMBs from Header");
		for(int i = 1; i<=16; i++) h[hname]->GetXaxis()->SetBinLabel(i,Form("%d",i-1));
		h[hname]->SetOption("textcolz");
		cnv[cnvname]->cd(2,1,1);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = Form("hist/hDDU_%s_DMB_unpacked_vs_DAV", ID.c_str());
		h[hname] = new TH2F(TString(hname.c_str()), "Number of unpacked DMBs vs. number of DMBs reporting DAV", 16 ,  0 , 16, 16, 0, 16);
		h[hname]->SetXTitle("Counter of active DMBs from Header");
		for(int i = 1; i<=16; i++) h[hname]->GetXaxis()->SetBinLabel(i,Form("%d",i-1));
		h[hname]->SetYTitle("Counter of unpacked DMBs");
		for(int i = 1; i<=16; i++) h[hname]->GetXaxis()->SetBinLabel(i,Form("%d",i-1));
		h[hname]->SetOption("textcolz");
		cnv[cnvname]->cd(3,1,1);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);
	if(debug_printout) LOG4CPLUS_DEBUG(logger_,
		// "D**EmuBookCommon> " << 
		"Canvas " << cnvname << " is created");

	cnvtitle = "DDU: Data Integrity Checks vs Event Number";
	cnvname = path_to_folder + cnvtitle;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),1,3);
		hname = Form("hist/hDDU_%s_Data_Format_Check_vs_nEvents", ID.c_str());
		h[hname] = new TH2F(TString(hname.c_str()), "Check DDU Data Format", 100000, 0, 100000, 3,  0 , 3);
		h[hname]->GetYaxis()->SetBinLabel(1,"Format OK");
		h[hname]->GetYaxis()->SetBinLabel(2,"Format Warning");
		h[hname]->GetYaxis()->SetBinLabel(3,"Format Error");
		h[hname]->SetXTitle("Sequential event number as received by DQM");
		h[hname]->SetLabelSize(0.1,"Y");
		// h[hname]->SetTitleSize(0.05,"X");
		h[hname]->SetOption("col");
		h[hname]->SetStats(false);
		cnv[cnvname]->cd(1,0,1);
		gPad->SetLeftMargin(0.15);
		h[hname]->Draw();
		TPaveText *pt6 = new TPaveText(0.7,0.7,0.85,0.8,"brNDC");
		pt6->SetTextAlign(12);
		pt6->AddText("=PROBLEMS");
		pt6->Draw();
		TPaveText *pt7 = new TPaveText(0.7,0.44,0.85,0.54,"brNDC");
		pt7->SetTextAlign(12);
		pt7->AddText("~OK");
		pt7->Draw();
		TPaveText *pt8 = new TPaveText(0.7,0.18,0.85,0.28,"brNDC");
		pt8->SetTextAlign(12);
		pt8->AddText("=OK");
		pt8->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = Form("hist/hDDU_%s_Unpacking_Match_vs_nEvents", ID.c_str());
		h[hname] = new TH2F(TString(hname.c_str()), "Match: Unpacked DMBs and DMBs Reporting DAV", 100000, 0, 100000, 2, 0, 2);
		h[hname]->GetYaxis()->SetBinLabel(1,"OK");
		h[hname]->GetYaxis()->SetBinLabel(2,"Mismatch");
		h[hname]->SetXTitle("Sequential event number as received by DQM");
	        h[hname]->SetLabelSize(0.1,"Y");
		// h[hname]->SetTitleSize(0.05,"X");
		h[hname]->SetOption("col");
		h[hname]->SetStats(false);
		cnv[cnvname]->cd(2,0,1);
		gPad->SetLeftMargin(0.15);
		h[hname]->Draw();
		TPaveText *pt1 = new TPaveText(0.7,0.7,0.85,0.8,"brNDC");
		pt1->SetTextAlign(12);
		pt1->AddText("=PROBLEMS");
		pt1->Draw();
		TPaveText *pt2 = new TPaveText(0.7,0.3,0.85,0.4,"brNDC");
		pt2->SetTextAlign(12);
		pt2->AddText("=OK");
		pt2->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = Form("hist/hDDU_%s_L1A_Increment_vs_nEvents", ID.c_str());
		h[hname] = new TH2F(TString(hname.c_str()), "Incremental L1A", 100000, 0, 100000, 3, 0, 3);
		h[hname]->GetYaxis()->SetBinLabel(1,"Increment=0");
		h[hname]->GetYaxis()->SetBinLabel(2,"Increment=1");
		h[hname]->GetYaxis()->SetBinLabel(3,"Increment>1");
		h[hname]->LabelsOption("u","Y");
		h[hname]->SetXTitle("Sequential event number as received by DQM");
		h[hname]->SetLabelSize(0.1,"Y");
		// h[hname]->SetTitleSize(0.05,"X");
		h[hname]->SetOption("col");
		h[hname]->SetStats(false);
		cnv[cnvname]->cd(3,0,1);
		gPad->SetLeftMargin(0.15);
		h[hname]->Draw();
		TPaveText *pt3 = new TPaveText(0.7,0.7,0.85,0.8,"brNDC");
		pt3->SetTextAlign(12);
		pt3->AddText("~OK");
		pt3->Draw();
		TPaveText *pt4 = new TPaveText(0.7,0.44,0.85,0.54,"brNDC");
		pt4->SetTextAlign(12);
		pt4->AddText("=OK");
		pt4->Draw();
		TPaveText *pt5 = new TPaveText(0.7,0.18,0.85,0.28,"brNDC");
		pt5->SetTextAlign(12);
		pt5->AddText("=PROBLEMS");
		pt5->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);
	if(debug_printout) LOG4CPLUS_DEBUG(logger_,
		// "D**EmuBookCommon> " <<
		"Canvas " << cnvname << " is created");

	cnvtitle = "DDU: Error Status from DDU Trailer vs Event Number";
	cnvname = path_to_folder + cnvtitle;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),1,1);
		hname = Form("hist/hDDU_%s_Trailer_ErrorStat_vs_nEvents", ID.c_str());
		h[hname] = new TH2F(TString(hname.c_str()), "", 100000,  0 , 100000, 32, 0, 32);
		h[hname]->SetXTitle("Sequential event number as received by DQM");
		for(int i=1; i<=32; i++) h[hname]->GetYaxis()->SetBinLabel(i, DDUHardwareErrorName[i].c_str());
		h[hname]->SetOption("col");
		cnv[cnvname]->cd(1,1,1);
		gPad->SetLeftMargin(0.35);
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);
	if(debug_printout) LOG4CPLUS_DEBUG(logger_,
		// "D**EmuBookCommon> " << 
		"Canvas " << cnvname << " is created");

	cnvtitle = "DDU: Error Status from DDU Trailer";
	cnvname = path_to_folder + cnvtitle;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),2,1);
		hname = Form("hist/hDDU_%sTrailer_ErrorStat_Table", ID.c_str());
		h[hname] = new TH2F(TString(hname.c_str()), "DDU Trailer Status Error Table", 1,  0 , 1, 32, 0, 32);
		h[hname]->GetXaxis()->SetBinLabel(1,"Number of events");
		for(int i=1; i<=32; i++) h[hname]->GetYaxis()->SetBinLabel(i, DDUHardwareErrorName[i].c_str());
		h[hname]->SetOption("textcolz");
		pad[hname] = new TPad("pad1", "pad1",0.005,0,0.625,0.945);
		pad[hname]->SetLeftMargin(0.5);
		pad[hname]->SetRightMargin(0.0);
		pad[hname]->SetFillColor(0);
		pad[hname]->SetGridy(1);
		cnv[cnvname]->cd();
		pad[hname]->Draw();
		pad[hname]->cd();
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = Form("hist/hDDU_%s_Trailer_ErrorStat_Rate", ID.c_str());
		h[hname] = new TH1F(TString(hname.c_str()), "", 32,  0 , 32);
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = Form("hist/hDDU_%s_Trailer_ErrorStat_Occupancy", ID.c_str());
		h[hname] = new TH1F(TString(hname.c_str()), "DDU Trailer Status Error Frecuency", 32,  0 , 32);
		h[hname]->SetMinimum(0.0);
		h[hname]->SetYTitle("Frequency (%)");
		h[hname]->SetFillColor(THistoFillColor);
		h[hname]->SetOption("texthbar1");
		pad[hname] = new TPad("pad2", "pad2",0.635,0,0.995,0.945);
		pad[hname]->SetLeftMargin(0.0);
		pad[hname]->SetRightMargin(0.05);
		pad[hname]->SetFillColor(0);
		cnv[cnvname]->cd();
		pad[hname]->Draw();
		pad[hname]->cd();
		h[hname]->Draw();
		gStyle->SetOptStat("e");
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);
	if(debug_printout) LOG4CPLUS_DEBUG(logger_,
		// "D**EmuBookCommon> " << 
		"Canvas " << cnvname << " is created");

	cnvtitle = "DDU: Event Buffer Size and DDU Word Count";
	cnvname = path_to_folder + cnvtitle;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),1,2);
		hname = Form("hist/hDDU_%s_Buffer_Size", ID.c_str());
                h[hname] = new TH1F(TString(hname.c_str()), "DDU Buffer Size", 128 ,0 ,65536 ); // 65536 = 2^16
                h[hname]->SetXTitle("Buffer Size in bytes");
                h[hname]->SetYTitle("Number of Events");
		h[hname]->SetFillColor(THistoFillColor);
                h[hname]->SetOption("bar1");
                cnv[cnvname]->cd(1);
                h[hname]->Draw();
		gStyle->SetOptStat("emro");
                consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = Form("hist/hDDU_%s_Word_Count", ID.c_str());
		h[hname] = new TH1F(TString(hname.c_str()), "DDU Word (64 bits) Count", 128,  0 , 8192); //8192 = 2^13
		h[hname]->SetXTitle("Number of Words in DDU (from DDU Trailer)");
		h[hname]->SetYTitle("Number of Events");
		h[hname]->SetFillColor(THistoFillColor);
		h[hname]->SetOption("bar1");
		cnv[cnvname]->cd(2);
		h[hname]->Draw();
		gStyle->SetOptStat("eomr");
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);
	if(debug_printout) LOG4CPLUS_DEBUG(logger_,
		// "D**EmuBookCommon> " <<
		"Canvas " << cnvname << " is created");
//KK
	cnvtitle = "DDU: State of CSCs";
	cnvname = path_to_folder + cnvtitle;
	cnv[cnvname] = new ConsumerCanvas(cnvname.c_str(),cnvname.c_str(),cnvtitle.c_str(),1,2);
                hname = Form("hist/hDDU_%s_CSC_Errors_Rate", ID.c_str());
                h[hname] = new TH1F(TString(hname.c_str()), "", 15 ,  1 , 16);
                consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = Form("hist/hDDU_%s_CSC_Errors", ID.c_str());
                h[hname] = new TH1F(TString(hname.c_str()), "Errors", 15 ,1 ,16);
                h[hname]->SetXTitle("CSC");
		for(int i = 1; i<=15; i++) h[hname]->GetXaxis()->SetBinLabel(i,Form("%d",i));
		h[hname]->SetYTitle("Efficiency (% of total number of events)");
		h[hname]->SetFillColor(THistoFillColor);
                h[hname]->SetOption("bar1");
                cnv[cnvname]->cd(1);
                h[hname]->Draw();
		gStyle->SetOptStat("emro");
                consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

                hname = Form("hist/hDDU_%s_CSC_Warnings_Rate", ID.c_str());
                h[hname] = new TH1F(TString(hname.c_str()), "", 15 ,  1 , 16);
                consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = Form("hist/hDDU_%s_CSC_Warnings", ID.c_str());
		h[hname] = new TH1F(TString(hname.c_str()), "Warnings", 15,  1 , 16);
		h[hname]->SetXTitle("CSC");
		for(int i = 1; i<=15; i++) h[hname]->GetXaxis()->SetBinLabel(i,Form("%d",i));
		h[hname]->SetYTitle("Efficiency (% of total number of events)");
		h[hname]->SetFillColor(THistoFillColor);
		h[hname]->SetOption("bar1");
		cnv[cnvname]->cd(2);
		h[hname]->Draw();
		gStyle->SetOptStat("eomr");
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);
	if(debug_printout) LOG4CPLUS_DEBUG(logger_,
		// "D**EmuBookCommon> " << 
		"Canvas " << cnvname << " is created");
//KK end

	canvases[0] = cnv;
	histos[0] = h;
	fListModified = true;
	// save(HistoFile.c_str());
	return h;
}
