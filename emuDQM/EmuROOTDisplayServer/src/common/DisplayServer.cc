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
// $Source: /afs/cern.ch/project/cvs/reps/tridas/TriDAS/emu/emuDQM/EmuROOTDisplayServer/src/common/DisplayServer.cc,v $
// $Revision: 1.3 $
// $Date: 2006/03/09 22:30:53 $
// $Author: barvic $
// $State: Exp $
// $Locker:  $
//*****************************************************************************

#include "DisplayServer.hh"

#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <cstring>

#include "TMessage.h"
#include "TString.h"

#include "MessageStorage.hh"
#include "SocketUtils.hh"
#include "ServerProtocol.hh"
#include "TH1.h"
#include "TCanvas.h"

using std::string;
using std::map;
using std::fstream;

static int numSigPipes = 0;

// "extern C" necesary to compile on LINUX and KCC 
extern "C"
{
  void pipeBroken(int sig)
  { 
    numSigPipes++;    
    std::cerr << "Display Server: Got signal SIGPIPE (" << sig << ")."  << std::endl;  
  }   
}


DisplayServer::DisplayServer(char* host, int consPort, TROOT* rt, 
  ConsumerList* cl, bool ut, bool wlf, bool verb) : 
  TNamed("DisplayServer","DisplayServer"),
  _hostName(host),
  _stateMonitorHostName("b0dap32.fnal.gov"), 
  _consumerPort(consPort), 
  _clientPort(9091), 
  _error(DS_OK), 
  _debugFlags(0x00),
  _updateTimeDiff(120000), 
  _useTimer(ut),
  _writeLogFile(wlf),
  _verboseFlag(verb), 
  _ss(0), 
  _conSock(0), 
  _currInfo(0), 
  _conList(cl), 
  _glRoot(rt), 
  _updateTimer(0)
{
  const std::string myName("DisplayServer::DisplayServer()");
  const std::string spaces("                              ");
 
  if (_writeLogFile) {
    std::string fileName("DS");
    fileName += std::string("_") + _hostName;
    char portString[100];
    sprintf(portString, "%d", _consumerPort);
    fileName += std::string("_") + std::string(portString);
    fileName += std::string(".log");
    _logFile.open(fileName.c_str(), std::ios::out);
  }
  // Set the signal handler for SIGPIPE.
  if ((int)signal(SIGPIPE, pipeBroken) == -1) 
    std::cerr << myName << ": ERROR setting signal handler for SIGPIPE." << std::endl;

  // Connect to the consumer server socket.
  // _conSock = new TSocket((TString)(_hostName.c_str()), _consumerPort);

  // If the consumer is running on b0dap68a, change the hostname reported
  // to b0dap68. This is a dirty fix to allow automated connection
  // from the HistoDisplay to SVTMon.
  // 24.01.2002 WW
  // Now we have to do the same thing with b0dap75. 05.12.2002 WW
  /*
  size_t dpos;
  if ( (dpos = _hostName.find("b0dap68a", 0)) != std::string::npos) {
    _hostName.replace(dpos, 8, "b0dap68");
  }   
  if ( (dpos = _hostName.find("b0dap75a", 0)) != std::string::npos) {
    _hostName.replace(dpos, 8, "b0dap75");
  }   

  if (_conSock->IsValid()) {
    std::cout << _clName << ": Connected successfully to CONSUMER socket at port "
         << _consumerPort << std::endl;
    std::cout << spaces << "on host " << _hostName << "." 
         << std::endl;
  }
  else {
    std::cerr << _clName << ": Consumer socket at port " << _consumerPort
         << " on host\n" << spaces << _hostName << " is not valid!" << std::endl;  
    _error = DS_NO_CONNECTION;
  } 
  
  if (_error < 0) ;
  else {
  
    _consMoni.Add(_conSock);
  */
    if (openClientServerSocket() < 0) {
      std::cerr << _clName << ": ERROR could not open server socket for clients."
	   << std::endl;
    }
  // }
  // The following 3 lines are included to prevent linking problems. -------
  TConsumerInfo* currInfo = new TConsumerInfo("Display Server", 1);
  delete currInfo;
  currInfo = 0;
  //------------------------------------------------------------------------
}

