#include "EmuPlotter.h"
#include "TClass.h"

/*
  Generate text file with DQM report
*/

int EmuPlotter::generateReport(std::string rootfile, std::string path, std::string runname) 
{


  gSystem->Load("libHistPainter");
  TFile* rootsrc = TFile::Open( rootfile.c_str());
  if (!rootsrc) {
    LOG4CPLUS_ERROR (logger_, "Unable to open " << rootfile.c_str());
    return -1;
  }
  if (!rootsrc->cd("DQMData")) {
    LOG4CPLUS_ERROR (logger_, "No histos folder in file");
    return -1;
  }

  std::ofstream tree_items;
  std::ofstream csc_list;

  TString command = Form("mkdir -p %s",path.c_str());
  gSystem->Exec(command.Data());

  std::string runNumber = runname;

  std::vector<std::string> EMU_folders;
  std::vector<std::string> DDU_folders;
  std::vector<std::string> CSC_folders;



  TDirectory *sourcedir = gDirectory;


  ME_List nodeME;

  std::string nodeTag = "EMU";
  MEs[nodeTag] = bookCommon(0);
  MECanvases[nodeTag] = bookCommonCanvases(0);
  nodeME = MEs[nodeTag];

  EmuMonitoringObject* mo=0;

  int CSCtype = 0;
  int CSCposition = 0;


  TKey *key = sourcedir->FindKey(nodeTag.c_str());
  if (key && key->ReadObj()->IsA()->InheritsFrom( "TDirectory" ) ) {
    sourcedir->cd(nodeTag.c_str());
    TDirectory* hdir= gDirectory;

      
    if (isMEvalid(nodeME, "DMB_Reporting", mo)) {
      if ( (key = hdir->FindKeyAny(mo->getFullName().c_str())) != NULL) {
	LOG4CPLUS_WARN(logger_,"Found " << mo->getName());
	TObject *obj = key->ReadObj();
	if ( obj->IsA()->InheritsFrom( "TH2" ) ) {
	  mo->setObject((MonitorElement*)obj);
	  TH2F* h2 = dynamic_cast<TH2F*>(obj);
	  unsigned long evt_cntr = 0;
	  unsigned long csc_cntr = 0;
	  unsigned long avg_events = 0;
	  std::vector< std::pair<std::string, unsigned long> > stats;
	  for (int i=h2->GetXaxis()->GetXmin(); i<= h2->GetXaxis()->GetXmax(); i++)
	    for (int j=h2->GetYaxis()->GetXmin(); j <= h2->GetYaxis()->GetXmax(); j++) {
	      unsigned long cnt = h2->GetBinContent(i, j);
	      if (cnt>0) {
		std::string cscTag(Form("CSC_%03d_%02d", i, j));
		std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
		stats.push_back(make_pair(cscName,cnt));
		evt_cntr+=cnt;
		csc_cntr++;
	      }	      
	    }
	  if (csc_cntr) {
	    avg_events=evt_cntr/csc_cntr;
	    if (avg_events >= 500) { // Detect Hot/Low eff chambers if average number of events is reasonable (>500)
	      for (int i=0; i<stats.size(); i++) {
		double fract=((double)stats[i].second)/avg_events;
		std::string cscName=stats[i].first;
		if (fract >= 5.) {
		  std::string diag=Form("Hot chamber with %.0f times more than average events counter (avg events=%d)",fract, avg_events);
		  report[cscName].push_back(diag);
		} else if (fract < 0.05) {
		  std::string diag=Form("Low efficiency chamber with %.2f fraction of average events counter (avg events=%d)",fract, avg_events);
		  report[cscName].push_back(diag);
		}
	      }
	    }
	  }

	}
      }
    }


    if (isMEvalid(nodeME, "DMB_Format_Errors_Fract", mo)) {
      if ( (key = hdir->FindKeyAny(mo->getFullName().c_str())) != NULL) {
	// LOG4CPLUS_WARN(logger_,"Found " << mo->getName());
	TObject *obj = key->ReadObj();
	if ( obj->IsA()->InheritsFrom( "TH2" ) ) {
	  mo->setObject((MonitorElement*)obj);
	  TH2F* h2 = dynamic_cast<TH2F*>(obj);
	  for (int i=h2->GetXaxis()->GetXmin(); i<= h2->GetXaxis()->GetXmax(); i++) 
	    for (int j=h2->GetYaxis()->GetXmin(); j <= h2->GetYaxis()->GetXmax(); j++) {
	      double z = h2->GetBinContent(i, j);
	      if (z>0) {
		std::string cscTag(Form("CSC_%03d_%02d", i, j));
		std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );

		float fract=z*100;
		std::string severity="";
		if (fract >= 80.) severity="critical";
		else if (fract >= 10.) severity="severe";
		else if (fract > 1.) severity="tolerable";
		else severity="minor";
		std::string diag=Form("%.1f%% Format Errors detected [%s])",fract,severity.c_str());
		report[cscName].push_back(diag);
		int ChamberID     = (((i) << 4) + j) & 0xFFF;
		TKey* csckey = sourcedir->FindKey(cscTag.c_str());
		if (csckey &&  csckey->ReadObj()->IsA()->InheritsFrom( "TDirectory")) {
		  EmuMonitoringObject* mo1=0;
		  if (MEs.find(cscTag) == MEs.end()) {
		    MEs[cscTag] = bookChamber(ChamberID);
		    MECanvases[cscTag] = bookChamberCanvases(ChamberID);		     
		  }
		  ME_List& cscME = MEs[cscTag];
		  sourcedir->cd(cscTag.c_str());
		  TDirectory* hdir= gDirectory;
		  if (isMEvalid(cscME, "BinCheck_Errors_Frequency", mo1)) {
		    if ( (csckey = hdir->FindKeyAny(mo1->getFullName().c_str())) != NULL) {
		      // LOG4CPLUS_WARN(logger_,cscName << " Found " << mo1->getName());
		      TObject *obj = csckey->ReadObj();
		      if ( obj->IsA()->InheritsFrom( "TH2" ) ) {
			// mo1->setObject((MonitorElement*)obj);
			 
			TH2F* h2 = dynamic_cast<TH2F*>(obj);
			 
			for (int err=h2->GetYaxis()->GetXmin(); err <= h2->GetYaxis()->GetXmax(); err++) {
			  double z = h2->GetBinContent(1, err);
			   
			  if (z>0) {
			    float fract=z*100;
			    std::string error_type = std::string(h2->GetYaxis()->GetBinLabel(err));
			    std::string diag=std::string(Form("Format: %.1f%% %s",fract,error_type.c_str()));
			    // LOG4CPLUS_WARN(logger_, cscTag << ": "<< diag);
			    report[cscName].push_back(diag);
			  }
			   
			}
			 
		      }
		    }
		  }
		   
		}
	
	      }
		
	    }
	}

      }
    }

    if (isMEvalid(nodeME, "DMB_input_fifo_full_Fract", mo)) {
      if ( (key = hdir->FindKeyAny(mo->getFullName().c_str())) != NULL) {
        // LOG4CPLUS_WARN(logger_,"Found " << mo->getName());
        TObject *obj = key->ReadObj();
        if ( obj->IsA()->InheritsFrom( "TH2" ) ) {
          mo->setObject((MonitorElement*)obj);
          TH2F* h2 = dynamic_cast<TH2F*>(obj);
          for (int i=h2->GetXaxis()->GetXmin(); i<= h2->GetXaxis()->GetXmax(); i++)
	    for (int j=h2->GetYaxis()->GetXmin(); j <= h2->GetYaxis()->GetXmax(); j++) {
	      double z = h2->GetBinContent(i, j);
	      if (z>0) {
		std::string cscTag(Form("CSC_%03d_%02d", i, j));
		std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );

		float fract=z*100;
		std::string severity="";
		if (fract >= 80.) severity="critical";
		else if (fract >= 10.) severity="severe";
		else if (fract > 1.) severity="tolerable";
		else severity="minor";
		std::string diag=Form("%.1f%% DMB-Input FIFO Full detected [%s])",fract,severity.c_str());

		report[cscName].push_back(diag);
		
		int ChamberID     = (((i) << 4) + j) & 0xFFF;
		TKey* csckey = sourcedir->FindKey(cscTag.c_str());
		if (csckey &&  csckey->ReadObj()->IsA()->InheritsFrom( "TDirectory")) {
		  EmuMonitoringObject* mo1=0;
		  if (MEs.find(cscTag) == MEs.end()) {
		    MEs[cscTag] = bookChamber(ChamberID);
		    MECanvases[cscTag] = bookChamberCanvases(ChamberID);		     
		  }
		  ME_List& cscME = MEs[cscTag];
		  sourcedir->cd(cscTag.c_str());
		  TDirectory* hdir= gDirectory;
		  if (isMEvalid(cscME, "BinCheck_DataFlow_Problems_Frequency", mo1)) {
		    if ( (csckey = hdir->FindKeyAny(mo1->getFullName().c_str())) != NULL) {
		      // LOG4CPLUS_WARN(logger_,cscName << " Found " << mo1->getName());
		      TObject *obj = csckey->ReadObj();
		      if ( obj->IsA()->InheritsFrom( "TH2" ) ) {
			// mo1->setObject((MonitorElement*)obj);
			 
			TH2F* h2 = dynamic_cast<TH2F*>(obj);
			 
			for (int err=h2->GetYaxis()->GetXmin(); err <=7; err++) {
			  double z = h2->GetBinContent(1, err);
			   
			  if (z>0) {
			    float fract=z*100;
			    std::string error_type = std::string(h2->GetYaxis()->GetBinLabel(err));
			    std::string diag=std::string(Form("DMB-Input FIFO: %.1f%% %s",fract,error_type.c_str()));
			    // LOG4CPLUS_WARN(logger_, cscTag << ": "<< diag);
			    report[cscName].push_back(diag);
			  }
			   
			}
			 
		      }
		    }
		  }
		   
		}
		
	      }

	    }
        }

      }
    }

    if (isMEvalid(nodeME, "DMB_input_timeout_Fract", mo)) {
      if ( (key = hdir->FindKeyAny(mo->getFullName().c_str())) != NULL) {
        // LOG4CPLUS_WARN(logger_,"Found " << mo->getName());
        TObject *obj = key->ReadObj();
        if ( obj->IsA()->InheritsFrom( "TH2" ) ) {
          mo->setObject((MonitorElement*)obj);
          TH2F* h2 = dynamic_cast<TH2F*>(obj);
          for (int i=h2->GetXaxis()->GetXmin(); i<= h2->GetXaxis()->GetXmax(); i++)
	    for (int j=h2->GetYaxis()->GetXmin(); j <= h2->GetYaxis()->GetXmax(); j++) {
	      double z = h2->GetBinContent(i, j);
	      if (z>0) {
		std::string cscTag(Form("CSC_%03d_%02d", i, j));
		std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );

		float fract=z*100;
		std::string severity="";
		if (fract >= 80.) severity="critical";
		else if (fract >= 10.) severity="severe";
		else if (fract > 1.) severity="tolerable";
		else severity="minor";
		std::string diag=Form("%.1f%% DMB-Input Timeout detected [%s])",fract,severity.c_str());

		report[cscName].push_back(diag);
		int ChamberID     = (((i) << 4) + j) & 0xFFF;
		TKey* csckey = sourcedir->FindKey(cscTag.c_str());
		if (csckey &&  csckey->ReadObj()->IsA()->InheritsFrom( "TDirectory")) {
		  EmuMonitoringObject* mo1=0;
		  if (MEs.find(cscTag) == MEs.end()) {
		    MEs[cscTag] = bookChamber(ChamberID);
		    MECanvases[cscTag] = bookChamberCanvases(ChamberID);		     
		  }
		  ME_List& cscME = MEs[cscTag];
		  sourcedir->cd(cscTag.c_str());
		  TDirectory* hdir= gDirectory;
		  if (isMEvalid(cscME, "BinCheck_DataFlow_Problems_Frequency", mo1)) {
		    if ( (csckey = hdir->FindKeyAny(mo1->getFullName().c_str())) != NULL) {
		      // LOG4CPLUS_WARN(logger_,cscName << " Found " << mo1->getName());
		      TObject *obj = csckey->ReadObj();
		      if ( obj->IsA()->InheritsFrom( "TH2" ) ) {
			// mo1->setObject((MonitorElement*)obj);
			 
			TH2F* h2 = dynamic_cast<TH2F*>(obj);
			 
			for (int err=8; err < h2->GetYaxis()->GetXmax(); err++) {
			  double z = h2->GetBinContent(1, err);
			   
			  if (z>0) {
			    float fract=z*100;
			    std::string error_type = std::string(h2->GetYaxis()->GetBinLabel(err));
			    std::string diag=std::string(Form("DMB-Input Timeout: %.1f%% %s",fract,error_type.c_str()));
			    // LOG4CPLUS_WARN(logger_, cscTag << ": "<< diag);
			    report[cscName].push_back(diag);
			  }
			   
			}
			 
		      }
		    }
		  }
		}

	      }
	    }
	}

      }
    }

    if (isMEvalid(nodeME, "DMB_wo_ALCT_Fract", mo)) {
      if ( (key = hdir->FindKeyAny(mo->getFullName().c_str())) != NULL) {
	LOG4CPLUS_WARN(logger_,"Found " << mo->getName());
	TObject *obj = key->ReadObj();
	if ( obj->IsA()->InheritsFrom( "TH2" ) ) {
	  mo->setObject((MonitorElement*)obj);
	  TH2F* h2 = dynamic_cast<TH2F*>(obj);
	  for (int i=h2->GetXaxis()->GetXmin(); i<= h2->GetXaxis()->GetXmax(); i++)
	    for (int j=h2->GetYaxis()->GetXmin(); j <= h2->GetYaxis()->GetXmax(); j++) {
	      double z = h2->GetBinContent(i, j);
	      if (z>0.95) {
		std::string cscTag(Form("CSC_%03d_%02d", i, j));
		std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
		// LOG4CPLUS_WARN(logger_, cscTag << ": "<< z*100 << "% Format errors detected");
		float fract=z*100;
		std::string severity="";
		severity="critical";
		std::string diag=Form("%.1f%% No ALCT Data [%s]",fract,severity.c_str());

		report[cscName].push_back(diag);
	      }

	    }
	}

      }
    }

    if (isMEvalid(nodeME, "DMB_wo_CLCT_Fract", mo)) {
      if ( (key = hdir->FindKeyAny(mo->getFullName().c_str())) != NULL) {
	LOG4CPLUS_WARN(logger_,"Found " << mo->getName());
	TObject *obj = key->ReadObj();
	if ( obj->IsA()->InheritsFrom( "TH2" ) ) {
	  mo->setObject((MonitorElement*)obj);
	  TH2F* h2 = dynamic_cast<TH2F*>(obj);
	  for (int i=h2->GetXaxis()->GetXmin(); i<= h2->GetXaxis()->GetXmax(); i++)
	    for (int j=h2->GetYaxis()->GetXmin(); j <= h2->GetYaxis()->GetXmax(); j++) {
	      double z = h2->GetBinContent(i, j);
	      if (z>0.95) {
		std::string cscTag(Form("CSC_%03d_%02d", i, j));
		std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );

		// LOG4CPLUS_WARN(logger_, cscTag << ": "<< z*100 << "% Format errors detected");
		float fract=z*100;
		std::string severity="";
		severity="critical";
		std::string diag=Form("%.1f%% No TMB Data [%s]",fract,severity.c_str());

		report[cscName].push_back(diag);
	      }

	    }
	}

      }
    }


    if (isMEvalid(nodeME, "DMB_wo_CFEB_Fract", mo)) {
      if ( (key = hdir->FindKeyAny(mo->getFullName().c_str())) != NULL) {
	LOG4CPLUS_WARN(logger_,"Found " << mo->getName());
	TObject *obj = key->ReadObj();
	if ( obj->IsA()->InheritsFrom( "TH2" ) ) {
	  mo->setObject((MonitorElement*)obj);
	  TH2F* h2 = dynamic_cast<TH2F*>(obj);
	  for (int i=h2->GetXaxis()->GetXmin(); i<= h2->GetXaxis()->GetXmax(); i++)
	    for (int j=h2->GetYaxis()->GetXmin(); j <= h2->GetYaxis()->GetXmax(); j++) {
	      double z = h2->GetBinContent(i, j);
	      if (z>0.95) {
		std::string cscTag(Form("CSC_%03d_%02d", i, j));
		std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );

		// LOG4CPLUS_WARN(logger_, cscTag << ": "<< z*100 << "% Format errors detected");
		float fract=z*100;
		std::string severity="";
		severity="critical";
		std::string diag=Form("%.1f%% No Data in one of the CFEBs [%s]",fract,severity.c_str());

		report[cscName].push_back(diag);
	      }

	    }
	}

      }
    }

    
    
    if (isMEvalid(nodeME, "DMB_L1A_out_of_sync_Fract", mo)) {
      if ( (key = hdir->FindKeyAny(mo->getFullName().c_str())) != NULL) {
	LOG4CPLUS_WARN(logger_,"Found " << mo->getName());
	TObject *obj = key->ReadObj();
	if ( obj->IsA()->InheritsFrom( "TH2" ) ) {
	  mo->setObject((MonitorElement*)obj);
	  TH2F* h2 = dynamic_cast<TH2F*>(obj);
	  for (int i=h2->GetXaxis()->GetXmin(); i<= h2->GetXaxis()->GetXmax(); i++)
	    for (int j=h2->GetYaxis()->GetXmin(); j <= h2->GetYaxis()->GetXmax(); j++) {
	      double z = h2->GetBinContent(i, j);
	      if (z>0) {
		std::string cscTag(Form("CSC_%03d_%02d", i, j));
		std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );

		// LOG4CPLUS_WARN(logger_, cscTag << ": "<< z*100 << "% Format errors detected");
		float fract=z*100;
		std::string severity="";
		if (fract >= 80.) severity="critical";
		else if (fract >= 10.) severity="severe";
		else if (fract > 1.) severity="tolerable";
		else severity="minor";

		std::string diag=Form("%.1f%% L1A out of sync [%s]",fract,severity.c_str());

		report[cscName].push_back(diag);
	      }

	    }
	}

      }
    }
    
    if (isMEvalid(nodeME, "DMB_Format_Warnings_Fract", mo)) {
      if ( (key = hdir->FindKeyAny(mo->getFullName().c_str())) != NULL) {
	LOG4CPLUS_WARN(logger_,"Found " << mo->getName());
	TObject *obj = key->ReadObj();
	if ( obj->IsA()->InheritsFrom( "TH2" ) ) {
	  mo->setObject((MonitorElement*)obj);
	  TH2F* h2 = dynamic_cast<TH2F*>(obj);
	  for (int i=h2->GetXaxis()->GetXmin(); i<= h2->GetXaxis()->GetXmax(); i++)
	    for (int j=h2->GetYaxis()->GetXmin(); j <= h2->GetYaxis()->GetXmax(); j++) {
	      double z = h2->GetBinContent(i, j);
	      if (z>0) {
		std::string cscTag(Form("CSC_%03d_%02d", i, j));
		std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );

		// LOG4CPLUS_WARN(logger_, cscTag << ": "<< z*100 << "% Format errors detected");
		float fract=z*100;
		std::string severity="";
		if (fract >= 80.) severity="critical";
		else if (fract >= 10.) severity="severe";
		else if (fract > 1.) severity="tolerable";
		else severity="minor";

		std::string diag=Form("%.1f%% CFEB B-Words [%s]",fract,severity.c_str());

		report[cscName].push_back(diag);
	      }

	    }
	}

      }
    }
    

  }
 
  showReport();
  return 0;
}

void EmuPlotter::showReport()
{
  std::map<std::string, std::vector<std::string> >::iterator itr;
  std::vector<std::string>::iterator err_itr;

  int csc_cntr=0;

  LOG4CPLUS_WARN(logger_, "<=== Automatically generated DQM Report ===>");
  for (itr = report.begin(); itr != report.end(); ++itr) {
    LOG4CPLUS_WARN(logger_, "<--- " <<itr->first);
    if (itr->first.find("ME") == 0) csc_cntr++;
    for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr) {
      LOG4CPLUS_WARN(logger_, (*err_itr));
    }
    LOG4CPLUS_WARN(logger_, "--->");
	
  }
  LOG4CPLUS_WARN(logger_, "Detected " << csc_cntr << " chambers with problems");
}

