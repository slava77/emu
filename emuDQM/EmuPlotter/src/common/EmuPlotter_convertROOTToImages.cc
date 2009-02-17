#include "EmuPlotter.h"
#include "TClass.h"

int EmuPlotter::convertROOTToImages(std::string rootfile, std::string path, std::string format, int width, int height, std::string runname, std::string filter) 
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
  tree_items.open((path+"/tree_items.js").c_str());
  tree_items << "var TREE_ITEMS = [\n"
	     << "    ['Run: " << runNumber << "', ''," << std::endl;

  csc_list.open((path+"/csc_list.js").c_str());
  csc_list << "var CSC_LIST = [\n"
	   << "    ['Run: " << runNumber << "'";

  std::vector<std::string> EMU_folders;
  std::vector<std::string> DDU_folders;
  std::vector<std::string> CSC_folders;

  TDirectory *sourcedir = gDirectory;

  TIter nextkey( sourcedir->GetListOfKeys() );
  TKey *key;

 
  while ( (key = (TKey*)nextkey())) {
    TObject *obj = key->ReadObj();
    if ( obj->IsA()->InheritsFrom( "TDirectory" ) ) // --- Found Directory
      {
	std::string name=obj->GetName();
	//      cout << name << endl;
	if (name.find("EMU") != std::string::npos) EMU_folders.push_back(name);
    	else if (name.find("DDU") != std::string::npos) DDU_folders.push_back(name);
    	else if (name.find("CSC") != std::string::npos) CSC_folders.push_back(name);

	std::map<std::string, ME_List >::iterator itr = MEs.find(name);

        /*  */      
	if (name.find("DDU_") != std::string::npos) {
	  int dduID = -1;
	  if (sscanf(name.c_str(), "DDU_%d", &dduID) == 1) {
	    LOG4CPLUS_WARN(logger_,"Found DDU with ID " << dduID);
	  }
	  if (MEs.size() == 0 || ((itr = MEs.find(name)) == MEs.end())) {
	    LOG4CPLUS_WARN(logger_, "List of MEs for " << name << " not found. Booking...");
	    MEs[name] = bookMEs("DDU", name);
            MECanvases[name] = bookMECanvases("DDU",name, Form(" DDU = %02d", dduID));
	    printMECollection(MEs[name]);
	  }

	} else if (name.find("CSC_") != std::string::npos) {
	  int crate = -1;
	  int slot = -1;
	  if (sscanf(name.c_str(), "CSC_%d_%d", &crate, &slot) == 2) {
	    LOG4CPLUS_WARN(logger_,"Found CSC crate" << crate << "/slot" << slot);
	  }
	  int ChamberID     = (((crate) << 4) + slot) & 0xFFF;
	  // std::cout << ChamberID << std::endl;
	  if (itr == MEs.end() || (MEs.size()==0)) {
	    LOG4CPLUS_WARN(logger_, "List of Histos for " << name <<  " not found. Booking...");
            MEs[name] = bookMEs("CSC",name);
            MECanvases[name] = bookMECanvases("CSC", name, Form(" Crate ID = %02d. DMB ID = %02d", crate, slot));
	    printMECollection(MEs[name]);
	  }

	} else if (name.find("EMU") != std::string::npos) {
	  int nodeID=0;
	  /*
	    int nodeID = -1;
	    if (sscanf(name.c_str(), "EMU_%d", &nodeID) == 1) {
	    LOG4CPLUS_WARN(logger_,"Found Global Node with ID " << nodeID);
	    }
	  */
	  if (MEs.size() == 0 || ((itr = MEs.find(name)) == MEs.end())) {
	    LOG4CPLUS_WARN(logger_, "List of MEs for " << name << " not found. Booking...");
            MEs[name] = bookMEs("EMU","EMU_Summary");
            MECanvases[name] = bookMECanvases("EMU","EMU");
	    printMECollection(MEs[name]);
	  }
	}
      
	if ((itr = MEs.find(name)) != MEs.end()) {
	  ME_List_const_iterator h_itr;
	  sourcedir->cd(name.c_str());
	  TDirectory* hdir= gDirectory;
	  for (h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr) {
	    // h_itr->second->Write();
	    std::string hname = h_itr->second->getFullName();
	    TKey *key;
	    if ( (key = hdir->FindKeyAny(hname.c_str())) != NULL) {
	      LOG4CPLUS_INFO(logger_, "Found histogram " << hname);
	      
	      TObject *obj = key->ReadObj();
	      if ( obj->IsA()->InheritsFrom( "TH1" ) ) {
		// if (fReset) 	h_itr->second->getObject()->Reset();
		//	h_itr->second->getObject()->Add((TH1*)obj);
		h_itr->second->setObject((MonitorElement*)obj);
		h_itr->second->applyParameters();
	      }
	      // delete obj;
		
	    } else {
	      LOG4CPLUS_ERROR(logger_, "Can not find " << hname);
	    }
	  }
	  
	  if (name.find("EMU") != std::string::npos) {
	    updateFractionHistos();
	    updateEfficiencyHistos();
	  }

	  std::string imgfile = "";
	  //	  for (itr = MECanvases.begin(); itr != MECanvases.end(); ++itr) {
	  std::string relpath = name;
	  if (relpath.find("CSC_") != std::string::npos) {
	    int crate=0;
	    int slot=0;
	    std::string csc_ptrn = "CSC_%d_%d";
	    if (sscanf(relpath.c_str(),csc_ptrn.c_str(), &crate, &slot) == 2) {
	      std::ostringstream st;
	      st << "crate" << crate << "/slot" << slot;
	      relpath =st.str();
	    }
	  }

	  tree_items << "            ['"<< name <<"', '"<< relpath <<"'," << std::endl;
	  MECanvases_List_const_iterator c_itr;

	  for (c_itr = MECanvases[name].begin(); c_itr != MECanvases[name].end(); ++c_itr) {
	    std::string fullname = c_itr->second->getName() + "." + format;
	    std::string relname = relpath + "/" + c_itr->second->getFolder() +"/" + fullname;
	    std::string fullpath  = path+"/"+relpath + "/" + c_itr->second->getFolder();
	    imgfile = fullpath + "/"+ fullname;
	    if (filter == "" || ((filter != "") && (name.find(filter) != std::string::npos))) {
	      TString command = Form("mkdir -p %s",fullpath.c_str());
	      gSystem->Exec(command.Data());
	      // me_itr = MEs.find(name);
	      // if (me_itr != MEs.end()) {
	      //LOG4CPLUS_WARN(logger_, imgfile);
	      c_itr->second->Draw(MEs[name], width, height,true);
	      c_itr->second->Print(imgfile.c_str());
	      // }
	    }
	    tree_items << "                    ['"<< c_itr->second->getTitle() << "','" << relname <<"']," << std::endl;
	  }
	  tree_items << "            ]," << std::endl;
	  // }



	  // saveCanvasImages(path, format, width, height, runname);
	  clearCanvasesCollection(MECanvases[name]);
	  clearMECollection(MEs[name]);
	  MECanvases.clear();
	  MEs.clear();

	}
      }
  }
  tree_items << "    ],\n"
	     << "];" << std::endl;
  tree_items.clear();
  tree_items.close();


  if (EMU_folders.size()) {
    std::sort(EMU_folders.begin(), EMU_folders.end());
    csc_list << ",\n['EMU',[";
    for (uint32_t i=0; i<EMU_folders.size(); i++ ) {
      csc_list << "'"<< EMU_folders.at(i) << ((i==EMU_folders.size())?"'":"',");
    }
    csc_list << "]]";
  }
  if (DDU_folders.size()) {
    std::sort(DDU_folders.begin(), DDU_folders.end());
    csc_list << ",\n['DDU',[";
    for (uint32_t i=0; i<DDU_folders.size(); i++ ) {
      csc_list << "'"<< DDU_folders.at(i) << ((i==DDU_folders.size())?"'":"',");
    }
    csc_list << "]]";
  }

  // Quick fix !!!TODO: Sorted output
  if (CSC_folders.size()) {
    std::sort(CSC_folders.begin(), CSC_folders.end());
    int crate=0;
    int slot=0;
    int cur_crate=-1;
    for (uint32_t i=0; i<CSC_folders.size(); i++ ) {
      std::string csc_ptrn = "CSC_%d_%d";
      if (sscanf(CSC_folders.at(i).c_str(),csc_ptrn.c_str(), &crate, &slot) == 2) {
	if (crate != cur_crate) {
	  if (cur_crate>=0) csc_list << "]]";
	  csc_list << ",\n['crate"<< crate << "', [";
	}
	csc_list << "'slot" << slot << "',";
      }
      cur_crate = crate;

    }
    csc_list << "]]";
  }
  
  csc_list << "]\n"
	   << "];" << std::endl;
  csc_list.clear();
  csc_list.close();


  createTreePage(path);
  createTreeEngine(path);
  createTreeTemplate(path);
  createHTMLNavigation(path);  

  generateCanvasesListFile(path+"/canvases_list.js", format);

  return 0;
}
