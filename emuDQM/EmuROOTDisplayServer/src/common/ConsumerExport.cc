//-------------------------------------------------------------------------
// File: ConsumerExport.cc
//
// The ConsumerExport class provides methods for the communication of the
// consumer with its display server.
//
// 07/05/2000 Started    Wolfgang Wagner 
//
//-------------------------------------------------------------------------
//****************************************************************************
// RCS Current Revision Record
//-----------------------------------------------------------------------------
// $Source: /afs/cern.ch/project/cvs/reps/tridas/TriDAS/emu/emuDQM/EmuROOTDisplayServer/src/common/ConsumerExport.cc,v $
// $Revision: 1.1 $
// $Date: 2006/02/16 00:05:14 $
// $Author: barvic $
// $State: Exp $
// $Locker:  $
//*****************************************************************************

// Comment on Error Handling:
// If a socket->send fails the socket is closed immediately.

#include "ConsumerExport.hh"

#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

#include <string>

#include "TConsumerInfo.hh"
#include "ServerProtocol.hh"
#include "SocketUtils.hh"

using std::string;

// #define CONSUMEREXPORT_DEBUG

const int serverStartFailCode = 20;  
static pid_t serverPid = 0;
static bool serverStartFailed = false;

extern "C"
{
  static void sigChildHandler(int sig)
  {
    const string myName = string("ConsumerExport#sigChildHandler");
    bool locDebug = false;
#ifdef CONSUMEREXPORT_DEBUG
    locDebug = true;
#endif

    if (locDebug) {
      std::cout << myName << ": Got signal ";
      if (sig == SIGCHLD) std::cout << "SIGCHLD (";
      std::cout << "= " << sig << ")." << std::endl;
    }
    pid_t waitRes;
    int status;
    waitRes = waitpid(serverPid, &status, WNOHANG);         
    if (locDebug)  std::cout << myName << ": Returned process: pid = " 
			<< waitRes << "." << std::endl;
    if (waitRes == serverPid) {
      // The returned process was the display server. 
      if (WIFEXITED(status)) {
        if (locDebug) std::cout << myName << ": Normal termination, exit status = " 
        	           << WEXITSTATUS(status) << std::endl;
        if (WEXITSTATUS(status) == serverStartFailCode) {
          std::cerr << myName <<  ": Failed to start the Server. The forked\n"
	       << "child process could not find the Server program."
	       << std::endl;
          serverStartFailed = true;
	}
      } 
      else {
        if (WIFSIGNALED(status)) {
          std::cerr << myName << ": Abnormal termination, signal number = "
	       << WTERMSIG(status) << 
#ifdef WCOREDUMP
	  WCOREDUMP(status) ? ". Core file generated." : ".";
#else
	  "."; 
#endif
	  std::cout << std::endl;
        }
      }
    }
    else {
      if (locDebug) std::cout << "Process other than the server returned." << std::endl;
    }
 
    return;
  }
}

ConsumerExport::ConsumerExport(const int firstPort=9050, 
  const bool restartEnable, const int reportToStateManager, const bool logFFlag,
  const bool servVerbose) : 
  _debug(false), 
  _restartEnable(restartEnable), 
  _sendingEnable(true),
  _logFileFlag(logFFlag),
  _serverVerboseFlag(servVerbose),
  _reportToStateManager(reportToStateManager), 
  _port(firstPort), 
  _serverPid(1), 
  _canvasString(0), 
  _consMonitor(new TMonitor), _consSS(0), _consSock(0)       
{
  const int maxPort=9999;
  const string myName = string("ConsumerExport::Constructor");
  
#ifdef CONSUMEREXPORT_DEBUG
    _debug = true;
#endif

  if ((int)signal(SIGCHLD, sigChildHandler) == -1) 
    std::cerr << myName << ": ERROR setting signal handler for SIGCHLD." 
	      << std::endl;
  SocketUtils:: installSIGPIPEHandler();

  _consSS      = new TServerSocket(_port,kTRUE);
  // Open server socket, Testing different ports  
  while( (!_consSS->IsValid()) && (_port < maxPort) ) {  
    delete _consSS;
    _port++;
    _consSS = new TServerSocket(_port,kTRUE);
    // We use the default blocking sockets for the consumer-server
    // connection.
  }
  if(_port == maxPort) {
    std::cerr << myName << ": Maximum port number reached. No free port "
         << "was found."
	 << " ==>> RETURN NOW ! " << std::endl;   
    _sendingEnable = false;
  }
  if (_consSS) {
    if (!_consSS->IsValid()) {
      std::cerr << myName << ": Server socket is not valid." << std::endl;
      _sendingEnable = false;
      delete _consSS;
      _consSS = 0;
    }  
    else {
      if ( (startServer() > 0) && (!serverStartFailed) ) connectServer();
      else {
        std::cerr << myName << ": ERROR: Failed to start the server program."  
             << std::endl;  
        std::cerr << myName << ": The Consumer will continue without sending "
	     << "monitoring information to the server." << std::endl;
        _sendingEnable = false;
        delete _consSS;
        _consSS = 0;
      }
    }
  }
  else {
    _sendingEnable = false;
  }
  if (_debug) std::cout << myName << ": End of Constructor." << std::endl;
}


