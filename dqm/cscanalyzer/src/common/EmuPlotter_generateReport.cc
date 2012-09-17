#include "emu/dqm/cscanalyzer/EmuPlotter.h"
#include "TClass.h"

/**
 *  Generate text file with DQM report
 */

int EmuPlotter::loadKillBitsConfig(std::string killbitsFile)
{
  /*
    if (killbitsFile == "") {
      return 1;
    }

    XMLPlatformUtils::Initialize();
    XercesDOMParser *parser = new XercesDOMParser();
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setDoNamespaces(true);
    parser->setDoSchema(true);
    parser->setValidationSchemaFullChecking(false); // this is default
    parser->setCreateEntityReferenceNodes(true);  // this is default
    parser->setIncludeIgnorableWhitespace (false);

    parser->parse(killbitsFile.c_str());
    DOMDocument *doc = parser->getDocument();
    DOMNodeList *l = doc->getElementsByTagName( XMLString::transcode("KillBits") );
    if( l->getLength() != 1 ){
      return 1;
    }

    DOMNodeList *itemList = doc->getElementsByTagName( XMLString::transcode("CSC") );
    if( itemList->getLength() == 0 ){
      return 1;
    }
    for(uint32_t i=0; i<itemList->getLength(); i++){
      std::map<std::string, std::string> obj_info;
      std::map<std::string, std::string>::iterator itr;
      DOMNodeList *children = itemList->item(i)->getChildNodes();
      for(unsigned int i=0; i<children->getLength(); i++){
        std::string paramname = std::string(XMLString::transcode(children->item(i)->getNodeName()));
        if ( children->item(i)->hasChildNodes() ) {
          std::string param = std::string(XMLString::transcode(children->item(i)->getFirstChild()->getNodeValue()));
          obj_info[paramname] = param;
        }
      }
      itr = obj_info.find("CSC");
      if (itr != obj_info.end()) {
        std::cout << "Found masks for " << itr->second << std::endl;
        if (obj_info["CFEBChans"] != "") {
          tmasks[itr->second]=parseMask(obj_info["CFEBChans"]);
        }
      }
    }
    delete parser;
  */
  return 0;
}


/**
 * Get CSC Chamber name from mapping
 */
std::string EmuPlotter::getCSCName(std::string cscID, int& crate, int& slot, int& CSCtype, int& CSCposition )
{
  //  int crate=0, slot=0;
  std::string cscName="";
  if (sscanf(cscID.c_str(), "CSC_%03d_%02d", &crate , &slot) == 2)
    {
      cscName=getCSCFromMap(crate,slot, CSCtype, CSCposition );
    }
  return cscName;
}

/**
 * Find and return pointer to Monitoring Element (ME)/Histogram in the ME storage container (ROOT file)
 */
MonitorElement* EmuPlotter::findME(std::string Folder, std::string name, TDirectory* rootfolder)
{

  MonitorElement* me = 0;
  EmuMonitoringObject* mo=0;

  if (rootfolder)   /// Look in ROOT file
    {

      std::string scope = Folder;
      std::string tag = Folder;

      if (Folder.find("EMU") != std::string::npos)
        {
          scope = "EMU";
          tag="EMU_Summary";
        }
      else if (Folder.find("DDU")  != std::string::npos)
        {
          scope = "DDU";
        }
      else if (Folder.find("CSC")  != std::string::npos)
        {
          scope = "CSC";
        }

      ME_List & listMEs = MEFactories[scope];

      if (isMEvalid(listMEs, name, mo))
        {

          mo->setPrefix(tag);

          TObject *obj = rootfolder->Get((Folder+"/"+mo->getFullName()).c_str());
          if (obj != NULL)
            {
              LOG4CPLUS_DEBUG(logger_, "Successfully Read " << Folder << "/" << mo->getFullName() << " object");
              // mo->setObject(reinterpret_cast<MonitorElement*>(obj));
              me = reinterpret_cast<MonitorElement*>(obj);
              return me;
            }
          else
            {
              LOG4CPLUS_WARN(logger_, "Unable to Read " << Folder << "/" << mo->getFullName() << " object");
            }

        }

    }
  else   /// Look in memory MEs list
    {

      std::map<std::string, ME_List >::iterator itr;
      if (MEs.size() != 0 && ((itr = MEs.find(Folder)) != MEs.end()))
        {
          ME_List& MElist = MEs[Folder];
          if (isMEvalid(MElist, name, mo))
            {
              if (mo) me = mo->getObject();
            }
        }

    }

  return me;

}

/**
 * Get list of ME folders in container (EMU, DDUs, CSCs)
 */
std::vector<std::string> EmuPlotter::getListOfFolders(std::string filter, TDirectory* dir)
{
  std::vector<std::string> dirlist;

  if (dir)
    {

      TList* keyslist = dir->GetListOfKeys();
      TIter next(keyslist);
      while (TObject* key = next())
        {
          if (key->IsA()->InheritsFrom( "TKey" ))
            {
              TObject* obj = (dynamic_cast<TKey*>(key))->ReadObj();
              if  (obj->IsA()->InheritsFrom( "TDirectory" ))
                {
                  std::string name = obj->GetName();
                  if (name.find(filter) != std::string::npos)
                    {
                      dirlist.push_back(name);
                    }
                }
              delete obj;
            }
        }

    }
  else
    {

      std::map<std::string, ME_List >::iterator itr;
      for (itr = MEs.begin(); itr != MEs.end(); ++itr)
        {
          if (itr->first.find(filter) != std::string::npos)
            {
              dirlist.push_back(itr->first);
            }
        }

    }

  return dirlist;

}

/**
 * Perform DQM quaility checks and generate report
 */
