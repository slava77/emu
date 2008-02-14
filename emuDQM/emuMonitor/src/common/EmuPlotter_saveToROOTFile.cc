#include "EmuPlotter.h"

void EmuPlotter::saveToROOTFile(std::string filename)
{
  LOG4CPLUS_WARN(logger_, "Saving MEs to ROOT File... " << filename)
    if (filename == "") {
      LOG4CPLUS_ERROR(logger_, "Empty ROOT file name ")
	return;
	}
  TFile f(filename.c_str(), "recreate");
  if (!f.IsZombie()) {
    fBusy = true;
    gStyle->SetPalette(1,0);
    f.cd();
    std::map<std::string, ME_List>::iterator itr;
    ME_List_const_iterator h_itr;

//     TDirectory * hdir = f.mkdir("histos");
    TDirectory * hdir = f.mkdir("DQMData");
    hdir->cd();
    for (itr = MEs.begin(); itr != MEs.end(); ++itr) {
      TDirectory * rdir = hdir->mkdir((itr->first).c_str());
      rdir->cd();
      for (h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr) {
	h_itr->second->Draw();	
	h_itr->second->Write();
      }
      hdir->cd();
    } 

    /*
      TDirectory* rdir = f.mkdir("Common");
      rdir->cd();
      for (h_itr = commonMEfactory.begin(); h_itr != commonMEfactory.end(); ++h_itr) {
      h_itr->second->Write();
      }
      f.cd();
    */
    f.Close();
    LOG4CPLUS_WARN(logger_, "Done.");
  //  saveCanvasesToROOTFile(filename);
    fBusy=false;
  } else {
    LOG4CPLUS_ERROR(logger_, "Unable to open output ROOT file " << filename);
  }
}

void EmuPlotter::saveCanvasesToROOTFile(std::string filename)
{
  LOG4CPLUS_WARN(logger_, "Saving Canvases to ROOT File... " << filename)
    if (filename == "") {
      LOG4CPLUS_ERROR(logger_, "Empty ROOT file name ")
	return;
	}

  
  TFile f(filename.c_str(), "update");
  if (!f.IsZombie()) {
    fBusy=true;
    gStyle->SetPalette(1,0);
    f.cd(); 
    std::map<std::string, ME_List>::iterator me_itr;
    std::map<std::string, MECanvases_List>::iterator itr;

    MECanvases_List_const_iterator h_itr;
  

    for (itr = MECanvases.begin(); itr != MECanvases.end(); ++itr) {
      TDirectory * rdir = f.mkdir((itr->first).c_str());
      rdir->cd();
      for (h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr) {
	me_itr = MEs.find(itr->first);
	std::string folder = h_itr->second->getFolder();
	if (!rdir->GetDirectory(folder.c_str(),false)) {
	  TDirectory *subdir = rdir->mkdir(folder.c_str());
	  subdir->cd();
	} else rdir->cd(folder.c_str());
	if (me_itr != MEs.end()) {
	  h_itr->second->Draw(me_itr->second);
	  h_itr->second->Write();
	}
      }
      f.cd();
    }
    f.Close();
    LOG4CPLUS_WARN(logger_, "Done.");
    fBusy=false;
  } else {
    LOG4CPLUS_ERROR(logger_, "Unable to open output ROOT file " << filename);
  }
}


