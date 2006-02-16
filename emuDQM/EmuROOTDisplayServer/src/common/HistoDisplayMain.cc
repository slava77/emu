// HistoDisplayMain.C
//  Main program for GUI of CDF Histogram Display.
//
//  1999 07 16 
//  Tetsuo Arisawa, Kouji Ikado, Kuni Kondo (Waseda Univ.)
//  Hans Wenzel (Karlsruhe)
//  Kaori Maeshima (Fermilab)
//
//  2001 05 19
//  Auto Connection to consumers as arguments such as
//  >HistoDisplayMain Consumer1 Consumer2
//

#include <iostream>

#include "TROOT.h"
#include "TApplication.h" 
#include "TGClient.h"
#include "TSocket.h"
#include <signal.h>
#include <sys/param.h>

#if defined SunOS
#include <netdb.h>             /* for SunOS <netinet/in.h> */
#endif

#include "HistoDisplay.hh"
#include "ServerProtocol.hh"

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
    signal(SIGINT,SIG_DFL);

    //std::cout << "open sockets" << std::endl;
    //gROOT->GetListOfSockets()->ls();
    TIter next(gROOT->GetListOfSockets());
    TSocket* socktmp = 0;
    while ( socktmp = (TSocket*)next() ) 
      {
	socktmp->Send( DspEndConnection.c_str() );
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



TROOT root("GUI", "GUI test environement");


int main(int argc, char **argv) 
{   
  signal(SIGABRT,abbruch);
  signal(SIGTERM,abbruch);
  signal(SIGSEGV,abbruch);
  signal(SIGTRAP,abbruch);
  signal(SIGKILL,abbruch);
  signal(SIGINT,abbruch);

  //signal(SIGPIPE,pipeBroken);
  
  //   TROOT root("GUI", "GUI test environement", initfuncs);
    TApplication theApp("App", &argc, argv);

    HistoDisplay Display(gClient->GetRoot(), 200, 400);

    if ( argc > 0 ) {

      for( int i = 1; i < argc ; i++ ) {

	//std::cout << " Consumer: " << argv[i] << std::endl;
	Display.AutoOpenConsumer( argv[i] );

      } //for( int i = 1; i < argc ; i++ )

    } //if ( argc > 0 )

    theApp.Run();

    return 0;

}


