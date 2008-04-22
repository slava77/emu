/*
 * =====================================================================================
 *
 *       Filename:  AddFunction.h
 *
 *    Description:  Function that adds histograms to each other and returns the
 *    result. Function can receive whatever number of histograms. Histograms 
 *    should be of the same type.
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


#ifndef AddFunction_h
#define AddFunction_h

#include "AbstractFunction.h"

class AddFunction: public AbstractFunction {

  public:

    MonitorElement* calc(FunctionParameters& vme) { 

      MonitorElement* me = NULL;
      for(unsigned int i = 0; i < vme.size(); i++) {
        if(vme.at(i) != NULL ) {
          if(me == NULL) {
            me = dynamic_cast<MonitorElement*>(vme.at(i)->Clone());
          } else {
            me->Add(vme.at(i));
          }
        }
      }
      return me;

    };

};

#endif
