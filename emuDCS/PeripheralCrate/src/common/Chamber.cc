//-----------------------------------------------------------------------
// $Id: Chamber.cc,v 1.1 2006/01/19 10:09:56 mey Exp $
// $Log: Chamber.cc,v $
// Revision 1.1  2006/01/19 10:09:56  mey
// UPdate
//
// Revision 1.5  2006/01/18 12:46:48  mey
// Update
//
// Revision 1.4  2006/01/16 20:29:06  mey
// Update
//
// Revision 1.3  2006/01/12 22:36:27  mey
// UPdate
//
// Revision 1.2  2006/01/12 11:32:43  mey
// Update
//
// Revision 1.1  2006/01/11 08:54:15  mey
// Update
//
// Revision 1.5  2005/12/16 17:49:39  mey
// Update
//
// Revision 1.4  2005/12/14 08:32:36  mey
// Update
//
// Revision 1.3  2005/12/06 13:30:10  mey
// Update
//
// Revision 1.2  2005/11/30 14:58:02  mey
// Update tests
//
// Revision 1.1  2005/10/28 13:09:04  mey
// Timing class
//
// Revision 2.16  2005/09/07 16:18:17  mey
// DMB timing routines
//
// Revision 2.15  2005/09/07 13:55:16  mey
// Included new timing routines from Jianhui
//
// Revision 2.14  2005/08/31 15:12:59  mey
// Bug fixes, updates and new routine for timing in DMB
//
// Revision 2.13  2005/08/23 19:27:18  mey
// Update MPC injector
//
// Revision 2.12  2005/08/23 15:49:55  mey
// Update MPC injector for random LCT patterns
//
// Revision 2.11  2005/08/22 16:58:33  mey
// Fixed bug in TMB-MPC injector
//
// Revision 2.10  2005/08/22 16:38:28  mey
// Added TMB-MPC injector
//
// Revision 2.9  2005/08/22 07:55:46  mey
// New TMB MPC injector routines and improved ALCTTiming
//
// Revision 2.8  2005/08/17 12:27:23  mey
// Updated FindWinner routine. Using FIFOs now
//
// Revision 2.7  2005/08/15 15:38:15  mey
// Include alct_hotchannel_file
//
// Revision 2.6  2005/08/15 11:00:10  mey
// Added pulsing to MPC Winner timing and several options for DAV DMB settings
//
// Revision 2.5  2005/08/12 19:45:27  mey
// Updated MPC printout in TMB-MPC timing
//
// Revision 2.4  2005/08/12 14:16:04  mey
// Added pulsing vor TMB-MPC delay
//
// Revision 2.2  2005/08/10 12:54:36  geurts
// Martin's updates
//
// Revision 2.1  2005/06/06 15:17:19  geurts
// TMB/ALCT timing updates (Martin vd Mey)
//
// Revision 2.0  2005/04/12 08:07:07  geurts
//  updated by M von der Mey sometimes
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
//! TMB ALCT/CFEB timing utility
/*! \brief TMB-ALCT/CFEB timing
 * \author Frank Geurts
 * \date January 2004
 * 
 * tmbtiming sets up the PeripheralCrate according to the
 * configuration in the XML file. It will pass on the timing
 * parameter to the TMB routines and runs the PHOS4 timing
 * routines on the first TMB defined in the XML-file.
 * For reference the results are written in two files
 * (tmb_alct.dat and tmb_scan.dat).
 *
 */
//
#include <stdio.h>
#include <iomanip>
#include <unistd.h> 
#include <string>
//
#include "Chamber.h"
//
using namespace std;
//
Chamber::Chamber(){
  //
  //
}
//
Chamber::~Chamber(){
  //
  //
}
//
