/*
 * =====================================================================================
 *
 *       Filename:  DivideFunction.h
 *
 *    Description:  Function that divides first histogram by the second and all other
 *    histograms supplied and returns result. Histograms should be of the same type.
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


#ifndef DivideFunction_h
#define DivideFunction_h

#include "AbstractFunction.h"

class DivideFunction: public AbstractFunction {

  public:

    MonitorElement* calc(FunctionParameters& vme) { 

      MonitorElement* me = dynamic_cast<MonitorElement*>(vme.at(0)->Clone());
      if(me != NULL) {
        for(unsigned int i = 1; i < vme.size(); i++) {
          if(vme.at(i) != NULL ) {
            me->Divide(vme.at(i));
          }
        }
      }

      if(me != NULL) {
        if(me->GetMaximum() > 0.001){
          me->SetMaximum(1.0);
        }
      }

      return me;

    };

};

#endif