DisplayServer::DisplayServer(char* host, int consPort, int cltPort, TROOT* rt, 
  ConsumerList* cl, bool ut, bool wlf, bool verb) : 
  TNamed("DisplayServer","DisplayServer"),
  _hostName(host),
  _stateMonitorHostName("b0dap32.fnal.gov"), 
  _consumerPort(consPort), 
  _clientPort(cltPort), 
  _error(DS_OK), 
  _debugFlags(0x00),
  _updateTimeDiff(120000), 
  _useTimer(ut),
  _writeLogFile(wlf),
  _verboseFlag(verb), 
  _ss(0), 
  _conSock(0), 
  _currInfo(0), 
  _conList(cl), 
  _glRoot(rt), 
  _updateTimer(0)
{
  const std::string myName("DisplayServer::DisplayServer()");
  const std::string spaces("                              ");
 
  if (_writeLogFile) {
    std::string fileName("DS");
    fileName += std::string("_") + _hostName;
    char portString[100];
    sprintf(portString, "%d", _consumerPort);
    fileName += std::string("_") + std::string(portString);
    fileName += std::string(".log");
    _logFile.open(fileName.c_str(), std::ios::out);
  }
  // Set the signal handler for SIGPIPE.
  if ((int)signal(SIGPIPE, pipeBroken) == -1) 
    std::cerr << myName << ": ERROR setting signal handler for SIGPIPE." << std::endl;

  // Connect to the consumer server socket.
  // _conSock = new TSocket((TString)(_hostName.c_str()), _consumerPort);

  // If the consumer is running on b0dap68a, change the hostname reported
  // to b0dap68. This is a dirty fix to allow automated connection
  // from the HistoDisplay to SVTMon.
  // 24.01.2002 WW
  // Now we have to do the same thing with b0dap75. 05.12.2002 WW
  /*
  size_t dpos;
  if ( (dpos = _hostName.find("b0dap68a", 0)) != std::string::npos) {
    _hostName.replace(dpos, 8, "b0dap68");
  }   
  if ( (dpos = _hostName.find("b0dap75a", 0)) != std::string::npos) {
    _hostName.replace(dpos, 8, "b0dap75");
  }   

  if (_conSock->IsValid()) {
    std::cout << _clName << ": Connected successfully to CONSUMER socket at port "
         << _consumerPort << std::endl;
    std::cout << spaces << "on host " << _hostName << "." 
         << std::endl;
  }
  else {
    std::cerr << _clName << ": Consumer socket at port " << _consumerPort
         << " on host\n" << spaces << _hostName << " is not valid!" << std::endl;  
    _error = DS_NO_CONNECTION;
  } 
  
  if (_error < 0) ;
  else {
  
    _consMoni.Add(_conSock);
  */
    if (openClientServerSocket() < 0) {
      std::cerr << _clName << ": ERROR could not open server socket for clients."
	   << std::endl;
    }
  // }
  // The following 3 lines are included to prevent linking problems. -------
  TConsumerInfo* currInfo = new TConsumerInfo("Display Server", 1);
  delete currInfo;
  currInfo = 0;
  //------------------------------------------------------------------------
}


const std::string DisplayServer::_clName = std::string("DisplayServer");


DisplayServer::~DisplayServer()
{
  _logFile.close();
  if (_updateTimer != NULL) {
	_updateTimer->Stop();
	delete _updateTimer;
  }
  if (_conSock != NULL) {
	_conSock->Close();
	delete _conSock;
  }
  if (_ss != NULL) {
	if (_ss->IsValid()) {
		_ss->Close();
	}	
	delete _ss;
	_ss = NULL;
  }
  if (_conList != NULL) {
	delete _conList;
  }
/*
  if (_currInfo != NULL) {
	_currInfo->clear();
	delete _currInfo;
  }
*/
}


