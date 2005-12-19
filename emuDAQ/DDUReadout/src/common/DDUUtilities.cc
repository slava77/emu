//-----------------------------------------------------------------------
// $Id: DDUUtilities.cc,v 1.1 2005/12/19 13:29:59 mey Exp $
// $Log: DDUUtilities.cc,v $
// Revision 1.1  2005/12/19 13:29:59  mey
// Update
//
// Revision 2.1  2005/11/03 09:20:55  mey
// New flags for switching between schar2 and schar3
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
#include "DDUUtilities.h"

#include <time.h>
#include <stdio.h>
#include <string.h>

using namespace std;

int DDUUtilities::HardwareDumper() {
  //
  string schar = "/dev/schar3";
  //
  bool setReset(1);
  //
  // read commandline arguments and set run-number
  unsigned long int maxEventPerFile(10000), maxEventTotal(1000000);
  bool updateDb(true);
  //
  HardwareDDU* ddu = new HardwareDDU(schar);
  ddu->openFile(schar);
  if ( setReset ) ddu->reset();
  ddu->enableBlock();
  BinaryEventStream* eventStream = new BinaryEventStream(runnumber_,maxEventPerFile);
  //

  //-- dbase entries  
  char cmd[1024]; time_t ttt; time(&ttt);
  if (updateDb){
    cout << "Start-phase dBase update ..." << endl;
    sprintf(cmd,"/home/dbase/lib/start_insert.pl %d  \"%s\"",runnumber_,ctime(&ttt));
    int pos=0; while( cmd[pos] && cmd[pos]!='\n' ) pos++; 
    if( cmd[pos]=='\n' ) cmd[pos]=' ';
    if( system(cmd) ) printf("Error executing /home/dbase/lib/start_insert.pl"); 
    sprintf(cmd,"/home/dbase/dbase/pcConfig2db.pl run=%d file=/home/pccntrl/EmuDAQ/PeripheralCrate/test/configTestBeam2004.xml",runnumber_);
    if( system(cmd) ) printf("Error executing /home/dbase/lib/dbwrapper.pl");
    cout << "Start-phase dBase update done." << endl;
  }
  //--

  eventStream->openFile();
  unsigned long int eventNumber(0);
  while(eventNumber<maxEventTotal) {
    std::cout << "Data... " << eventNumber << std::endl;
    ddu->readNextEvent();
    char *data = ddu->data();
    unsigned int dataLength=ddu->dataLength();
    if (data){
      eventStream->writeEnv(data, dataLength);
      eventNumber++;
    }
}
  delete ddu;
  eventStream->closeFile();

  //-- dbase entries  
  if (updateDb){
    cout << "End-phase dBase update ..." << endl;
    sprintf(cmd,"/home/dbase/lib/end_insert.pl %d \"%s\" %ld",runnumber_,ctime(&ttt),eventNumber);
    int pos=0; while( cmd[pos] && cmd[pos]!='\n' ) pos++; 
    if( cmd[pos]=='\n' ) cmd[pos]=' ';
    if( system(cmd) ) printf("Error executing /home/dbase/lib/end_insert.pl");
    cout << "End-phase dBase update done." << endl;
  }
  //--

  delete eventStream;

}
