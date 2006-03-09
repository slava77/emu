//-------------------------------------------------------------------------
// File: DisplayServer.hh
//
// The DisplayServer class comprises methods to receive monitoring data from
// the consumers, store them and send them to the HistoDisplay clients.
// The communication between the consumers, the DisplayServer and the
// HistoDisplay is implemented with TSockets.
//
// Author: Wolfgang Wagner 
//
//-------------------------------------------------------------------------
//****************************************************************************
// RCS Current Revision Record
//-----------------------------------------------------------------------------
// $Source: /afs/cern.ch/project/cvs/reps/tridas/TriDAS/emu/emuDQM/EmuROOTDisplayServer/include/DisplayServer.hh,v $
// $Revision: 1.2 $
// $Date: 2006/03/09 00:17:06 $
// $Author: barvic $
// $State: Exp $
// $Locker:  $
//*****************************************************************************
#ifndef DISPLAYSERVER_HH
#define DISPLAYSERVER_HH

#include <iostream>
#include <string>
#include <fstream>
#include <map>

#include "TServerSocket.h"
#include "TSocket.h"
#include "TMonitor.h"
#include "TList.h"
#include "TROOT.h"
#include "TTimer.h"
#include "TNamed.h"

// Include files from Consumer Framework
#include "TConsumerInfo.hh"
#include "ConsumerList.hh"
#include "MessageStorage.hh"
using namespace std;

const char barrier[] = 
  "===========================================================================";


class DisplayServer : TNamed
{
public:
  DisplayServer(char* host, int consPort, TROOT* rt, ConsumerList* cl, bool useTimer=true, bool writeLogFile=false, bool verbose=false);
  DisplayServer(char* host, int consPort, int cltPort, TROOT* rt, ConsumerList* cl, bool useTimer=true, bool writeLogFile=false, bool verbose=false);
  ~DisplayServer();

  enum DSErrorCode {
    DS_OK = 0,
    DS_NO_CONNECTION = -1,
    DS_BAD_CLIENT_SSOCKET = -2
  };
  // Error code explaination:
  // DS_NO_CONNECTION = It was not possible to connect to the server socket 
  //                    of the consumer.  

  int  loop();
  TConsumerInfo* getCurrentInfo();
  void setCurrentInfo(TConsumerInfo* info) { if (info) {_currInfo = info;}}
  void updateStorage(MessageStorage* storage); 
  int  errorFlag() const { return _error;}
		 
private:
  static const std::string _clName;
  std::string _hostName;
  std::string _stateMonitorHostName;
  int   _consumerPort;
  int   _clientPort;
  int   _error;
  int   _debugFlags;
  int   _updateTimeDiff;
  bool  _useTimer; 
  bool  _writeLogFile;
  bool  _verboseFlag;
  std::map<std::string,bool> _clientMap;
  TServerSocket* _ss;      // Server socket for the display clients.
  TSocket* _conSock;       // Socket for consumer connection. 
  TMonitor _consMoni;      // Monitor for the consumer socket.
  TMonitor _clientMoni;    // Monitor for the client sockets. 
  TList _currList;         // List to store the objects currently provided.
  TList _outdatedList;     // List of objects which are not provided anymore.
  TConsumerInfo* _currInfo;
  ConsumerList*  _conList; 
  TROOT*     _glRoot;      // Global Root
  TTimer*    _updateTimer; // ROOT timer for triggering the status update   
  std::ofstream   _logFile;

  // private methods
  int  pollConsumer();
  void connectClient();
  void pollClients();
  Bool_t HandleTimer(TTimer* timer);
  void editStorageList(const std::string& nameString);
  void updateOutdatedList(const std::string& nameString);
  void updateStateManager();
  void setRequireUpdateBits();
  int  openClientServerSocket(); 

  DisplayServer(const DisplayServer& ds);
  DisplayServer& operator=(const DisplayServer& ds);

};  
  
#endif
