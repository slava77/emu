#include "EmuPlotter.h"

int EmuPlotter::loadFromROOTFile(string rootfile, bool fReset) 
{

  gSystem->Load("libHistPainter");
  TFile* rootsrc = TFile::Open( rootfile.c_str());
  if (!rootsrc) {
    LOG4CPLUS_ERROR (logger_, "Unable to open " << rootfile.c_str());
    return -1;
  }
  if (!rootsrc->cd("histos")) {
    LOG4CPLUS_ERROR (logger_, "No histos folder in file");
    return -1;
  }

  TDirectory *sourcedir = gDirectory;

  TIter nextkey( sourcedir->GetListOfKeys() );
  TKey *key;

  while ( (key = (TKey*)nextkey())) {
    TObject *obj = key->ReadObj();
    if ( obj->IsA()->InheritsFrom( "TDirectory" ) ) 
      {
	string name=obj->GetName();
	//      cout << name << endl;
	std::map<std::string, ME_List >::iterator itr;
	if (name.find("DDU_") != std::string::npos) {
	  int dduID = -1;
	  if (sscanf(name.c_str(), "DDU_%d", &dduID) == 1) {
	    LOG4CPLUS_WARN(logger_,"Found DDU with ID " << dduID);
	  }
	  if (MEs.size() == 0 || ((itr = MEs.find(name)) == MEs.end())) {
	    LOG4CPLUS_WARN(logger_, "List of MEs for " << name << " not found. Booking...");
	    MEs[name] = bookDDU(dduID);
	    MECanvases[name] = bookDDUCanvases(dduID);
	    printMECollection(MEs[name]);
	  }
	} else if (name.find("CSC_") != std::string::npos) {
	  int crate = -1;
	  int slot = -1;
	  if (sscanf(name.c_str(), "CSC_%d_%d", &crate, &slot) == 2) {
	    LOG4CPLUS_WARN(logger_,"Found CSC crate" << crate << "/slot" << slot);
	  }
	  int ChamberID     = (((crate) << 4) + slot) & 0xFFF;
	  if (itr == MEs.end() || (MEs.size()==0)) {
	    LOG4CPLUS_WARN(logger_, "List of Histos for " << name <<  " not found");
	    MEs[name] = bookChamber(ChamberID);
	    MECanvases[name] = bookChamberCanvases(ChamberID);
	    
	    printMECollection(MEs[name]);
	  }
	} else if (name.find("EMU_") != std::string::npos) {
	  int nodeID = -1;
	  if (sscanf(name.c_str(), "EMU_%d", &nodeID) == 1) {
	    LOG4CPLUS_WARN(logger_,"Found Global Node with ID " << nodeID);
	  }
	  if (MEs.size() == 0 || ((itr = MEs.find(name)) == MEs.end())) {
	    LOG4CPLUS_WARN(logger_, "List of MEs for " << name << " not found. Booking...");
	    MEs[name] = bookCommon(nodeID);
	    MECanvases[name] = bookCommonCanvases(nodeID);
	    printMECollection(MEs[name]);
	  }
	}
      
	if ((itr = MEs.find(name)) != MEs.end()) {
	  ME_List_const_iterator h_itr;
	  sourcedir->cd(name.c_str());
	  TDirectory* hdir= gDirectory;
	  for (h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr) {
	    // h_itr->second->Write();
	    string hname = h_itr->second->getFullName();
	    TKey *key;
	    if ( (key = hdir->FindKey(hname.c_str())) != NULL) {
	      LOG4CPLUS_INFO(logger_, "Found histogram " << hname);
	      TObject *obj = key->ReadObj();
	      if ( obj->IsA()->InheritsFrom( "TH1" ) ) {
		if (fReset) 	h_itr->second->getObject()->Reset();
		h_itr->second->getObject()->Add((TH1*)obj);
	      }
	    } else {
	      LOG4CPLUS_ERROR(logger_, "Can not find " << hname);
	    }
		
	  }
	}

      }
  }

  return 0;
}
