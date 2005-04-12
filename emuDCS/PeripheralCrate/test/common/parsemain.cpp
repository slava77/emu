//-----------------------------------------------------------------------
// $Id: parsemain.cpp,v 2.0 2005/04/12 08:07:07 geurts Exp $
// $Log: parsemain.cpp,v $
// Revision 2.0  2005/04/12 08:07:07  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include <iostream>
#include "PeripheralCrateParser.h"

int main()
{
  PeripheralCrateParser parser;
  parser.parseFile("config.xml");  
  
  return 0;
}


