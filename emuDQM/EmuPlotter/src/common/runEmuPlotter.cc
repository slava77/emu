///
///      @file  runEmuPlotter.cc
///     @brief  EmuPlotter program executable
///
/// Detailed description starts here.
///
///    @author  Victor Barashko (VB), Valdas Rapsevicius (VR)
///
///  @internal
///    Created  03/31/2008
///   Revision  ---
///   Compiler  gcc/g++
///    Company  CERN, CH
///  Copyright  Copyright (c) 2008
///
/// This source code is released for free distribution under the terms of the
/// GNU General Public License as published by the Free Software Foundation.
///=====================================================================================
///

// *******************************************************
// Include Section
// *******************************************************

// Application include section

#include "EmuFileReader.h"
#include "EmuSpyReader.h"
#include "EmuPlotter.h"

// System include section

#include <iomanip>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// Commons include section

#include "log4cplus/logger.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/helpers/appenderattachableimpl.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/helpers/pointer.h"
#include "log4cplus/spi/loggingevent.h"
#include "coloredlayout.h"
#include "tclap/CmdLine.h"

// ROOT includes
#include <TPRegexp.h>

// *******************************************************
// Namespace Section
// *******************************************************

using namespace log4cplus;
using namespace log4cplus::helpers;
using namespace log4cplus::spi;

// *******************************************************
// Local Definition Section
// *******************************************************

#define DEFAULT_NUMBER_OF_EVENTS  0xFFFFFFFF
#define DEFAULT_START_EVENT       0
#define DEFAULT_CONFIG_DIR_PREFIX "."
#define DEFAULT_CONFIG_DIR_SUFFIX "/config/"
#define FILE_PROTOCOL             "file://"
#define DEFAULT_HISTO_BOOK_FILE   "emuDQMBooking.xml"
#define DEFAULT_CANVAS_FILE       "emuDQMCanvases.xml"
#define DEFAULT_CSCMAP_FILE       "csc_map.txt"
#define IMG_FORMAT                "png"
#define IMG_WIDTH                 1200
#define IMG_HEIGHT                900
#define DEFAULT_DDU_MASK          0xFFFFDFFF
#define DEFAULT_BIN_MASK          0x16CBF3F6

// Function shortcuts
#define REMATCH(pat, str)         (TPRegexp(pat).MatchB(str))
#define REREPLACE(pat, str, rep)  { TString s(str); TPRegexp(pat).Substitute(s, rep); str = s; }

// *******************************************************
// Function Section
// *******************************************************

/**
 * @brief  EmuPlotter main function
 * @param  argc number of arguments
 * @param  argv pointer to the array of arguments
 * @return result code, 0 if no error
 */
