//-----------------------------------------------------------------------
// $Id: EmuPeripheralCrate.cc,v 2.0 2005/04/12 08:07:05 geurts Exp $
// $Log: EmuPeripheralCrate.cc,v $
// Revision 2.0  2005/04/12 08:07:05  geurts
// *** empty log message ***
//
// Revision 1.5  2004/06/01 21:53:11  tfcvs
// Allow for i2o messaging (LG/FG)
//
//-----------------------------------------------------------------------
#include "EmuPeripheralCrate.h"
#include "PeripheralCrateParser.h"


EmuPeripheralCrate::EmuPeripheralCrate() 
: theConfigFileName("/home/pccntrl/EmuDAQ/PeripheralCrate/test/configTestBeam2004.xml")
{
  exportParam("Command", theCommand);
  exportParam("BoardType", theCommandBoardType);
  exportParam("Slot", theCommandSlot);
  exportParam("Crate", theCommandCrate);


  exportParam("ConfigFileName", theConfigFileName);
  // user commands

  soapBindMethod (this, &EmuPeripheralCrate::sayHello, "SayHello");
  soapBindMethod (this, &EmuPeripheralCrate::executeCommand, "ExecuteCommand");
}


void EmuPeripheralCrate::plugin() {
  LOG4CPLUS_INFO(logger_, "EmuPeripheralCrate is loaded");
}


void EmuPeripheralCrate::Configure() throw(exception) {
  std::cout << "Parsing file " << theConfigFileName.c_str() << std::endl;
  PeripheralCrateParser parser;
  parser.parseFile(theConfigFileName.c_str());
  theController.configure();
  LOG4CPLUS_INFO(logger_, "EmuPeripheralCrate is configured ");
}


void EmuPeripheralCrate::Enable() throw(exception) {
  theController.enable();
  string message = "EmuPeripheralCrate enabled for command:";
}


void EmuPeripheralCrate::Disable() throw(exception) {
	  theController.disable();
}


SOAPMessage EmuPeripheralCrate::executeCommand(SOAPMessage &node) {
  std::cout << "inside executeCommand" << std::endl;
  std::cout << "Crate = "<< theCommandCrate <<  "    Slot = " << theCommandSlot << std::endl;
  theController.selector().setCrate(theCommandCrate);
  theController.selector().setSlot(theCommandSlot);
  std::cout << "calling executeCommand() for TestBeamController" << std::endl;
  theController.executeCommand(theCommandBoardType, theCommand);
  SOAPMessage emptyReply;
  return emptyReply;
  }


SOAPMessage EmuPeripheralCrate::sayHello(SOAPMessage &node) {
  std::cout << "kyky - hello" << std::endl;
  SOAPMessage n1;
  return n1;
}

bool EmuPeripheralCrate::checkEnabled() {
  bool retval = true;
  if(state() != Enabled) {
    XDAQ_RAISE(xdaqException, "Need to be Enabled to run this");
    retval = false;
  }
  return retval;
}

void EmuPeripheralCrate::GUIpass(GUIreturn & temp)
// Sends a confirmation message to the GUIDriver
{
// Creates the GUI I2O message
  int framesize = sizeof(GUI_I2OMsgFrame);
  BufRef *GUIbufRef = xdaq::frameAlloc(framesize);

  GUI_I2OMsgFrame *GUIframe = (GUI_I2OMsgFrame*)GUIbufRef->data();
  I2O_PRIVATE_MESSAGE_FRAME *GUIpvtMsgFrame = (I2O_PRIVATE_MESSAGE_FRAME*)GUIframe;
  I2O_MESSAGE_FRAME         *GUIstdMsgFrame = (I2O_MESSAGE_FRAME*)GUIframe;

  GUIpvtMsgFrame->XFunctionCode    = GUIDriverI2oMsgCode;
  GUIpvtMsgFrame->OrganizationID   = XDAQ_ORGANIZATION_ID;

  GUIstdMsgFrame->MessageSize      = framesize >> 2;
  GUIstdMsgFrame->InitiatorAddress = tid();
  GUIstdMsgFrame->TargetAddress    = xdaq::getTid("GUIDriver", 0);
  GUIstdMsgFrame->Function         = I2O_PRIVATE_MESSAGE;
  GUIstdMsgFrame->VersionOffset    = 0;
  GUIstdMsgFrame->MsgFlags         = 0;  // Point-to-point

// Sets the GUI I2O message values
  strcpy(GUIframe->errorMsg, temp.errorMsg);
  GUIframe->value = temp.value;
  GUIframe->msgCode = temp.msgCode;

// Sends the GUI I2O message
  xdaq::frameSend(GUIbufRef);

}


void EmuPeripheralCrate::StartPRBSMode(BufRef *TFbufRef)
{  
  std::cout << "PRBS Mode Started" << std::endl;
  GUIreturn temp;

  PRBSModeMessage *TFframe = (PRBSModeMessage*)TFbufRef->data();
  I2O_PRIVATE_MESSAGE_FRAME *TFpvtMsgFrame = (I2O_PRIVATE_MESSAGE_FRAME*)TFframe;
  
  temp.msgCode = TFpvtMsgFrame->XFunctionCode;

  GUIpass(temp);
}

void EmuPeripheralCrate::EndPRBSMode(BufRef *TFbufRef)
{
  std::cout << "PRBS Mode Ended" << std::endl;
  GUIreturn temp;

  PRBSModeMessage *TFframe = (PRBSModeMessage*)TFbufRef->data();
  I2O_PRIVATE_MESSAGE_FRAME *TFpvtMsgFrame = (I2O_PRIVATE_MESSAGE_FRAME*)TFframe;

  temp.msgCode = TFpvtMsgFrame->XFunctionCode;

  GUIpass(temp);
}    
