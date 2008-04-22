/*
 * =====================================================================================
 *
 *       Filename:  SetFunction.h
 *
 *    Description:  Buildin function that returns second histogram or NULL.
 *    Used for assign operations Set(h1, h2).
 *
 *        Version:  1.0
 *        Created:  04/15/2008 03:36:40 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Valdas Rapsevicius (VR), Valdas.Rapsevicius@cern.ch
 *        Company:  CERN, CH
 *
 * =====================================================================================
 */


#ifndef SetFunction_h
#define SetFunction_h

#include "AbstractFunction.h"

class SetFunction: public AbstractFunction {

  public:

    MonitorElement* calc(FunctionParameters& vme) { 

      MonitorElement* me = NULL;
      // 
      // If provided histograms less then two - return NULL
      //
      if(vme.size() < 2) {
        return me;
      }

      //
      // else - return second element
      //
      me = dynamic_cast<MonitorElement*>(vme.at(1)->Clone());
      return me;

    };

};

#endif
