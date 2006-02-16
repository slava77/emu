//-------------------------------------------------------------------------
// File: ServerProtocol.hh
//
// This file contains string declarations for the communication protocol
// between the ConsumerExport class and the DisplayServer class on the hand
// and the DisplayServer and the HistoDisplay on the other hand.
//
// Author: Wolfgang Wagner 
//
//-------------------------------------------------------------------------
#ifndef SERVERPROTOCOL_HH
#define SERVERPROTOCOL_HH

#include <string>

using std::string;

const string DspEndConnection  = string("END CONNECTION");
const string DspConsumerSend   = string("CONSUMER SEND");
const string DspConsumerFinish = string("CONSUMER FINISHED");
const string DspRequestNewInfo = string("REQUEST NEW TCONSUMERINFO");    
const string DspObjectNotFound = string("ERROR: Requested Object not found.");
const string DspExitNow        = string("EXIT NOW");
 
#endif
