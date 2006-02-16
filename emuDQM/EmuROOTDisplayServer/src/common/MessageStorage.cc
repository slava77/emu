//*************************************************************************
//                   MESSAGE STORAGE CLASS
//   Implementation file     
//*************************************************************************
//****************************************************************************
// RCS Current Revision Record
//-----------------------------------------------------------------------------
// $Source: /afs/cern.ch/project/cvs/reps/tridas/TriDAS/emu/emuDQM/EmuROOTDisplayServer/src/common/MessageStorage.cc,v $
// $Revision: 1.1 $
// $Date: 2006/02/16 00:05:14 $
// $Author: barvic $
// $State: Exp $
// $Locker:  $
//*****************************************************************************

#include "MessageStorage.hh"
  
MessageStorage::MessageStorage() : _message(NULL)
{
} 

MessageStorage::MessageStorage(const char* name)
: TNamed(name,name), _message(NULL)
{
}

MessageStorage::~MessageStorage()
{
 delete _message; 
}

void MessageStorage::updateMessage(TMessage *mess)
{
  delete _message;
  _message = mess;
}