ConsumerExport::~ConsumerExport()
{
  if (_consSock) {
    _consSock->Send(DspEndConnection.c_str());
    _consSock->Close();
  }

  delete [] _canvasString;

}


int ConsumerExport::_maxStringLength=1000;


int ConsumerExport::send(TConsumerInfo *consinfo, Bool_t modFlag)
{ 
  // This method sends out the consumer info and the whole list of objects 
  // stored in consumer info via the socket to the display server. 
  // The protocol with a leading "CONSUMER SEND"
  // and a trailing "CONSUMER FINISHED" is implemented.
  //
  // Return value: number of messages which were successfully sent
  //               If the socket is no longer valid: -1
  //
  // If the number of consequetively failed messages exceeds a limit 
  // (_failedLimit) the socket is closed. As a consequence IsValid
  // returns false and the send-method returns -1.
  //   
  const string myName = string("ConsumerExport::send()");
  
  static Bool_t firstCall=kTRUE;
  static int    sendErrors=0;
  int rtvalue=0; 
  Int_t sendRes=0;
 
  // std::cout << myName << ": begin of function." << std::endl;
  if (_sendingEnable) {
    if (_consSock && _consSock->IsValid()) { 
      TMessage mess(kMESS_OBJECT);
      mess.Reset();
      SocketUtils sockUt(_consMonitor, _consSock);
      //-----------------------------------------------------
      // 1.) Send start key word "CONSUMER SEND"
      if (!sockUt.sendStringMessage(DspConsumerSend)) {
        sendErrors++;
        return -4;
      } 
      //-----------------------------------------------------
      // 2.) send the consumer info
      if (_consSock && _consSock->IsValid()) {
        if (consinfo) {
          mess.Reset();
          mess.WriteObject(consinfo);
          sendRes = _consSock->Send(mess);
          if (sendRes == -1) {
            errorReact(sendRes);
            sendErrors++;
            return -5;
          }
          else {
            rtvalue++;
          }
        }
        else {
          std::cerr << myName << ": Pointer to TConsumerInfo is NULL." << std::endl;
          return -3;
        } 
      }
      //-----------------------------------------------------
      // 3.) send all objects in the TConsumerInfo list of objects
      if ((consinfo->list())->GetSize() > 0) {
        char* ctrlString;
        if (firstCall || modFlag || consinfo->isModified()) {
	  // Set the signal handler for SIGPIPE in the first event.
          // We need to overwrite the handler set by others.
	  if (firstCall) SocketUtils::installSIGPIPEHandler();
          // Create a list of all objects to be sent. 
          // Find out whether the default string length for the canvases
          // is large enough. 
          int actualLength=lengthNeeded(consinfo->list());
          if (_maxStringLength < actualLength) {
            _maxStringLength = actualLength;       
          }
          createCanvasString(consinfo->list());
          firstCall  = kFALSE;
          ctrlString = _canvasString;
          consinfo->setModified(false);
        }
        else {
          ctrlString = "Unmodified";
        }	
        if (_consSock && _consSock->IsValid()) {
          mess.Reset();
          // 4.) Send the string with the objects' name
          sendRes=_consSock->Send(ctrlString);
          if (sendRes == -1) {
            errorReact(sendRes);
            sendErrors++;
            return -6;
	  }
          else {
            rtvalue++;
	  }
        }  
        send(consinfo->list());
      }
      //-----------------------------------------------------
      // 5.) Send the finish string 
      mess.Reset();
      if (!sockUt.sendStringMessage(DspConsumerFinish)) {
	sendErrors++;
	return -7; 
      }
    }
    else {
      std::cerr << myName << ": ERROR: Socket to display server is no "
           << "longer valid." << std::endl;
      // Here we introduce a cut off on the number of sendErrors.
      // If the number of sendErrors is above a threshold of 50,
      // we do not restart the server anymore. 
      if (_consSS && _restartEnable && (sendErrors < 50)) {
        // It makes only sense to try to reconnect if the consumer server
        // socket is still there.  
        if (!serverStartFailed) reestablishServer();
        firstCall = kTRUE;
      }
      else {
        // Disable the sending. 
        _sendingEnable = false;
      }
      rtvalue = -1;         
    }
  }
  else { // Sending is disabled.
    rtvalue = -2;
  } 

  return rtvalue;
}


