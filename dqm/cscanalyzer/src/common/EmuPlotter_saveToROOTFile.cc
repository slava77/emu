#include "emu/dqm/cscanalyzer/EmuPlotter.h"

void EmuPlotter::saveToROOTFile(std::string filename)
{
  LOG4CPLUS_WARN(logger_, "Saving MEs to ROOT File... " << filename);

  if (filename == "")
  {
    LOG4CPLUS_ERROR(logger_, "Empty ROOT file name ");
    return;
  }

  TFile f(filename.c_str(), "recreate");
  if (!f.IsZombie())
  {
    appBSem_.take();
    updateFractionHistos();
    updateCSCHistos();
    updateEfficiencyHistos();
    gStyle->SetPalette(1,0);
    f.cd();
    std::map<std::string, ME_List>::iterator itr;
    ME_List_const_iterator h_itr;
    TDirectory * hdir = f.mkdir("DQMData");
    hdir->cd();


    /// Write data Format Version field
    std::stringstream st;
    st.str("");
    TObjString FormatVersion;
    st << "<FormatVersion>" << theFormatVersion << "</FormatVersion>";   
    FormatVersion.SetString(st.str().c_str()); 
    FormatVersion.Write(st.str().c_str());
    
    TObjString RunNumber;
    st.str("");
    st.clear();
    st << "<RunNumber>" << runNumber << "</RunNumber>";
    RunNumber.SetString(st.str().c_str()); 
    RunNumber.Write(st.str().c_str());

    TObjString AnalyzerVersion;
    st.str("");
    st.clear();
    st << "<AnalyzerVersion>" << emudqmcscanalyzer::versions << "</AnalyzerVersion>";
    AnalyzerVersion.SetString(st.str().c_str());
    AnalyzerVersion.Write(st.str().c_str());

    for (itr = MEs.begin(); itr != MEs.end(); ++itr)
    {
      TDirectory * rdir = hdir->mkdir((itr->first).c_str());
      rdir->cd();
      for (h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr)
      {
        h_itr->second->Write();
      }
      hdir->cd();
    }
    f.Close();
    appBSem_.give();
    LOG4CPLUS_WARN(logger_, "Done.");
    //  saveCanvasesToROOTFile(filename);
  }
  else
  {
    LOG4CPLUS_ERROR(logger_, "Unable to open output ROOT file " << filename);
  }
}

void EmuPlotter::saveCanvasesToROOTFile(std::string filename)
{

  LOG4CPLUS_WARN(logger_, "Saving Canvases to ROOT File... " << filename);

  if (filename == "")
  {
    LOG4CPLUS_ERROR(logger_, "Empty ROOT file name ");
    return;
  }


  TFile f(filename.c_str(), "update");
  if (!f.IsZombie())
  {
    gStyle->SetPalette(1,0);
    f.cd();
    std::map<std::string, ME_List>::iterator me_itr;
    std::map<std::string, MECanvases_List>::iterator itr;

    MECanvases_List_const_iterator h_itr;

    appBSem_.take();
    for (itr = MECanvases.begin(); itr != MECanvases.end(); ++itr)
    {
      TDirectory * rdir = f.mkdir((itr->first).c_str());
      rdir->cd();
      for (h_itr = itr->second.begin(); h_itr != itr->second.end(); ++h_itr)
      {
        me_itr = MEs.find(itr->first);
        std::string folder = h_itr->second->getFolder();
        if (!rdir->GetDirectory(folder.c_str(),false))
        {
          TDirectory *subdir = rdir->mkdir(folder.c_str());
          subdir->cd();
        }
        else rdir->cd(folder.c_str());
        if (me_itr != MEs.end())
        {
          h_itr->second->setRunNumber(runNumber);
          h_itr->second->Draw(me_itr->second, true);
          h_itr->second->Write();
        }
      }
      f.cd();
    }
    f.Close();
    appBSem_.give();
    LOG4CPLUS_WARN(logger_, "Done.");
  }
  else
  {
    LOG4CPLUS_ERROR(logger_, "Unable to open output ROOT file " << filename);
  }
}