int EmuPlotter::generateReport(std::string rootfile, std::string path, std::string runname)
{

  gSystem->Load("libHistPainter");

  TFile* rootsrc = TFile::Open( rootfile.c_str());

  if (!rootsrc)
    {
      LOG4CPLUS_ERROR (logger_, "Unable to open " << rootfile.c_str());
      return -1;
    }

  if (!rootsrc->cd("DQMData"))
    {
      LOG4CPLUS_ERROR (logger_, "No histos folder in file");
      return -1;
    }

  //  loadKillBitsConfig(xmlKillBitsCfgFile);

  std::ofstream tree_items;
  std::ofstream csc_list;

  TString command = Form("mkdir -p %s",path.c_str());
  gSystem->Exec(command.Data());

  std::string runNumber = runname;

  TDirectory *sourcedir = gDirectory;

  std::vector<std::string> EMU_folders;
  std::vector<std::string> DDU_folders=getListOfFolders("DDU", sourcedir);
  std::vector<std::string> CSC_folders=getListOfFolders("CSC", sourcedir);

  ReportEntry entry;

  std::string hname="";

  int CSCtype = 0;
  int CSCposition = 0;


  std::map<std::string, uint32_t>::iterator stats_itr;

  /*************************
   * DDUs checks and stats *
   *************************/
  uint32_t ddu_evt_cntr 	= 0;
  uint32_t ddu_cntr 		= 0;
  uint32_t ddu_avg_events 	= 0;
  uint32_t ddu_median_events 	= 0;

  std::map<std::string, uint32_t> ddu_stats;
  hname = "All_DDUs_in_Readout";
  MonitorElement* me = findME("EMU", hname,  sourcedir);

  if (me)
    {

      TH1D* h = reinterpret_cast<TH1D*>(me);

      TH1D* h_tmp = new TH1D("temp", "temp", 1000, h->GetMinimum(), h->GetMaximum()+1);

      std::vector<uint32_t> median_list;

      for (int i=int(h->GetXaxis()->GetXmin()); i<= MAX_DDU; i++) // Check only 36 DDUs
        {

          uint32_t cnt = uint32_t(h->GetBinContent(i));
          if (cnt>0)
            {
              ddu_cntr++;
              std::string dduName = Form("DDU_%02d", i);
              ddu_stats[dduName] = cnt;
              ddu_evt_cntr+=cnt;
              h_tmp->Fill(cnt);

              median_list.push_back(cnt);
            }

        }

      /// Calculate median for number of DDU events
      uint32_t list_size = median_list.size();
      if ( list_size > 0 )
        {
          uint32_t list_size = median_list.size();
          if ( list_size > 1 )
            {

              std::sort(median_list.begin(), median_list.end()); /// Sort list

              if ( (list_size % 2) == 0)
                {
                  ddu_median_events = (median_list[list_size/2] + median_list[(list_size/2)-1])/2;
                }
              else
                {
                  ddu_median_events = median_list[list_size/2];
                }

            }
          else
            {
              ddu_median_events = median_list[0];
            }
        }

      std::cout << "===> List size: " << list_size << " Median DDU events: " << ddu_median_events << std::endl;

      ddu_avg_events = (uint32_t)h_tmp->GetMean();
      delete h_tmp;

      if (ddu_cntr)
        {
          int hot_ddus = 0;
          int low_ddus = 0;
          dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d DDUs in Readout", ddu_cntr),NONE,"ALL_DDU_IN_READOUT"));
          dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("Total number of DDU events: %d ", ddu_evt_cntr),NONE,"ALL_DDU_EVENTS_COUNTER"));
//          ddu_avg_events=ddu_evt_cntr/ddu_cntr;
          dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("Average number of events per DDU: %d ", ddu_avg_events),NONE));
	  dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("Median number of events per DDU: %d ", ddu_median_events),NONE));
          if (ddu_avg_events >= 500)   // Detect different efficiency DDUs if average number of events is reasonable (>500)
            {
              for (stats_itr=ddu_stats.begin(); stats_itr != ddu_stats.end(); ++stats_itr)
                {
                  // double fract=((double)stats_itr->second)/ddu_avg_events;
	          double fract=((double)stats_itr->second)/ddu_median_events;
                  std::string dduName=stats_itr->first;
                  if (fract > 5.)
                    {
                      std::string diag=Form("Hot readout DDU: %d events, %f times more than median DDU events counter (median events=%d)",
                                            stats_itr->second, fract, ddu_median_events);
                      dqm_report.addEntry(dduName, entry.fillEntry(diag, SEVERE, "ALL_HOT_DDU_IN_READOUT"));
                      hot_ddus++;
                    }
                  else if (fract < 0.2)
                    {
                      std::string diag=Form("Low readout DDU: %d events, %f fraction of median DDU events counter (median events=%d)",
                                            stats_itr->second, fract, ddu_median_events);
                      dqm_report.addEntry(dduName, entry.fillEntry(diag, TOLERABLE, "ALL_LOW_DDU_IN_READOUT"));
                      low_ddus++;
                    }
                }
              if (hot_ddus) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d Hot DDUs", hot_ddus), SEVERE, "ALL_HOT_DDU_IN_READOUT"));
              if (low_ddus) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d Low ReadoutEfficiency DDUs", low_ddus), TOLERABLE, "ALL_LOW_DDU_IN_READOUT"));
            }

        }
      else
        {
          dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d DDUs with data", ddu_cntr),CRITICAL,"ALL_NO_DDU_IN_READOUT"));
          //      report["EMU Summary"].push_back(Form("%d DDUs with data [critical]", ddu_cntr));
        }

      if (me) delete me;
    }
  else
    {
      LOG4CPLUS_WARN(logger_,"Can not find " << hname);
    }


  /********************************
   * Check for DDU trailer errors *
   ********************************/
  uint32_t ddu_with_errs = 0;
  uint32_t ddu_err_cntr = 0;
  std::map<std::string, uint32_t> ddu_err_stats;
  hname = "All_DDUs_Trailer_Errors";
  me = findME("EMU", hname,  sourcedir);

  if (me)
    {
      TH2D* h = reinterpret_cast<TH2D*>(me);
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
        {
          uint32_t cnt = uint32_t(h->GetBinContent(i,2));
          if (cnt>0)
            {
              ddu_with_errs++;
              std::string dduName = Form("DDU_%02d", i);
              ddu_err_stats[dduName] = cnt;
              ddu_err_cntr+=cnt;

            }
        }
      if (ddu_with_errs)
        {
          dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d DDUs with Trailer Error Status", ddu_with_errs),
                              NONE, "ALL_DDU_WITH_TRAILER_ERRORS"));
          dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("Total number of DDU events with Trailer Error Status: %d ", ddu_err_cntr),
                              NONE, "ALL_DDU_WITH_TRAILER_ERRORS"));
          if (ddu_avg_events >= 500)   // Detect DDUs with Trailer Errors Status if average number of events is reasonable (>500)
            {
              for (stats_itr=ddu_err_stats.begin(); stats_itr != ddu_err_stats.end(); ++stats_itr)
                {
                  std::string dduName=stats_itr->first;
                  uint32_t events = ddu_stats[dduName];
                  double fract=(((double)stats_itr->second)/events)*100;
                  DQM_SEVERITY severity=NONE;
                  if (fract >= 50.) severity=SEVERE;
                  else if (fract >20.) severity=TOLERABLE;
                  else if (fract > 1.) severity=MINOR;
                  std::string diag=Form("DDU Trailer Error Status: %d events, (%f%%)",
                                        stats_itr->second, fract);
                  dqm_report.addEntry(dduName, entry.fillEntry(diag, severity, "DDU_WITH_TRAILER_ERRORS"));
                }
            }

        }
      if (me) delete me;

    }
  else
    {
      LOG4CPLUS_WARN(logger_,"Can not find " << hname);
    }

  /*******************************
   * Check for DDU Format errors *
   *******************************/
  uint32_t ddu_with_fmt_errs = 0;
  uint32_t ddu_fmt_err_cntr = 0;
  std::map<std::string, uint32_t> ddu_fmt_err_stats;
  hname = "All_DDUs_Format_Errors";
  me = findME("EMU", hname,  sourcedir);

  if (me)
    {
      TH2D* h = reinterpret_cast<TH2D*>(me);
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
        {
          uint32_t cnt = uint32_t(h->GetBinContent(i,2));
          if (cnt>0)
            {
              ddu_with_fmt_errs++;
              std::string dduName = Form("DDU_%02d", i);
              ddu_fmt_err_stats[dduName] = cnt;
              ddu_fmt_err_cntr+=cnt;

            }
        }
      if (ddu_with_fmt_errs)
        {
          dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d DDUs with detected Format Errors", ddu_with_fmt_errs), NONE,"ALL_DDU_WITH_FORMAT_ERRORS"));
          dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("Total number of DDU events with detected Format Errors: %d ", ddu_fmt_err_cntr), NONE,"ALL_DDU_WITH_FORMAT_ERRORS"));
          if (ddu_avg_events >= 500)   // Detect DDUs with Format Errors if average number of events is reasonable (>500)
            {
              for (stats_itr=ddu_fmt_err_stats.begin(); stats_itr != ddu_fmt_err_stats.end(); ++stats_itr)
                {
                  std::string dduName=stats_itr->first;
                  uint32_t events = ddu_stats[dduName];
                  double fract=(((double)stats_itr->second)/events)*100;
                  DQM_SEVERITY severity=NONE;
                  if (fract >= 20.) severity=CRITICAL;
                  else if (fract >= 10.) severity=SEVERE;
                  else if (fract >5.) severity=TOLERABLE;
                  else if (fract >0.5) severity=MINOR;
                  std::string diag=Form("DDU Detected Format Errors: %d events, (%f%%)",
                                        stats_itr->second, fract);
                  dqm_report.addEntry(dduName, entry.fillEntry(diag, severity,"DDU_WITH_FORMAT_ERRORS"));
                }
            }

        }
      if (me) delete me;
    }
  else
    {
      LOG4CPLUS_WARN(logger_,"Can not find " << hname);
    }
  
  /*****************************
   * Check for DDU Live Inputs *
   *****************************/
  uint32_t ddu_live_inputs = 0;
  std::map<std::string, uint32_t> ddu_live_inp_stats;
  hname =  "All_DDUs_Live_Inputs";
  me = findME("EMU", hname,  sourcedir);

  if (me)
    {
      TH2D* h = reinterpret_cast<TH2D*>(me);
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
        for (int j=int(h->GetYaxis()->GetXmin()); j<= int(h->GetYaxis()->GetXmax()); j++)
          {
            uint32_t cnt = uint32_t(h->GetBinContent(i,j));
            if (cnt>0)
              {
                ddu_live_inputs++;
                std::string dduName = Form("DDU_%02d", i);
                ddu_live_inp_stats[dduName] |= (1<<(j-1));

              }

          }
      dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d DDU Live Inputs detected", ddu_live_inputs),NONE,"ALL_DDU_WITH_LIVE_INPUTS"));

      if (me) delete me;
    }
  else
    {
      LOG4CPLUS_WARN(logger_,"Can not find " << hname);
    }
   
  /**********************************
   * Check for DDU Inputs with Data *
   **********************************/
  uint32_t ddu_inp_w_data = 0;
  std::map<std::string, uint32_t> ddu_inp_data_stats;
  hname = "All_DDUs_Inputs_with_Data";
  me = findME("EMU", hname,  sourcedir);

  if (me)
    {
      TH2D* h = reinterpret_cast<TH2D*>(me);
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
        for (int j=int(h->GetYaxis()->GetXmin()); j<= int(h->GetYaxis()->GetXmax()); j++)
          {
            uint32_t cnt = uint32_t(h->GetBinContent(i,j));
            if (cnt>0)
              {
                ddu_inp_w_data++;
                std::string dduName = Form("DDU_%02d", i);
                ddu_inp_data_stats[dduName] |= (1<<(j-1));

              }

          }
      dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d DDU Inputs with Data detected", ddu_inp_w_data),NONE,"ALL_DDU_WITH_DATA"));
      if (ddu_avg_events >= 500)
        {
          for (stats_itr= ddu_inp_data_stats.begin(); stats_itr !=  ddu_inp_data_stats.end(); ++stats_itr)
            {
              std::string dduName=stats_itr->first;
              uint32_t live_inputs = ddu_live_inp_stats[dduName];
              uint32_t with_data = ddu_inp_data_stats[dduName];
              for (int i=0; i<16; i++)
                {
                  if ( ((live_inputs>>i) & 0x1) && !((with_data>>i) & 0x01))
                    {
                      std::string diag=Form("DDU Input #%d: No Data",i+1);
                      dqm_report.addEntry(dduName, entry.fillEntry(diag, MINOR, "DDU_NO_INPUT_DATA"));

                    }
                }
            }
        }
      if (me) delete me;
    }
  else
    {
      LOG4CPLUS_WARN(logger_,"Can not find " << hname);
    }



  /*************************************** 
   * Check for DDU Inputs in ERROR state *
   ***************************************/
  uint32_t ddu_inp_w_errors = 0;
  std::map<std::string, uint32_t> ddu_inp_w_errors_stats;
  std::map<std::string, std::vector<std::pair<uint32_t,double> > > ddu_inp_w_errors_ratio;
  hname = "All_DDUs_Inputs_Errors";
  me = findME("EMU", hname,  sourcedir);

  if (me)
    {
      TH2D* h = reinterpret_cast<TH2D*>(me);
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
        {
          double events = h->GetBinContent(i,1) + h->GetBinContent(i,2);
          for (int j=3; j<= int(h->GetYaxis()->GetXmax()); j++)
            {
              uint32_t cnt = uint32_t(h->GetBinContent(i,j));
              std::string dduName = Form("DDU_%02d", i);
              if (cnt>0)
                {
                  ddu_inp_w_errors++;
                  ddu_inp_w_errors_stats[dduName] |= (1<<(j-3));
                }
              if (events) ddu_inp_w_errors_ratio[dduName].push_back(std::make_pair(cnt, (cnt/events)*100.) );

            }
        }
      if (ddu_inp_w_errors)
        {
          dqm_report.addEntry("EMU Summary", 
		entry.fillEntry(Form("%d DDU Inputs in ERROR state detected on %d DDUs", ddu_inp_w_errors, ddu_inp_w_errors_stats.size()),NONE, "ALL_DDU_INPUT_IN_ERROR_STATE"));
          if (ddu_avg_events >= 500)   // Detect DDUs Inputs in ERROR state if average number of events is reasonable (>500)
            {
              for (stats_itr= ddu_inp_w_errors_stats.begin(); stats_itr !=  ddu_inp_w_errors_stats.end(); ++stats_itr)
                {
                  std::string dduName=stats_itr->first;
                  uint32_t err_inputs = ddu_inp_w_errors_stats[dduName];
                  for (int i=0; i<16; i++)
                    {
                      if ( (err_inputs>>i) & 0x1)
                        {
                          DQM_SEVERITY severity=NONE;
                          double fract = ddu_inp_w_errors_ratio[dduName][i].second;
                          if (fract >= 50.) severity=SEVERE;
                          else if (fract >20.) severity=TOLERABLE;
                          else if (fract > 1.) severity=MINOR;
                          std::string diag=Form("DDU Input #%d: detected ERROR state in %d events, (%f%%)",i+1,
                                                ddu_inp_w_errors_ratio[dduName][i].first, fract );
                          dqm_report.addEntry(dduName, entry.fillEntry(diag, severity, "DDU_INPUT_IN_ERROR_STATE" ));

                        }
                    }
                }
            }
        }
      if (me) delete me;

    }
  else
    {
      LOG4CPLUS_WARN(logger_,"Can not find " << hname);
    }

  /*******************************************
   * Check for DDU Inputs with WARNING state *
   *******************************************/
  uint32_t ddu_inp_w_warn = 0;
  std::map<std::string, uint32_t> ddu_inp_w_warn_stats;
  std::map<std::string, std::vector<std::pair<uint32_t,double> > > ddu_inp_w_warn_ratio;
  hname = "All_DDUs_Inputs_Warnings";
  me = findME("EMU", hname,  sourcedir);

  if (me)
    {
      TH2D* h = reinterpret_cast<TH2D*>(me);
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
        {
          double events = h->GetBinContent(i,1) + h->GetBinContent(i,2);
          for (int j=3; j<= int(h->GetYaxis()->GetXmax()); j++)
            {
              uint32_t cnt = uint32_t(h->GetBinContent(i,j));
              std::string dduName = Form("DDU_%02d", i);
              if (cnt>0)
                {
                  ddu_inp_w_warn++;
                  ddu_inp_w_warn_stats[dduName] |= (1<<(j-3));

                }
              if (events) ddu_inp_w_warn_ratio[dduName].push_back(std::make_pair(cnt, (cnt/events)*100.) );

            }
        }

      if (ddu_inp_w_warn)
        {
          dqm_report.addEntry("EMU Summary",
		entry.fillEntry(Form("%d DDU Inputs in WARNING state detected on %d DDUs", ddu_inp_w_warn, ddu_inp_w_warn_stats.size()), NONE, "ALL_DDU_INPUT_IN_WARNING_STATE"));
          if (ddu_avg_events >= 500)   // Detect DDUs Inputs in ERROR state if average number of events is reasonable (>500)
            {
              for (stats_itr= ddu_inp_w_warn_stats.begin(); stats_itr !=  ddu_inp_w_warn_stats.end(); ++stats_itr)
                {
                  std::string dduName=stats_itr->first;
                  uint32_t warn_inputs = ddu_inp_w_warn_stats[dduName];
                  for (int i=0; i<16; i++)
                    {
                      if ( (warn_inputs>>i) & 0x1)
                        {
                          std::string diag=Form("DDU Input #%d: detected WARNING state in %d events (%f%%)",i+1,
                                                ddu_inp_w_warn_ratio[dduName][i].first,
                                                ddu_inp_w_warn_ratio[dduName][i].second);
                          dqm_report.addEntry(dduName, entry.fillEntry(diag, MINOR, "DDU_INPUT_IN_WARNING_STATE"));

                        }
                    }
                }
            }
        }

      if (me) delete me;
    }
  else
    {
      LOG4CPLUS_WARN(logger_,"Can not find " << hname);
    }


  /*****************************
   * Chambers checks and stats *
   *****************************/
  uint32_t csc_evt_cntr = 0;
  uint32_t csc_cntr = 0;
  uint32_t csc_avg_events = 0;
  std::map<std::string, uint32_t> csc_stats;

  std::map<std::string, bool> deadALCT;
  std::map<std::string, bool> deadCLCT;

  hname = "CSC_Reporting";
  me = findME("EMU", hname,  sourcedir);

  if (me)
    {
      TH2D* h = reinterpret_cast<TH2D*>(me);
      int n_csc_types = int(h->GetYaxis()->GetXmax());
      std::vector<uint32_t> csc_type_stats(n_csc_types);
      std::vector<uint32_t> csc_type_avg_events(n_csc_types);
      std::vector<uint32_t> csc_type_cntr(n_csc_types);
      TH1D* h_tmp1 = new TH1D("temp1", "temp1", 1000, h->GetMinimum(), h->GetMaximum()+1);
      for (int j=int(h->GetYaxis()->GetXmax())-1; j>= int(h->GetYaxis()->GetXmin()); j--)
        {
          TH1D* h_tmp = new TH1D("temp", "temp", 1000, h->GetMinimum(), h->GetMaximum()+1);
          for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
            {
              std::string cscName = Form("%s/%02d", (emu::dqm::utils::getCSCTypeName(j)).c_str(), i);
              uint32_t cnt = uint32_t(h->GetBinContent(i, j+1));

              if (cnt>0)
                {
                  csc_type_stats[j] += cnt;
                  csc_type_cntr[j]++;
                  csc_stats[cscName] = cnt;
                  csc_evt_cntr+=cnt;
                  csc_cntr++;
                  h_tmp1->Fill(cnt);
                  h_tmp->Fill(cnt);

                  //  std::cout << cscName << ": #Events: " << cnt << std::endl;

                }
            }
          csc_type_avg_events[j] = (uint32_t)h_tmp->GetMean();
          delete h_tmp;
          std::string diag=Form("%s: Total #Events: %d, Average #Events per CSC: %d, Active CSCs: %d",
                                (emu::dqm::utils::getCSCTypeName(j)).c_str(),
                                csc_type_stats[j],
                                csc_type_avg_events[j],
                                csc_type_cntr[j] );
          dqm_report.addEntry("EMU Summary", entry.fillEntry(diag));

        }

      csc_avg_events = (uint32_t)h_tmp1->GetMean();
      delete h_tmp1;

      if (csc_cntr && csc_avg_events)
        {
          // csc_avg_events = csc_evt_cntr/csc_cntr;
          int hot_cscs = 0;
          int low_cscs = 0;
          dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs with data", csc_cntr)));
          dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("Total number of CSC events: %d ", csc_evt_cntr)));
          dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("Average number of events per CSC: %d", csc_avg_events)));


          for (int j=int(h->GetYaxis()->GetXmax())-1; j>= int(h->GetYaxis()->GetXmin()); j--)
            {

              bool isHotCSCPresent = false;  // Is there hot CSC present in ring, which could screw up other chamber efficiency

              for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
                {
                  std::string cscName = Form("%s/%02d", (emu::dqm::utils::getCSCTypeName(j)).c_str(), i);
                  if ( (csc_stats[cscName]>0) && (csc_type_avg_events[j]>300) )
                    {
                      double fract=((double)(csc_stats[cscName]))/csc_type_avg_events[j];
                      if (fract >= 10.)
                        {
                          std::string diag=Form("Hot chamber: %d events, %.f times more than %s type average events counter (avg events=%d)",
                                                csc_stats[cscName], fract,
                                                (emu::dqm::utils::getCSCTypeName(j)).c_str(),
                                                csc_type_avg_events[j]);
                          dqm_report.addEntry(cscName, entry.fillEntry(diag, CRITICAL, "CSC_HOT_CHAMBER"));
                          hot_cscs++;
                          isHotCSCPresent = true;
                        }
                      else if (csc_stats[cscName] >= 20*csc_avg_events)
                        {
                          std::string diag=Form("Hot chamber: %d events, %.f times more than system average events counter (avg events=%d)",
                                                csc_stats[cscName], csc_stats[cscName]/(1.*csc_avg_events),
                                                (int)csc_avg_events);
                          dqm_report.addEntry(cscName, entry.fillEntry(diag, CRITICAL, "CSC_HOT_CHAMBER"));
                          hot_cscs++;
                          isHotCSCPresent = true;
                        }

                    }
                }

              for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
                {
                  std::string cscName = Form("%s/%02d", (emu::dqm::utils::getCSCTypeName(j)).c_str(), i);
                  if ( (csc_stats[cscName]>0) && (csc_type_avg_events[j]>300) )
                    {
                      double fract=((double)(csc_stats[cscName]))/csc_type_avg_events[j];
                      double avg = round(100.*fract)/100.;
                      if ((avg < 0.05 ) && !isHotCSCPresent)
                        {
                          std::string diag=Form("Low efficiency chamber: %d events, %f fraction of %s type average events counter (avg events=%d)",
                                                csc_stats[cscName], fract,
                                                (emu::dqm::utils::getCSCTypeName(j)).c_str(),
                                                csc_type_avg_events[j]);
                          dqm_report.addEntry(cscName, entry.fillEntry(diag, TOLERABLE, "CSC_LOW_EFF_CHAMBER"));
                          low_cscs++;
                        }

                    }
                }



            }

          if (hot_cscs) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d Hot CSCs", hot_cscs), CRITICAL, "ALL_HOT_CHAMBERS"));
          if (low_cscs) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d Low Efficiency CSCs", low_cscs), TOLERABLE, "ALL_LOW_EFF_CHAMBERS"));
        }


      if (me) delete me;

    }
  else
    {
      LOG4CPLUS_WARN(logger_,"Can not find " << hname);
    }


  // == Check for chambers with Format Errors
  hname =  "DMB_Format_Errors_Fract";
  me = findME("EMU", hname,  sourcedir);
  MonitorElement* me2 = findME("EMU", "DMB_Format_Errors",  sourcedir);
  if (me && me2)
    {
      TH2D* h1 = reinterpret_cast<TH2D*>(me);
      TH2D* h2 = reinterpret_cast<TH2D*>(me2);
      int csc_cntr=0;
      uint32_t min_events=50;
      for (int i=int(h1->GetXaxis()->GetXmin()); i<= int(h1->GetXaxis()->GetXmax()); i++)
        for (int j=int(h1->GetYaxis()->GetXmin()); j <= int(h1->GetYaxis()->GetXmax()); j++)
          {
            double z = h1->GetBinContent(i, j);

            if (z>0) // If chamber has format errors
              {
                csc_cntr++;
                std::string cscTag(Form("CSC_%03d_%02d", i, j));
                std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
                uint32_t events =  uint32_t(h2->GetBinContent(i, j));

                float fract=z*100;
                DQM_SEVERITY severity=NONE;
                uint32_t csc_events = csc_stats[cscName];
                if (csc_events>min_events)
                  {
                    if (fract >= 80.) severity=CRITICAL;
                    else if (fract >= 20.) severity=SEVERE;
                    else if (fract > 5.) severity=TOLERABLE;
                    else if (fract > 0.5) severity=MINOR;
                  }
                std::string diag=Form("Format Errors: %d events (%.3f%%))",events, z*100);
                dqm_report.addEntry(cscName,entry.fillEntry(diag,severity,"CSC_WITH_FORMAT_ERRORS"));


                // --- Get Format Errors Details
                MonitorElement* me3 = findME(cscTag, "BinCheck_Errors_Frequency",  sourcedir);
                MonitorElement* me4 = findME(cscTag, "BinCheck_ErrorStat_Table",  sourcedir);
                if (me3 && me4)
                  {

                    TH2D* h3 = reinterpret_cast<TH2D*>(me3);
                    TH2D* h4 = reinterpret_cast<TH2D*>(me4);

                    for (int err=int(h3->GetYaxis()->GetXmin()); err <= int(h3->GetYaxis()->GetXmax()); err++)
                      {
                        double z = h3->GetBinContent(1, err);

                        if (z>0)
                          {
                            uint32_t events = uint32_t(h4->GetBinContent(1, err));
                            float fract=z*100;
                            DQM_SEVERITY severity=NONE;
                            if (csc_events>min_events)
                              {
                                if (fract >= 80.) severity=CRITICAL;
                                else if (fract >= 20.) severity=SEVERE;
                                else if (fract > 5.) severity=TOLERABLE;
                                else if (fract > 0.5) severity=MINOR;
                              }
                            std::string error_type = std::string(h3->GetYaxis()->GetBinLabel(err));
                            std::string diag=std::string(Form("\tFormat Errors: %s %d events (%.3f%%)",error_type.c_str(), events, z*100));
                            // LOG4CPLUS_WARN(logger_, cscTag << ": "<< diag);
                            dqm_report.addEntry(cscName, entry.fillEntry(diag,severity,"CSC_WITH_FORMAT_ERRORS"));
                          }
                      }
                  }
                // ---

              }
          }
      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs with Format Errors", csc_cntr),NONE,"ALL_CHAMBERS_WITH_FORMAT_ERRORS"));

      if (me) delete me;
      if (me2) delete me2;
    }
  else
    {
      LOG4CPLUS_WARN(logger_,"Can not find " << hname);
    }


  // == Check for chambers with DMB-Input FIFO Full
  me = findME("EMU", "DMB_input_fifo_full_Fract",  sourcedir);
  me2 = findME("EMU", "DMB_input_fifo_full",  sourcedir);
  if (me && me2)
    {
      TH2D* h1 = reinterpret_cast<TH2D*>(me);
      TH2D* h2 = reinterpret_cast<TH2D*>(me2);
      int csc_cntr=0;
      uint32_t min_events=50;
      for (int i=int(h1->GetXaxis()->GetXmin()); i<= int(h1->GetXaxis()->GetXmax()); i++)
        for (int j=int(h1->GetYaxis()->GetXmin()); j <= int(h1->GetYaxis()->GetXmax()); j++)
          {
            double z = h1->GetBinContent(i, j);

            if (z>0) // If chamber has DMB Input FIFO Full
              {
                csc_cntr++;
                std::string cscTag(Form("CSC_%03d_%02d", i, j));
                std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
                uint32_t events =  uint32_t(h2->GetBinContent(i, j));
                uint32_t csc_events = csc_stats[cscName];

                float fract=round(z*100);
                DQM_SEVERITY severity=MINOR;
                if (csc_events>min_events)
                  {
                    if (fract >= 80.) severity=CRITICAL;
                    else if (fract >= 10.) severity=SEVERE;
                    else if (fract > 1.) severity=TOLERABLE;
                  }
                std::string diag=Form("DMB-Input FIFO Full: %d events (%.3f%%)",events, z*100);
                dqm_report.addEntry(cscName, entry.fillEntry(diag, severity,"CSC_WITH_INPUT_FIFO_FULL"));


                // --- Get Format Errors Details
                MonitorElement* me3 = findME(cscTag, "BinCheck_DataFlow_Problems_Frequency",  sourcedir);
                MonitorElement* me4 = findME(cscTag, "BinCheck_DataFlow_Problems_Table",  sourcedir);
                if (me3 && me4)
                  {

                    TH2D* h3 = reinterpret_cast<TH2D*>(me3);
                    TH2D* h4 = reinterpret_cast<TH2D*>(me4);

                    for (int err=int(h3->GetYaxis()->GetXmin()); err <= 7; err++)
                      {
                        double z = h3->GetBinContent(1, err);

                        if (z>0)
                          {
                            uint32_t events = uint32_t(h4->GetBinContent(1, err));
                            float fract=round(z*100);
                            DQM_SEVERITY severity=MINOR;
                            if (csc_events>min_events)
                              {
                                if (fract >= 80.) severity=CRITICAL;
                                else if (fract >= 10.) severity=SEVERE;
                                else if (fract > 1.) severity=TOLERABLE;
                              }
                            std::string error_type = std::string(h3->GetYaxis()->GetBinLabel(err));
                            std::string diag=std::string(Form("DMB-Input FIFO Full: %s %d events (%.3f%%)",error_type.c_str(), events, z*100));
                            // LOG4CPLUS_WARN(logger_, cscTag << ": "<< diag);
                            dqm_report.addEntry(cscName, entry.fillEntry(diag, severity, "CSC_WITH_INPUT_FIFO_FULL"));
                          }
                      }
                  }
                // ---

              }
          }
      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs with DMB-Input FIFO Full", csc_cntr),NONE,"ALL_CHAMBERS_WITH_INPUT_FIFO_FULL"));
      if (me) delete me;
      if (me2) delete me2;

    }

  // == Check for chambers with DMB-Input Timeout
  me = findME("EMU", "DMB_input_timeout_Fract",  sourcedir);
  me2 = findME("EMU", "DMB_input_timeout",  sourcedir);
  if (me && me2)
    {
      TH2D* h1 = reinterpret_cast<TH2D*>(me);
      TH2D* h2 = reinterpret_cast<TH2D*>(me2);
      int csc_cntr=0;
      uint32_t min_events=50;
      for (int i=int(h1->GetXaxis()->GetXmin()); i<= int(h1->GetXaxis()->GetXmax()); i++)
        for (int j=int(h1->GetYaxis()->GetXmin()); j <= int(h1->GetYaxis()->GetXmax()); j++)
          {
            double z = h1->GetBinContent(i, j);

            if (z>0) // If chamber has DMB Input Timeout
              {
                csc_cntr++;
                std::string cscTag(Form("CSC_%03d_%02d", i, j));
                std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
                uint32_t events =  uint32_t(h2->GetBinContent(i, j));
                uint32_t csc_events = csc_stats[cscName];

                float fract=round(z*100);
                DQM_SEVERITY severity=MINOR;
                if (csc_events>min_events)
                  {
                    if (fract >= 80.) severity=CRITICAL;
                    else if (fract >= 10.) severity=SEVERE;
                    else if (fract > 1.) severity=TOLERABLE;
                  }
                std::string diag=Form("DMB-Input Timeout: %d events (%.3f%%)",events, z*100);
                dqm_report.addEntry(cscName, entry.fillEntry(diag,severity, "CSC_WITH_INPUT_TIMEOUT"));


                // --- Get Format Errors Details
                MonitorElement* me3 = findME(cscTag, "BinCheck_DataFlow_Problems_Frequency",  sourcedir);
                MonitorElement* me4 = findME(cscTag, "BinCheck_DataFlow_Problems_Table",  sourcedir);
                if (me3 && me4)
                  {

                    TH2D* h3 = reinterpret_cast<TH2D*>(me3);
                    TH2D* h4 = reinterpret_cast<TH2D*>(me4);

                    for (int err=8; err<int(h3->GetYaxis()->GetXmax())-2; err++)
                      {
                        double z = h3->GetBinContent(1, err);

                        if (z>0)
                          {
                            uint32_t events = uint32_t(h4->GetBinContent(1, err));
                            float fract=round(z*100);
                            DQM_SEVERITY severity=MINOR;
                            if (csc_events>min_events)
                              {
                                if (fract >= 80.) severity=CRITICAL;
                                else if (fract >= 10.) severity=SEVERE;
                                else if (fract > 1.) severity=TOLERABLE;
                              }
                            std::string error_type = std::string(h3->GetYaxis()->GetBinLabel(err));
                            std::string diag=std::string(Form("DMB-Input Timeout: %s %d events (%.3f%%)",error_type.c_str(), events, z*100));
                            // LOG4CPLUS_WARN(logger_, cscTag << ": "<< diag);
                            dqm_report.addEntry(cscName, entry.fillEntry(diag, severity, "CSC_WITH_INPUT_TIMEOUT" ));
                          }
                      }
                  }
                // ---

              }
          }
      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs with DMB-Input Timeouts", csc_cntr), NONE, "ALL_CHAMBERS_WITH_INPUT_TIMEOUT"));
      if (me) delete me;
      if (me2) delete me2;

    }

  // == Check for missing ALCT data blocks
  me = findME("EMU", "DMB_wo_ALCT_Fract",  sourcedir);
  if (me)
    {
      TH2D* h = reinterpret_cast<TH2D*>(me);
      int csc_cntr=0;
      uint32_t min_events=200;
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
        for (int j=int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++)
          {
            double z = h->GetBinContent(i, j);
            if (round(z*100)>95.)
              {
                csc_cntr++;
                std::string cscTag(Form("CSC_%03d_%02d", i, j));
                std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
                uint32_t csc_events = csc_stats[cscName];
                float fract=z*100;
                deadALCT[cscName]=false;
                if (csc_events>min_events)
                  {
                    deadALCT[cscName] = true;

                    std::string diag=Form("No ALCT Data: %.1f%%",fract);
                    dqm_report.addEntry(cscName, entry.fillEntry(diag, CRITICAL, "CSC_WITHOUT_ALCT"));
                  }
                else
                  {
                    if ((csc_events > 50) && (z == 1.))
                      {
                        std::string diag=Form("No ALCT Data (low stats): %.1f%%",fract);

                        dqm_report.addEntry(cscName, entry.fillEntry(diag, TOLERABLE, "CSC_WITHOUT_ALCT"));
                      }
                  }
              }

          }
      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs without ALCT data", csc_cntr), NONE, "ALL_CHAMBERS_WITHOUT_ALCT"));
      if (me) delete me;
    }


  // == Check for missing CLCT data blocks
  me = findME("EMU", "DMB_wo_CLCT_Fract",  sourcedir);
  if (me)
    {
      TH2D* h = reinterpret_cast<TH2D*>(me);
      int csc_cntr=0;
      uint32_t min_events=200;
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
        for (int j=int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++)
          {
            double z = h->GetBinContent(i, j);
            if (round(z*100)>95.)
              {
                csc_cntr++;
                std::string cscTag(Form("CSC_%03d_%02d", i, j));
                std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
                uint32_t csc_events = csc_stats[cscName];
                float fract=z*100;
                if (csc_events>min_events)
                  {
                    deadCLCT[cscName] = true;

                    std::string diag=Form("No CLCT Data: %.1f%%",fract);
                    dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL, "CSC_WITHOUT_CLCT"));
                  }
                else
                  {
                    if ((csc_events > 50) && (z == 1.))
                      {
                        std::string diag=Form("No CLCT Data (low stats): %.1f%%",fract);

                        dqm_report.addEntry(cscName, entry.fillEntry(diag,TOLERABLE, "CSC_WITHOUT_CLCT"));
                      }
                  }
              }

          }
      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs without CLCT data", csc_cntr), NONE, "ALL_CHAMBERS_WITHOUT_CLCT"));
      if (me) delete me;
    }

  // == Check for missing CFEB data blocks
  me = findME("EMU", "DMB_wo_CFEB_Fract",  sourcedir);
  if (me)
    {
      TH2D* h = reinterpret_cast<TH2D*>(me);
      int csc_cntr=0;
      uint32_t min_events=200;
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
        for (int j=int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++)
          {
            double z = h->GetBinContent(i, j);
            if (round(z*100)>95.)
              {
                csc_cntr++;
                std::string cscTag(Form("CSC_%03d_%02d", i, j));
                std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
                uint32_t csc_events = csc_stats[cscName];
                float fract=z*100;
                if (csc_events>min_events)
                  {
                    std::string diag=Form("No CFEB Data: %.1f%%",fract);

                    dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL, "CSC_WITHOUT_CFEB"));
                  }
                else
                  {
                    if ((csc_events > 50) && (z == 1.))
                      {
                        std::string diag=Form("No CFEB Data (low stats): %.1f%%",fract);

                        dqm_report.addEntry(cscName, entry.fillEntry(diag,TOLERABLE, "CSC_WITHOUT_CFEB"));
                      }
                  }
              }

          }
      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs without CFEB data", csc_cntr),NONE,"ALL_CHAMBERS_WITHOUT_CFEB"));
      if (me) delete me;
    }

  // == Check for missing ALCT Timing issues
  me = findME("EMU", "CSC_ALCT0_BXN_rms",  sourcedir);
  if (me)
    {
      TH2D* h = reinterpret_cast<TH2D*>(me);
      int csc_cntr=0;
      uint32_t min_events=200;
      double rms_limit = 1.81;
      for (int j=int(h->GetYaxis()->GetXmax())-1; j>= int(h->GetYaxis()->GetXmin()); j--)
        for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
          {
            std::string cscName = Form("%s/%02d", (emu::dqm::utils::getCSCTypeName(j)).c_str(), i);
            double limit = rms_limit;
            if (emu::dqm::utils::isME42(cscName)) limit = rms_limit + 0.2; // Handle ME42 chambers, which have different timing pattern
            double z = h->GetBinContent(i, j+1);
            double avg = round(z*10.)/10.;
            if (avg > limit)
              {
                csc_cntr++;
                uint32_t csc_events = csc_stats[cscName];
                if (csc_events>min_events)
                  {
                    std::string diag=Form("ALCT Timing problem (ALCT0 BXN - L1A BXN) RMS: %.3f ( >%.2f )",z, limit);

                    dqm_report.addEntry(cscName, entry.fillEntry(diag,SEVERE, "CSC_ALCT_TIMING"));
                  }
              }

          }
      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs with ALCT Timing Problems", csc_cntr),NONE,"ALL_CHAMBERS_ALCT_TIMING"));
      if (me) delete me;
    }


  // == Check for missing CLCT Timing issues
  me = findME("EMU", "CSC_CLCT0_BXN_rms",  sourcedir);
  if (me)
    {
      TH2D* h = reinterpret_cast<TH2D*>(me);
      int csc_cntr=0;
      uint32_t min_events=200;
      double rms_limit = 2.11;
      for (int j=int(h->GetYaxis()->GetXmax())-1; j>= int(h->GetYaxis()->GetXmin()); j--)
        for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
          {
            std::string cscName = Form("%s/%02d", (emu::dqm::utils::getCSCTypeName(j)).c_str(), i);
            if (!deadALCT[cscName])   ///* Don't check CLCT Timing if ALCT is dead on this chamber
              {
                double limit = rms_limit;
                if (emu::dqm::utils::isME42(cscName)) limit = rms_limit + 1.0; // Handle ME42 chambers, which have different timing pattern
                double z = h->GetBinContent(i, j+1);
                double avg = round(z*10.)/10.;
                if (avg > limit)
                  {
                    csc_cntr++;
                    uint32_t csc_events = csc_stats[cscName];
                    if (csc_events>min_events)
                      {
                        std::string diag=Form("CLCT Timing problem (CLCT0 BXN - L1A BXN) RMS: %.3f ( >%.2f )",z, limit);

                        dqm_report.addEntry(cscName, entry.fillEntry(diag,SEVERE, "CSC_CLCT_TIMING"));
                      }
                  }
              }

          }
      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs with CLCT Timing Problems", csc_cntr),NONE,"ALL_CHAMBERS_CLCT_TIMING"));
      if (me) delete me;
    }

  // == Check for chambers with L1A out of sync
  me = findME("EMU", "DMB_L1A_out_of_sync_Fract",  sourcedir);
  me2 = findME("EMU", "DMB_L1A_out_of_sync",  sourcedir);
  if (me)
    {
      TH2D* h = reinterpret_cast<TH2D*>(me);
      TH2D* h2 = reinterpret_cast<TH2D*>(me2);
      int csc_cntr=0;
      uint32_t min_events=50;
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
        for (int j=int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++)
          {
            double z = h->GetBinContent(i, j);
            if (z>0)
              {
                csc_cntr++;
                uint32_t events = uint32_t(h2->GetBinContent(i, j));
                std::string cscTag(Form("CSC_%03d_%02d", i, j));
                std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
                uint32_t csc_events = csc_stats[cscName];
                float fract=round(z*100);
                DQM_SEVERITY severity=MINOR;
                if (csc_events>min_events)
                  {
                    if (fract >= 80.) severity=CRITICAL;
                    else if (fract >= 10.) severity=SEVERE;
                    else if (fract > 1.) severity=TOLERABLE;
                  }

                std::string diag=Form("L1A out of sync: %d events (%.3f%%)",events, z*100);

                dqm_report.addEntry(cscName, entry.fillEntry(diag,severity, "CSC_WITH_L1A_OUT_OF_SYNC"));

                MonitorElement* me3 = 0;

                // Prepare list of L1A histos
                std::vector<std::pair<std::string, std::string> > l1a_histos;
                for (int icfeb=0; icfeb<5; icfeb++)
                  {
                    l1a_histos.push_back(make_pair( Form("CFEB%d", icfeb+1 ), Form("CFEB%d_DMB_L1A_diff", icfeb) ));
                  }
                l1a_histos.push_back(make_pair( "ALCT","ALCT_DMB_L1A_diff") );
                l1a_histos.push_back(make_pair( "CLCT","CLCT_DMB_L1A_diff") );
                l1a_histos.push_back(make_pair( "DDU","DMB_DDU_L1A_diff") );

                // -- Find which board has L1A out of sync
                for (uint32_t k = 0; k < l1a_histos.size(); k++)
                  {
                    me3 = findME(cscTag, l1a_histos[k].second,  sourcedir);
                    if (me3)
                      {
                        TH1D* h3 = reinterpret_cast<TH1D*>(me3);
                        if (h3->GetMean() != 0)
                          {
                            diag=Form("L1A out of sync: %s",(l1a_histos[k].first).c_str());

                            dqm_report.addEntry(cscName, entry.fillEntry(diag, severity, "CSC_WITH_L1A_OUT_OF_SYNC"));
                          }
                      }

                  }

              }

          }

      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs with L1A out of sync", csc_cntr),NONE,"ALL_CHAMBERS_WITH_L1A_OUT_OF_SYNC"));
      if (me) delete me;
      if (me2) delete me2;

    }

  /**********************
   * Per Chamber checks *
   **********************/
  for (uint32_t i=0; i<CSC_folders.size(); i++)
    {
      int crate=0, slot =0;
      uint32_t min_events = 300;
      std::string cscName = getCSCName(CSC_folders[i], crate, slot, CSCtype, CSCposition);
      int nCFEBs = emu::dqm::utils::getNumCFEBs(cscName);
      bool ME11 = emu::dqm::utils::isME11(cscName);
      // bool ME42 = emu::dqm::utils::isME42(cscName);
      bool isBeam = false; // Assume that this is Cosmic run and not Collisions 

      // int nStrips = emu::dqm::utils::getNumStrips(cscName);
      int nWireGroups = emu::dqm::utils::getNumWireGroups(cscName);
      std::vector<int> deadCFEBs(5,0);
      std::vector<int> lowEffCFEBs(5,0);
      std::vector<int> badCFEBs(5,0);
      int nbadCFEBs = 0;

      if (csc_stats[cscName] < min_events)
        {
          std::string diag=Form("Not enough events for per-chamber checks (%d events, needs > %d)", csc_stats[cscName], min_events);
          dqm_report.addEntry(cscName, entry.fillEntry(diag,NONE));
          continue;
        }

      std::vector< std::pair<int,int> > hvSegMap = emu::dqm::utils::getHVSegmentsMap(cscName);


      // -- CFEBs DAV checks
      me = findME(CSC_folders[i], "Actual_DMB_CFEB_DAV_Frequency",  sourcedir);
      me2 = findME(CSC_folders[i], "Actual_DMB_CFEB_DAV_Rate",  sourcedir);
      if (me && me2)
        {
          TH1D* h = reinterpret_cast<TH1D*>(me);
          // TH1D* h1 = reinterpret_cast<TH1D*>(me2);

          vector<double> cfebs;
          if  ( h->GetEntries() > min_events)
            {

              double avg_cfeb_occup = 0;
              TH1D* h_tmp = new TH1D("temp", "temp", 100, h->GetMinimum(), h->GetMaximum()+1);
              double val=0;


              for (int icfeb=0; icfeb< nCFEBs; icfeb++)
                {
                  val = round(h->GetBinContent(icfeb+1));
                  cfebs.push_back(val);
                  if (!ME11 || (ME11 && icfeb!=4) ) h_tmp->Fill(val);

                }
              avg_cfeb_occup = round(h_tmp->GetMean());

              delete h_tmp;


              // Try to detect Beam run using ME11 CFEB DAV pattern (CFEB5 should ~ 2 times higher than average
              if (ME11 && ((avg_cfeb_occup*2.5)<cfebs[4])) isBeam = true;


              for (int icfeb=0; icfeb< nCFEBs; icfeb++)
                {
                  double z=cfebs[icfeb];

                  if (z==0)
                    {
                      std::string diag=Form("CFEB Dead: CFEB%d DAV %.3f%%", icfeb+1, z);
                      deadCFEBs[icfeb]=1;    // Mark this CFEB as dead
                      dqm_report.addEntry(cscName, entry.fillEntry(diag,SEVERE, "CSC_WITH_LOW_CFEB_DAV_EFF"));
                      badCFEBs[icfeb]=1;
                      nbadCFEBs ++;
                    }

                  else
                    {
                      if (!isBeam || (isBeam && !ME11 && icfeb!=4)) // Usual occupancy check algorithm for cosmic runs
                        {
                          if ((round(z) < 5) || (round(z) < 0.4*avg_cfeb_occup))
                            {
                              std::string diag=Form("CFEB Low efficiency: CFEB%d DAV %.3f%%", icfeb+1, z);
                              lowEffCFEBs[icfeb]=1;
                              dqm_report.addEntry(cscName, entry.fillEntry(diag,TOLERABLE, "CSC_WITH_LOW_CFEB_DAV_EFF"));
                              badCFEBs[icfeb]=1;
                              nbadCFEBs ++;
                            }
                        }
                      else if (ME11 && icfeb == 4)
                        {
                          // if (ME11 && isBeam) std::cout << cscName << " Beam Run detected" << std::endl;
                          if (round(z) < avg_cfeb_occup*2)
                            {
                              std::string diag=Form("CFEB Low efficiency: CFEB%d DAV %.3f%%", icfeb+1, z);
                              lowEffCFEBs[icfeb]=1;
                              dqm_report.addEntry(cscName, entry.fillEntry(diag,TOLERABLE, "CSC_WITH_LOW_CFEB_DAV_EFF"));
                              badCFEBs[icfeb]=1;
                              nbadCFEBs ++;
                            }
                        }
                    }

                }
            }

          if (me) delete me;
          if (me2) delete me2;


        }

      int nActiveCFEBs = nCFEBs-nbadCFEBs;


      std::vector<bool> loweredHVsegment(6,false);
      if  (nActiveCFEBs > 0)
        {
          // Expecting active CFEBs

          double avgSCAlayer=0;
          std::vector<double> layerSCAsums;
          // -- CFEB SCA Occupancies Checks
          for (int ilayer=1; ilayer<=6; ilayer++)
            {
              std::string name = Form("CFEB_ActiveStrips_Ly%d",ilayer);

              me = findME(CSC_folders[i], name , sourcedir);
              if (me)
                {
                  TH1D* h = reinterpret_cast<TH1D*>(me);
                  int nentries = (int)h->GetEntries();
                  double allSCAsum = h->Integral();
                  layerSCAsums.push_back(allSCAsum);
                  avgSCAlayer += allSCAsum;
                  std::vector<double> SCAsums;
                  SCAsums.clear();
                  int noSCAs = 0;
                  double low_sca_thresh = 0.2;
                  double high_sca_thresh = 2.5;

                  // std::cout << cscName << " ly " << ilayer << ", sca sum: " << allSCAsum;
                  if ( nentries >= (10*16*nActiveCFEBs) )
                    {
                      // -- Check for dead SCAs CFEBs
                      for (int icfeb=0; icfeb < nCFEBs; icfeb++)
                        {

                          if (badCFEBs[icfeb] == 1)
                            {
                              SCAsums.push_back(0.0);
                              continue; // Skip already known bad CFEBs
                            }

                          double cfeb_sca_sum = h->Integral(icfeb*16+1, (icfeb+1)*16);
                          SCAsums.push_back(cfeb_sca_sum);
                          // std::cout << ", " << cfeb_sca_sum;


                          if (cfeb_sca_sum == 0)
                            {
                              std::string diag=Form("CFEB No SCA Data: CFEB%d Layer%d", icfeb+1, ilayer);
                              dqm_report.addEntry(cscName, entry.fillEntry(diag,SEVERE,"CSC_CFEB_NO_SCA_DATA"));
                              noSCAs++;
                              // std::cout << cscName << " " << diag << std::endl;
                            }
                        }
                      // std::cout << ", " << allSCAsum/(nActiveCFEBs-noSCAs) << std::endl;
                    }
                  else
                    {
                      if ( (nentries == 0) && ((int)csc_stats[cscName] >= (10*16*nActiveCFEBs)) && !ME11 )
                        for (int icfeb=0; icfeb < nCFEBs; icfeb++)
                          {
                            std::string diag=Form("CFEB No SCA Data: CFEB%d Layer%d", icfeb+1, ilayer);
                            dqm_report.addEntry(cscName, entry.fillEntry(diag,SEVERE,"CSC_CFEB_NO_SCA_DATA"));
                          }
                    }

                  if ( nentries >= (25*16*nActiveCFEBs) )
                    {
                      if (nActiveCFEBs - noSCAs > 0)
                        {
                          // Check that still active CFEBs present
                          double avg_sca_occupancy = allSCAsum/(nActiveCFEBs-noSCAs);
                          double avg_sca_ch_occupancy = (avg_sca_occupancy/(16*nentries))*100;
                          // std::cout << cscName << "> avg SCA: " << avg_sca_occupancy << ", ";
                          for (int icfeb=0; icfeb < nCFEBs; icfeb++)
                            {
                              //      std::cout << "CFEB"<< (icfeb+1) << ": " << (double(SCAsums[icfeb])) << ", ";
                              // Avg. strip SCA occupancy > 5.
                              bool isLowEff = false;
                              double cfeb_sca_sum = SCAsums[icfeb];
                              if ( (icfeb == 4) && ME11) cfeb_sca_sum/=2;
                              if (cfeb_sca_sum)
                                {

                                  if ( (cfeb_sca_sum < low_sca_thresh*avg_sca_occupancy) && (lowEffCFEBs[icfeb] != 1))
                                    {
                                      if (cfeb_sca_sum < 0.01*avg_sca_occupancy)
                                        {
                                          std::string diag=Form("CFEB No SCA Data: CFEB%d Layer%d", icfeb+1, ilayer);
                                          dqm_report.addEntry(cscName, entry.fillEntry(diag,SEVERE,"CSC_CFEB_SCA_LOW_EFF"));
                                        }
                                      else
                                        {
                                          std::string diag=Form("CFEB Low SCA Efficiency: CFEB%d Layer%d (%.3f%% < %.1f%% from average)", icfeb+1, ilayer,
                                                                (cfeb_sca_sum/avg_sca_occupancy)*100., low_sca_thresh*100 );
                                          dqm_report.addEntry(cscName, entry.fillEntry(diag, TOLERABLE, "CSC_CFEB_SCA_LOW_EFF"));
                                        }
                                      // std::cout << cscName << " "  << diag << std::endl;
                                      isLowEff = true;
                                    }



                                  if ( cfeb_sca_sum >= high_sca_thresh*avg_sca_occupancy )
                                    {
                                      std::string diag=Form("CFEB Noisy/Hot CFEB SCAs: CFEB%d Layer%d (%.1f > %.1f times from average)", icfeb+1, ilayer,
                                                            cfeb_sca_sum/avg_sca_occupancy, high_sca_thresh);
                                      dqm_report.addEntry(cscName, entry.fillEntry(diag,SEVERE, "CSC_CFEB_SCA_NOISY"));
                                      // std::cout << cscName << " " << diag << std::endl;
                                    }

                                  double avg_strip_occupancy = cfeb_sca_sum/16;
                                  for (int ch=1; ch <=16; ch++)
                                    {
                                      double ch_occup = h->GetBinContent(ch+icfeb*16);
                                      double ch_val = 100*ch_occup/nentries;
                                      double ch_ratio = ch_occup / avg_strip_occupancy;

                                      if ( (icfeb == 4) && ME11) ch_val/=2;

                                      if (ch_val > high_sca_thresh*avg_sca_ch_occupancy )
                                        // if ((avg_strip_occupancy > 40) && (ch_ratio > high_sca_thresh*avg_strip_occupancy))
                                        {
                                          std::string diag = Form("CFEB Hot/Noisy SCA channel: CFEB%d Layer%d Ch#%d (occupancy %.1f times > average)",
                                                                  icfeb+1, ilayer, ch, ch_val/avg_sca_ch_occupancy);
                                          dqm_report.addEntry(cscName, entry.fillEntry(diag,MINOR, "CSC_CFEB_SCA_NOISY_CHANNEL"));
                                          //                  std::cout << cscName << " " << diag << std::endl;
                                        }
                                      // if ( nentries >= (100*16*nActiveCFEBs) )
                                      if (avg_strip_occupancy > 40)
                                        {
                                          double ch_thresh = 0.10;
                                          if ( ((ch == 1) && (icfeb == 0))
                                               || ((ch == 16) && (icfeb == nCFEBs-1)) ) ch_thresh = 0.0; /// First and Last strips have lower occupancy
                                          // if (ch_val == 0 && !isLowEff && (lowEffCFEBs[icfeb] != 1))
                                          if ((ch_ratio < ch_thresh) && !isLowEff && (lowEffCFEBs[icfeb] != 1))
                                            {
                                              std::string diag = Form("Dead SCA channel: CFEB%d Layer%d Ch#%d (occupancy %.3f)", icfeb+1, ilayer, ch, ch_ratio);
                                              dqm_report.addEntry(cscName, entry.fillEntry(diag,MINOR, "CSC_CFEB_SCA_DEAD_CHANNEL"));
                                              // std::cout << cscName << " " << diag << std::endl;
                                            }
                                        }
                                    }
                                }
                            }
                          // std::cout << std::endl;
                        }
                      if (me) delete me;

                    }
                }

            }
          //* Try to detect lowered ME11 HV segment
          avgSCAlayer/=6;
          for (unsigned int i=0; i < layerSCAsums.size(); i++)
            {
              double fract = layerSCAsums[i]/avgSCAlayer;
              if (ME11 && (fract > 0.1) && fract < 0.8 )
                {
                  std::string diag = Form("Lowered HV Segment: Layer%d (SCA efficiency %.2f of average)", i+1, fract);
                  dqm_report.addEntry(cscName, entry.fillEntry(diag,MINOR, "CSC_LOWERED_HV_SEGMENT"));
                  loweredHVsegment[i] = true;
                }
            }
        }

      if  (nActiveCFEBs > 0)
        {
          // Expecting active CFEBs
          // -- CFEB Comparators Occupancies Checks
          for (int ilayer=1; ilayer<=6; ilayer++)
            {
              std::string name = Form("CLCT_Ly%d_Rate",ilayer);

              me = findME(CSC_folders[i], name , sourcedir);
              if (me)
                {
                  TH1D* h = reinterpret_cast<TH1D*>(me);
                  int nentries = (int)h->GetEntries();
                  double allCompsum = 0; // h->Integral();
                  std::vector<double> Compsums;
                  Compsums.clear();
                  int noComps = 0;
                  double low_comp_thresh = 0.2;
                  double high_comp_thresh = 2.5;

                  // std::cout << cscName << " ly " << ilayer << ", comp sum: " << allCompsum;

                  if ( nentries >= (5*32*nActiveCFEBs) )
                    {
                      for (int icfeb=0; icfeb < nCFEBs; icfeb++)
                        {

                          if (badCFEBs[icfeb] == 1 )
                            {
                              Compsums.push_back(0.0);
                              continue;
                            }// Skip dead CFEBs

                          double cfeb_comp_sum = h->Integral(icfeb*32+1, (icfeb+1)*32);

                          // std::cout << ", " << cfeb_comp_sum;

                          // Don't count ME11 CFEB5 occupancy during Beam
                          if (!isBeam || (isBeam && ME11 && (icfeb!=4) )) allCompsum += cfeb_comp_sum;

                          Compsums.push_back(cfeb_comp_sum);

                          if ( (cfeb_comp_sum == 0) && (lowEffCFEBs[icfeb] != 1))
                            {

                              std::string diag=Form("CFEB No Comparators Data: CFEB%d Layer%d", icfeb+1, ilayer);
                              dqm_report.addEntry(cscName, entry.fillEntry(diag,SEVERE, "CSC_CFEB_NO_COMPARATORS_DATA"));
                              noComps++;
                            }
                          else
                            {
                              if (isBeam && ME11 && (icfeb==4)) noComps++; // Don't count ME11 CFEB5 occupancy during Beam
                            }

                        }
                    }

                  // std::cout << ", " << allCompsum/(nActiveCFEBs-noComps) << std::endl;

                  if ( nentries >= (25*32*nActiveCFEBs) )
                    {
                      if (nActiveCFEBs - noComps > 0)
                        {
                          // Check that still active CFEBs present
                          double avg_comp_occupancy = allCompsum/(nActiveCFEBs-noComps);
                          double avg_comp_ch_occupancy = avg_comp_occupancy/16;
                          for (int icfeb=0; icfeb < nCFEBs; icfeb++)
                            {
                              double avg_eff = (100*Compsums[icfeb])/(18.*csc_stats[cscName]);
                              double avg = round(avg_eff*100.)/100.;
                              if (Compsums[icfeb])
                                {
                                  if ( !isBeam || (isBeam && ME11 && icfeb!=4) )
                                    {
                                      // Standard occupancy check logic for Cosmic run

                                      // if ( (Compsums[icfeb] < low_comp_thresh*avg_comp_occupancy) && (lowEffCFEBs[icfeb] != 1))
                                      if ( (avg < low_comp_thresh) && (lowEffCFEBs[icfeb] != 1) && (!loweredHVsegment[ilayer-1]))
                                        {
                                          if (avg < 0.01)
                                            {
                                              std::string diag=Form("CFEB No Comparators Data: CFEB%d Layer%d", icfeb+1, ilayer);
                                              dqm_report.addEntry(cscName, entry.fillEntry(diag,SEVERE, "CSC_CFEB_COMPARATORS_LOW_EFF"));
                                            }
                                          else
                                            {
                                              std::string diag=Form("CFEB Low Comparators Efficiency: CFEB%d Layer%d (%.3f%% < %.1f%% threshold)", icfeb+1, ilayer,
                                                                    avg, low_comp_thresh);
                                              dqm_report.addEntry(cscName, entry.fillEntry(diag,TOLERABLE, "CSC_CFEB_COMPARATORS_LOW_EFF"));
                                            }
                                        }

                                      // if ( Compsums[icfeb] >= high_comp_thresh*avg_comp_occupancy )
                                      if ( avg >= high_comp_thresh)
                                        {
                                          std::string diag=Form("CFEB Hot/Noisy CFEB Comparators: CFEB%d Layer%d (%.1f > %.1f threshold)", icfeb+1, ilayer,
                                                                avg, high_comp_thresh);
                                          dqm_report.addEntry(cscName, entry.fillEntry(diag,SEVERE, "CSC_CFEB_COMPARATORS_NOISY"));
                                        }

                                      for (int ch=1; ch <=32; ch++)
                                        {
                                          double ch_val = h->GetBinContent(ch+icfeb*32-1);
                                          if (ch_val > 8*avg_comp_ch_occupancy)
                                            {
                                              std::string diag = Form("CFEB Hot/Noisy Comparator channel: CFEB%d Layer%d HStrip%d (occupancy %.1f times > average)",
                                                                      icfeb+1, ilayer, ch+icfeb*32,
                                                                      ch_val/avg_comp_ch_occupancy);
                                              dqm_report.addEntry(cscName, entry.fillEntry(diag,TOLERABLE, "CSC_CFEB_COMPARATORS_NOISY_CHANNEL"));
                                            }
                                        }
                                    }
                                  else if (ME11 && icfeb==4)  // Occupancy check logic for ME11 CFEB5 with Beam run
                                    {
                                      double me11_cfeb5_low_comp_thresh = 1.7;
                                      double me11_cfeb5_high_comp_thresh = 5.;
                                      if ( (avg < me11_cfeb5_low_comp_thresh) && (lowEffCFEBs[icfeb] != 1) && (!loweredHVsegment[ilayer-1]) )
                                        // if ( (Compsums[icfeb] < low_comp_thresh*avg_comp_occupancy) && (lowEffCFEBs[icfeb] != 1))
                                        {
                                          std::string diag=Form("CFEB Low Comparators Efficiency: CFEB%d Layer%d (%.3f%% < %.1f%% threshold)", icfeb+1, ilayer,
                                                                avg, me11_cfeb5_low_comp_thresh);
                                          dqm_report.addEntry(cscName, entry.fillEntry(diag,TOLERABLE, "CSC_CFEB_COMPARATORS_LOW_EFF"));
                                        }

                                      if ( avg >= me11_cfeb5_high_comp_thresh )
                                        // if ( Compsums[icfeb] >= high_comp_thresh*avg_comp_occupancy )
                                        {
                                          std::string diag=Form("CFEB Hot/Noisy CFEB Comparators: CFEB%d Layer%d (%.1f%% > %.1f%% threshold)", icfeb+1, ilayer,
                                                                avg, me11_cfeb5_high_comp_thresh);
                                          dqm_report.addEntry(cscName, entry.fillEntry(diag,TOLERABLE, "CSC_CFEB_COMPARATORS_NOISY"));
                                        }
                                    }


                                }
                            }
                        }
                    }
                  if (me) delete me;


                }
            }
        } // expecting active CFEBs


      // -- Anode Occupancies and HV Segments Checks
      for (int ilayer=1; ilayer<=6; ilayer++)
        {
          std::string name = Form("ALCT_Ly%d_Efficiency",ilayer);

          me = findME(CSC_folders[i], name , sourcedir);
          if (me)
            {
              TH1D* h = reinterpret_cast<TH1D*>(me);
              double ent = h->GetEntries();

              // For ME11 if occupancy histo is empty then HV segment is OFF
              if (((int)csc_stats[cscName] > 20*nWireGroups) && ME11 && (ent==0) && !deadALCT[cscName])
                {
                  std::string diag=Form("No HV at Segment%d Layer%d", 1, ilayer );
                  dqm_report.addEntry(cscName, entry.fillEntry(diag,TOLERABLE, "CSC_NO_HV_SEGMENT"));

                }


              if ( ent > 20*nWireGroups)
                {

                  double avg_anode_occup = 0;
                  double anode_max = 0;
                  vector<double> afebs;
                  vector<double> hvsegs;
                  vector<int> no_hv_segments;
                  vector<int> noisy_hv_segments;
                  for (uint32_t hvseg=0; hvseg < hvSegMap.size(); hvseg++)
                    {
                      double val = (h->Integral(hvSegMap[hvseg].first, hvSegMap[hvseg].second)/((hvSegMap[hvseg].second-hvSegMap[hvseg].first)*ent))*100.;
                      hvsegs.push_back(val);
                      if (val>anode_max) anode_max=val;
                      // std::cout << cscName << " ly" << ilayer << " ent:" <<   ent << " seg" << hvseg << " " << val << " sum " << h->Integral() << std::endl;
                    }

                  TH1D* h_tmp = new TH1D("temp", "temp", 1000, 0, anode_max+1);
                  for (uint32_t hvseg=0; hvseg < hvSegMap.size(); hvseg++)
                    {
                      h_tmp->Fill(hvsegs[hvseg]);
                    }
                  avg_anode_occup = h_tmp->GetMean();
                  delete h_tmp;
                  for (uint32_t hvseg=0; hvseg < hvSegMap.size(); hvseg++)
                    {
                      double z=hvsegs[hvseg];
                      double avg = round(z*100.)/100.;
                      if (avg < 0.15 )
                        {
                          std::string diag=Form("No HV at Segment%d Layer%d, Anode Occupancy: %.2f%%", hvseg+1, ilayer, z );
                          dqm_report.addEntry(cscName, entry.fillEntry(diag,TOLERABLE, "CSC_NO_HV_SEGMENT"));
                          no_hv_segments.push_back(hvseg);
                        }
                    }

                  anode_max = 0;

                  for (int32_t iseg=0; iseg < nWireGroups/8; iseg++)
                    {
                      double val = (h->Integral(iseg*8+1, (iseg+1)*8)/(8*ent))*100;
                      afebs.push_back(val);
                      if (val>anode_max) anode_max=val;
                      // std::cout << cscName << " ly" << ilayer << " ent:" <<   ent << " seg" << iseg << " " << val << std::endl;
                    }

                  h_tmp = new TH1D("temp", "temp", 1000, 0, anode_max+1);
                  for (int32_t iseg=0; iseg < nWireGroups/8; iseg++)
                    {
                      h_tmp->Fill(afebs[iseg]);
                    }
                  avg_anode_occup = h_tmp->GetMean();
                  delete h_tmp;

                  for (int32_t iseg=0; iseg < nWireGroups/8; iseg++)
                    {
                      double z=afebs[iseg];
                      int hvseg = emu::dqm::utils::getHVSegmentNumber(cscName, iseg);
                      int afeb = iseg*3+(ilayer+1)/2;

                      if ( (z < 0.15)
                           && (find(no_hv_segments.begin(), no_hv_segments.end(), hvseg ) == no_hv_segments.end())
                           && (find(noisy_hv_segments.begin(), noisy_hv_segments.end(), hvseg ) == noisy_hv_segments.end()))
                        {

                          if (z==0)
                            {
                              std::string diag=Form("ALCT No Anode Data: AFEB%d Layer%d", afeb, ilayer);
                              dqm_report.addEntry(cscName, entry.fillEntry(diag,SEVERE, "CSC_ALCT_NO_ANODE_DATA"));
                            }
                          else
                            {
                              std::string diag=Form("ALCT Low Anode Efficiency: AFEB%d Layer%d", afeb, ilayer);
                              dqm_report.addEntry(cscName, entry.fillEntry(diag,TOLERABLE, "CSC_ALCT_NO_ANODE_DATA"));
                            }

                        }
                      else if (z > 10.)
                        {
                          std::string diag=Form("Noisy Anodes Segment: AFEB%d Layer%d", afeb, ilayer );
                          // !!!! Change to different test ID !!!!
                          dqm_report.addEntry(cscName, entry.fillEntry(diag,TOLERABLE, "CSC_ALCT_AFEB_NOISY"));
                        }

                    }


                }
              delete me;
            }
        }


    }


  // == Check for chambers with format warnings (CFEB B-Words)
  me = findME("EMU", "DMB_Format_Warnings_Fract",  sourcedir);
  me2 = findME("EMU", "DMB_Format_Warnings",  sourcedir);
  if (me)
    {
      TH2D* h = reinterpret_cast<TH2D*>(me);
      TH2D* h2 = reinterpret_cast<TH2D*>(me2);
      int csc_cntr=0;
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
        for (int j=int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++)
          {
            double z = h->GetBinContent(i, j);
            if (z>0)
              {
                csc_cntr++;
                uint32_t events = uint32_t(h2->GetBinContent(i, j));
                std::string cscTag(Form("CSC_%03d_%02d", i, j));
                std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
                float fract=z*100;
                DQM_SEVERITY severity=NONE;
                if (events > 10)
                  {
                    if (fract >= 80.) severity=CRITICAL;
                    else if (fract >= 20.) severity=SEVERE;
                    else if (fract > 5.) severity=TOLERABLE;
                    else if (fract > 0.5) severity=MINOR;
                  }

                std::string diag=Form("CFEB B-Words: %d events (%.3f%%)",events, z*100);

                dqm_report.addEntry(cscName, entry.fillEntry(diag, severity, "CSC_WITH_BWORDS"));
              }

          }
      if (csc_cntr) dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("%d CSCs with CFEB B-Words", csc_cntr), NONE, "ALL_CHAMBERS_WITH_BWORDS"));
      if (me) delete me;

    }

  int csc_w_problems=0;
  T_DQMReport& report = dqm_report.getReport();
  T_DQMReport::iterator itr;
  for (itr = report.begin(); itr != report.end(); ++itr)
    {
      if (itr->first.find("ME") == 0)
        {
          csc_w_problems++;
        }
    }
  dqm_report.addEntry("EMU Summary", entry.fillEntry(Form("Found %d CSCs with issues of different severity", csc_w_problems)));

  showReport();
  path = (path.size()?path+"/":"");
  saveReport(path+"dqm_report_v"+dqm_report.getVersion()+".txt", runname);
  saveReportJSON(path+"dqm_report_v"+dqm_report.getVersion()+".js", runname);
  return 0;
}

