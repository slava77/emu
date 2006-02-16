//-------------------------------------------------------------------------
// File: SocketUtils.hh
//
// The SocketUtils class provides methods to conveniently handle socket 
// communication in a standard way throughout the Consumer package.
//
// 10/02/2000 Started    Wolfgang Wagner 
//
//-------------------------------------------------------------------------
#ifndef SOCKETUTILS_HH
#define SOCKETUTILS_HH

#include <iostream>
#include <string>

#include "TSocket.h"
#include "TMessage.h"
#include "TMonitor.h"
#include "TROOT.h" 

class SocketUtils
{
public:
  SocketUtils(TMonitor* moni, TSocket* sock);
  ~SocketUtils();

  static int nBrokenPipes();
  static int installSIGPIPEHandler();
  const std::string getStringMessage();
  const std::string getHostname() const {return _hostname;}
  const std::string getId() const {return _id;}
  bool sendStringMessage(const std::string message);   
  void closeSocket();
  bool error() const { return _errFlag;}

protected:
  TMonitor* _moni;
  TSocket*  _socket;  
  std::string    _hostname; 
  std::string    _id;
  bool      _errFlag;

private:
  SocketUtils(const SocketUtils& su);
  SocketUtils& operator=(const SocketUtils& su);
     
};

#endif