pid_t ConsumerExport::getServerPid()
{
  return _serverPid;
}


int ConsumerExport::send(TList *objlist)
{ 
  // This method sends out a whole list of objects via the socket 
  // to the display server.
  //
  // Return value: number of objects in the list which were sent
  //               If the socket is no longer valid: -1
  //
  // If the number of consequtively failed messages exceeds a limit 
  // (_failedLimit) the socket is closed. As a consequence IsValid
  // returns false and the send-method returns -1.
  // 
  int rtvalue=0; 
  Int_t sendRes=0;

  if (_sendingEnable) {
    if (_consSock && _consSock->IsValid()) { 
      TMessage mess(kMESS_OBJECT);
      //-----------------------------------------------------
      TListIter iter(objlist);
      while (TObject *obj = iter()) {
	if (_consSock && _consSock->IsValid()) { 
	  mess.Reset();
	  mess.WriteObject(obj);
	  sendRes=_consSock->Send(mess);
	  if (sendRes == -1) errorReact(sendRes);
	  else {
	    rtvalue++;
	  } 
	}    
      }
    }
    else {
      std::cerr << "ConsumerExport: ERROR: Socket to display server is no "
	   << "longer valid." << std::endl;
      rtvalue = -1;         
    } 
  }
  else { // Sending is disabled.
    rtvalue = -2;
  } 

  return rtvalue;
}

int ConsumerExport::send(const char *messString)
{
  // This method simply sends out a string to the consumer.
  //
  // Return value:  1 if no error occured
  //                0 if the send of the string failed
  //               -1 if the socket is no longer valid
  // 
  int rtvalue=0;  
  Int_t sendRes=0;

  if (_sendingEnable) {
    if( _consSock && _consSock->IsValid()) { 
      sendRes=_consSock->Send(messString);
      if (sendRes == -1) errorReact(sendRes);
      else {
	rtvalue++;
      } 
    }
    else {
      std::cout << "ConsumerExport: ERROR: Socket to display server is no "
	   << "longer valid." << std::endl;
      rtvalue = -1;         
    } 
  }
  else { // Sending is disabled.
    rtvalue = -2;
  } 

  return rtvalue;
}

int ConsumerExport::send(const TObject *object)
{
  // This method sends a single objects via the socket to the display server.
  // The protocol with a leading "CONSUMER SEND" and a trailing 
  // "CONSUMER FINISHED" is implemented.
  //
  // Return value:  1 if no error occured
  //                0 if the send failed       
  //               -1 if the socket is no longer valid
  int rtvalue=0; 
  Int_t sendRes=0;

  if (_sendingEnable) {
    if (_consSock && _consSock->IsValid()) { 
      TMessage mess(kMESS_OBJECT);
      mess.Reset();
      sendRes=_consSock->Send(DspConsumerSend.c_str());
      if (sendRes == -1) errorReact(sendRes); 
      //-----------------------------------------------------    
      if (_consSock && _consSock->IsValid()) {
	mess.Reset();
	mess.WriteObject(object);
	sendRes=_consSock->Send(mess);
	if (sendRes == -1) errorReact(sendRes);
	else {
	  rtvalue=1;
	}
      }
      //-----------------------------------------------------
      mess.Reset();
      sendRes=_consSock->Send(DspConsumerFinish.c_str());
      if (sendRes == -1) errorReact(sendRes);
    }
    else {
      std::cout << "ConsumerExport: ERROR: Socket to display server is no "
	   << "longer valid." << std::endl;
      rtvalue = -1;         
    }
  }
  else { // Sending is disabled.
    rtvalue = -2;
  } 

  return rtvalue;
} 


