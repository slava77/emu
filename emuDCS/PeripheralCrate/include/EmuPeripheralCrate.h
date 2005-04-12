//-----------------------------------------------------------------------
// $Id: EmuPeripheralCrate.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: EmuPeripheralCrate.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
// Revision 1.2  2004/06/01 21:53:12  tfcvs
// Allow for i2o messaging (LG/FG)
//
//-----------------------------------------------------------------------
#ifndef EmuPeripheralCrate_h
#define EmuPeripheralCrate_h

#include <list>
#include <string>
#include <exception>
#include "xdaq.h"
#include "TestBeamCrateController.h"
#include "PCDriverI2oListener.h"

class EmuPeripheralCrate : public xdaqApplication, public PCDriverI2oListener {
public:
  EmuPeripheralCrate();
  void plugin();
  void ParameterSet(list<string> & paraNames) {};
  void ParameterGet(list<string> & paraNames) {};

  void Configure() throw(exception);
  void Enable()    throw(exception);
  void Suspend()   throw(exception) {};
  void Resume()    throw(exception) {};
  void Halt()      throw(exception) {};
  void Disable()   throw(exception);

  void fifo_reset() throw(exception) {};
  
  SOAPMessage sayHello(SOAPMessage &node);
   
  /// after the user sets the command parameters, 
  /// theCommand, theCommandBoardType, and theCommandSlot,
  /// then he/she calls
  /// this routine to execute them
  SOAPMessage executeCommand(SOAPMessage &node);

  void GUIpass(GUIreturn &temp);

  void StartPRBSMode(BufRef *TFbufRef);
  void EndPRBSMode(BufRef *TFbufRef);

protected:

  bool checkEnabled();
  /// which command you want to send to the board
  string theCommand;
  /// which type of board should receive the command
  string theCommandBoardType;
  /// the slot to accept the command.  -1 means ALL
  string theCommandSlot;
  /// the crate to accept the command.  -1 means ALL
  string theCommandCrate;

  string theConfigFileName;
  TestBeamCrateController theController;
};

#endif