// == Show DQM Report
void EmuPlotter::showReport()
{

  T_DQMReport::iterator itr;
  vector<ReportEntry>::iterator err_itr;
  T_DQMReport& report = dqm_report.getReport();


  LOG4CPLUS_WARN(logger_, "<=== Automatically generated DQM Report ===>");

  LOG4CPLUS_WARN(logger_, "<--- Analysis Version " << dqm_report.getVersion() << " ---> ");
  for (itr = report.begin(); itr != report.end(); ++itr)
    {
      if (itr->first.find("EMU") == 0)
        {
          LOG4CPLUS_WARN(logger_, "<--- " <<itr->first << " ---> ");
          for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr)
            {
              std::string sev = "";
              if (err_itr->severity != NONE)
                sev = Form(" [%s] ",DQM_SEVERITY_STR[err_itr->severity]);

              LOG4CPLUS_WARN(logger_, err_itr->descr << sev);
            }
        }
    }

  LOG4CPLUS_WARN(logger_, "<--- DDUs Detailed Report --->");
  for (itr = report.begin(); itr != report.end(); ++itr)
    {
      if (itr->first.find("DDU") == 0)
        {
          LOG4CPLUS_WARN(logger_, "[= " <<itr->first << " =]");
          for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr)
            {
              std::string sev = "";
              if (err_itr->severity != NONE)
                sev = Form(" [%s] ",DQM_SEVERITY_STR[err_itr->severity]);

              LOG4CPLUS_WARN(logger_, err_itr->descr << sev);

            }
        }
    }

  LOG4CPLUS_WARN(logger_, "<--- CSCs Detailed Report --->");
  for (itr = report.begin(); itr != report.end(); ++itr)
    {
      if (itr->first.find("ME") == 0)
        {
          LOG4CPLUS_WARN(logger_, "[= " <<itr->first << " =]");
          for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr)
            {
              std::string sev = "";
              if (err_itr->severity != NONE)
                sev = Form(" [%s] ",DQM_SEVERITY_STR[err_itr->severity]);

              LOG4CPLUS_WARN(logger_, err_itr->descr << sev);

            }
        }
    }

}


