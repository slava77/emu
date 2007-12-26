//-----------------------------------------------------------------------
// $Id: ParserTester.cpp,v 1.1 2007/12/26 11:25:03 liu Exp $
// $Log: ParserTester.cpp,v $
// Revision 1.1  2007/12/26 11:25:03  liu
// new parser and applications
//
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 1.1  2006/07/14 09:32:47  mey
// test parser
//
// Revision 1.2  2006/02/01 21:03:23  mey
// Got rid of TestBeam
//
// Revision 1.1  2006/02/01 19:45:02  mey
// UPdate
//
// Revision 2.1  2005/06/15 13:54:46  geurts
// Changed default run behavior.
// testbeamcontrol runs a full Configure and Enable. Use -i option for
// interactive use, including the possibility to execute Disable
//
// Revision 2.0  2005/04/12 08:07:07  geurts
// *** empty log message ***
//
//-----------------------------------------------------------------------
#include <iostream>
#include "XMLParser.h"
#include <unistd.h>

int main(int argc, char **argv)
{
  char *xmlFile = "config.xml";
  bool doInteractive(false);

  // process command line arguments
  if (argc>1)
    for (int i=1;i<argc;i++){
      if (!strcmp(argv[i],"-h")){                                                                     
        std::cout << "Usage: " << argv[0] <<"[-f (file="<< xmlFile <<")]" << std::endl;
        exit(0);
      }
      if (!strcmp(argv[i],"-f")) xmlFile=argv[++i];
      if (!strcmp(argv[i],"-i")) doInteractive=true;
    }

 
  std::cout << "PeripheralCrate configuration file: " << xmlFile << std::endl;
 
  XMLParser parser;
  parser.parseFile(xmlFile);
  
  
  return 0;
}


