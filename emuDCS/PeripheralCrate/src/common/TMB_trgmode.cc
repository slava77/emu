//-----------------------------------------------------------------------
// $Id: TMB_trgmode.cc,v 3.3 2006/08/09 09:39:47 mey Exp $
// $Log: TMB_trgmode.cc,v $
// Revision 3.3  2006/08/09 09:39:47  mey
// Moved TMB_trgmode to TMB.cc
//
// Revision 3.2  2006/08/03 19:01:29  mey
// Update
//
// Revision 3.1  2006/08/03 18:50:49  mey
// Replaced sleep with ::sleep
//
// Revision 3.0  2006/07/20 21:15:48  geurts
// *** empty log message ***
//
// Revision 2.10  2006/06/12 12:47:17  mey
// Update
//
// Revision 2.9  2006/06/09 12:19:39  mey
// UPdate
//
// Revision 2.8  2006/04/27 18:46:04  mey
// UPdate
//
// Revision 2.7  2006/03/20 09:10:43  mey
// Update
//
// Revision 2.6  2006/03/03 07:59:20  mey
// Update
//
// Revision 2.5  2006/01/23 14:21:59  mey
// Update
//
// Revision 2.4  2005/09/22 12:54:52  mey
// Update
//
// Revision 2.3  2005/09/05 06:49:37  mey
// Fixed address
//
// Revision 2.2  2005/08/11 08:13:04  mey
// Update
//
// Revision 2.1  2005/06/06 15:17:18  geurts
// TMB/ALCT timing updates (Martin vd Mey)
//
// Revision 2.0  2005/04/12 08:07:06  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "TMB.h"
#include "JTAG_constants.h"
#include "VMEController.h"
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include "TMB_constants.h"

