/** \file testSystem.cc
 * $Id: testSystem.cc,v 1.1 2011/09/09 16:27:30 khotilov Exp $
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

  return 0;
}
