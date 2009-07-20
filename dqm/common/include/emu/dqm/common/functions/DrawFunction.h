/*
 * =====================================================================================
 *
 *       Filename:  DrawFunction.h
 *
 *    Description:  Function that draws histograms.
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


#ifndef DrawFunction_h
#define DrawFunction_h

#include "AbstractFunction.h"

class DrawFunction: public AbstractFunction
{

public:

  MonitorElement* calc(FunctionParameters& vme)
  {

    MonitorElement* me;

    for (unsigned int i = 0; i < vme.size(); i++)
      {
        if (vme.at(i) != NULL)
          {
            me = dynamic_cast<MonitorElement*>(vme.at(i)->Clone());
            me->Draw();
          }
      }

    return NULL;

  };

};

#endif