int EmuPlotter::save_ALCT_CLCT_Match_Data(std::string rootfile, std::string path, std::string runname)
{
  TFile* rootsrc = TFile::Open( rootfile.c_str());
  if (!rootsrc)
    {
      LOG4CPLUS_ERROR (logger_, "Unable to open " << rootfile.c_str());
      return -1;
    }
  if (!rootsrc->cd("DQMData"))
    {
      LOG4CPLUS_ERROR (logger_, "No histos folder in file");
      return -1;
    }


  TString command = Form("mkdir -p %s",path.c_str());
  gSystem->Exec(command.Data());

  std::string runNumber = runname;

  TDirectory *sourcedir = gDirectory;

  std::vector<std::string> CSC_folders=getListOfFolders("CSC", sourcedir);


  int CSCtype = 0;
  int CSCposition = 0;


  std::ostringstream strout;
  // == Perform per Chamber Checks
  for (uint32_t i=0; i<CSC_folders.size(); i++)
    {
      int crate=0, slot =0;
      std::string cscName = getCSCName(CSC_folders[i], crate, slot, CSCtype, CSCposition);

      // ALCT-CLCT Match
      MonitorElement* me = findME(CSC_folders[i], "ALCT_Match_Time",  sourcedir);
      if (me)
        {
          TH1D* h = reinterpret_cast<TH1D*>(me);
          strout << cscName << " " << fixed << setprecision(3) << h->GetMean()<< " " << setprecision(3) << h->GetRMS() << " " << setprecision(0) << h->GetEntries() << std::endl;
        }
    }
  path = (path.size()?path+"/":"");
  std::string filename = (path+"alct_clct_match.txt");
  std::ofstream fout(filename.c_str());
  LOG4CPLUS_WARN(logger_, "Saving ALCT-CLCT Match data to " << filename);
  fout << "ChamberID  Mean  RMS  Entries" << std::endl;


  fout << strout.str();
  fout.close();
  return 0;

}