void ConsumerExport::errorReact(const Int_t sendRes)
{
  // This method implements the error reaction if the sending of a message  
  // fails.

  std::cerr << "ConsumerExport: ERROR: Socket-Send returned with "
       << sendRes << ".\n" 
       << "               ==> Socket connection to the Display Server is \n"
       << "                   going to be closed IMMEDIATELY."
       << std::endl;
  _consSock->Close();
  _consSock = 0;
}


void ConsumerExport::createCanvasString(TList *canList)
{
  delete [] _canvasString;
  _canvasString = new char[_maxStringLength];

  strcpy(_canvasString, "Modified");
  TListIter iter(canList);
  while (TObject *obj = iter()) {
    strcat(_canvasString, "$");
    strcat(_canvasString, obj->GetName());
  } 

}

int ConsumerExport::lengthNeeded(TList *canList)
{
  int length=0;

  TListIter iter(canList);
  while (TObject *obj = iter()) {
    length += strlen(obj->GetName());
    length++;      // Take also the $ signs into account
  } 
  
  length += strlen("Unmodified");

  length += 10;   // Add some extra margin 

  return length;
}


int ConsumerExport::startServer()
{
  // Starts the display server using fork and exclp.

  const string myName = string("ConsumerExport::startServer()");

  int rtValue = 0;
  char portChar[128];
  char reportToSmChar[128];
  string optArgs("-");
  char fullPath[128];
  char fullPath2[128];

  sprintf(fullPath, "Server");
  sprintf(portChar, "%d", _port);
  sprintf(reportToSmChar, "%d", _reportToStateManager);
  // Now create a string of optional arguments.
  // The Server understands the format "-vfod"
  if (_serverVerboseFlag) optArgs += string("v");
  if (_logFileFlag)       optArgs += string("f");

  if ( (_serverPid = fork()) < 0) { 
    std::cerr << myName << ": ERROR: fork failed." << std::endl;  
    rtValue = _serverPid;
  }
  else {  // Fork successful.
    rtValue = _serverPid;
    if (_serverPid == 0) {  // Child process
      int execRes = 0;
      if (_debug) std::cout << myName << ": fullPath = " << fullPath << std::endl;
      if ( (execRes = execlp(fullPath, "Server", portChar, reportToSmChar,
                      optArgs.c_str(), (char *) 0)) < 0) {  
        if (getenv("CONSUMERBINDIR")) {
          if (_debug) std::cout << "consumerbindir:" << getenv("CONSUMERBINDIR") 
                           << std::endl;
          sprintf(fullPath, "%s/Server", getenv("CONSUMERBINDIR"));
          if ( (execRes = execlp(fullPath, "Server", portChar, reportToSmChar,
                          optArgs.c_str(), (char *) 0)) < 0) {  
            if (getenv("CDFSOFT2_DIR")) {
              // std::cout << "CDFSOFT2_DIR = " << getenv("CDFSOFT2_DIR") << std::endl; 
              sprintf(fullPath2, "%s/bin/%s/Server", getenv("CDFSOFT2_DIR"),
                getenv("BFARCH"));
              if ( (execRes = execlp(fullPath2, "Server", portChar, 
                    reportToSmChar, optArgs.c_str(), (char *) 0)) < 0) {  
                std::cerr << myName << ": ERROR: execlp failed. rt = " << execRes 
                     << std::endl;
                std::cerr << myName << ": ERROR: Could not find the Server "
		     << "Executable.\n Three different possibilities were "
		     << "checked, all of them failed." << std::endl;
                std::cerr << myName << "1.) Default PATH. " << std::endl;  
                std::cerr << myName << "2.) using $CONSUMERBINDIR: " << fullPath
		     << "." << std::endl;
                std::cerr << myName << "3.) using the release directory "
		     << "$CDFSOFT2_DIR: " << fullPath2 << std::endl; 
              }
            }
          } 
        } 
        else {
          if (getenv("CDFSOFT2_DIR")) {
            // std::cout << "CDFSOFT2_DIR = " << getenv("CDFSOFT2_DIR") << std::endl; 
            sprintf(fullPath2, "%s/bin/%s/Server", getenv("CDFSOFT2_DIR"),
            getenv("BFARCH"));
            if ( (execRes = 
                  execlp(fullPath2, "Server", portChar, reportToSmChar, 
                         optArgs.c_str(), (char *) 0)) < 0) {  
              std::cerr << myName << ": ERROR: execlp failed. rt = " << execRes << std::endl;
              std::cerr << myName << ": ERROR: Could not find the Server "
	           << "Executable. Three different possibilities\n were "
	           << "checked. All of them FAILED !!!" << std::endl;
              std::cerr << myName << ": 1.) Server not in PATH." << std::endl;  
              std::cerr << myName << ": 2.) $CONSUMERBINDIR is not set." << std::endl;
              std::cerr << myName << ": 3.) Using the release directory "
		     << "$CDFSOFT2_DIR: " << fullPath2 << std::endl; 
              std::cerr << myName << ": Therefore, the child process EXISTS NOW!"
		   << std::endl;
              // Since the Server program could not be found anywhere,
              // the child has to exit.
              exit(serverStartFailCode); 
            }
	  }
        }
      }
    }
    else {  // Mother process
      serverPid = _serverPid;
      sleep(1);
      if (serverStartFailed) rtValue = -10;
    }
  }
  time_t locTime; 
  _serverStartTime = time(&locTime); 

  return rtValue;
}


