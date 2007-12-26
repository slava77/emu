//-----------------------------------------------------------------------
// $Id: parsemain.cpp,v 1.1 2007/12/26 11:25:02 liu Exp $
// $Log: parsemain.cpp,v $
// Revision 1.1  2007/12/26 11:25:02  liu
// new parser and applications
//
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.1  2006/07/13 15:46:38  mey
// New Parser strurture
//
// Revision 2.0  2005/04/12 08:07:07  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include <iostream>
#include "XMLParser.h"

int main()
{
  XMLParser parser;
  parser.parseFile("config.xml");  
  
  return 0;
}


