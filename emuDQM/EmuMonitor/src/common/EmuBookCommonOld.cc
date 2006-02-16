#include "EmuLocalPlotter.h"

//	Booking of common histograms
map<string, TH1*> EmuLocalPlotter::book_common() {
	map<string, TH1*> h;
	map<string, ConsumerCanvas*> cnv;
//	map<string, TH1*> hslides = histos[SLIDES_ID];
//	map<string, ConsumerCanvas*> cnvslides = canvases[SLIDES_ID];
	stringstream stname;
	string hname, cnvname;
	TPad * pad;

// TODO: Get Histo list from configuration file

        bool fERROR[19], fWARNING[2];
	string sERROR[19], sWARNING[2];
// == Define strings describing ERRORs
	sERROR[0] = "DDU Trailer Missing";
	sERROR[1] = "DDU Header Missing";
	sERROR[3] = "DDU Word Count Error";
	sERROR[2] = "DDU CRC Error (not yet implemented)";

	sERROR[4] = "DMB Trailer Missing";
	sERROR[5] = "DMB Header Missing";

	sERROR[6] = "ALCT Trailer Missing";
	sERROR[7] = "ALCT Header Missing";
	sERROR[8] = "ALCT Word Count Error";
	sERROR[9] = "ALCT CRC Error";

	sERROR[10] = "TMB Trailer Missing";
	sERROR[11] = "TMB Header Missing";
	sERROR[12] = "TMB Word Count Error";
	sERROR[13] = "TMB CRC Error (not yet implemented)";

	sERROR[14] = "CFEB Word Count Per Sample Error";
	sERROR[15] = "CFEB Sample Count Error";
	sERROR[16] = "CFEB CRC Error";

	sERROR[17] = "ALCT Trailer Bit Error";
        sERROR[18] = "Buffer 8-bytes Boundary Error";

	 	// == Define strings defining WARNINGs
	sWARNING[0] = "Extra words DDU Trailer-Header";
	sWARNING[1] = "CFEB B-Words";


//TF ( Track Finder )
	cnvname = "Track Finder Canvas 1";
	cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),2,2);

	stname.clear();
	//if(folders) stname << "TrackFinder/" << "hch4BX";
	/*else*/        stname << "TF_hch4BX"; stname >> hname;
	h[hname] = new TH1F(TString(hname.c_str()), "Chamber 4 BX Distribution", 7, 0, 7);
	h[hname]->SetXTitle("BX");
	h[hname]->SetYTitle("Number of events");
	h[hname]->SetFillColor(15); //48
	//h[hname]->SetOption("bar1");
	h[hname]->SetStats(false);
	cnv[cnvname]->cd(1);
	h[hname]->Draw();
	consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

	stname.clear();
	//if(folders) stname << "TrackFinder/" << "hch5BX";
	/*else*/        stname << "TF_hch5BX"; stname >> hname;
	h[hname] = new TH1F(TString(hname.c_str()), "Chamber 5 BX Distribution", 7, 0, 7);
	h[hname]->SetXTitle("BX");
	h[hname]->SetYTitle("Number of events");
	h[hname]->SetFillColor(15); //48
	//h[hname]->SetOption("bar1");
	h[hname]->SetStats(false);
	cnv[cnvname]->cd(2);
	h[hname]->Draw();
	consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

	stname.clear();
	//if(folders) stname << "TrackFinder/" << "hch8BX";
	/*else*/        stname << "TF_hch8BX"; stname >> hname;
	h[hname] = new TH1F(TString(hname.c_str()), "Chamber 8 BX Distribution", 7, 0, 7);
	h[hname]->SetXTitle("BX");
	h[hname]->SetYTitle("Number of events");
	h[hname]->SetFillColor(15); //48
	//h[hname]->SetOption("bar1");
	h[hname]->SetStats(false);
	cnv[cnvname]->cd(3);
	h[hname]->Draw();
	consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

	stname.clear();
	//if(folders) stname << "TrackFinder/" << "hch9BX";
	/*else*/        stname << "TF_hch9BX"; stname >> hname;
	h[hname] = new TH1F(TString(hname.c_str()), "Chamber 9 BX Distribution", 7, 0, 7);
	h[hname]->SetXTitle("BX");
	h[hname]->SetYTitle("Number of events");
	h[hname]->SetFillColor(15); //48
	//h[hname]->SetOption("bar1");
	h[hname]->SetStats(false);
	cnv[cnvname]->cd(4);
	h[hname]->Draw();
	consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);

	cnvname = "Track Finder Canvas 2";
	cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),2,2);

	stname.clear();
	stname << "TF_hLCTs"; stname >> hname;
	h[hname] = new TH1F(TString(hname.c_str()), "LCTs in Chamber", 16, 0, 16);
	h[hname]->SetXTitle("CSC");
	h[hname]->SetYTitle("Efficiency");
	h[hname]->SetFillColor(15); //48 ??
	//h[hname]->SetOption("bar1");
	h[hname]->SetStats(false);
	cnv[cnvname]->cd(1);
	h[hname]->Draw();
	consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

	stname.clear();
	stname << "TF_hCLCTs"; stname >> hname;
	h[hname] = new TH1F(TString(hname.c_str()), "Only CLCTs in Chamber", 16, 0, 16);
	h[hname]->SetXTitle("CSC");
	h[hname]->SetYTitle("Efficiency");
	h[hname]->SetFillColor(15); //48 ??
	//h[hname]->SetOption("bar1");
	h[hname]->SetStats(false);
	cnv[cnvname]->cd(2);
	h[hname]->Draw();
	consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

	stname.clear();
	stname << "TF_hALCTs"; stname >> hname;
	h[hname] = new TH1F(TString(hname.c_str()), "Only ALCTs in Chamber", 16, 0, 16);
	h[hname]->SetXTitle("CSC");
	h[hname]->SetYTitle("Efficiency");
	h[hname]->SetFillColor(15); //48 ??
	//h[hname]->SetOption("bar1");
	h[hname]->SetStats(false);
	cnv[cnvname]->cd(3);
	h[hname]->Draw();
	consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

	stname.clear();
	stname << "TF_hCORLCTs"; stname >> hname;
	h[hname] = new TH1F(TString(hname.c_str()), "Only Correlated LCTs in Chamber", 16, 0, 16);
	h[hname]->SetXTitle("???");
	h[hname]->SetYTitle("Efficiency");
	h[hname]->SetFillColor(15); //48 ??
	//h[hname]->SetOption("bar1");
	h[hname]->SetStats(false);
	cnv[cnvname]->cd(4);
	h[hname]->Draw();
	consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);


	cnvname = "Track Finder Canvas 3";
	cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),2,2);

	stname.clear();
	stname << "TF_hEff"; stname >> hname;
	h[hname] = new TH1F(TString(hname.c_str()), "LCT Eff vs. Chamber", 16, 0, 16);
	h[hname]->SetXTitle("CSC");
	h[hname]->SetYTitle("Efficiency");
	h[hname]->SetFillColor(15); //48 ??
	//h[hname]->SetOption("bar1");
	h[hname]->SetStats(false);
	cnv[cnvname]->cd(1);
	h[hname]->Draw();
	consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

	stname.clear();
	stname << "hL1As"; stname >> hname;
	h[hname] = new TH1F(TString(hname.c_str()), "L1As", 16, 0, 16);
	h[hname]->SetXTitle("CSC");
	h[hname]->SetYTitle("Efficiency");
	h[hname]->SetFillColor(15); //48 ??
	//h[hname]->SetOption("bar1");
	h[hname]->SetStats(false);
	cnv[cnvname]->cd(2);
	h[hname]->Draw();
	consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

	stname << "hL1AvsBXN"; stname >> hname;
	h[hname] = new TH2F(TString(hname.c_str()), "L1A vs. BX", 16, 0, 16, 16, 0, 16);
	h[hname]->SetXTitle("CSC");
	h[hname]->SetYTitle("Efficiency");
	h[hname]->SetFillColor(15); //48 ??
	//h[hname]->SetOption("bar1");
	h[hname]->SetStats(false);
	cnv[cnvname]->cd(3);
	h[hname]->Draw();
	consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
