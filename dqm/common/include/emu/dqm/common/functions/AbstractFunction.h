/*
 * =====================================================================================
 *
 *       Filename:  AbstractFunction.h
 *
 *    Description:  Base Transformer Class.
 *
 *        Version:  1.0
 *        Created:  04/14/2008 10:07:42 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Valdas Rapsevicius (VR), Valdas.Rapsevicius@cern.ch
 *        Company:  CERN, CH
 *
 * =====================================================================================
 */


#ifndef AbstractFunction_h
#define AbstractFunction_h

#include <string>
#include <iostream>
#include <math.h>
#include <vector>

#include "../EmuMonitoringObject.h"

using namespace std;

typedef vector<MonitorElement*> FunctionParameters;
typedef FunctionParameters::iterator FunctionParametersIterator;

class AbstractFunction
{

public:

  /**
   * @brief  Virtual transformation function to be implemented in
   * transformation class.
   *
   * @description Please notice that parameters forwarded by vme
   * are clones (!) of real histograms and will be deleted immediatelly after
   * function exit ;) If you wish to output anything please Clone smth and
   * do whatever u want.
   *
   * @param  vme Monitor Element (historgram)
   *
   * @return void
   */
  virtual MonitorElement* calc(FunctionParameters& vme)
  {
    return NULL;
  };

};

#endif
