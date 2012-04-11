/** \file testException.cc
 * $Id: testException.cc,v 1.1 2012/04/11 21:36:18 khotilov Exp $
 *
 * tests for the emu::exception::Exception
 *
 */

#include "emu/exception/Exception.h"

using namespace std;


int main(int argc, char **argv)
{

  try
  {
    XCEPT_RAISE(emu::exception::DBException, "test Exception");
  }
  catch (emu::exception::Exception &e)
  {
    cout<<"HTML:"<<endl;
    cout<<e.toHTML()<<endl<<endl;
    cout<<"JSON:"<<endl;
    cout<<e.toJSON()<<endl<<endl;
  }

  return 0;
}