//TF end



        cnvname = "DDU: Readout Errors and Warnings in Tables";
        cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),1,2);
		hname = "hBinCheck_Errors";
		h[hname] = new TH2F(TString(hname.c_str()), "DDU Data Format Errors", 1, 0, 1, bin_checker.nERRORS, 0, bin_checker.nERRORS);
        h[hname]->GetXaxis()->SetBinLabel(1, "Entire event");
        for (int i=0; i < sizeof(fERROR); i++)
        h[hname]->GetYaxis()->SetBinLabel(i+1, sERROR[i].c_str());
		h[hname]->SetOption("coltext");
		gStyle->SetOptStat("e");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = "hBinCheck_Warnings";
                h[hname] = new TH2F(TString(hname.c_str()), "DDU Data Format Warnings", 1, 0, 1, bin_checker.nWARNINGS, 0, bin_checker.nWARNINGS);
                h[hname]->GetXaxis()->SetBinLabel(1, "Entire Event");
                for (int i=0; i < sizeof(fWARNING); i++)
		    h[hname]->GetYaxis()->SetBinLabel(i+1, sWARNING[i].c_str());
                h[hname]->SetOption("coltext");
		gStyle->SetOptStat("e");
		cnv[cnvname]->cd(2);
		h[hname]->Draw();
                consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);

		hname = "hDDU_Readout_Errors";
                h[hname] = new TH2F(TString(hname.c_str()), "DDU Readout Errors", 1, 0, 1, 16 ,0 ,16);
                h[hname]->GetXaxis()->SetBinLabel(1, "Number of Events");
		h[hname]->GetYaxis()->SetBinLabel(16,"DDU Trailer Missing (2 headers in a row)");
		h[hname]->GetYaxis()->SetBinLabel(15,"DDU Trailer Missing (overflow)");
		h[hname]->GetYaxis()->SetBinLabel(14,"DDU Trailer bit error flag from hardware");
		h[hname]->GetYaxis()->SetBinLabel(13,"Word Count mismatch");
                h[hname]->SetOption("coltext");
                gStyle->SetOptStat("e");