int DisplayServer::loop()
{
  //-----------------------------------------------------------------------
  // Indefinite loop: Polling for active sockets which have received messages.
  // Polling between consumer and client monitors alternates.
  const std::string myName("DisplayServer::loop()");
  int consStat = 1;
  while (1) {
/*
    consStat = pollConsumer();
    if (consStat < 0) {  // Exit in case of an protocol error.
      std::cerr << myName << ": ERROR: poll consumer returned with: "
           << consStat << std::endl;
      std::cerr << myName << ": Protocol between consumer and display server\n"
		<< "was not obeyed! Display Sever will exit!" << std::endl;
      return (0);
    }
    if (consStat == 2) {  // REgular exit.
      if (_verboseFlag) { 
        std::cerr << "Display Server: There is no active socket from the consumer.\n"
	     << "                " << "==> going to RETURN NOW !" << std::endl;
      }
      TIter allSock(gROOT->GetListOfSockets());
      TSocket* socktmp = 0;
      while ( socktmp = (TSocket*)allSock() ) {
	socktmp->Close();
	gROOT->GetListOfSockets()->Remove(socktmp);
      }
      return (0);
    } 
*/
    connectClient(); 
    pollClients();
  }
}


int DisplayServer::pollConsumer()
{
  // Return values:
  //   Error codes:
  //   -1 : Did not receive a valid message.
  //   -2 : Did not receive the TConsumerInfo.
  //   -3 : Protocol is not obeyed. Key word "Unmodified"
  //        was expexted.
  //   -4 : Socket selected is not the consumer socket.
  //   -5 : Protocol is not obeyed. Key word "CONSUMER SEND"
  //        was expected.
  //   Regular return codes:
  //    0 : No consumer monitor active.
  //    1 : Standard return at end of loop.
  //    2 : Do a regular exit.
  //    3 : Consumer send "finished" without any objects sent.
 
  const std::string myName("DisplayServer::pollConsumer()");
  const std::string spaces("                             ");

  static int consSendCycle=0;
  static int numConsPoll=0;   // Count the number of polls 
  static std::string oldNameString;
  TConsumerInfo* newInfo = 0;

  if (!_consMoni.GetActive()) {
    return 0;
  }

  TSocket* sock0 = _consMoni.Select(50);
  numConsPoll++;
  if (sock0 != (TSocket*)-1) {       
    if (sock0 && _conSock) {
      if (sock0->GetInetAddress().GetHostName() !=
	  _conSock->GetInetAddress().GetHostName()) {
        std::cerr << myName << ": Expected consumer socket to be selected."
             << std::endl;
	return -4;
      }
    }      
    // For debugging:
    // std::cout << "Returned with valid sock0. # of polls = " 
    //      << numConsPoll << std::endl;
    // Reset the number of polls if the connection was successful.
    numConsPoll=0;
    // Process message
    SocketUtils sockUt0(&_consMoni, sock0);
    std::string ctrlString = sockUt0.getStringMessage();
    // For debugging only:---------------------------
    // std::cout << _clName << ": Got string: " << ctrlString << std::endl;
    //-----------------------------------------------
    if (ctrlString == DspEndConnection) {
      std::string consumerName; 
      if (_currInfo) consumerName = std::string(_currInfo->GetTitle());
      else           consumerName = "Consumer";
      std::cout << myName << ": INFO: " << consumerName << " sent message: "
           << ctrlString << "\n" << spaces              
           << "==> Going to terminate connection to " << consumerName;
      std::cout << " running on\n" 
           << spaces << sock0->GetInetAddress().GetHostName() << std::endl;
      sockUt0.closeSocket();
      //---------------------------------------------
      _conList->setStatus(0,ConsumerList::Finished);
//      _conList->sendList(_stateMonitorHostName.c_str(), 9090);
      sleep(1);
      if (_ss) {
        _ss->Close();
        _glRoot->GetListOfSockets()->Remove(_ss);
        delete _ss;
        _ss = 0;
      }
      //---------------------------------------------       
      // Now there should be only the client sockets left.
      TIter next(_glRoot->GetListOfSockets());
      TSocket* socktmp = 0;
      while (socktmp = (TSocket*)next()) {
        socktmp->Send(DspEndConnection.c_str());
        sleep(1);
	socktmp->Close();
	_glRoot->GetListOfSockets()->Remove(socktmp);
      }
      return 2;
    }
    else {
      if (ctrlString == DspConsumerSend) {
        consSendCycle++;
        // For debugging:
        // std::cout << "Send Cycle: " << consSendCycle << std::endl; 
        _glRoot->DeleteAll();
        TMessage* conmess = 0;
        //---------------------------------------------------------
        // 2.) Receive the TConsumerInfo 
        sock0->Recv(conmess);  
        if (conmess) { 
          if (conmess->What() == kMESS_OBJECT) {
            TObject *rcvobject = (TObject*)conmess->ReadObject( 
                                 conmess->GetClass());
            // For debugging: -------------------------------
  	    // std::cout << _clName << ": objectname = " 
	    //     << rcvobject->GetName() << std::endl;
            //-----------------------------------------------
	    if(rcvobject->InheritsFrom("TConsumerInfo")) {
              newInfo = (TConsumerInfo*) rcvobject;
              if (newInfo) {
	        delete _currInfo;
	        _currInfo = newInfo;
	      }  
            }
	    else {
	      std::cerr << _clName << ": pollConsumer(): ERROR: "
	           << "Did not receive the TConsumerInfo!"
	           << std::endl;
	      return -2;
            }		
          }
          delete conmess;
	}  
        else {
          // Error:
          std::cerr << _clName << ": pollConsumer(): ERROR: "
	       << "Didn't receive a valid message." << std::endl;  	    
          return -1; 
        }  
        //---------------------------------------------------------
        // 3.) Receive the string with the object names.
        std::string namesString = sockUt0.getStringMessage();
        // For Debugging: 
        // std::cout << "Got names-String: " << namesString << std::endl;
        // If the list of objects is empty, ConsumerExport sends 
        // directly "CONSUMER FINISHED". In this case we simply 
        // return.
        if (namesString == DspConsumerFinish) {
          return 3;
	}
        if (strncmp(namesString.c_str(), "Modified", 8) == 0) {
          if (_debugFlags & 0x4) {
            std::cout << _clName << ": pollConsumer(): DEBUG INFO: "
                 << "Going to edit the storage list." << std::endl;
          }		
          editStorageList(namesString);
          setRequireUpdateBits(); 
        }
        else { 
          if (strncmp(namesString.c_str(), "Unmodified", 10) != 0) {
            std::cerr << _clName << ": pollConsumer(): ERROR: "
                 << "Expected key word 'Unmodified'." << std::endl;
	    return -3;
	  }
	}  
        //-----------------------------------------------------
        // 4.) Receive the objects expect to fill the list.
        if (_currList.GetSize() > 0) {
          TListIter storeIter(&_currList);
          Int_t     recvRes = 0;
          while (MessageStorage *mesStor = (MessageStorage*)storeIter()) {
            if (sock0) recvRes = sock0->Recv(conmess);
            if ((recvRes != -1) && (recvRes != 0)) { 
              if (conmess->What() == kMESS_OBJECT) {
                mesStor->updateMessage(conmess);
                // For Debugging:
                // std::cout << _clName << ": pollConsumer(): INFO: "
	        //      << "stored " << mesStor->GetName() << std::endl;
              }
              else {
                std::cerr << _clName << ": pollConsumer(): Expected an object."
                     << std::endl;
                continue;
              }
	    }
            else {
              std::cerr << _clName << ": ERROR reading object messages!" << std::endl;
              std::cerr << _clName << ": Closing socket!" <<  std::endl;
              sockUt0.closeSocket();
              continue;
	    }
            // Make sure that conmess does not point anymore to
            // the objects in the storage list.
            conmess = 0;   
          }
        }  
        else {
          std::cerr << _clName << ": pollConsumer(): Storage list empty."
               << std::endl;
        }
        //---------------------------------------------------------
        // 5.) Wait for the end string to be sent.
        std::string endString = sockUt0.getStringMessage();
        if (endString != DspConsumerFinish)
          std::cerr << _clName << ": pollConsumer(): ERROR: "
	       << "Expected different end string. Got " << endString
	       << std::endl;
        //----------------------------------------------------
        if (consSendCycle == 1) {
          // updateStateManager();
          if (!_updateTimer && _useTimer) {
            _updateTimer = new TTimer(this, _updateTimeDiff);
            _updateTimer->Reset();
            _updateTimer->TurnOn(); 
          }
        }
      }
      else {
        std::cerr << _clName << ": Got an unexpected string from the "
		  << "consumer. Expected: '" << DspConsumerSend 
		  << "'. Received: '" << ctrlString << "'." << std::endl;
        return -5;
      }
    } // else of if (string=="END CONNECTION")
    if (sockUt0.error() && (numSigPipes > 0)) {
      std::cerr << myName << ": Socket Error on the consumer side." << std::endl;
      std::cerr << "Number of SIGPIPES = " << numSigPipes << std::endl;
      std::cerr << "Going to exit!" << std::endl; 
      return 2;
    }
  } 

  return 1;
}