int main(int argc, char **argv) {

  // Logger stuff initialization, terminal makeup
  Logger logroot = Logger::getRoot();
  Logger logger = Logger::getInstance("runEmuPlotter");
  SharedAppenderPtr appender(new ConsoleAppender());
  appender->setLayout( std::auto_ptr<Layout>(new SimpleColoredLayout()) );
  logroot.addAppender(appender);
  logger.setLogLevel(INFO_LOG_LEVEL);

  try {

    // Defining TCLAP CommandLine object
    TCLAP::CmdLine cmd("EMU Plotter application", ' ', "2.0");

    // Setting default parameters
    std::string cfgDir = DEFAULT_CONFIG_DIR_PREFIX;
    if (getenv("HOME") && string(getenv("HOME")).size()) cfgDir=getenv("HOME");
    cfgDir += DEFAULT_CONFIG_DIR_SUFFIX;

    // *******************************************************
    // Setting Command Line arguments
    // *******************************************************
    
    // for global configuration
    TCLAP::ValueArg<std::string> cfgDirArg("", "configdir", "[GLOBAL] Configuration directory", false, cfgDir, "path to dir");
    TCLAP::SwitchArg cfgOnlyArg("C","printcfg", "[GLOBAL] Print configuration and exit (useful for command line arguments debugging).", false);
    
    // for data/root file (unlabeled)
    TCLAP::UnlabeledValueArg<std::string> dataArg("datafile","[SWITCH] Event data or ROOT file to process. If no -d or -r flags are defined then application guesses on file extension: .root is considered to be root file, other - data file.", true, "", "path to data or ROOT file");
    TCLAP::SwitchArg dataSwitchArg("d","isdata", "[SWITCH] Explicitly defines that unlabeled item is the Event DATA file.", false);
    TCLAP::SwitchArg rootSwitchArg("r","isroot", "[SWITCH] Explicitly defines that unlabeled item is the ROOT file.", false);

    // for event data processing
    TCLAP::ValueArg<uint32_t> numberOfEventsArg("e","evnum","[DATA] Number of events to process", false, DEFAULT_NUMBER_OF_EVENTS, "number");
    TCLAP::ValueArg<uint32_t> startEventArg("s","evstart","[DATA] Start event number", false, DEFAULT_START_EVENT, "number");
    TCLAP::ValueArg<std::string> histoFileArg("","rootfile","[DATA] ROOT file where the histograms to be written to.", false, "", "path to file");
    TCLAP::ValueArg<uint32_t> dduMaskArg("","ddumask","[DATA] DDU check mask", false, DEFAULT_DDU_MASK, "double word integer");
    TCLAP::ValueArg<uint32_t> binMaskArg("","binmask","[DATA] Bin check mask, ex. 0xF778FFF6 ignores CFEB DAV error and Sample Count Error", false, DEFAULT_BIN_MASK, "double word integer");
    TCLAP::ValueArg<std::string> cfgBookArg("", "hbookfile", "[DATA] Histogram booking XML file", false, "", "path to file");
    TCLAP::ValueArg<std::string> cfgCSCMapArg("", "cscmapfile", "[DATA] CSC Map TXT file", false, "", "path to file");
    TCLAP::SwitchArg forwardRootArg("f","forwardroot", "[DATA] If set then forwards newly created ROOT file for image processing.", false);
    
    // for Root histograms processing
    TCLAP::ValueArg<std::string> filterArg("","filter","[ROOT] ROOT file folder filter", false, "", "string");
    TCLAP::ValueArg<std::string> cfgCanvasArg("", "canvasfile", "[ROOT] Histogram image canvas XML file", false, "", "path to file");
    TCLAP::ValueArg<std::string> cfgPlotsArg("", "plotsdir", "[ROOT] Path to save histogram images and browser DHTML application", false, "", "path to dir");

    // *******************************************************

    // Adding Command Line arguments
    cmd.add(dataArg);
    cmd.add(cfgOnlyArg);
    cmd.add(cfgDirArg);
    cmd.add(cfgBookArg);
    cmd.add(cfgCanvasArg);
    cmd.add(cfgCSCMapArg);
    cmd.add(numberOfEventsArg);
    cmd.add(startEventArg);
    cmd.add(filterArg);
    cmd.add(histoFileArg);
    cmd.add(dataSwitchArg);
    cmd.add(rootSwitchArg);
    cmd.add(dduMaskArg);
    cmd.add(binMaskArg);
    cmd.add(forwardRootArg);

    // Parse the argv array.
    cmd.parse(argc, argv);

    // Getting Command Line arguments
    cfgDir = cfgDirArg.getValue();
    if(!REMATCH("\\/$", cfgDir)) cfgDir += "/";

    std::string xmlHistosBookingCfg = cfgBookArg.getValue();
    if(!xmlHistosBookingCfg.size()) xmlHistosBookingCfg = FILE_PROTOCOL + cfgDir + DEFAULT_HISTO_BOOK_FILE;
    
    std::string xmlCanvasesCfg = cfgCanvasArg.getValue();
    if(!xmlCanvasesCfg.size()) xmlCanvasesCfg = FILE_PROTOCOL + cfgDir + DEFAULT_CANVAS_FILE;

    std::string cscMapFile = cfgCSCMapArg.getValue();
    if(!cscMapFile.size()) cscMapFile = cfgDir + DEFAULT_CSCMAP_FILE;
    
    uint32_t startEvent = startEventArg.getValue();
    uint32_t NumberOfEvents = numberOfEventsArg.getValue();

    std::string datafile = dataArg.getValue();
    std::string filter = filterArg.getValue();
    std::string histofile = histoFileArg.getValue();
    std::string plotsdir = cfgPlotsArg.getValue();

    bool isData = dataSwitchArg.getValue();
    bool isRoot = rootSwitchArg.getValue();
    bool forwardRoot = forwardRootArg.getValue();
    bool configOnly = cfgOnlyArg.getValue();

    uint32_t dduCheckMask = dduMaskArg.getValue();
    uint32_t binCheckMask = binMaskArg.getValue();

    // Determine type of the datafile (if not set or not correctly set explicitly!)
    if(isData == isRoot){
      if (REMATCH("\\.root$", datafile)) {
        isData = false;
        isRoot = true;
      }else{
        isData = true;
        isRoot = false;
      }
    }
    
    // Process some variables controlled by switches...
    if(isRoot){
      histofile = datafile;
    }else{
      if(!histofile.size()){
        histofile = datafile;
        REREPLACE("\\.[a-zA-Z0-9]*$", histofile, ".root");
      }
      if(forwardRoot) isRoot = true;
    }
    if(!plotsdir.size()) {
      plotsdir = histofile;
      REREPLACE("\\.[a-zA-Z0-9]*$", plotsdir, ".plots");
    }

    // FYI: Printing information about parameters to be used
    LOG4CPLUS_INFO(logger, "[GLOBAL] cfgDir = " << cfgDir);
    LOG4CPLUS_INFO(logger, "[GLOBAL] cfgOnly = " << boolalpha << configOnly);
    LOG4CPLUS_INFO(logger, "[SWITCH] dataFile = " << datafile);
    LOG4CPLUS_INFO(logger, "[SWITCH] isData = " << boolalpha << isData);
    LOG4CPLUS_INFO(logger, "[SWITCH] isRoot = " << boolalpha << isRoot);
    LOG4CPLUS_INFO(logger, "[DATA] startEvent = " << startEvent);
    LOG4CPLUS_INFO(logger, "[DATA] numberOfEvents = " << NumberOfEvents);
    LOG4CPLUS_INFO(logger, "[DATA] rootFile = " << histofile);
    LOG4CPLUS_INFO(logger, "[DATA] dduCheckMask = " << setbase(16) << dduCheckMask);
    LOG4CPLUS_INFO(logger, "[DATA] binCheckMask = " << setbase(16) << binCheckMask);
    LOG4CPLUS_INFO(logger, "[DATA] forwardRoot = " << boolalpha << forwardRoot);
    LOG4CPLUS_INFO(logger, "[DATA] cscMapFile = " << cscMapFile);
    LOG4CPLUS_INFO(logger, "[DATA] xmlHistosBookingCfg = " << xmlHistosBookingCfg);
    LOG4CPLUS_INFO(logger, "[ROOT] xmlCanvasesCfg = " << xmlCanvasesCfg);
    LOG4CPLUS_INFO(logger, "[ROOT] filter = " << filter);
    LOG4CPLUS_INFO(logger, "[ROOT] plotsDir = " << plotsdir);

    // If -C was set - exit after configuration arguments 
    // where printed on the screen...
    if(configOnly) return 0;

    // Determining if the source file size is larger than 0
    // and if this file exists and is accessible as well :)
    struct stat stats;
    if (stat(datafile.c_str(), &stats) < 0) {
      LOG4CPLUS_FATAL(logger, datafile << ": " << strerror(errno));
      exit(-1);  
    }

    // Creating and initializing EmuPlotter object
    EmuPlotter* plotter = new EmuPlotter();
    plotter->setLogLevel(WARN_LOG_LEVEL);
    plotter->setUnpackingDebug(false);
    plotter->setUnpackingLogLevel(OFF_LOG_LEVEL);
    plotter->setCSCMapFile(cscMapFile);
    plotter->setXMLHistosBookingCfgFile(xmlHistosBookingCfg);
    plotter->setXMLCanvasesCfgFile(xmlCanvasesCfg);
    plotter->book();

    // Data is to be processed! Lets move in...
    if(isData){

      // Creating EMU Raw File Reader and Opening File
      EmuFileReader ddu(datafile.c_str(), EmuReader::DDU);
      ddu.open(datafile.c_str());
      LOG4CPLUS_INFO (logger, "Opened data file " << datafile);
    
      // Try to extract Node ID from data file name (should match pattern EmuRUInn)
      uint32_t node = 0;
      if (REMATCH("EmuRUI[0-9]+", datafile)) {
        std::string nodestr = datafile;
        REREPLACE("EmuRUI([0-9]+)", nodestr, "$1");
        node = atoi(nodestr.c_str());
        LOG4CPLUS_INFO (logger, "Found Node ID " << node);
      }

      // Data specific plotter things to set
      plotter->setHistoFile(histofile.c_str());  
      if (dduCheckMask >= 0) plotter->setDDUCheckMask(dduCheckMask);
      if (binCheckMask >= 0) plotter->setBinCheckMask(binCheckMask);

      // We will count everything. So counters...
      long t0 = time(0);
      uint32_t cnt=0;

      // Looping through Events
      while (ddu.readNextEvent()) {
        cnt++;

        // Check the status of the reader. If something goes wrong - skip event
        int status = 0;
        if( ddu.getErrorFlag()==EmuFileReader::Type2 ) status |= 0x8000;
        if( ddu.getErrorFlag()==EmuFileReader::Type3 ) status |= 0x4000;
        if( ddu.getErrorFlag()==EmuFileReader::Type4 ) status |= 0x2000;
        if( ddu.getErrorFlag()==EmuFileReader::Type5 ) status |= 0x1000;
        if( ddu.getErrorFlag()==EmuFileReader::Type6 ) status |= 0x0800;
        if(status) continue;

        // Fill in histograms if appropriate
        if ( cnt >= startEvent && cnt <= (startEvent + NumberOfEvents) ) {
          LOG4CPLUS_DEBUG (logger, "Event#"<< dec << cnt << " **** Buffer size: " << ddu.dataLength() << " bytes");
          plotter->processEvent(ddu.data(), ddu.dataLength(), status, node);
          if (cnt%1000 == 0) LOG4CPLUS_INFO (logger, "Processed Events: "<< dec << cnt);
        }

        if ( (cnt + 1) > (startEvent + NumberOfEvents) ) break;  
      }

      // Provide some run stats
      long t1 = time(0);
      if ( (t1 - t0) > 0 ) {
        LOG4CPLUS_INFO (logger, "Total Events: " << plotter->getTotalEvents() << ", Readout Rate: " << (plotter->getTotalEvents()/(t1-t0)) << " Events/sec" );
        LOG4CPLUS_INFO (logger, "Good Events: " << plotter->getGoodEventsCount() <<  ", Bad Events: " << plotter->getBadEventsCount());
        LOG4CPLUS_INFO (logger, "Unpacked CSCs Events: " << plotter->getTotalUnpackedCSCs() <<  ", Unpacking Rate: " << (plotter->getTotalUnpackedCSCs()/(t1-t0)) << " CSCs/sec");
      }

      // Print out the list of histograms created
      std::map<std::string, ME_List >::iterator itr;
      LOG4CPLUS_INFO (logger, "List of MEs:");
      for (itr=plotter->GetMEs().begin(); itr != plotter->GetMEs().end(); ++itr) {
        LOG4CPLUS_INFO (logger, itr->first);
      }
      LOG4CPLUS_INFO (logger, "Run time: " << t1-t0 << " seconds");

      // Save and close the reader
      plotter->saveToROOTFile(histofile.c_str());
      ddu.close();

    }

    // Lets process ROOT file if appropriate
    if (isRoot) {
      
      LOG4CPLUS_INFO (logger, "Load MEs from ROOT file " << histofile);

      // Declaring and creating a name of the run (out of histofile)
      std::string runname = histofile;
      REREPLACE("\\.[a-zA-Z0-9]*$", runname, "");
      
      // Go go go
      plotter->convertROOTToImages(histofile, plotsdir.c_str(), IMG_FORMAT, IMG_WIDTH, IMG_HEIGHT, runname, filter);

    }

    // Remove plotter object
    delete plotter;

  } catch (TCLAP::ArgException &e) { 

    LOG4CPLUS_ERROR (logger, "error: " << e.error() << " for arg " << e.argId());
    return 1;

  }

  return 0;

};

