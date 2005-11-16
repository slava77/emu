//-----------------------------------------------------------------------
// $Id: DDUReader.h,v 2.4 2005/11/16 02:41:31 kkotov Exp $
// $Log: DDUReader.h,v $
// Revision 2.4  2005/11/16 02:41:31  kkotov
//
// Removed old version of FileReader. New version now inherits DDUReader.
//
// Revision 2.3  2005/10/03 20:20:14  geurts
// Removed hardware-related implementations out of DDUReader, created dependency on driver-include files.
// - openFile is virtual function, HardwareDDU and FileReaderDDU take care of its own implementation
// - schar.h and eth_hook_2.h contain driver and bigphys parameters shared by the DDUReadout and eth_hook_2
//
// Revision 2.2  2005/10/03 19:20:22  geurts
// BigPhys/Gbit driver and reader updates to prevent bigphys data corruption
//
// Revision 2.1  2005/09/26 17:11:49  tumanov
// new data format
//
// Revision 2.0  2005/04/13 10:52:56  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#ifndef DDUReader_h
#define DDUReader_h

#include <fstream>
#include <string>
#include "EventReader.h"

class DDUReader : public EventReader {
private:
	unsigned short* theBuffer;     // Event buffer
	size_t          theDataLength; // Event length in bytes ( not in shorts! )

protected:
	virtual int readDDU(unsigned short **buf, const bool debug = false) = 0; // Not changed for compatibility with HardwareDDU class
	int fd_schar; // Left for compatibility with HardwareDDU class

public:
	virtual /*const*/ char* data(void) { return (char*)theBuffer; } // According to GenericRUI we allow everybody to modify our data
	virtual int dataLength(void) /*const*/ { return theDataLength; } // This should be a constant method, but GenericRUI doesn't seem to care about such ditails!

	virtual bool readNextEvent(void);
	virtual int  eventNumber(void) /*const*/;
	virtual int  triggerID(void) /*const*/ { return eventNumber(); }

	virtual int  openFile(std::string filename) = 0;
	virtual void closeFile(void);

	DDUReader(void):EventReader(){}
	virtual ~DDUReader(void){};

protected: // Everything bellow should go to HardwareDDU class

	//all below needed for ddu2004 only
	// new additions for MemoryMapped DDU
	char *buf_data;
	char *buf_start;
	unsigned long int buf_pnt;
	unsigned long int buf_end;
	unsigned long int buf_eend;
	unsigned long int buf_pnt_kern;

	char *ring_start;
	unsigned long int ring_size;
	unsigned long int ring_pnt;
	int short ring_loop;
	int short ring_loop_kern;
	int short ring_loop_kern2;
	int short timeout;  // timeout waiting for event
	int short packets; // # of packets in event
	int short pmissing;    // packets are  missing at beginning
	int short pmissing_prev; // packets are missing at end
	int short end_event;   // end of event seen
	int short overwrite;   // overwrite

	char *tail_start;
};

#endif