int EmuPlotter::save_CSCCounters(std::string rootfile, std::string path, std::string runname)
{
  TFile* rootsrc = TFile::Open( rootfile.c_str());
  if (!rootsrc)
    {
      LOG4CPLUS_ERROR (logger_, "Unable to open " << rootfile.c_str());
      return -1;
    }
  if (!rootsrc->cd("DQMData"))
    {
      LOG4CPLUS_ERROR (logger_, "No histos folder in file");
      return -1;
    }


  TString command = Form("mkdir -p %s",path.c_str());
  gSystem->Exec(command.Data());

  std::string runNumber = runname;

  TDirectory *sourcedir = gDirectory;

  std::vector<std::string> CSC_folders=getListOfFolders("CSC", sourcedir);


  int CSCtype = 0;
  int CSCposition = 0;


  std::ostringstream strout;
  cscCounters.clear();
  MonitorElement* mes = findME("EMU", "DMB_Format_Errors",  sourcedir);
  TH2D* hs = reinterpret_cast<TH2D*>(mes);
  // == Perform per Chamber Checks
  for (uint32_t i=0; i<CSC_folders.size(); i++)
    {
      int crate=0, slot =0;
      std::string cscName = getCSCName(CSC_folders[i], crate, slot, CSCtype, CSCposition);
      CSCCounters& trigCnts = cscCounters[CSC_folders[i]];


      MonitorElement* me = findME(CSC_folders[i], "Actual_DMB_FEB_DAV_Rate",  sourcedir);
      if (me)
        {
          //      TH1D* h = reinterpret_cast<TH1D*>(me);
          trigCnts["ALCT"] =  (uint32_t)me->GetBinContent(1);
          trigCnts["CLCT"] =  (uint32_t)me->GetBinContent(2);
          trigCnts["CFEB"] =  (uint32_t)me->GetBinContent(3);
        }

      me = findME(CSC_folders[i], "Actual_DMB_FEB_DAV_Frequency",  sourcedir);
      if (me)
        {
          // TH1D* h = reinterpret_cast<TH1D*>(me);
          trigCnts["DMB"] =  (uint32_t)me->GetEntries();
        }

      trigCnts["BAD"] = (uint32_t)hs->GetBinContent(crate, slot);

    }


  path = (path.size()?path+"/":"");
  std::string filename = (path+"csc_counters.js");
  std::ofstream fout(filename.c_str());
  LOG4CPLUS_WARN(logger_, "Saving CSC Counters " << filename);

  fout << "var CSC_COUNTERS=[" << std::endl;
  fout << "['Run: "<< runname <<"'," << std::endl;

  if (!cscCounters.empty())
    {
      std::map<std::string, CSCCounters>::iterator citr;
      for (citr=cscCounters.begin(); citr != cscCounters.end(); ++citr)
        {
          fout << "['" << citr->first << "',[";
          CSCCounters::iterator itr;
          for (itr=citr->second.begin(); itr != citr->second.end(); ++itr)   // == Loop and Output Counters
            {
              fout << "['"<< itr->first << "','" << itr->second <<"'],";
            }
          fout << "]]," << std::endl;
        }
    }

  fout << "]]" << std::endl;
  fout.close();
  return 0;

}




