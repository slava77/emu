//-----------------------------------------------------------------------
// HardwareMonitor.cpp  2005/10/11   --JRG
//   begin with HardwareDumper.cpp,v 2.0,  but
//     only dumps events with DDU reported problem, and stops 63 events
//     after Critical error, Single Error or Warning
//   change to schar3 (was schar2) in 3 places
//   dboff is hard-coded (updateDb=false)
//   default maxEventTotal=4e9
//
//-----------------------------------------------------------------------
// $Id: HardwareMonitor.cpp,v 1.1 2005/11/03 09:17:09 mey Exp $
// $Log: HardwareMonitor.cpp,v $
// Revision 1.1  2005/11/03 09:17:09  mey
// New file from Jason
//
// Revision 2.0  2005/04/13 10:52:58  geurts
// Makefile
//
// Revision 1.5  2004/10/10 11:59:50  tfcvs
// added extra cout lines to indicate start/stop of dbase updates (FG)
//
// Revision 1.4  2004/10/07 17:58:53  tfcvs
// dbase interface has changed, updates provided by Khristian (FG)
//
// Revision 1.3  2004/10/03 17:40:40  tfcvs
// preprocessor defines are bad, commandline arguments are better: enabling dbase updates is now done thru commandline option (FG)
//
// Revision 1.2  2004/10/03 16:18:20  tfcvs
// introduced new memory mapped DDU readout, switchable with the USE_DDU2004 switch (FG)
//
//-----------------------------------------------------------------------
#include <iostream>
#include <stdlib.h>
#include "FileReaderDDU.h"
#include "HardwareDDU.h"
#include "BinaryEventStream.h"

#include <time.h>
#include <stdio.h>
#include <string.h>

using namespace std;

int main(int argc, char *argv[]) {
  string schar = "/dev/schar3";

  // read commandline arguments and set run-number
  int runnumber(0);
  unsigned long int maxEventPerFile(10000), maxEventTotal(4000000000);
  bool updateDb(true);
  if (argc>1)
    for (int i=1;i<argc;i++){
      if (!strcmp(argv[i],"-h")) {
	cout << argv[0] << " [-r run#=" << runnumber << "] [-n evt/file=" 
             << maxEventPerFile << "] [-m maxevt=" << maxEventTotal << "] [-dboff (switch dbase update off)]" << endl;
        exit(1);}
      if (!strcmp(argv[i],"-r")) runnumber=(int)atof(argv[++i]);
      if (!strcmp(argv[i],"-n")) maxEventPerFile=(int)atof(argv[++i]);
      if (!strcmp(argv[i],"-m")) maxEventTotal=(int)atof(argv[++i]);
      if (!strcmp(argv[i],"-dboff")) updateDb=false;
    }

  HardwareDDU* ddu = new HardwareDDU("/dev/schar3");
  ddu->openFile("/dev/schar3");
  ddu->reset();
  ddu->enableBlock();
  BinaryEventStream* eventStream = new BinaryEventStream(runnumber,maxEventPerFile);


  //-- dbase entries  
  char cmd[1024]; time_t ttt; time(&ttt);
  updateDb=false;
  if (updateDb){
    cout << "Start-phase dBase update ..." << endl;
    sprintf(cmd,"/home/dbase/lib/start_insert.pl %d  \"%s\"",runnumber,ctime(&ttt));
    int pos=0; while( cmd[pos] && cmd[pos]!='\n' ) pos++; 
    if( cmd[pos]=='\n' ) cmd[pos]=' ';
    if( system(cmd) ) printf("Error executing /home/dbase/lib/start_insert.pl"); 
    sprintf(cmd,"/home/dbase/dbase/pcConfig2db.pl run=%d file=/home/pccntrl/EmuDAQ/PeripheralCrate/test/configTestBeam2004.xml",runnumber);
    if( system(cmd) ) printf("Error executing /home/dbase/lib/dbwrapper.pl");
    cout << "Start-phase dBase update done." << endl;
  }
  //--

  eventStream->openFile();
  unsigned long int eventNumber(0);
  unsigned long int DDUprob=0;
  unsigned short int DDUwarn=0, DDUerror=0, DDUcritical=0, gotDDUcrit=0;
  while(eventNumber<maxEventTotal) {
    ddu->readNextEvent();
    char *data = ddu->data();
    unsigned int dataLength=ddu->dataLength();

// JRG, assume that dataLength is byte count
    //    if (dataLength>7) cout << " " << dataLength;
    if (dataLength>7 && dataLength<=64){
      printf("\n Empty: %d bytes.  3rd-to-last=%02x  Next-to-last=%02x  Last=%02x \n",dataLength,data[dataLength-2]&0xff,data[dataLength-1]&0xff,data[dataLength]&0xff);
      //      cout << endl << " Empty: " << dataLength << " bytes.  3rd-to-last=" << (unsigned short int)data[dataLength-2] << " Next-to-last=" << (unsigned short int)data[dataLength-1] << " Last=" << (unsigned short int)data[dataLength] << endl;
      while ( dataLength>7 && 
	      (data[dataLength-1]&0x00ff)==0x00ff && 
	      (data[dataLength-2]&0x00ff)==0x00ff && 
	      (data[dataLength-3]&0x00ff)==0x00ff && 
	      (data[dataLength-4]&0x00ff)==0x00ff ) {
	dataLength -= 8;
	printf(" Empty: %d bytes.  3rd-to-last=%02x  Next-to-last=%02x  Last=%02x \n",dataLength,data[dataLength-2]&0xff,data[dataLength-1]&0xff,data[dataLength]&0xff);      }
    }

    if (data){
      DDUwarn=((data[dataLength-8]&0x0010)|(data[dataLength-13]&0x0080));
      DDUerror=(data[dataLength-11]&0x0040);
      DDUcritical=((data[dataLength-8]&0x0060)|(data[dataLength-11]&0x0080));
      // JRG debug, print to LOG:  data[dataLength/2-i] for i=1 to 16
      if (DDUwarn>0||DDUerror>0||DDUcritical>0){
	DDUprob++;
	// print warn/err/crit values to LOG?  also eventNumber (dec & hex)
	eventStream->writeEnv(data, dataLength);
	if (DDUcritical>0&&gotDDUcrit==0){
	  gotDDUcrit=1;
	  maxEventTotal=eventNumber+63;  // stop 63 events after critical
	}
      }
      // else accumulate average event size in bytes?
      eventNumber++;
    }
  }
  delete ddu;
  eventStream->closeFile();
  // print #events, average size & DDUprob total (and %) to LOG; run time too?

  //-- dbase entries  
  if (updateDb){
    cout << "End-phase dBase update ..." << endl;
    sprintf(cmd,"/home/dbase/lib/end_insert.pl %d \"%s\" %ld",runnumber,ctime(&ttt),eventNumber);
    int pos=0; while( cmd[pos] && cmd[pos]!='\n' ) pos++; 
    if( cmd[pos]=='\n' ) cmd[pos]=' ';
    if( system(cmd) ) printf("Error executing /home/dbase/lib/end_insert.pl");
    cout << "End-phase dBase update done." << endl;
  }
  //--

  delete eventStream;

}
