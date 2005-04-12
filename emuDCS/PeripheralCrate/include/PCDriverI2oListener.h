//-----------------------------------------------------------------------
// $Id: PCDriverI2oListener.h,v 2.0 2005/04/12 08:07:03 geurts Exp $
// $Log: PCDriverI2oListener.h,v $
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
// Revision 1.1  2004/06/01 21:53:11  tfcvs
// Allow for i2o messaging (LG/FG)
//
//-----------------------------------------------------------------------
// Description: Header file for PCDriverI2oListener class
//
#ifndef PCDRIVERI2OLISTENER_H
#define PCDRIVERI2OLISTENER_H
#include "i2oListener.h"
#include "BufRef.h"
#include "PCI2oMsgs.h"

/**
 * Declares the I2O methods of an PCDriver Listener.
 */
class PCDriverI2oListener: public virtual i2oListener
{
public:

    /**
     * Binds the I2O methods of this listener.
     */
    PCDriverI2oListener();

    /**
     * See declaration of implementing class.
     */
    virtual void StartPRBSMode(BufRef *TFbufRef) = 0;
    virtual void EndPRBSMode(BufRef *TFbufRef) = 0;    

};

#endif