void DisplayServer::connectClient()
{
  // Check for requests by the display clients 
  TSocket* sock1=0;
  sock1 =  _ss->Accept();
  if (sock1 != (TSocket*)-1) {
    std::string clientHostname(sock1->GetInetAddress().GetHostName());
    int  clientPort = sock1->GetInetAddress().GetPort();
    std::cout << _clName << ": connectClient(): Client connected from " 
	 << clientHostname << " on port " << clientPort << "." << std::endl;
    _clientMoni.Add(sock1);
    char portChar[20];
    sprintf(portChar, "%d", clientPort);
    std::string id = clientHostname + std::string(":") + std::string(portChar);
    _clientMap[id] = false;
  }

  return;
}


void DisplayServer::pollClients()
{
  const std::string myName("DisplayServer::pollClients()");

  if (!_clientMoni.GetActive()) return;

  // wait for message 
  TSocket* sock2 = 0;
  sock2 = _clientMoni.Select(50);
  if (sock2 != (TSocket*)-1) {
    SocketUtils sockUt2(&_clientMoni, sock2);
    std::string request = sockUt2.getStringMessage();
    // For debugging purposes:
    if (_logFile.good()) {
      _logFile << _clName << ": Got requestString: " << request << std::endl;
      _logFile << "                Size = " << request.size() << std::endl;
    }
    if (request.size() == 0) {
      std::cerr << myName << ": request empty" << std::endl;
      if (_logFile.good()) {
        _logFile << myName << ": request empty" << std::endl;
      }
      return;
    }  
    if (request == "---") {
      std::cerr << myName << ": request = --- (probably socket not valid)." << std::endl;
      if (_logFile.good()) {
        _logFile << myName << ": request = --- (probably socket not valid)." 
		 << std::endl;
      }
      return;
    }
    if (request == DspEndConnection) {
      std::cout << myName << ": Terminating connection to "  
	   << sockUt2.getHostname() << "." << std::endl;
      sockUt2.closeSocket();
      _clientMap.erase(sockUt2.getId());
      return;
    }  
    if (request == "ConsumerInfo") {  
      TMessage infMess(kMESS_OBJECT);
      infMess.Reset();
      infMess.WriteObject(_currInfo);
      int sendRes = 0;
      sendRes = sock2->Send(infMess);
      if (sendRes == -1) {
	std::cerr << myName << "Error sending TConsumerInfo." << std::endl;
	sockUt2.closeSocket();
      }	
      if (_clientMap[sockUt2.getId()]) _clientMap[sockUt2.getId()] = false;      
    }
    else {
      // Assume the received requestString is a request for an object.
      // Get object by name and send it
      if (_clientMap[sockUt2.getId()]) {
        Int_t sendRes=0;
	sendRes = sock2->Send(DspRequestNewInfo.c_str());
        if ( sendRes == -1) {
          std::cerr << myName << ": ERROR: Socket->Send returned "
               << "with " << sendRes << ".\n" 
               << "               ==> The socket will be closed."
               << std::endl;
          // Close the socket in case of an error.
          sockUt2.closeSocket();
        }
      }        
      else {
        Bool_t objFound = kFALSE; 
        TListIter iter(&_currList);
        while (MessageStorage* obj = (MessageStorage*) iter()) {
          if (obj->GetName() == request) {
            objFound = kTRUE;  
            TMessage sendMess(kMESS_OBJECT);
            TMessage* storeMess = obj->getMessage();
            Int_t size = storeMess->BufferSize();
            // Int_t bufSize = size+200;  
            Int_t bufSize = size;
            char* buffer = new char[bufSize];
            // For debugging: 
            if (_logFile.good()) {            
              _logFile << myName << "Object: " << request << "  Buffer Size:" 
		       << storeMess->BufferSize() << std::endl;
	    }
            storeMess->Reset();
            storeMess->SetReadMode();
            storeMess->ReadBuf(buffer,bufSize);
            storeMess->Reset();
	    sendMess.Reset();
            sendMess.SetWriteMode();
	    sendMess.WriteBuf(buffer,bufSize);

            Int_t sendRes=0;
            sendRes=sock2->Send(sendMess);
            if ( sendRes == -1) {
              std::cerr << myName << ": ERROR: pollClients(): "
	           << "Socket->Send returned with " << sendRes << ".\n" 
                   << "               ==> The socket will be closed."
                   << std::endl;
              // Close the socket in case of an error.
              sockUt2.closeSocket();
            }
            // For debugging:
            if (_logFile.good()) {                
              _logFile << "SendRes = " << sendRes << std::endl;
	    }
            delete [] buffer;
            continue;
          }
        }	
        if (!objFound) {
          Int_t sendRes=0;
	  sendRes = sock2->Send(DspObjectNotFound.c_str());
          // std::cout << myName << "Object " << request 
	  //      << " not found in regular list." << std::endl;
          if ( sendRes == -1) {
            std::cerr << myName << ": ERROR: Socket->Send returned "
                 << "with " << sendRes << ".\n" 
                 << "               ==> The socket will be closed."
                 << std::endl;
            // Close the socket in case of an error.
            sockUt2.closeSocket();
          }
        }
      }          
    } // else of if (request == "ConsumerInfo")
  } // if (sock2 != (TSocket*)-1)
  
  return;
}


