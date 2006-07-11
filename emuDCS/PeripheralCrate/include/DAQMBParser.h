//-----------------------------------------------------------------------
// $Id: DAQMBParser.h,v 2.2 2006/07/11 15:00:38 mey Exp $
// $Log: DAQMBParser.h,v $
// Revision 2.2  2006/07/11 15:00:38  mey
// Update
//
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
class Chamber;
class DAQMBParser{
public:
  DAQMBParser(){}
  explicit DAQMBParser(xercesc::DOMNode * pNode, int crate);
  std::vector<CFEB> parseCFEBs(xercesc::DOMNode * pNode);   

  /// the last one parsed
  DAQMB * daqmb() const {return daqmb_;}

  inline void SetCSC(Chamber * csc){csc_ = csc;}

private:
  DAQMB * daqmb_;//last one parsed
  Chamber * csc_;
};

#endif