/*         cnvname = "BinChecker: Warnings";
        cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),1,1);
                hname = "hBinCheck_Warnings";
                h[hname] = new TH2F(TString(hname.c_str()), "", 1,  0 , 1, sizeof(fWARNING), 0, sizeof(fWARNING));
                h[hname]->GetXaxis()->SetBinLabel(1, "Number of Events");
                for (int i=0; i < sizeof(fWARNING); i++)
		    h[hname]->GetYaxis()->SetBinLabel(i+1, sWARNING[i].c_str());
                h[hname]->SetOption("textcol");
		h[hname]->SetStats(false);
		cnv[cnvname]->cd(1,1,1);
		h[hname]->Draw();
                consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
                consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]); */

/*         cnvname = "DDU: Readout Errors";
        cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),1,1);
                hname = "hDDU_Readout_Errors";
                h[hname] = new TH1F(TString(hname.c_str()), "", 16 ,0 ,16);
                h[hname]->SetXTitle("Error Type");
                h[hname]->SetYTitle("Number of Events");
                h[hname]->SetFillColor(48);
                h[hname]->SetOption("bar1");
                gStyle->SetOptStat("e");
                cnv[cnvname]->cd(1,1,1);
                h[hname]->Draw();
                consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
        consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]); */



	cnvname = "DDU: Event Buffer Size";
        cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),1,1);
                hname = "hDDU_Buffer_Size";
                h[hname] = new TH1F(TString(hname.c_str()), "", 64000 ,0 ,64000 );
                h[hname]->SetXTitle("Buffer Size in bytes");
                h[hname]->SetYTitle("Number of Events");
		h[hname]->SetFillColor(48);
                h[hname]->SetOption("bar1");
                gStyle->SetOptStat("e");
                cnv[cnvname]->cd(1,1,1);
                h[hname]->Draw();
                consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
//      cnvslides[cnvname] = cnv[cnvname];
//      consinfo->addObject(TString(cnvname.c_str()),SLIDES,0,cnv[cnvname]);
        consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);

	cnvname = "DDU: Unpacked DMBs";
	cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),1,1);
		hname = "hCSC_Unpacked";
		h[hname] = new TH2F(TString(hname.c_str()), "", 300 ,0 , 300, 16, 0, 16);
		h[hname]->SetXTitle("Crate ID");
		h[hname]->SetYTitle("DMB ID");
		h[hname]->SetOption("coltext");
		gStyle->SetOptStat("e");
		cnv[cnvname]->cd(1,1,1);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
