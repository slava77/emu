//-----------------------------------------------------------------------
// $Id: PCI2oMsgs.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: PCI2oMsgs.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
// Revision 1.1  2004/06/01 21:53:11  tfcvs
// Allow for i2o messaging (LG/FG)
//
//-----------------------------------------------------------------------
// Description: Header file for PCI2oMsgs class
#ifndef PCI2OMSGS_H
#define PCI2OMSGS_H
#include "i2o.h"

// I2O Message Codes

const unsigned short GUIDriverI2oMsgCode              = 0x0014;
const unsigned short PCDriverStartPRBSModeMsgCode     = 0x0021;
const unsigned short PCDriverEndPRBSModeMsgCode       = 0x0022;
// GUIreturn used internally for error passing

struct GUIreturn
{
    // error/execution check
    char errorMsg[512];

    // which subroutine
    unsigned short msgCode;

    // holds the processID as assigned in the SchDriver
    long int processID;

    // return parameters
    long int value;
};

// I2O Message Frames

struct GUI_I2OMsgFrame
// This is the message returned by the PCDriver to the GUIDriver
{
    // I2O specific part of the frame
    I2O_PRIVATE_MESSAGE_FRAME PvtMessageFrame;

    // error/execution check
    char errorMsg[256];

    // which subroutine
    unsigned short msgCode;

    // return parameters
    long int value;
};

struct PRBSModeMessage
// Used in MPC mode changes for PRBS tests
{
  I2O_PRIVATE_MESSAGE_FRAME PvtMessageFrame;

  unsigned short msgCode;
};

#endif