TConsumerInfo* DisplayServer::getCurrentInfo()
{
  return _currInfo;  
}


void DisplayServer::editStorageList(const std::string& nameString)
{
  const std::string myName("DisplayServer::editStorageList()");
  char* help1 = 0;
  char* help2 = 0;

  // Delete all objects from the current list of objects. 
  TListIter iter2(&_currList);
  while (MessageStorage* obj = (MessageStorage*) iter2()) {
    if (_logFile.good()) {
      _logFile << myName << ": deleting object:" << obj->GetName() << std::endl;
    }
    delete obj;
  }
  _currList.Clear();
  if (_currList.GetSize() > 0) {
    std::cerr << myName << ": list size should be 0." << std::endl;
    if (_logFile.good()) {
      _logFile << myName << ": list size should be 0." << std::endl;
    }
  }  
  // Extract the names of the Canvases, which are supposed to be separated
  // by $ signs.
  // Build the new list.
  //Find the first $:
  std::string cpy2Name(nameString);
  help1 = const_cast<char*>(strchr(cpy2Name.c_str(),'$'));
  while ((help2 = const_cast<char*>(strtok(help1, "$")))) {
    if (_logFile.good()) {
      _logFile << myName << ": Found String: " << help2 << std::endl;
    }  
    MessageStorage *storage = new MessageStorage(help2);
    _currList.AddLast(storage);    
    help1 = 0; 
  } 

  return;
}

