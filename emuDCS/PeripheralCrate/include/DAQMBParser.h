//-----------------------------------------------------------------------
// $Id: DAQMBParser.h,v 2.1 2006/05/19 15:13:32 mey Exp $
// $Log: DAQMBParser.h,v $
// Revision 2.1  2006/05/19 15:13:32  mey
// UPDate
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef DAQMBParser_h
#define DAQMBParser_h
#include <vector>
#include <xercesc/dom/DOM.hpp>

class CFEB;
class DAQMB;
class DAQMBParser{
public:
  DAQMBParser(){}
  explicit DAQMBParser(xercesc::DOMNode * pNode, int crate);
  std::vector<CFEB> parseCFEBs(xercesc::DOMNode * pNode);   

  /// the last one parsed
  DAQMB * daqmb() const {return daqmb_;}

protected:
  DAQMB * daqmb_;//last one parsed
};

#endif






