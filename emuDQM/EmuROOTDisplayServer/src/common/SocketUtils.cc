//-------------------------------------------------------------------------
// File: SocketUtils.cc
//
// The SocketUtils class provides methods to conveniently handle socket 
// communication in a standard way throughout the Consumer package.
//
// 10/02/2000 Started    Wolfgang Wagner 
//
//-------------------------------------------------------------------------
//****************************************************************************
// RCS Current Revision Record
//-----------------------------------------------------------------------------
// $Source: /afs/cern.ch/project/cvs/reps/tridas/TriDAS/emu/emuDQM/EmuROOTDisplayServer/src/common/SocketUtils.cc,v $
// $Revision: 1.2 $
// $Date: 2006/03/09 00:17:06 $
// $Author: barvic $
// $State: Exp $
// $Locker:  $
//*****************************************************************************

#include <cstdio>
#include <signal.h>

#include "SocketUtils.hh"

static int brokenPipe = 0;

extern "C"
{
  static void pipeBroken(int sig)
  {    
    brokenPipe++;
  } 
}

SocketUtils::SocketUtils(TMonitor* moni, TSocket* sock) : 
  _moni(moni), 
  _socket(sock), 
  _id(":"),
  _errFlag(false)
{
  const std::string myName("SocketUtils::SocketUtils()"); 

  _hostname = std::string(sock->GetInetAddress().GetHostName());
  int    port = sock->GetInetAddress().GetPort();
  char portChar[20];
  sprintf(portChar, "%d", port);
  _id = _hostname + std::string(":") + std::string(portChar);
  // Install a signal handler for SIGPIPE
  installSIGPIPEHandler();
}

SocketUtils::~SocketUtils()
{
}

int SocketUtils::nBrokenPipes() {return brokenPipe;}

int SocketUtils::installSIGPIPEHandler() 
{
  const std::string myName ("SocketUtils::installSIGPIPEHandler()");

  int instRes = (int)signal(SIGPIPE,pipeBroken);
  if (instRes == -1) {
    std::cerr << myName << ": ERROR setting signal handler for SIGPIPE." 
	      << std::endl;
  }
  
  return instRes;
}

const std::string SocketUtils::getStringMessage()
{
  char*     mess = 0;
  TMessage* clientMess = 0;
  Int_t     recvRes = 0;
  std::string    clientString("---");

  if ( _socket && (_socket != (TSocket*)-1) ) {
    recvRes = _socket->Recv(clientMess);
    if ((recvRes != -1) && (recvRes != 0)) {
      if (clientMess) { 
        if (clientMess->What() == kMESS_STRING) {
          int messLength = clientMess->BufferSize() + 10; 
          mess = new char[messLength];
          clientMess->ReadString(mess,messLength);                  
          if (mess) clientString = std::string(mess);
          delete mess;
          mess = 0;
        } 
      }
      else {
        closeSocket();
        _errFlag = true; 
      }
    } 
    else {
     closeSocket();
     _errFlag = true;
    }
  } 
  delete clientMess;
  clientMess = 0;

  return clientString;
}


bool SocketUtils::sendStringMessage(const std::string message)
{
  const std::string myName("SocketUtils::sendStringMessage()");

  if ( _socket && (_socket != (TSocket*)-1) && _socket->IsValid()) {
    // We need to beware of SIGPIPES.
    int nSigPipesBefore = brokenPipe;
    int sendRes = _socket->Send(message.c_str());
    if (brokenPipe > nSigPipesBefore) {
      // A SIGPIPE signal occurred, most probably in response to our
      // effort to send the message.
      // As a result, we close the socket. 
      std::cerr << myName << ": SIGPIPE occurred when sending message string:\n"
		<< message << ". The socket has to be closed."
		<< std::endl;
      closeSocket();
      _errFlag = true; 
      return false;
    }
    if (sendRes == -1) {
      closeSocket();
      _errFlag = true; 
      return false;
    } 
  }
  else return false;

  return true;
}   



void SocketUtils::closeSocket()
{
  const std::string myName("SocketUtils::closeSocket()");

  // Close the socket, remove it from the socket lists and delete it. 
  if ( _socket && (_socket != (TSocket*)-1) ) {
    if (_moni) _moni->Remove(_socket);
    _socket->Close();
    delete _socket;
   _socket = 0;
  }
  else {
    std::cerr << "SocketUtils::closeSocket(): ERROR: _socket not valid." << std::endl;
  }  

  return;
}