void DisplayServer::updateStorage(MessageStorage* storage)
{
   const std::string myName("DisplayServer::updateStorage()"); 
//   MessageStorage* newitem = new MessageStorage(storage.GetName());
//   newitem->updateMessage(storage.getMessage());
   //cout << "Rick updating object:" << storage->GetName() << ":" << storage->getMessage()->BufferSize() << " : storage->getMessage()->GetClass() = " << storage->getMessage()->GetClass() << std::endl;
   if (TObject* item = _currList.FindObject(storage->GetName())) {
          if (_logFile.good()) 
            _logFile << myName << ": deleting object:" << item->GetName() << std::endl;
         _currList.Remove(item);
	 delete item;

   }
   _currList.AddLast(storage);
    if (_logFile.good()) 
         _logFile << myName << ": updating object:" << storage->GetName() << ":" << storage->getMessage()->BufferSize() << std::endl;
//   _currList.Sort();
    TObject* myobject = _currList.FindObject(storage->GetName());
    // myobject->ls();
    //cout << "Does that object inherit from TCanvas ?  " << myobject->InheritsFrom(TCanvas::Class()) << std::endl;
    //cout << "Does that object inherit from TH1 ?  " << myobject->InheritsFrom(TH1::Class()) << std::endl;
}

void DisplayServer::updateOutdatedList(const std::string& nameString)
{
  const std::string myName("DisplayServer::updateOutdatedList()");
  char* help1 = 0;
  char* help2 = 0;

  // Add those objects to "oldList" which are in list but not named
  // in nameString anymore.
  if (_debugFlags & 0x8) {
    std::cout << "Outdated Size = " << _outdatedList.GetSize() << "  list size = "
	 << _currList.GetSize() << std::endl;
    std::cout << myName << "Received the following name string: \n"
	 << nameString << std::endl;
  }  
  char* cpyName = new char[nameString.size()+1];
  TListIter iter1(&_currList);
  while (MessageStorage* obj = (MessageStorage*) iter1()) {
    Bool_t found = kFALSE;
    // For debugging:
    // std::cout << myName << ": " << obj->GetName() << std::endl;
    //Find the first $:
    // CASE 1 :
    // Check whether objects were removed from the object list by the
    // consumer, i.d.
    // the object is in the currList (list of objects maintained by 
    // the server), but is not in the object list sent by the server. 
    strcpy(cpyName, nameString.c_str());
    help1 = const_cast<char *>(strchr(cpyName,'$'));
    while ((help2 = const_cast<char*>(strtok(help1, "$"))) && (!found)) {
      if (obj->GetName() == std::string(help2)) found = kTRUE;	 
      help1 = 0; 
    }
    if (!found) {
      // CASE 1 = TRUE
      // std::cout << myName << ": Object " << obj->GetName() 
      //      << " from current list not in new list sent by the server."
      //      << std::endl;
      // CASE 1.1 
      // Check whether an object of this type exists already in
      // oldList.
      TListIter iterOld(&_outdatedList);
      Bool_t oldFound = kFALSE;
      while (MessageStorage* oldObj = (MessageStorage*) iterOld()) {       
        if (oldObj->GetName() == obj->GetName()) {
          // CASE 1.1 = TRUE
          // The object exists already in the outdated object list.
          // Therefore, we do not need to add a new entry in the 
          // list, but rather simply update the existing object. 
          // std::cout << myName << ": Object " << obj->GetName()
	  //      << " exists in outdated list." << std::endl;
	  oldObj->updateMessage(obj->getMessage());
	  oldFound = kTRUE;
	  continue;
        }      
      }
      // CASE 1.2 
      // The object does not exist in the outdated list. Therefore,
      // we have to add it.
      if (!oldFound) {
	std::cout << myName << ": INFO: The object '"
	     << obj->GetName() << "' was removed\nfrom the object list"
	     << " by the consumer.\nThe last copy is kept by the Display"
	     << " Server." << std::endl; 
	_outdatedList.AddLast(obj);
      }	
    }
  }
  delete cpyName;

  return;
}


