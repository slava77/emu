//-----------------------------------------------------------------------
// $Id: EventReader.h,v 2.0 2005/04/13 10:53:01 geurts Exp $
// $Log: EventReader.h,v $
// Revision 2.0  2005/04/13 10:53:01  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
// Description: An interface to something that provides
//              a stream of bits as data
#ifndef __EVENT_READER_H__
#define __EVENT_READER_H__

class EventReader
{
public:
	virtual bool  readNextEvent() = 0;
	virtual char *data() = 0;
	virtual int   dataLength() = 0; // in bytes
	virtual int   eventNumber() = 0;
};

#endif  // ifndef __EVENT_READER_H__

