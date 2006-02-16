//------------------------------------------------------------------------
//                             DISPLAY SERVER
//------------------------------------------------------------------------
//
// This program establishes a socket connection to a consumer which 
// provides monitoring information. The display server receives the
// data via the socket and stores it in its local ROOT. Display 
// clients also connect via sockets to the server and request particular 
// objects which are then send out to them by the server.
//
// Authors: Hartmut Stadie  (IEKP Karlsruhe)
//          Wolfgang Wagner (IEKP Karlsruhe)
// 
//------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/param.h>

#if defined SunOS
#include <netdb.h>             /* for SunOS <netinet/in.h> */
#endif

// Include files from ROOT
#include "TSystem.h"
#include "TApplication.h"

#include "DisplayServer.hh"

ConsumerList conslist("");
TROOT server("ServerROOT","Root of the Server");
// necesary to compile signal handler on LINUX and KCC 
extern "C"
{
  void abbruch(int sig)
  {
    //switch to old signalhandler to avoid loops!
    signal(SIGSEGV,SIG_DFL);
    signal(SIGABRT,SIG_DFL);
    signal(SIGTRAP,SIG_DFL);
    signal(SIGKILL,SIG_DFL);
    signal(SIGTERM,SIG_DFL);
  
    std::cout << "Display Server: End with signal " << sig << " ( = ";
    switch (sig) { 
      case SIGTERM:
	std::cout << "SIGTERM )";
	conslist.setStatus(0,ConsumerList::Dead);
        break;
      case SIGABRT:    
        std::cout << "SIGABRT )";
	conslist.setStatus(0,ConsumerList::Dead);
        break; 
      case SIGTRAP:
        std::cout << "SIGTRAP )";
	conslist.setStatus(0,ConsumerList::Crashed);
        break;
      case SIGSEGV:
        std::cout << "SIGSEGV )";
	conslist.setStatus(0,ConsumerList::Crashed);
        break;
      case SIGINT:
        // This should not occur anymore: SIG_IGN defined as signal 'handler'
	std::cout << "SIGINT )";
	break;
      case SIGKILL:
	std::cout << "SIGKILL )";
	conslist.setStatus(0,ConsumerList::Dead);
	break;
      case -1:
	std::cout << "-1 )" ;
	conslist.setStatus(0,ConsumerList::Finished);
	break;
      case SIGPIPE:
        // This should not occur anymore: SIG_IGN defined as signal 'handler'
	std::cout << "SIGPIPE )";
	break;
      default:    
        std::cout << "??? )";
	conslist.setStatus(0,ConsumerList::Unknown);
        break;
      }
    std::cout << std::endl;
    std::cout << "Report end to the StateManager" << std::endl;
    conslist.sendList("b0dap30.fnal.gov",9090); 
    //std::cout << "open sockets" << std::endl;
    // gROOT->GetListOfSockets()->ls();
    TIter next(gROOT->GetListOfSockets());
    TSocket* socktmp = 0;
    while ( socktmp = (TSocket*)next() ) 
      {
	socktmp->Close();
	gROOT->GetListOfSockets()->Remove(socktmp);
	//    // is called by close
      }
    std::cout << "sockets closed" << std::endl;
    // gROOT->GetListOfSockets()->ls();
    //conslist.print();
    exit(sig);
  }
}



int main(int argc, char **argv)    
{
  const std::string myName("Display Server");
  bool useRootTimer = true;
  bool verboseFlag  = false;
  bool logFileFlag  = false;
  int consumerPort=9050;
  int timerFlag = 2;
  // timerFlag = 0  => Do not use any timer.
  // timerFlag = 1  => Use the SIGALRM timer.
  // timerFlag = 2  => Use the ROOT timer.    

  signal(SIGABRT,abbruch);
  signal(SIGTERM,abbruch);
  signal(SIGINT,SIG_IGN);
  signal(SIGSEGV,abbruch);
  signal(SIGTRAP,abbruch);
  signal(SIGKILL,abbruch);
  signal(SIGPIPE,SIG_IGN);
  signal(SIGALRM,SIG_IGN);

  //-----------------------------------------------------------------------
  // Analyse the command line arguments.
  if (argc<2) {
    std::cerr << "Display server: There was no consumer port number given in "
         << "the command line\n"
         << "                " << "==>> RETURN NOW !" << std::endl;
    exit(-1);
  }  
  if (!(consumerPort=atoi(argv[1]))) {
    std::cerr << "Display Server: Command line argument argv[1] is not an integer.\n"
         << "==>> RETURN NOW !" << std::endl;
    exit(-2);
  }
  if (argc > 2) {
    timerFlag = atoi(argv[2]);
    if (timerFlag == 2) useRootTimer = true;
    else                useRootTimer = false;
  }

  // TApplication recognizes the following options: 
  //  -? : help
  //  -h : help
  //  -b : run in batch mode without graphics
  //  -n : do not execute logon and logoff macros as specified in .rootrc
  //  -q : exit after processing command line macro files
  //  -l : do not show splash screen
  //
  // TApplication removes the recognized options from the option list.
  TApplication app("Server",&argc,argv);

  int c=0;
  int nArg = argc;
  if (argc>=4) {
    argv++; // Skip the first argument
    argv++; // Skip the second argument
    while (--nArg > 2 && (*++argv)[0] == '-') {
      while (c = *++argv[0])
        switch (c) {
	case 'v':
          std::cout << myName << ": Setting VERBOSE mode!" << std::endl;  
          verboseFlag = true;
          break;
        case 'f':
          std::cout << myName << ": Writing display server log file!" << std::endl;
          logFileFlag = true;
          break;
        case 'o':
          std::cout << "Display Server Options" << std::endl;
          std::cout << "======================" << std::endl;
          std::cout << "Usage: Server <portNumber> [-v][-f][-o][-d]" << std::endl;
          std::cout << "v : VERBOSE mode" << std::endl;
          std::cout << "f : write log file" << std::endl;
          std::cout << "o : Print out options" << std::endl;
          std::cout << "d : dummay option" << std::endl; 
          break;    
        case 'd':
          // Dummy option: Do not do anything. Use to allow fixed number
          // of options when starting Server from ConsumerExport.
          break;
        default:
          std::cout << "Illegal option " << c << std::endl;
          argc = 1;
          break;   
        }    
    }
  }

  gROOT->cd();  

  // The following assumes that the display server runs on the same
  // machine as the consumer itself.
  char hostName[MAXHOSTNAMELEN];
  gethostname(hostName, MAXHOSTNAMELEN); 

  DisplayServer ds(hostName, consumerPort, gROOT, &conslist, useRootTimer, 
                   logFileFlag, verboseFlag);
  if (ds.errorFlag() < 0) {
    std::cerr << myName << ": Failure when instantiating the DisplayServer object.\n"
	 << "==>> EXIT NOW !" << std::endl;
    exit(-1);
  }
  int loopRes = 0;
  loopRes = ds.loop();
  exit(loopRes);  
}

