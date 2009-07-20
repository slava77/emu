#include "emu/dqm/cscanalyzer/EmuPlotter.h"
#include "TClass.h"

/*
  Generate Online DQM report
*/


int EmuPlotter::generateOnlineReport(std::string runname)
{
  appBSem_.take();
  dqm_report.clearReport();
  TDirectory *sourcedir = NULL;

  std::vector<std::string> EMU_folders;
  std::vector<std::string> DDU_folders=getListOfFolders("DDU", sourcedir);
  std::vector<std::string> CSC_folders=getListOfFolders("CSC", sourcedir);

  ReportEntry entry;

  std::string hname="";

  int CSCtype = 0;
  int CSCposition = 0;


  std::map<std::string, uint32_t>::iterator stats_itr;

  std::map<std::string, uint32_t> csc_stats;
  MonitorElement* me = NULL;

  // == Check for chambers with Format Errors
  hname =  "DMB_Format_Errors_Fract";
  me = findME("EMU", hname,  sourcedir);
  MonitorElement* me2 = findME("EMU", "DMB_Format_Errors",  sourcedir);
  if (me && me2)
    {
      TH2F* h1 = dynamic_cast<TH2F*>(me);
      TH2F* h2 = dynamic_cast<TH2F*>(me2);
      int csc_cntr=0;
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
                if (fract >= 80.) severity=CRITICAL;
                else if (fract >= 10.) severity=SEVERE;
                else if (fract > 1.) severity=TOLERABLE;
                else severity=MINOR;
                std::string diag=Form("Format Errors: %d events (%.3f%%))",events, fract);
                dqm_report.addEntry(cscName,entry.fillEntry(diag,severity,"CSC_WITH_FORMAT_ERRORS"));


                // --- Get Format Errors Details
                MonitorElement* me3 = findME(cscTag, "BinCheck_Errors_Frequency",  sourcedir);
                MonitorElement* me4 = findME(cscTag, "BinCheck_ErrorStat_Table",  sourcedir);
                if (me3 && me4)
                  {

                    TH2F* h3 = dynamic_cast<TH2F*>(me3);
                    TH2F* h4 = dynamic_cast<TH2F*>(me4);

                    for (int err=int(h3->GetYaxis()->GetXmin()); err <= int(h3->GetYaxis()->GetXmax()); err++)
                      {
                        double z = h3->GetBinContent(1, err);

                        if (z>0)
                          {
                            uint32_t events = uint32_t(h4->GetBinContent(1, err));
                            float fract=z*100;
                            DQM_SEVERITY severity=NONE;
                            if (fract >= 80.) severity=CRITICAL;
                            else if (fract >= 10.) severity=SEVERE;
                            else if (fract > 1.) severity=TOLERABLE;
                            else severity=MINOR;
                            std::string error_type = std::string(h3->GetYaxis()->GetBinLabel(err));
                            std::string diag=std::string(Form("\tFormat Errors: %s %d events (%.3f%%)",error_type.c_str(), events, fract));
                            // LOG4CPLUS_WARN(logger_, cscTag << ": "<< diag);
                            dqm_report.addEntry(cscName, entry.fillEntry(diag,severity,"CSC_WITH_FORMAT_ERRORS"));
                          }
                      }
                  }
                // ---

              }
          }
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
      TH2F* h1 = dynamic_cast<TH2F*>(me);
      TH2F* h2 = dynamic_cast<TH2F*>(me2);
      int csc_cntr=0;
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

                float fract=z*100;
                DQM_SEVERITY severity=NONE;
                if (fract >= 80.) severity=CRITICAL;
                else if (fract >= 10.) severity=SEVERE;
                else if (fract > 1.) severity=TOLERABLE;
                else severity=MINOR;
                std::string diag=Form("DMB-Input FIFO Full: %d events (%.3f%%)",events, fract);
                dqm_report.addEntry(cscName, entry.fillEntry(diag, severity,"CSC_WITH_INPUT_FIFO_FULL"));


                // --- Get Format Errors Details
                MonitorElement* me3 = findME(cscTag, "BinCheck_DataFlow_Problems_Frequency",  sourcedir);
                MonitorElement* me4 = findME(cscTag, "BinCheck_DataFlow_Problems_Table",  sourcedir);
                if (me3 && me4)
                  {

                    TH2F* h3 = dynamic_cast<TH2F*>(me3);
                    TH2F* h4 = dynamic_cast<TH2F*>(me4);

                    for (int err=int(h3->GetYaxis()->GetXmin()); err <= 7; err++)
                      {
                        double z = h3->GetBinContent(1, err);

                        if (z>0)
                          {
                            uint32_t events = uint32_t(h4->GetBinContent(1, err));
                            float fract=z*100;
                            DQM_SEVERITY severity=NONE;
                            if (fract >= 80.) severity=CRITICAL;
                            else if (fract >= 10.) severity=SEVERE;
                            else if (fract > 1.) severity=TOLERABLE;
                            else severity=MINOR;
                            std::string error_type = std::string(h3->GetYaxis()->GetBinLabel(err));
                            std::string diag=std::string(Form("DMB-Input FIFO Full: %s %d events (%.3f%%)",error_type.c_str(), events, fract));
                            // LOG4CPLUS_WARN(logger_, cscTag << ": "<< diag);
                            dqm_report.addEntry(cscName, entry.fillEntry(diag, severity, "CSC_WITH_INPUT_FIFO_FULL"));
                          }
                      }
                  }
                // ---

              }
          }
    }

  // == Check for chambers with DMB-Input Timeout
  me = findME("EMU", "DMB_input_timeout_Fract",  sourcedir);
  me2 = findME("EMU", "DMB_input_timeout",  sourcedir);
  if (me && me2)
    {
      TH2F* h1 = dynamic_cast<TH2F*>(me);
      TH2F* h2 = dynamic_cast<TH2F*>(me2);
      int csc_cntr=0;
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

                float fract=z*100;
                DQM_SEVERITY severity=NONE;
                if (fract >= 80.) severity=CRITICAL;
                else if (fract >= 10.) severity=SEVERE;
                else if (fract > 1.) severity=TOLERABLE;
                else severity=MINOR;
                std::string diag=Form("DMB-Input Timeout: %d events (%.3f%%)",events, fract);
                dqm_report.addEntry(cscName, entry.fillEntry(diag,severity, "CSC_WITH_INPUT_TIMEOUT"));


                // --- Get Format Errors Details
                MonitorElement* me3 = findME(cscTag, "BinCheck_DataFlow_Problems_Frequency",  sourcedir);
                MonitorElement* me4 = findME(cscTag, "BinCheck_DataFlow_Problems_Table",  sourcedir);
                if (me3 && me4)
                  {

                    TH2F* h3 = dynamic_cast<TH2F*>(me3);
                    TH2F* h4 = dynamic_cast<TH2F*>(me4);

                    for (int err=8; err<int(h3->GetYaxis()->GetXmax())-2; err++)
                      {
                        double z = h3->GetBinContent(1, err);

                        if (z>0)
                          {
                            uint32_t events = uint32_t(h4->GetBinContent(1, err));
                            float fract=z*100;
                            DQM_SEVERITY severity=NONE;
                            if (fract >= 80.) severity=CRITICAL;
                            else if (fract >= 10.) severity=SEVERE;
                            else if (fract > 1.) severity=TOLERABLE;
                            else severity=MINOR;
                            std::string error_type = std::string(h3->GetYaxis()->GetBinLabel(err));
                            std::string diag=std::string(Form("DMB-Input Timeout: %s %d events (%.3f%%)",error_type.c_str(), events, fract));
                            // LOG4CPLUS_WARN(logger_, cscTag << ": "<< diag);
                            dqm_report.addEntry(cscName, entry.fillEntry(diag, severity, "CSC_WITH_INPUT_TIMEOUT" ));
                          }
                      }
                  }
                // ---

              }
          }
    }

  // == Check for missing ALCT data blocks
  me = findME("EMU", "DMB_wo_ALCT_Fract",  sourcedir);
  if (me)
    {
      TH2F* h = dynamic_cast<TH2F*>(me);
      int csc_cntr=0;
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
        for (int j=int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++)
          {
            double z = h->GetBinContent(i, j);
            if (z>0.95)
              {
                csc_cntr++;
                std::string cscTag(Form("CSC_%03d_%02d", i, j));
                std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
                float fract=z*100;
                std::string diag=Form("No ALCT Data: %.1f%%",fract);

                dqm_report.addEntry(cscName, entry.fillEntry(diag, CRITICAL, "CSC_WITHOUT_ALCT"));
              }

          }
    }

  // == Check for missing CLCT data blocks
  me = findME("EMU", "DMB_wo_CLCT_Fract",  sourcedir);
  if (me)
    {
      TH2F* h = dynamic_cast<TH2F*>(me);
      int csc_cntr=0;
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
        for (int j=int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++)
          {
            double z = h->GetBinContent(i, j);
            if (z>0.95)
              {
                csc_cntr++;
                std::string cscTag(Form("CSC_%03d_%02d", i, j));
                std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
                float fract=z*100;
                std::string diag=Form("No CLCT Data: %.1f%%",fract);

                dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL, "CSC_WITHOUT_CLCT"));
              }

          }
    }

  // == Check for missing CFEB data blocks
  me = findME("EMU", "DMB_wo_CFEB_Fract",  sourcedir);
  if (me)
    {
      TH2F* h = dynamic_cast<TH2F*>(me);
      int csc_cntr=0;
      for (int i=int(h->GetXaxis()->GetXmin()); i<= int(h->GetXaxis()->GetXmax()); i++)
        for (int j=int(h->GetYaxis()->GetXmin()); j <= int(h->GetYaxis()->GetXmax()); j++)
          {
            double z = h->GetBinContent(i, j);
            if (z>0.95)
              {
                csc_cntr++;
                std::string cscTag(Form("CSC_%03d_%02d", i, j));
                std::string cscName=getCSCFromMap(i,j, CSCtype, CSCposition );
                float fract=z*100;
                std::string diag=Form("No CFEB Data: %.1f%%",fract);

                dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL, "CSC_WITHOUT_CFEB"));
              }

          }
    }


  // == Check for chambers with L1A out of sync
  me = findME("EMU", "DMB_L1A_out_of_sync_Fract",  sourcedir);
  me2 = findME("EMU", "DMB_L1A_out_of_sync",  sourcedir);
  if (me)
    {
      TH2F* h = dynamic_cast<TH2F*>(me);
      TH2F* h2 = dynamic_cast<TH2F*>(me2);
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
                if (fract >= 80.) severity=CRITICAL;
                else if (fract >= 10.) severity=SEVERE;
                else if (fract > 1.) severity=TOLERABLE;
                else severity=MINOR;

                std::string diag=Form("L1A out of sync: %d events (%.3f%%)",events, fract);

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
                        TH1F* h3 = dynamic_cast<TH1F*>(me3);
                        if (h3->GetMean() != 0)
                          {
                            diag=Form("L1A out of sync: %s",(l1a_histos[k].first).c_str());

                            dqm_report.addEntry(cscName, entry.fillEntry(diag, severity, "CSC_WITH_L1A_OUT_OF_SYNC"));
                          }
                      }

                  }

              }

          }
    }

  // == Perform per Chamber Checks
  for (uint32_t i=0; i<CSC_folders.size(); i++)
    {
      int crate=0, slot =0;
      //    std::cout << getCSCName(CSC_folders[i], crate, slot, CSCtype, CSCposition) << std::endl;
      std::string cscName = getCSCName(CSC_folders[i], crate, slot, CSCtype, CSCposition);
      int nCFEBs = emu::dqm::utils::getNumCFEBs(cscName);
      // int nStrips = getNumStrips(cscName);
      int nWireGroups = emu::dqm::utils::getNumWireGroups(cscName);
      std::vector<int> deadCFEBs(5,0);
      std::vector<int> lowEffCFEBs(5,0);
      std::vector<int> badCFEBs(5,0);
      int nbadCFEBs = 0;
      std::vector< std::pair<int,int> > hvSegMap = emu::dqm::utils::getHVSegmentsMap(cscName);
      //    bool isme11 = isME11(cscName);

      // int deadALCT=0;


      // -- CFEBs DAV checks
      me = findME(CSC_folders[i], "Actual_DMB_CFEB_DAV_Frequency",  sourcedir);
      me2 = findME(CSC_folders[i], "Actual_DMB_CFEB_DAV_Rate",  sourcedir);
      if (me && me2)
        {
          TH1F* h = dynamic_cast<TH1F*>(me);
          // TH1F* h1 = dynamic_cast<TH1F*>(me2);

          if  ( h->GetEntries() > nCFEBs)
            {
              for (int icfeb=0; icfeb< nCFEBs; icfeb++)
                {
                  double z =  h->GetBinContent(icfeb+1);
                  if (z < 10)
                    {
                      //	    uint32_t events = uint32_t(h1->GetBinContent(icfeb+1));

                      std::string diag=Form("CFEB Dead/Low efficiency: CFEB%d DAV %.3f%%", icfeb+1, z);
                      if (z==0)
                        {
                          deadCFEBs[icfeb]=1;    // Mark this CFEB as dead
                        }
                      else
                        {
                          lowEffCFEBs[icfeb]=1;
                        }
                      badCFEBs[icfeb]=1;
                      nbadCFEBs ++;

                      dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL, "CSC_WITH_LOW_CFEB_DAV_EFF"));
                    }
                }
            }
        }

      int nActiveCFEBs = nCFEBs-nbadCFEBs;

      if  (nActiveCFEBs > 0)
        { // Expecting active CFEBs

          // -- CFEB SCA Occupancies Checks
          for (int ilayer=1; ilayer<=6; ilayer++)
            {
              std::string name = Form("CFEB_ActiveStrips_Ly%d",ilayer);

              me = findME(CSC_folders[i], name , sourcedir);
              if (me)
                {
                  TH1F* h = dynamic_cast<TH1F*>(me);
                  int nentries = (int)h->GetEntries();
                  double allSCAsum = h->Integral();
                  std::vector<double> SCAsums;
                  SCAsums.clear();
                  int noSCAs = 0;
                  double low_sca_thresh = 0.2;
                  double high_sca_thresh = 2.5;

                  if ( nentries >= (2*16*nActiveCFEBs) )
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

                          if (cfeb_sca_sum == 0)
                            {
                              std::string diag=Form("CFEB No SCA Data: CFEB%d Layer%d", icfeb+1, ilayer);
                              dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL,"CSC_CFEB_NO_SCA_DATA"));
                              noSCAs++;
                              // std::cout << cscName << " " << diag << std::endl;
                            }
                        }
                    }

                  if ( nentries >= (10*16*nActiveCFEBs) )
                    {
                      if (nActiveCFEBs - noSCAs > 0)
                        { // Check that still active CFEBs present
                          double avg_sca_occupancy = allSCAsum/(nActiveCFEBs-noSCAs);
                          double avg_sca_ch_occupancy = avg_sca_occupancy/16;
                          // std::cout << cscName << "> avg SCA: " << avg_sca_occupancy << ", ";
                          for (int icfeb=0; icfeb < nCFEBs; icfeb++)
                            {
                              //		std::cout << "CFEB"<< (icfeb+1) << ": " << (double(SCAsums[icfeb])) << ", ";
                              // Avg. strip SCA occupancy > 5.
                              bool isLowEff = false;
                              if (SCAsums[icfeb])
                                {

                                  if ( (SCAsums[icfeb] < low_sca_thresh*avg_sca_occupancy) && (lowEffCFEBs[icfeb] != 1))
                                    {
                                      std::string diag=Form("CFEB Low SCA Efficiency: CFEB%d Layer%d (%.3f%% < %.1f%% from average)", icfeb+1, ilayer,
                                                            (SCAsums[icfeb]/avg_sca_occupancy)*100., low_sca_thresh*100 );
                                      dqm_report.addEntry(cscName, entry.fillEntry(diag, CRITICAL, "CSC_CFEB_SCA_LOW_EFF"));
                                      // std::cout << cscName << " "  << diag << std::endl;
                                      isLowEff = true;
                                    }

                                  if ( SCAsums[icfeb] >= high_sca_thresh*avg_sca_occupancy )
                                    {
                                      std::string diag=Form("CFEB Noisy/Hot CFEB SCAs: CFEB%d Layer%d (%.1f > %.1f times from average)", icfeb+1, ilayer,
                                                            SCAsums[icfeb]/avg_sca_occupancy, high_sca_thresh);
                                      dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL, "CSC_CFEB_SCA_NOISY"));
                                      // std::cout << cscName << " " << diag << std::endl;
                                    }

                                  for (int ch=1; ch <=16; ch++)
                                    {
                                      double ch_val = h->GetBinContent(ch+icfeb*16);
                                      if (ch_val > 4*avg_sca_ch_occupancy)
                                        {
                                          std::string diag = Form("CFEB Hot/Noisy SCA channel: CFEB%d Layer%d Ch#%d (occupancy %.1f times > average)", icfeb+1, ilayer, ch+icfeb*16,
                                                                  ch_val/avg_sca_ch_occupancy);
                                          dqm_report.addEntry(cscName, entry.fillEntry(diag,SEVERE, "CSC_CFEB_SCA_NOISY_CHANNEL"));
                                          //					std::cout << cscName << " " << diag << std::endl;
                                        }
                                      /*
                                        if (ch_val == 0 && !isLowEff && (lowEffCFEBs[icfeb] != 1)) {
                                        std::string diag = Form("Dead SCA channel: CFEB%d Layer%d Ch#%d", icfeb+1, ilayer, ch);
                                        dqm_report.addEntry(cscName, entry.fillEntry(diag,TOLERABLE));
                                        std::cout << cscName << " " << diag << std::endl;
                                        }
                                      */
                                    }
                                }
                            }
                          // std::cout << std::endl;
                        }
                    }
                }

            }
        }

      if  (nActiveCFEBs > 0)
        { // Expecting active CFEBs
          // -- CFEB Comparators Occupancies Checks
          for (int ilayer=1; ilayer<=6; ilayer++)
            {
              std::string name = Form("CLCT_Ly%d_Rate",ilayer);

              me = findME(CSC_folders[i], name , sourcedir);
              if (me)
                {
                  TH1F* h = dynamic_cast<TH1F*>(me);
                  int nentries = (int)h->GetEntries();
                  double allCompsum = h->Integral();
                  std::vector<double> Compsums;
                  Compsums.clear();
                  int noComps = 0;
                  double low_comp_thresh = 0.2;
                  double high_comp_thresh = 2.5;
                  // std::cout << cscName << ": " << ilayer << " "  << allCompsum << std::endl;

                  if ( nentries >= (32*nActiveCFEBs) )
                    {
                      //	  if (h->GetMaximum() > 0.1) {

                      // std::cout << cscName << " " << allCompsum << ", ";
                      for (int icfeb=0; icfeb < nCFEBs; icfeb++)
                        {

                          if (badCFEBs[icfeb] ==1 )
                            {
                              Compsums.push_back(0.0);
                              continue;
                            }// Skip dead CFEBs

                          double cfeb_comp_sum = h->Integral(icfeb*32+1, (icfeb+1)*32);
                          Compsums.push_back(cfeb_comp_sum);
                          // std::cout << "CFEB" << (icfeb+1) << ": " << cfeb_comp_sum << ", ";
                          if (cfeb_comp_sum == 0 && (lowEffCFEBs[icfeb] != 1))
                            {

                              std::string diag=Form("CFEB No Comparators Data: CFEB%d Layer%d", icfeb+1, ilayer);
                              dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL, "CSC_CFEB_NO_COMPARATORS_DATA"));
                              noComps++;
                              // std::cout << cscName << " " << diag << std::endl;
                            }

                        }
                      // std::cout << std::endl;
                    }



                  if ( nentries >= (5*32*nActiveCFEBs) )
                    {
                      if (nActiveCFEBs - noComps > 0)
                        { // Check that still active CFEBs present
                          double avg_comp_occupancy = allCompsum/(nActiveCFEBs-noComps);
                          double avg_comp_ch_occupancy = avg_comp_occupancy/16;
                          // std::cout << cscName << "> avg SCA: " << avg_sca_occupancy << ", ";
                          for (int icfeb=0; icfeb < nCFEBs; icfeb++)
                            {
                              if (Compsums[icfeb])
                                {

                                  if ( (Compsums[icfeb] < low_comp_thresh*avg_comp_occupancy) && (lowEffCFEBs[icfeb] != 1))
                                    {
                                      std::string diag=Form("CFEB Low Comparators Efficiency: CFEB%d Layer%d (%.3f%% < %.1f%% from average)", icfeb+1, ilayer,
                                                            (Compsums[icfeb]/avg_comp_occupancy)*100., low_comp_thresh*100 );
                                      dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL, "CSC_CFEB_COMPARATORS_LOW_EFF"));
                                      // std::cout << cscName << " "  << diag << std::endl;
                                    }

                                  if ( Compsums[icfeb] >= high_comp_thresh*avg_comp_occupancy )
                                    {
                                      std::string diag=Form("CFEB Hot/Noisy CFEB Comparators: CFEB%d Layer%d (%.1f > %.1f times from average)", icfeb+1, ilayer,
                                                            Compsums[icfeb]/avg_comp_occupancy, high_comp_thresh);
                                      dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL, "CSC_CFEB_COMPARATORS_NOISY"));
                                      //  std::cout << cscName << " " << diag << std::endl;
                                    }

                                  for (int ch=1; ch <=32; ch++)
                                    {
                                      double ch_val = h->GetBinContent(ch+icfeb*32);
                                      if (ch_val > 5*avg_comp_ch_occupancy)
                                        {
                                          std::string diag = Form("CFEB Hot/Noisy Comparator channel: CFEB%d Layer%d HStrip%d (occupancy %.1f times > average)",
                                                                  icfeb+1, ilayer, ch+icfeb*32,
                                                                  ch_val/avg_comp_ch_occupancy);
                                          dqm_report.addEntry(cscName, entry.fillEntry(diag,SEVERE, "CSC_CFEB_COMPARATORS_NOISY_CHANNEL"));
                                          // std::cout << cscName << " " << diag << std::endl;
                                        }
                                      /*
                                        if (ch_val == 0 && !isLowEff && (lowEffCFEBs[icfeb] != 1)) {
                                        std::string diag = Form("Dead SCA channel: CFEB%d Layer%d Ch#%d", icfeb+1, ilayer, ch);
                                        dqm_report.addEntry(cscName, entry.fillEntry(diag,TOLERABLE));
                                        std::cout << cscName << " " << diag << std::endl;
                                        }
                                      */
                                    }
                                }
                            }
                          // std::cout << std::endl;
                        }
                    }

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
              TH1F* h = dynamic_cast<TH1F*>(me);
              // if (h->GetEntries() > nWireGroups) {
              if (h->GetMaximum() > 0.1)
                {
                  for (int32_t iseg=0; iseg < nWireGroups/8; iseg++)
                    {
                      if (h->Integral(iseg*8+1+1, (iseg+1)*8+1) == 0)
                        {
                          int afeb = iseg*3+(ilayer+1)/2;
                          std::string diag=Form("ALCT No Anode Data: AFEB%d Layer%d", afeb, ilayer);
                          dqm_report.addEntry(cscName, entry.fillEntry(diag,CRITICAL, "CSC_ALCT_NO_ANODE_DATA"));
                        }
                    }

                  for (uint32_t hvseg=0; hvseg < hvSegMap.size(); hvseg++)
                    {
                      if (h->Integral(hvSegMap[hvseg].first+1, hvSegMap[hvseg].second+1) == 0)
                        {
                          std::string diag=Form("No HV: Segment%d Layer%d", hvseg+1, ilayer);
                          dqm_report.addEntry(cscName, entry.fillEntry(diag,SEVERE, "CSC_NO_HV_SEGMENT"));
                          // std::cout << cscName << " " << diag << std::endl;
                        }
                    }

                }
            }
        }


    }


  // == Check for chambers with format warnings (CFEB B-Words)
  me = findME("EMU", "DMB_Format_Warnings_Fract",  sourcedir);
  me2 = findME("EMU", "DMB_Format_Warnings",  sourcedir);
  if (me)
    {
      TH2F* h = dynamic_cast<TH2F*>(me);
      TH2F* h2 = dynamic_cast<TH2F*>(me2);
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
                if (fract >= 80.) severity=CRITICAL;
                else if (fract >= 10.) severity=SEVERE;
                else if (fract > 1.) severity=TOLERABLE;
                else severity=MINOR;

                std::string diag=Form("CFEB B-Words: %d events (%.3f%%)",events, fract);

                dqm_report.addEntry(cscName, entry.fillEntry(diag, severity, "CSC_WITH_BWORDS"));
              }

          }
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

  //  showReport();
  appBSem_.give();
  return 0;
}










