//*************************************************************************
//                  MESSAGE STORAGE CLASS
// is used inside the display server of the Consumer Project.
// An instance of the this class is used to store a pointer to a message
// and the name of the message.
//   
//     
//
//************************************************************************
//****************************************************************************
// RCS Current Revision Record
//-----------------------------------------------------------------------------
// $Source: /afs/cern.ch/project/cvs/reps/tridas/TriDAS/emu/emuDQM/EmuROOTDisplayServer/include/MessageStorage.hh,v $
// $Revision: 1.2 $
// $Date: 2006/03/09 00:17:06 $
// $Author: barvic $
// $State: Exp $
// $Locker:  $
//*****************************************************************************
#ifndef MESSAGESTORAGE_HH
#define MESSAGESTORAGE_HH

#include "TNamed.h"
#include "TMessage.h"

class MessageStorage : public TNamed
{
public:
  MessageStorage(); 
  MessageStorage(const MessageStorage& ms);
  MessageStorage(const char* name);
  ~MessageStorage();

  MessageStorage& operator=(const MessageStorage& ms);
  TMessage* getMessage()  {return _message;}  
  void updateMessage(TMessage *mess);

private:
  TMessage* _message;
};

#endif