// == Save DQM Report to file
void EmuPlotter::saveReport(std::string filename, std::string runname)
{
  T_DQMReport::iterator itr;
  vector<ReportEntry>::iterator err_itr;


  T_DQMReport& report = dqm_report.getReport();

  std::ofstream out(filename.c_str());
  LOG4CPLUS_INFO(logger_, " Saving DQM report to " << filename);
  out << "<=== Automatically generated at " << emu::dqm::utils::now() << " DQM Report ===>" << std::endl;
  out << "<--- Analysis Version " << dqm_report.getVersion() << " ---> "<< std::endl;


  out << std::endl;
  for (itr = report.begin(); itr != report.end(); ++itr)
    {
      if (itr->first.find("EMU") == 0)
        {
          out << "<--- " <<itr->first << " --->" << std::endl;
          for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr)
            {
              out <<"\t" <<err_itr->descr;
              if (err_itr->severity != NONE)
                out << " [" << DQM_SEVERITY_STR[err_itr->severity]<< "] ";
              out << std::endl;
            }
        }
    }

  out << std::endl;
  out << "<--- DDUs Detailed Report --->" << std::endl;
  for (itr = report.begin(); itr != report.end(); ++itr)
    {
      if (itr->first.find("DDU") == 0)
        {
          out << "[= " <<itr->first << " =]" << std::endl;
          for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr)
            {
              out <<"\t" <<err_itr->descr;
              if (err_itr->severity != NONE)
                out << " [" << DQM_SEVERITY_STR[err_itr->severity]<< "] ";
              out << std::endl;
            }
        }
    }

  out << std::endl;
  out << "<--- CSCs Detailed Report --->" << std::endl;
  for (itr = report.begin(); itr != report.end(); ++itr)
    {
      if (itr->first.find("ME") == 0)
        {
          out << "[= " <<itr->first << " =]" << std::endl;
          for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr)
            {
              out <<"\t" <<err_itr->descr;
              if (err_itr->severity != NONE)
                out << " [" << DQM_SEVERITY_STR[err_itr->severity]<< "] ";
              out << std::endl;

            }
        }
    }

}