//	cnvslides[cnvname] = cnv[cnvname];
//	consinfo->addObject(TString(cnvname.c_str()),SLIDES,0,cnv[cnvname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);

	cnvname = "DDU: L1A and BXN Counters";
        cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),1,2);
		hname = "hDDU_BXN";
		h[hname] = new TH1F(TString(hname.c_str()), "", 4096 ,  0 , 4096);
		h[hname]->SetXTitle("BXN Counter Number");
		h[hname]->SetYTitle("Number of Events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("bar1");
		gStyle->SetOptStat("e");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = "hDDU_L1A_Increment";
		h[hname] = new TH1F(TString(hname.c_str()), "Incremental change in DDU L1A number since previous event", 100 ,  0 , 100);
		h[hname]->SetXTitle("L1A counter increment");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("bar1");
		gStyle->SetOptStat("e");
		cnv[cnvname]->cd(2);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
//	cnvslides[cnvname] = cnv[cnvname];
//	consinfo->addObject(TString(cnvname.c_str()),SLIDES,0,cnv[cnvname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);

	cnvname = "DDU: Active Inputs";
	cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),1,2);
		hname = "hDDU_DMB_DAV_Header_Occupancy_Rate";
		h[hname] = new TH1F(TString(hname.c_str()), "", 16 ,  0 , 16);
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = "hDDU_DMB_DAV_Header_Occupancy";
		h[hname] = new TH1F(TString(hname.c_str()), "DMBs reporting DAV in Header (data available)", 16 ,  0 , 16);
		h[hname]->SetXTitle("DMB position in DDU");
		h[hname]->SetYTitle("Efficiency");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("bar1text");
		gStyle->SetOptStat("e");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = "hDDU_DMB_DAV_Trailer_Occupancy_Rate";
		h[hname] = new TH1F(TString(hname.c_str()), "DMBs reporting DAV in Trailer (data available)", 16 ,  0 , 16);
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = "hDDU_DMB_DAV_Trailer_Occupancy";
		h[hname] = new TH1F(TString(hname.c_str()), "DMBs reporting DAV in Trailer (data available)", 16 ,  0 , 16);
		h[hname]->SetXTitle("DMB position in DDU");
		h[hname]->SetYTitle("Efficiency");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("bar1text");
		gStyle->SetOptStat("e");
		cnv[cnvname]->cd(2);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);

	cnvname = "DDU: Counter of Active DMBs";
	cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),1,3);
		hname = "hDDU_DMB_Active_Header_Count";
		h[hname] = new TH1F(TString(hname.c_str()), "Number of active DMBs reporting DAV in Header", 15 ,  0 , 15);
		h[hname]->SetXTitle("Number of active DMBs");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("bar1text");
		gStyle->SetOptStat("e");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = "hDDU_DMB_Active_Trailer_Count";
		h[hname] = new TH1F(TString(hname.c_str()), "Number of active DMBs reporting DAV in Trailer", 15 ,  0 , 15);
		h[hname]->SetXTitle("Number of active DMBs");
		h[hname]->SetYTitle("Number of events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("bar1text");
		gStyle->SetOptStat("e");
		cnv[cnvname]->cd(2);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = "hDDU_DMB_DAV_Active_Header_Count_vs_DMB_Active_Trailer_Count";
		h[hname] = new TH2F(TString(hname.c_str()), "", 16 ,  0 , 16, 16, 0, 16);
		h[hname]->SetXTitle("Number of active DMBs from Trailer");
		h[hname]->SetYTitle("Number of active DMBs from Header");
		h[hname]->SetOption("coltext");
		gStyle->SetOptStat("e");
		cnv[cnvname]->cd(3,1,1);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);


	cnvname = "DDU: DMB_DAV Count vs DMB_Active Count";
	cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),1,2);
		hname = "hDDU_DMB_DAV_Header_Count_vs_DMB_Active_Header_Count";
		h[hname] = new TH2F(TString(hname.c_str()), "DMB DAV Header Count vs DMB Active Header Count", 16, 0, 16, 16, 0, 16);
		h[hname]->SetXTitle("Number of active DMBs from Header");
		h[hname]->SetYTitle("Number of DAV DMBs from Header");
		h[hname]->SetOption("coltext");
		gStyle->SetOptStat("e");
		cnv[cnvname]->cd(1,1,1);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = "hDDU_DMB_DAV_Trailer_Count_vs_DMB_Active_Trailer_Count";
		h[hname] = new TH2F(TString(hname.c_str()), "DMB DAV Trailer Count vs DMB Active Trailer Count", 16 ,  0 , 16, 16, 0, 16);
		h[hname]->SetXTitle("Number of active DMBs from Trailer");
		h[hname]->SetYTitle("Number of DAV DMBs from Trailer");
		h[hname]->SetOption("coltext");
		gStyle->SetOptStat("e");
		cnv[cnvname]->cd(2,1,1);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);

	cnvname = "DDU: Error Status Occupancy";
	cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),2,1);
		hname = "hDDU_Header_ErrorStat_Rate";
		h[hname] = new TH1F(TString(hname.c_str()), "", 32,  0 , 32);
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = "hDDU_Header_ErrorStat";
		h[hname] = new TH1F(TString(hname.c_str()), "DDU Header Status Error Occupancy", 32,  0 , 32);
		h[hname]->SetYTitle("Efficiency");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("bar1text");
		gStyle->SetOptStat("e");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = "hDDU_Trailer_ErrorStat_Rate";
		h[hname] = new TH1F(TString(hname.c_str()), "", 32,  0 , 32);
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = "hDDU_Trailer_ErrorStat";
		h[hname] = new TH1F(TString(hname.c_str()), "DDU Trailer Status Error Occupancy", 32,  0 , 32);
		h[hname]->SetYTitle("Efficiency");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("bar1text");
		gStyle->SetOptStat("e");
		cnv[cnvname]->cd(2);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);

	cnvname = "DDU: Header and Trailer Markers Check";
	cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),1,1);
		hname = "hDDU_Header_Trailer_Markers_Check_Rate";
		h[hname] = new TH1F(TString(hname.c_str()), "", 4,  0 , 4);
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);

		hname = "hDDU_Header_Trailer_Markers_Check";
		h[hname] = new TH1F(TString(hname.c_str()), "", 4,  0 , 4);
		h[hname]->SetYTitle("Efficiency");
		h[hname]->GetXaxis()->SetBinLabel(1,"Header OK");
		h[hname]->GetXaxis()->SetBinLabel(2,"Header Fail");
		h[hname]->GetXaxis()->SetBinLabel(3,"Trailer OK");
		h[hname]->GetXaxis()->SetBinLabel(4,"Trailer Fail");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("bar1text");
		gStyle->SetOptStat("e");
		cnv[cnvname]->cd(1);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);

	cnvname = "DDU: DMBs Unpacked vs DMBs Active";
	cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),1,1);
		hname = "hDDU_DMB_unpacked_vs_DAV";
		h[hname] = new TH2F(TString(hname.c_str()), "Number of unpacked DMBs vs. number of DMBs reporting DAV", 15 ,  0 , 15, 15, 0, 15);
		h[hname]->SetXTitle("Number of DMBs reporting DAV (Active DMB)");
		h[hname]->SetYTitle("Number of unpacked DMBs");
		h[hname]->SetOption("coltext");
		gStyle->SetOptStat("e");
		cnv[cnvname]->cd(1,1,1);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);

	cnvname = "DDU: Data Integrity Checks vs L1A";
        cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),1,3);
	{
		hname = "hDDU_Unpacking_Match_vs_L1A";
		h[hname] = new TH2F(TString(hname.c_str()), "Match: Unpacked DMBs and DMBs Reporting DAV", 100000,  0 , 100000, 2, 0, 2);
		h[hname]->GetYaxis()->SetBinLabel(1,"OK");
		h[hname]->GetYaxis()->SetBinLabel(2,"Mismatch");
		h[hname]->SetXTitle("L1A Number");
		h[hname]->SetLabelSize(0.1,"Y");
		h[hname]->SetTitleSize(0.05,"X");
		h[hname]->SetOption("col");
		h[hname]->SetStats(false);
		cnv[cnvname]->cd(1,0,1);
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

		hname = "hDDU_L1A_Increment_vs_L1A";
		h[hname] = new TH2F(TString(hname.c_str()), "Incremental L1A", 100000,  0 , 100000, 3, 0, 3);
		h[hname]->LabelsOption("u","Y");
		h[hname]->GetYaxis()->SetBinLabel(1,"Increment=0");
		h[hname]->GetYaxis()->SetBinLabel(2,"Increment=1");
		h[hname]->GetYaxis()->SetBinLabel(3,"Increment>1");
		h[hname]->SetXTitle("L1A Number");
		h[hname]->SetLabelSize(0.1,"Y");
		h[hname]->SetTitleSize(0.05,"X");
		h[hname]->SetOption("col");
		h[hname]->SetStats(false);
		cnv[cnvname]->cd(2,0,1);
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

		hname = "hDDU_Header_Trailer_Markers_Check_vs_L1A";
		h[hname] = new TH2F(TString(hname.c_str()), "Check DDU Header and Trailer Markers", 100000, 0, 100000, 4,  0 , 4);
		h[hname]->GetYaxis()->SetBinLabel(1,"Trailer Fail");
		h[hname]->GetYaxis()->SetBinLabel(2,"Trailer OK");
		h[hname]->GetYaxis()->SetBinLabel(3,"Header OK");
		h[hname]->GetYaxis()->SetBinLabel(4,"Header Fail");
		h[hname]->SetXTitle("L1A Number");
		h[hname]->SetLabelSize(0.1,"Y");
		h[hname]->SetTitleSize(0.05,"X");
		h[hname]->SetOption("col");
		h[hname]->SetStats(false);
		cnv[cnvname]->cd(3,0,1);
		h[hname]->Draw();
		TPaveText *pt6 = new TPaveText(0.7,0.75,0.85,0.85,"brNDC");
		pt6->SetTextAlign(12);
		pt6->AddText("=PROBLEMS");
		pt6->Draw();
		TPaveText *pt7 = new TPaveText(0.7,0.55,0.85,0.65,"brNDC");
		pt7->SetTextAlign(12);
		pt7->AddText("=OK");
		pt7->Draw();
		TPaveText *pt8 = new TPaveText(0.7,0.35,0.85,0.45,"brNDC");
		pt8->SetTextAlign(12);
		pt8->AddText("=OK");
		pt8->Draw();
		TPaveText *pt9 = new TPaveText(0.7,0.15,0.85,0.25,"brNDC");
		pt9->SetTextAlign(12);
		pt9->AddText("=PROBLEMS");
		pt9->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	}
