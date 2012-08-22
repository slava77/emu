/*
 * =====================================================================================
 *
 *       Filename:  CSCDqmFact.h
 *
 *    Description:  Local DQM Fact used by client to DW
 *
 *        Version:  1.0
 *        Created:  02/05/2010 05:29:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Valdas Rapsevicius (VR), Valdas.Rapsevicius@cern.ch
 *        Company:  CERN, CH
 *
 * =====================================================================================
 */

#ifndef CSCDqmFact_h
#define CSCDqmFact_h

#include "emu/base/Fact.h"

using namespace std;


class EmuMonitorFact {
public:
  enum ParameterName_t { state, stateChangeTime, dqmEvents, dqmRate, cscRate, cscDetected, cscUnpacked, nParameters };
  static const char*  getTypeName() { return typeName_; }
  static const string getParameterName( const ParameterName_t p ) { return parameterNames_[p]; }
protected:
  static const char* const typeName_; 
  static const char* const parameterNames_[nParameters];
};


class DqmReportFact {
public:
  enum ParameterName_t { testId, nParameters };
  static const char*  getTypeName() { return typeName_; }
  static const string getParameterName( const ParameterName_t p ) { return parameterNames_[p]; }
protected:
  static const char* const typeName_; 
  static const char* const parameterNames_[nParameters];
};

#endif