// == Save DQM Report to file
void EmuPlotter::saveReportJSON(std::string filename, std::string runname)
{
  T_DQMReport::iterator itr;
  vector<ReportEntry>::iterator err_itr;


  T_DQMReport& report = dqm_report.getReport();

  std::ofstream out(filename.c_str());
  LOG4CPLUS_INFO(logger_, " Saving JSON DQM report to " << filename);
  out << "var DQM_REPORT = { \"run\": \"" << std::string(basename((char *)(runname.c_str())))
      << "\", \"genDate\": \"" << emu::dqm::utils::now()
      << "\", \"version\": \"" << dqm_report.getVersion()
      <<  "\", \"report\":\n[" << std::endl;


  for (itr = report.begin(); itr != report.end(); ++itr)
    {
      if (itr->first.find("EMU") == 0)
        {
          std::string scope = "EMU";
          out << "{\"objID\": \"" << scope << "\", \"name\": \"" << itr->first << "\", \"list\": [" << std::endl;
          for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr)
            {
              out << "\t{\"testID\": \"" << err_itr->testID
                  << "\", \"scope\": \"" << scope
                  << "\", \"descr\": \"" << err_itr->descr
                  << "\", \"severity\": \"" << err_itr->severity << "\"}";
              if ((err_itr+1) != itr->second.end()) out << ",";
              out << std::endl;
            }
          out << "]}," << std::endl;
        }
    }

  for (itr = report.begin(); itr != report.end(); ++itr)
    {
      if (itr->first.find("DDU") == 0)
        {
          std::string scope = "DDU";
          out << "{\"objID\": \"" << itr->first << "\", \"name\": \"" << itr->first << "\", \"list\": [" << std::endl;
          for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr)
            {
              out << "\t{\"testID\": \"" << err_itr->testID
                  << "\", \"scope\": \"" << scope
                  << "\", \"descr\": \"" << err_itr->descr
                  << "\", \"severity\": \"" << err_itr->severity << "\"}";
              if ((err_itr+1) != itr->second.end()) out << ",";
              out << std::endl;
            }
          out << "]}," << std::endl;
        }
    }

  for (itr = report.begin(); itr != report.end(); ++itr)
    {
      if (itr->first.find("ME") == 0)
        {
          std::string scope = "CSC";
          out << "{\"objID\": \"" << itr->first << "\", \"name\": \"" << itr->first << "\", \"list\": [" << std::endl;
          for (err_itr = itr->second.begin(); err_itr != itr->second.end(); ++err_itr)
            {
              out << "\t{\"testID\": \"" << err_itr->testID
                  << "\", \"scope\": \"" << scope
                  << "\", \"descr\": \"" << err_itr->descr
                  << "\", \"severity\": \"" << err_itr->severity << "\"}";
              if ((err_itr+1) != itr->second.end()) out << ",";
              out << std::endl;
            }
          out << "]},";
          out << std::endl;
        }
    }

  out << "]\n};" << std::endl;


}


