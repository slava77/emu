//*************************************************************************
//                  MESSAGE STORAGE CLASS
// is used inside the display server of the Consumer Project.
// An instance of the this class is used to store a pointer to a message
// and the name of the message.
//   
//     
//
//************************************************************************
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
