/** \file testSystem.cc
 * $Id: testSystem.cc,v 1.2 2012/04/11 21:36:19 khotilov Exp $
 *
 * tests for the System utilities
 *
 */

#include "emu/utils/System.h"

using namespace emu::utils;
using namespace std;


int main(int argc, char **argv)
{

  SCSI_t s = getSCSI( "ATA", "Maxtor");

  cout<< emu::utils::findSCSIDevice( s ) << endl;

  return 0;
}
