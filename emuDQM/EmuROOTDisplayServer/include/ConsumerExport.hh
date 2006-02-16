//-------------------------------------------------------------------------
// File: ConsumerExport.hh
// Header file for ConsumerExport.cc
//
// The ConsumerExport class provides methods for the communication of the
// consumer with its display server.
//
// File history:
//
// 07/05/2000  Started class                         Wolfgang Wagner 
// 07/28/2000  Added benchmarking and CANVAS mode    Wolfgang Wagner 
//------------------------------------------------------------------------

#ifndef CONSUMEREXPORT_HH
#define CONSUMEREXPORT_HH 1

#include <cstdio>
#include <sys/types.h>
#include <time.h>

#include <iostream>

#include "TSocket.h"
#include "TServerSocket.h"
#include "TMessage.h"
#include "TMonitor.h"
#include "TList.h"

class TConsumerInfo;

class ConsumerExport
{
public:
  ConsumerExport(const int firstPort, const bool restartEnable=false, 
                 const int reportToStateManager=2, const bool logFileFlag=false,
                 const bool serverVerbose=false);
  ~ConsumerExport();
    
  int send(const char *messString);
  int send(TConsumerInfo *consinfo, Bool_t modFlag=kFALSE);
  pid_t getServerPid();
  static int getMaxString() {return _maxStringLength;}
  static void setMaxString(int newLength) {_maxStringLength = newLength;}

private:
  bool           _debug;
  bool           _restartEnable;
  bool           _sendingEnable;
  bool           _logFileFlag;
  bool           _serverVerboseFlag;
  int            _reportToStateManager;  
  int            _port;
  pid_t          _serverPid;
  time_t         _serverStartTime;
  static int     _maxStringLength;
  char*          _canvasString;
  TMonitor*      _consMonitor;
  TServerSocket* _consSS;
  TSocket*       _consSock;

  // Declare the copy constructor private, but don't define it.
  ConsumerExport(const ConsumerExport& ce);
  ConsumerExport& operator=(const ConsumerExport& ce);

  int send(TList *list);
  int send(const TObject *object); 
  void errorReact(const Int_t sendRes);
  void createCanvasString(TList *canvasList);
  int  lengthNeeded(TList *canvasList);
  int  startServer();
  int  connectServer();
  int  reestablishServer(); 
};

#endif

