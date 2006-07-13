//-----------------------------------------------------------------------
// $Id: parsemain.cpp,v 2.1 2006/07/13 15:46:38 mey Exp $
// $Log: parsemain.cpp,v $
// Revision 2.1  2006/07/13 15:46:38  mey
// New Parser strurture
//
// Revision 2.0  2005/04/12 08:07:07  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include <iostream>
#include "EMUParser.h"

int main()
{
  EMUParser parser;
  parser.parseFile("config.xml");  
  
  return 0;
}


