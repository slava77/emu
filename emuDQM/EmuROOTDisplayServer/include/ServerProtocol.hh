//-------------------------------------------------------------------------
// File: ServerProtocol.hh
//
// This file contains std::string declarations for the communication protocol
// between the ConsumerExport class and the DisplayServer class on the hand
// and the DisplayServer and the HistoDisplay on the other hand.
//
// Author: Wolfgang Wagner 
//
//-------------------------------------------------------------------------
//****************************************************************************
// RCS Current Revision Record
//-----------------------------------------------------------------------------
// $Source: /afs/cern.ch/project/cvs/reps/tridas/TriDAS/emu/emuDQM/EmuROOTDisplayServer/include/ServerProtocol.hh,v $
// $Revision: 1.3 $
// $Date: 2007/11/27 15:27:20 $
// $Author: barvic $
// $State: Exp $
// $Locker:  $
//*****************************************************************************
#ifndef SERVERPROTOCOL_HH
#define SERVERPROTOCOL_HH

#include <string>

using std::string;

const std::string DspEndConnection  = std::string("END CONNECTION");
const std::string DspConsumerSend   = std::string("CONSUMER SEND");
const std::string DspConsumerFinish = std::string("CONSUMER FINISHED");
const std::string DspRequestNewInfo = std::string("REQUEST NEW TCONSUMERINFO");    
const std::string DspObjectNotFound = std::string("ERROR: Requested Object not found.");
const std::string DspExitNow        = std::string("EXIT NOW");
 
#endif