void DisplayServer::updateStateManager()
{
  // Fill consumer list and send the list to the state manager. 

  const std::string myName("DisplayServer::updateStateManager()");
  const std::string spaces("                                   ");

  if (_currInfo) {
    _conList->addEntry(_currInfo->GetTitle(), (TString)(_hostName.c_str()),
		       _clientPort, _currInfo->nevents(), _currInfo->runnumber(),
            	       ConsumerList::Running);
    //conslist.print();
    if (_debugFlags & 0x10)
      std::cout << myName << ": Consumer information will be sent to\n"
		<< spaces << "the State Manager running on "
		<< _stateMonitorHostName << "." << std::endl;
    // Send list to StateManager
    _conList->sendList(_stateMonitorHostName.c_str(), 9090);
  }
  else {
    std::cerr << myName << ": Consumer Info not available." << std::endl;
    _conList->addEntry("Unknown", (TString)(_hostName.c_str()), _clientPort,
     	               0, 0, ConsumerList::Running);
    _conList->sendList(_stateMonitorHostName.c_str(), 9090);
  }

  return;
}


Bool_t DisplayServer::HandleTimer(TTimer* timer)
{
  // updateStateManager();

  return kTRUE;
}


void DisplayServer::setRequireUpdateBits()
{
  std::map<std::string,bool>::iterator it;
  for (it=_clientMap.begin(); it!=_clientMap.end(); ++it) {
    //std::string name = (*it).first;
    //_clientMap[name] = true;
    (*it).second = true;
  }
  return;
}