//	cnvslides[cnvname] = cnv[cnvname];
//	consinfo->addObject(TString(cnvname.c_str()),SLIDES,0,cnv[cnvname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);

	cnvname = "DDU: Error Status in Table";
        cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),1,1);
		hname = "hDDU_ErrorStat";
		h[hname] = new TH2F(TString(hname.c_str()), "", 2,  0 , 2, 32, 0, 32);
		h[hname]->GetXaxis()->SetBinLabel(1,"Header");
		h[hname]->GetXaxis()->SetBinLabel(2,"Trailer");
		h[hname]->GetYaxis()->SetBinLabel(1, "CFEB CRC Error: 01");
		h[hname]->GetYaxis()->SetBinLabel(2, "DMB-DDU L1A Mismatch: 02");
		h[hname]->GetYaxis()->SetBinLabel(3, "Lost/New Fibers: 03");
		h[hname]->GetYaxis()->SetBinLabel(4, "INPUT FIFO Full: 04");
		h[hname]->GetYaxis()->SetBinLabel(5, "Two Single Bit-Vote Errors: 05");
		h[hname]->GetYaxis()->SetBinLabel(6, "Multiple Bit-Vote Error: 06");
		h[hname]->GetYaxis()->SetBinLabel(7, "Timeout Error: 07");
		h[hname]->GetYaxis()->SetBinLabel(8, "Extra Control Words: 08");
		h[hname]->GetYaxis()->SetBinLabel(9, "Missing Control Words: 09");
		h[hname]->GetYaxis()->SetBinLabel(10,"CFEB Lost Samples: 10");
		h[hname]->GetYaxis()->SetBinLabel(11,"Control FPGA Clock-DLL Error: 11");
		h[hname]->GetYaxis()->SetBinLabel(12,"Hardware Bit-Vote Errors: 12");
		h[hname]->GetYaxis()->SetBinLabel(13,"INPUT FIFO Near Full: 13");
		h[hname]->GetYaxis()->SetBinLabel(14,"DDU Single Event Warning: 14");
		h[hname]->GetYaxis()->SetBinLabel(15,"DDU Single Event Error: 15");
		h[hname]->GetYaxis()->SetBinLabel(16,"DDU Critical Error: 16");
		h[hname]->GetYaxis()->SetBinLabel(17,"TMB/ALCT-DDU L1A Mismatch: 17");
		h[hname]->GetYaxis()->SetBinLabel(18,"TMB/ALCT Word Count Error: 18");
		h[hname]->GetYaxis()->SetBinLabel(19,"ALCT Error: 19");
		h[hname]->GetYaxis()->SetBinLabel(20,"TMB Error: 20");
		h[hname]->GetYaxis()->SetBinLabel(21,"S-Link Not Ready: 21");
		h[hname]->GetYaxis()->SetBinLabel(22,"S-Link Full Bit Present: 22");
		h[hname]->GetYaxis()->SetBinLabel(23,"Local DAQ FPGA Clock-DLL Error: 23");
		h[hname]->GetYaxis()->SetBinLabel(24,"One Single Bit-Vote Error: 24");
		h[hname]->GetYaxis()->SetBinLabel(25,"No Live Fibers: 25");
		h[hname]->GetYaxis()->SetBinLabel(26,"Data Stuck in FIFO: 26");
		h[hname]->GetYaxis()->SetBinLabel(27,"L1A-FIFO Full: 27");
		h[hname]->GetYaxis()->SetBinLabel(28,"Wrong First Word: 28");
		h[hname]->GetYaxis()->SetBinLabel(29,"Local DAQ Fiber Error: 29");
		h[hname]->GetYaxis()->SetBinLabel(30,"Local DAQ FIFO Near Full: 30");
		h[hname]->GetYaxis()->SetBinLabel(31,"Local DAQ FIFO Full: 31");
		h[hname]->GetYaxis()->SetBinLabel(32,"DDU Output Constricted: 32");
		h[hname]->SetOption("textcol");
		h[hname]->SetStats(false);
		cnv[cnvname]->cd(1,1,1);
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);



	cnvname = "DDU: Word Count";
	cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),1,1);
		hname = "hDDU_Word_Count";
		h[hname] = new TH1F(TString(hname.c_str()), "DDU Word64 Count", 128,  0 , 8192);
		h[hname]->SetXTitle("Number of Words in DDU");
		h[hname]->SetYTitle("Number of Events");
		h[hname]->SetFillColor(48);
		h[hname]->SetOption("bar1");
		cnv[cnvname]->cd(1);
		gStyle->SetOptStat("eom");
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);

	cnvname = "DDU: DMBs with CFEB Active-DAV Mismatches";
	cnv[cnvname] = new ConsumerCanvas(TString(cnvname.c_str()),TString(cnvname.c_str()),TString(cnvname.c_str()),1,1);
		hname = "hCSC_CFEB_Active_DAV_Mismatch";
		h[hname] = new TH2F(TString(hname.c_str()), "DMB Header: CFEB Active-DAV Mismatches", 300,  0, 300, 16, 0, 16);
		h[hname]->SetXTitle("Crate ID");
		h[hname]->SetYTitle("DMB ID");
		h[hname]->SetOption("coltext");
		cnv[cnvname]->cd(1,1,1);
		gStyle->SetOptStat("e");
		h[hname]->Draw();
		consinfo->addObject(TString(hname.c_str()), TString(""), 0, h[hname]);
	consinfo->addObject(TString(cnvname.c_str()),TString(""),0,cnv[cnvname]);

	canvases[0] = cnv;
//	canvases[SLIDES_ID] = cnvslides;
//	histos[SLIDES_ID] = hslides;
	fListModified = true;
	save(HistoFile.c_str());
	return h;
}


