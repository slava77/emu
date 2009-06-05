/*
 * =====================================================================================
 *
 *       Filename:  runEmuDBImport.cc
 *
 *    Description:  MO Importer to Database application
 *
 *        Version:  1.0
 *        Created:  05/26/2009 02:22:11 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Valdas Rapsevicius (VR), Valdas.Rapsevicius@cern.ch
 *        Company:  CERN, CH
 *
 * =====================================================================================
 */

#include <sstream>
//#define HAVE_SSTREAM

#include <iostream>
#include <sys/resource.h>
#include <errno.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

#include "DQM/CSCMonitorModule/interface/CSCDQM_Utility.h"
#include "DQM/CSCMonitorModule/interface/CSCDQM_Logger.h"

#include "CSCDQMDB_Utility.h"
#include "DBImporter.h"
#include "tclap/CmdLine.h"

#define GETARG(n, t) dynamic_cast<TCLAP::t*>(arguments[n])->getValue()

typedef enum ArgType {
  DBCONFIGFILE,
  ROOTFILE,
  RUNNUMBER,
  LOGLEVEL
};

static LogLevel LogLevels [] = { 
  OFF_LOG_LEVEL, 
  FATAL_LOG_LEVEL, 
  ERROR_LOG_LEVEL, 
  WARN_LOG_LEVEL, 
  INFO_LOG_LEVEL, 
  DEBUG_LOG_LEVEL, 
  TRACE_LOG_LEVEL 
};

const static TPRegexp reRunNumber(".*([0-9]{5,}).*");

oracle::occi::Environment *env = 0;
oracle::occi::Connection *conn = 0;

void signal_handler(int sig) {
  switch(sig) {
    case SIGHUP:
    case SIGTERM:
      if (conn) {
        conn->rollback();
        env->terminateConnection(conn);
      }
      if (env)  oracle::occi::Environment::terminateEnvironment(env);
      exit(0);
  }
}

int main( int argc, char ** argv ) {
    
  appender->setLayout(std::auto_ptr<Layout>(new SimpleColoredLayout()));
  logroot.addAppender(appender);

  TCLAP::CmdLine cmd("EMU DQM data import to Database application", ' ', "1.0");
  std::map<ArgType, TCLAP::Arg*> arguments;

  std::string dbcfg = "", rootFile = "";
  if (getenv("HOME") && std::string(getenv("HOME")).size()) dbcfg = getenv("HOME");
  dbcfg += "/config/dqmdb.cfg";

  arguments[DBCONFIGFILE] = new TCLAP::ValueArg<std::string>("d", "dbcfg", "Database configuration file", false, dbcfg, "path to file");
  arguments[ROOTFILE]     = new TCLAP::UnlabeledValueArg<std::string>("rootfile", "ROOT file to import", true, "", "path to .root file");
  arguments[RUNNUMBER]    = new TCLAP::ValueArg<unsigned int>("r", "runnumber", "Run number. If not provided - file name is being parsed for run number", false, 0, "run number");
  arguments[LOGLEVEL]     = new TCLAP::ValueArg<uint32_t>("l", "loglevel", "[Log level: 0 - OFF_LOG_LEVEL, 1 - FATAL_LOG_LEVEL, 2 - ERROR_LOG_LEVEL, 3 - WARN_LOG_LEVEL, 4 - INFO_LOG_LEVEL, 5 - DEBUG_LOG_LEVEL, 6 - TRACE_LOG_LEVEL", false, 4, "number");
   
  for (std::map<ArgType, TCLAP::Arg*>::iterator it = arguments.begin(); it != arguments.end(); it++) { cmd.add(*(it->second)); }
  cmd.parse(argc, argv);
    
  dbcfg = GETARG(DBCONFIGFILE, ValueArg<std::string>);
  cscdqmdb::ConfigFile cfile(dbcfg);
  const std::string userName = cfile.getValue("username", "");
  const std::string password = cfile.getValue("password", "");
  const std::string connectString = cfile.getValue("connect", "");

  rootFile = GETARG(ROOTFILE, ValueArg<std::string>);

  logger.setLogLevel(LogLevels[GETARG(LOGLEVEL, ValueArg<uint32_t>)]);

  unsigned int runNumber = GETARG(RUNNUMBER, ValueArg<unsigned int>);
  if (runNumber == 0) {
    if (cscdqm::Utility::regexMatch(reRunNumber, rootFile)) {
      if (!cscdqm::stringToNumber(runNumber, cscdqm::Utility::regexReplaceStr(reRunNumber, rootFile, "$1"), std::dec)) {
        runNumber = 0;
      }
    }
  }
  LOG_INFO << "Run number = " << runNumber;
  if (runNumber == 0) {
    LOG_ERROR << "Run number can not be determined. Please provide it explicitly!";
    exit(1);
  }

  while (arguments.begin() != arguments.end()) {
    if (arguments.begin()->second) delete arguments.begin()->second;
    arguments.erase(arguments.begin());
  }

  signal(SIGCHLD, SIG_IGN); /* ignore child */
  signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
  signal(SIGTTOU, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGHUP, signal_handler); /* catch hangup signal */
  signal(SIGTERM, signal_handler); /* catch kill signal */


  env = oracle::occi::Environment::createEnvironment(oracle::occi::Environment::OBJECT);

  {
    
    conn = env->createConnection(userName, password, connectString);
    
    try {

      DBImporter importer(rootFile, runNumber, conn);
      importer.generate();
      importer.finalize();

    } catch (oracle::occi::SQLException& e) {
      if (conn) {
        conn->rollback();
        env->terminateConnection(conn);
      }
      if (env)  oracle::occi::Environment::terminateEnvironment(env);
      throw e;
    }

    if (conn) env->terminateConnection(conn);

  }

  if (env) oracle::occi::Environment::terminateEnvironment(env);
  
  return 0;

}

