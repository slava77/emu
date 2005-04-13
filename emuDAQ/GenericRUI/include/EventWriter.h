//-----------------------------------------------------------------------
// $Id: EventWriter.h,v 2.0 2005/04/13 10:53:01 geurts Exp $
// $Log: EventWriter.h,v $
// Revision 2.0  2005/04/13 10:53:01  geurts
// Makefile
//
// Revision 1.2  2004/09/23 05:43:43  tfcvs
// *** empty log message ***
//
// Revision 1.1  2004/08/17 21:41:11  tfcvs
// initial version of RUI skeleton
//
// Revision 1.2  2004/06/11 08:22:23  tfcvs
// *** empty log message ***
//
//-----------------------------------------------------------------------
#ifndef __EVENT_WRITER_H__
#define __EVENT_WRITER_H__

class EventWriter
{
public:
  virtual int writeEvent(char *data,int dataLength) = 0;
};

#endif  // ifndef __EVENT_WRITER_H__
