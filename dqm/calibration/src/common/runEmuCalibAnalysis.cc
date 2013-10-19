#include <iomanip>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include <sys/resource.h>

#include "log4cplus/logger.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/helpers/appenderattachableimpl.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/helpers/pointer.h"
#include "log4cplus/spi/loggingevent.h"

#include "emu/dqm/common/coloredlayout.h"

// emu/emuDAQ - based readout for XDAQ6
// #include "EmuFileReader.h"
// #include "EmuSpyReader.h"

// emu/daq - based readout for XDAQ7
#include "emu/daq/reader/RawDataFile.h"
#include "emu/daq/reader/Spy.h"

#include "emu/dqm/calibration/Test_Generic.h"
#include "emu/dqm/calibration/Test_CFEB02.h"
#include "emu/dqm/calibration/Test_CFEB03.h"
#include "emu/dqm/calibration/Test_CFEB04.h"
#include "emu/dqm/calibration/Test_11_AFEBNoise.h"
#include "emu/dqm/calibration/Test_16_CFEBConnectivity.h"
#include "emu/dqm/calibration/Test_19_CFEBComparators.h"
#include "emu/dqm/calibration/Test_21_CFEBComparatorLogic.h"
#include "emu/dqm/calibration/Test_25_ALCTTrigger.h"
#include "emu/dqm/calibration/Test_AFEB05.h"
#include "emu/dqm/calibration/Test_AFEB06.h"
#include "emu/dqm/calibration/Test_AFEB07.h"
#include "emu/dqm/calibration/Test_GasGain.h"
#include "emu/dqm/calibration/Test_30_PipelineDepth.h"


using namespace log4cplus;
using namespace log4cplus::helpers;
using namespace log4cplus::spi;

// == Show usage info
void usage()
{
}