int ConsumerExport::connectServer()
{
  const string myName = string("ConsumerExport::connectServer()");
  int res = 1;

  if (_debug) 
    std::cout << myName << ": Waiting for display server to connect." << std::endl;
  if (_consSS) {
    _consSock = _consSS->Accept();
    if (_debug) std::cout << myName << ": Accept returned." << std::endl; 
    if (_consSock && _consSock->IsValid()) {
      if (_debug) { 
        std::cout << myName << ": Connection to display server okay." << std::endl; 
      }
      _consSock->SetOption(kNoDelay,1);
      _consMonitor->Add(_consSock, TMonitor::kWrite);
      res = 0;
    }
    else {
      std::cerr << myName << ": ERROR: the socket connection between display "
           << "server and \nconsumer failed." << std::endl;
      res = -1;
    }        
  }
  else {
    res = -2;
    std::cerr << myName << ": ERROR: consumer server socket not valid." 
	 << std::endl;
    std::cerr << myName << ": Further reconnect trials don't make sense." << std::endl;
  }

  return res;
}


int ConsumerExport::reestablishServer()
{
  const string myName = string("ConsumerExport::reestablishServer()");  
  const int minReconnectTime = 30;
  int rtValue = 1;
  bool locDebug = false;   
#ifdef CONSUMEREXPORT_DEBUG
  locDebug = true;
#endif
  // Try to restart and reconnect to the display server,
  // but only if at least a minimum difference passed since the last
  // restart.   
  time_t nowTime;
  time(&nowTime);
  if ( (difftime(nowTime, _serverStartTime) > minReconnectTime) &&
       _restartEnable) {
    // Check whether the previous server existed.
    pid_t waitRes;
    int status;
    waitRes = waitpid(_serverPid, &status, WNOHANG);         
    if (locDebug) std::cout << myName << ": waitRes = " << waitRes << std::endl;
    if ( ((!WIFEXITED(status)) && (!WIFSIGNALED(status))) 
#ifdef WIFSTOPPED
|| WIFSTOPPED(status) 
#endif 
#ifdef WIFCONTINUED
|| WIFCONTINUED(status) 
#endif
      ) {
      // The server did not terminat neither normally nor abnormally.
      // In this case we try to kill the process to make sure
      // it terminated before starting a new process.
      std::cerr << myName << ": Server was still alive. Need to kill it."
           << std::endl; 
      kill(_serverPid, SIGKILL);
    }
    sleep(2);
    // Check /proc
    char procFileName[200];
    sprintf(procFileName, "/proc/%d/stat", ((int)_serverPid));
    FILE* fp;
    if ((fp = fopen(procFileName, "r"))) {
      std::cerr << myName << ": Server was still alive (/proc entry). "
	   << "Need to kill it." << std::endl; 
      kill(_serverPid, SIGKILL);
      fclose(fp);
    }
    std::cout << myName << ": Going to restart the server." << std::endl;
    _serverStartTime = time(&nowTime);
    if (startServer() != -1) rtValue = connectServer();
    sleep(1);
  }
  else {
    rtValue = -2;
  }

  return rtValue;
}