int DisplayServer::openClientServerSocket() 
{
  const std::string myName("DisplayServer::openClientServerSocket()");
  const std::string spaces("               ");

  int rtVal = DS_OK;

  // Open a server socket for the display clients to connect 
  _ss = new TServerSocket(_clientPort, kTRUE);
  // Open a server socket for the clients, testing different ports
  while (! _ss->IsValid()) {  
    delete _ss;
    _clientPort++;
    _ss = new TServerSocket(_clientPort, kTRUE);
  }
  if (!(_ss->IsValid())) {
    std::cout << myName << ": server socket not valid." << std::endl;
    _error = DS_BAD_CLIENT_SSOCKET;
    rtVal  = DS_BAD_CLIENT_SSOCKET;
  }
  else {    
    std::cout << barrier << "\n\n" 
         << myName << ": Server socket for HISTO DISPLAY" << std::endl;
    std::cout << spaces << "clients at PORT " << _clientPort << " available.\n" 
	 << std::endl; 
    std::cout << spaces << "Use   " << _hostName << ":" << _clientPort << std::endl; 
    std::cout << spaces << "as input stream to connect from the HistoDisplay" << std::endl;
    std::cout << spaces << "to the Display Server.\n\n"  
         << barrier << "\n" 
         << std::endl;
    // Disable blocking mode for this server socket  
    _ss->SetOption(kNoBlock,1);  
  }

  return rtVal;
} 