int main(int argc, char **argv)
{
  /*
    if (argc <2) {
                  std::cerr << argv[0] << ": Not enough parameters." << std::endl;
      usage();
                  exit(-1);
          }
  */

  Logger logroot = Logger::getRoot();
  Logger logger = Logger::getInstance("runEmuTests");

  SharedAppenderPtr appender(new ConsoleAppender());
  appender->setLayout( std::auto_ptr<Layout>(new SimpleColoredLayout()) );

  logroot.addAppender(appender);

  logger.setLogLevel(INFO_LOG_LEVEL);


  if (argc <2)
  {
    LOG4CPLUS_FATAL(logger, argv[0] << ": Not enough parameters.");
    usage();
    exit(-1);
  }


  uint32_t NumberOfEvents = 0xFFFFFFFF;
  uint32_t startEvent = 0;
  std::string cfgDir="";
  if (getenv("HOME") && string(getenv("HOME")).size())
    cfgDir=getenv("HOME"); //+"/config/";

  cfgDir+="/config";
  if (getenv("DQMCONFIG") && string(getenv("DQMCONFIG")).size()) cfgDir=getenv("DQMCONFIG");
  /*
    std::string xmlHistosBookingCfg = "http://cms-dqm03.phys.ufl.edu/dqm/results/emuDQMBooking.xml";
          std::string xmlCanvasesCfg = "http://cms-dqm03.phys.ufl.edu/dqm/results/emuDQMCanvases.xml";
    std::string cscMapFile = "/csc_data/results/csc_map.txt";
    std::string xmlTestCfg = "http://cms-dqm03.phys.ufl.edu/dqm/results/emuTests.xml";
    std::string masksCfg = "http://cms-dqm03.phys.ufl.edu/dqm/results/emuChanMasks.xml";
  */
  std::string testID=""; // test ID
  std::string xmlHistosBookingCfg = "file://" + cfgDir + "/emuDQMBooking.xml";
  std::string xmlCanvasesCfg = "file://" + cfgDir + "/emuDQMCanvases.xml";
  std::string cscMapFile = cfgDir+"/csc_map.txt";
  std::string xmlTestCfg = "file://" + cfgDir +"/emuTests.xml";
  std::string masksCfg = "file://" + cfgDir + "/emuChanMasks.xml";
  std::string datafile = "";
  std::string histofile = "dqm_results.root";
  std::string SQLiteDB = cfgDir+"/csc_map.db";
  
  std::string cscAFEBCalibFolder = cfgDir+"/afeb_thresholds/";
  
  //for test 19 (Test_19_CFEBComparators.cc)
  //std::string cscCFEBCalibFolder = cfgDir+"/cfeb_thresholds/";

  std::string plotsdir = "images";  // Output images path
  std::string imgFormat = "png";  // Output image format
  // uint32_t imgWidth = 1200;  // Output image width
  // uint32_t imgHeight = 800;  // Output image height

  uint32_t dduCheckMask=0xFFFFDFFF;
//        uint32_t binCheckMask=0xF7FB3BF6;
  uint32_t binCheckMask=0xFFFFFFFF;

  uint32_t node=0;

  // Try to set unlimited stack size (needed for Test CFEB04 - Gains)
  struct rlimit rl;
  int result;
  result = getrlimit(RLIMIT_STACK, &rl);
  if (result == 0)
  {
    rl.rlim_cur = RLIM_INFINITY;
    result = setrlimit(RLIMIT_STACK, &rl);
    if (result != 0)
    {
      fprintf(stderr, "setrlimit returned result = %d\n", result);
    }
  }

  switch (argc)
  {
  case 7:
    binCheckMask = strtoul(argv[6], NULL, 0);
  case 6:
    dduCheckMask = strtoul(argv[5], NULL, 0);
  case 5:
    startEvent = atoi(argv[4]);
  case 4:
    histofile = argv[3];
  case 3:
    NumberOfEvents = atoi(argv[2]);
  case 2:
    datafile = argv[1];
    break;
  }

  struct stat stats;
  if (stat(datafile.c_str(), &stats)<0)
  {
    LOG4CPLUS_FATAL(logger, datafile << ": " <<
                    strerror(errno));
    exit(-1);
  }


  emu::daq::reader::RawDataFile ddu(datafile.c_str(), emu::daq::reader::Base::DDU);
//  EmuFileReader ddu(datafile.c_str(), EmuReader::DDU);
  ddu.open(datafile.c_str());
  LOG4CPLUS_INFO (logger, "Opened data file " << datafile);

  LOG4CPLUS_INFO (logger,  "Reading Events from " << startEvent
                  << " to " << NumberOfEvents);



  /*  EmuTests test_analyzer;
    test_analyzer.setLogLevel(WARN_LOG_LEVEL);
    test_analyzer.setUnpackingDebug(false);
    test_analyzer.setUnpackingLogLevel(OFF_LOG_LEVEL);
    test_analyzer.setCSCMapFile(cscMapFile);
  */
//        test_analyzer.setXMLHistosBookingCfgFile(xmlHistosBookingCfg);
//        test_analyzer.setXMLCanvasesCfgFile(xmlCanvasesCfg);
//         test_analyzer.book();
  // Test_CFEB02 test_analyzer(datafile);
  Test_Generic* test_analyzer = NULL;

  if ( (datafile.find("Test_11") != std::string::npos) ||
            (datafile.find("test11") != std::string::npos) ||
	    (datafile.find("Test11") != std::string::npos) )
    {
      LOG4CPLUS_INFO(logger, "Detected data for Test 11: AFEB Counting Noise");
      test_analyzer = new Test_11_AFEBNoise(datafile);
      xmlTestCfg = "file://" + cfgDir + "/emuTest_11_AFEBNoise.xml";
      testID = "CFEB02";
    }
  else if ( (datafile.find("ALCT_Connectivity") != std::string::npos) ||
	    (datafile.find("Test_12") != std::string::npos) )
    {   
      LOG4CPLUS_INFO(logger, "Detected data for Test AFEB05: Connectivity");
      test_analyzer = new Test_AFEB05(datafile);
      xmlTestCfg = "file://" + cfgDir +"/emuTest_AFEB05.xml";
      testID = "AFEB05";
    }
  else if ( (datafile.find("ALCT_Thresholds") != std::string::npos) ||
	    (datafile.find("Test_13") != std::string::npos) )
    {
      LOG4CPLUS_INFO(logger, "Detected data for Test AFEB06: Thresholds and Analog Noise");
      test_analyzer = new Test_AFEB06(datafile);
      xmlTestCfg = "file://" + cfgDir +"/emuTest_AFEB06.xml";
      testID = "AFEB06";
    }
  else if ( (datafile.find("ALCT_Delays") != std::string::npos) ||
	    (datafile.find("test14") != std::string::npos) ||
	    (datafile.find("Test_14") != std::string::npos) )
    {
      LOG4CPLUS_INFO(logger, "Detected data for Test AFEB07: AFEB-ALCT Time Delays");
      test_analyzer = new Test_AFEB07(datafile);
      xmlTestCfg = "file://" + cfgDir +"/emuTest_AFEB07.xml";
      testID = "AFEB07";
    }
  else if ( (datafile.find("CFEB_SCAPed") != std::string::npos) ||
	    (datafile.find("Test_15") != std::string::npos) ||
	    (datafile.find("test15") != std::string::npos) )
    {
      LOG4CPLUS_INFO(logger, "Detected data for Test CFEB02: Pedestals and Noise");
      test_analyzer = new Test_CFEB02(datafile);
      xmlTestCfg = "file://" + cfgDir +"/emuTest_CFEB02.xml";
      testID = "CFEB02";
    }
  else if ( (datafile.find("Test_16") != std::string::npos) ||
            (datafile.find("test16") != std::string::npos) ||
	    (datafile.find("Test16") != std::string::npos) )
    {
      LOG4CPLUS_INFO(logger, "Detected data for Test 16: CFEB Connectivity");
      test_analyzer = new Test_16_CFEBConnectivity(datafile);
      xmlTestCfg = "file://" + cfgDir + "/emuTest_16_CFEBConnectivity.xml";
      testID = "16_CFEBConnectivity";
    }
  else if ( (datafile.find("CFEB_CrossTalk") != std::string::npos) ||
	    (datafile.find("Test_17_") != std::string::npos) )
    {
      LOG4CPLUS_INFO(logger, "Detected data for Test CFEB03: Pulse Response and Cross Talks");
      test_analyzer = new Test_CFEB03(datafile);
      xmlTestCfg = "file://" + cfgDir +"/emuTest_CFEB03.xml";
      testID = "CFEB03";
    }
  else if ( (datafile.find("CFEB_Gains") != std::string::npos) ||
	    (datafile.find("Test_17b") != std::string::npos) ||
	    (datafile.find("test17b") != std::string::npos) )
    {
      LOG4CPLUS_INFO(logger, "Detected data for Test CFEB04: Amplifier Gain");
      test_analyzer = new Test_CFEB04(datafile);
      xmlTestCfg = "file://" + cfgDir +"/emuTest_CFEB04.xml";
      testID = "CFEB04";
    }
  else if ( (datafile.find("Test_19") != std::string::npos) ||
            (datafile.find("test19") != std::string::npos) ||
	    (datafile.find("Test19") != std::string::npos) )
    {
      LOG4CPLUS_INFO(logger, "Detected data for Test 19: CFEB Comparator Thresholds and Noise");
      test_analyzer = new Test_19_CFEBComparators(datafile);
      xmlTestCfg = "file://" + cfgDir + "/emuTest_19_CFEBComparators.xml";
      testID = "19_CFEBComparators";
    }
  else if ( (datafile.find("CFEBComparatorLogic") != std::string::npos) ||
            (datafile.find("test21") != std::string::npos) ||
            (datafile.find("test_21") != std::string::npos) ||
            (datafile.find("Test_21") != std::string::npos) ||
	    (datafile.find("Test21") != std::string::npos) )
    {
      LOG4CPLUS_INFO(logger, "Detected data for Test 21: CFEB Comparator Logic (left/right half-strip) test");
      test_analyzer = new Test_21_CFEBComparatorLogic(datafile);
      xmlTestCfg = "file://" + cfgDir + "/emuTest_21_CFEBComparatorLogic.xml";
      testID = "21_CFEBComparatorLogic";
    }
  else if ( (datafile.find("Test_25") != std::string::npos) ||
            (datafile.find("test25") != std::string::npos) ||
	    (datafile.find("Test25") != std::string::npos) )
    {
      LOG4CPLUS_INFO(logger, "Detected data for Test 25: ALCT Self-Trigger");
      test_analyzer = new Test_25_ALCTTrigger(datafile);
      xmlTestCfg = "file://" + cfgDir + "/emuTest_25_ALCTTrigger.xml";
      testID = "25_ALCTTrigger";
    }
  //note that historically this was called Step 24, but the present implementation uses step27 files as input
  else if (datafile.find("STEP_27") != std::string::npos
	   || datafile.find("Test_27") != std::string::npos)
    {
      LOG4CPLUS_INFO(logger, "Detected data for Gas Gain Test");
      test_analyzer = new Test_GasGain(datafile);
      xmlTestCfg = "file://" + cfgDir +"/emuTest_GasGain.xml";
      testID = "GasGain";
    }
  else if ( (datafile.find("Test_30") != std::string::npos) ||
            (datafile.find("test30") != std::string::npos) ||
	    (datafile.find("Test30") != std::string::npos) )
    {
      LOG4CPLUS_INFO(logger, "Detected data for Test 30: Pipeline depth");
      test_analyzer = new Test_30_PipelineDepth(datafile);
      xmlTestCfg = "file://" + cfgDir + "/emuTest_30_PipelineDepth.xml";
      testID = "30_PipelineDepth";
    }
  else
    {
      LOG4CPLUS_FATAL (logger, "Unrecognizable Test Type for data file name " << datafile);
      return -1;
    }
  
  
  test_analyzer->setSQLiteDBName(SQLiteDB);
  test_analyzer->init();
  // test_analyzer->setCSCMapFile(cscMapFile); ///* Getting rid of csc_map.txt based mapping access to use only sqlite
  test_analyzer->setConfigFile(xmlTestCfg);
  test_analyzer->setMasksFile(masksCfg, testID);
  test_analyzer->setAFEBCalibFolder(cscAFEBCalibFolder);

  Logger::getInstance("CSCRawUnpacking").setLogLevel(OFF_LOG_LEVEL);

  if (datafile.find(".bin") != std::string::npos)
  {
    histofile = datafile;
    if (histofile.rfind("/") != std::string::npos)
      histofile.erase(0, histofile.rfind("/")+1);
    plotsdir = histofile;
    plotsdir = plotsdir.replace(plotsdir.find(".bin"), 4, ".plots");
    histofile = histofile.replace(histofile.find(".bin"), 4, ".root");
    test_analyzer->setOutDir(plotsdir);
    test_analyzer->setRootFile(histofile);
  }
  if (datafile.find(".raw") != std::string::npos)
  {
    histofile = datafile;
    if (histofile.rfind("/") != std::string::npos)
      histofile.erase(0, histofile.rfind("/")+1);
    plotsdir = histofile;
    plotsdir = plotsdir.replace(plotsdir.find(".raw"), 4, ".plots");
    histofile = histofile.replace(histofile.find(".raw"), 4, ".root");
    test_analyzer->setOutDir(plotsdir);
    test_analyzer->setRootFile(histofile);
  }



  // Try to extract Node ID from data file name (should match pattern EmuRUInn)
  if (datafile.find("EmuRUI") != std::string::npos)
  {
    std::string nodestr = datafile.substr(datafile.find("EmuRUI"), 8);
    nodestr.erase(0,6); // remove "EmuRUI"
    node = atoi(nodestr.c_str());
    LOG4CPLUS_INFO (logger, "Found Node ID " << node);
  }
  /*
    if (datafile.find(".root") != std::string::npos) {
      LOG4CPLUS_WARN (logger, "Load MEs from ROOT file " << datafile);
      histofile = datafile;
                  if (histofile.rfind("/") != std::string::npos)
                          histofile.erase(0, histofile.rfind("/")+1);
                  plotsdir = histofile;
      std::string runname = histofile;
      runname = runname.replace(runname.find(".root"), 5, "");
                  plotsdir = plotsdir.replace(plotsdir.find(".root"), 5, ".plots");
      test_analyzer.loadFromROOTFile(datafile);
      test_analyzer.saveCanvasImages(plotsdir.c_str(), imgFormat , imgWidth, imgHeight, runname);
      test_analyzer.generateLayout("csc-layouts.py", "EMU");
      return 0;
    }
  */
  /*
    test_analyzer.setHistoFile(histofile.c_str());
    if (dduCheckMask >= 0) {
            test_analyzer.setDDUCheckMask(dduCheckMask);
    }
    if (binCheckMask >= 0) {
      test_analyzer.setBinCheckMask(binCheckMask);
    }
  */
  long t0, t1;
  t0 = time(0);

  uint32_t cnt=0;

  while (ddu.readNextEvent())
  {
    cnt++;
    int status = 0;
    if ( ddu.getErrorFlag()==emu::daq::reader::RawDataFile::Type2 ) status |= 0x8000;
    if ( ddu.getErrorFlag()==emu::daq::reader::RawDataFile::Type3 ) status |= 0x4000;
    if ( ddu.getErrorFlag()==emu::daq::reader::RawDataFile::Type4 ) status |= 0x2000;
    if ( ddu.getErrorFlag()==emu::daq::reader::RawDataFile::Type5 ) status |= 0x1000;
    if ( ddu.getErrorFlag()==emu::daq::reader::RawDataFile::Type6 ) status |= 0x0800;
    /*
                    if( ddu.getErrorFlag()==EmuFileReader::Type2 ) status |= 0x8000;
                    if( ddu.getErrorFlag()==EmuFileReader::Type3 ) status |= 0x4000;
                    if( ddu.getErrorFlag()==EmuFileReader::Type4 ) status |= 0x2000;
                    if( ddu.getErrorFlag()==EmuFileReader::Type5 ) status |= 0x1000;
                    if( ddu.getErrorFlag()==EmuFileReader::Type6 ) status |= 0x0800;
    */

    if (status) continue;

    if ((cnt>=startEvent) && (cnt<=(startEvent+NumberOfEvents)))
    {
      LOG4CPLUS_DEBUG (logger, "Event#"<< dec << cnt << " **** Buffer size: " << ddu.dataLength() << " bytes");
      
      const char * dataModified = ddu.data();
      uint8_t header1;
      memcpy(&header1, dataModified, 1);
      header1 &= 0x0F;
      header1 |= (0x7 << 4);
      memcpy((void *)dataModified, &header1, 1);

      test_analyzer->analyze(dataModified, ddu.dataLength(), status, node);
      if (cnt%1000 == 0) LOG4CPLUS_INFO (logger, "Processed Events: "<< dec << cnt);
    }
    if (cnt+1>(startEvent+NumberOfEvents)) break;
  }
  t1 = time(0);

  int dtime = t1-t0;
  if (t1-t0==0) dtime=1;
  LOG4CPLUS_INFO (logger, "Total Events: " << test_analyzer->getTotalEvents() << ", Readout Rate: " << (test_analyzer->getTotalEvents()/dtime) << " Events/sec" );
  /*
    LOG4CPLUS_INFO (logger, "Good Events: " << test_analyzer.getGoodEventsCount() <<  ", Bad Events: " << test_analyzer.getBadEventsCount());
    LOG4CPLUS_INFO (logger, "Unpacked CSCs Events: " << test_analyzer.getTotalUnpackedCSCs() <<  ", Unpacking Rate: " << (test_analyzer.getTotalUnpackedCSCs()/(t1-t0)) << " CSCs/sec");
  */
  LOG4CPLUS_INFO (logger, "Run time: " << dtime << " seconds");

  test_analyzer->finish();

//  test_analyzer.saveToROOTFile(histofile.c_str());
  delete test_analyzer;
  ddu.close();

  return 0;
};

