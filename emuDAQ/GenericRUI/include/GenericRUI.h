//-----------------------------------------------------------------------
// $Id: GenericRUI.h,v 2.0 2005/04/13 10:53:01 geurts Exp $
// $Log: GenericRUI.h,v $
// Revision 2.0  2005/04/13 10:53:01  geurts
// Makefile
//
//
//-----------------------------------------------------------------------
#ifndef __GenericRUI_h__
#define __GenericRUI_h__

#include "xdaq.h"
#include "RUIDataRequestListener.h"
#include "EventReader.h"
#include "EventWriter.h"
#include <unistd.h>

/** This class will constantly poll its EventReader and send the events to a RU.  It can also send events, after a suitable prescaling, to some monitor, and respond to requests for its current event.
*/

class RUITaskImpl
{
public:

    virtual int taskSvc() = 0;
};


class RUITask : public Task
{
public:

    RUITask(char *name, RUITaskImpl *taskImpl) :
    Task(name),
    taskImpl_(taskImpl)
    {
    }


    /**
     * Entry point of RUI task thread.
     */
    int svc()
    {
        return taskImpl_->taskSvc();
    }


private:

    RUITaskImpl *taskImpl_;
};


/**
 * \ingroup xdaqApps
 * \brief Test readout unit input (RUI)
 */
class GenericRUI : public virtual xdaqApplication, public RUITaskImpl, public RUIDataRequestListener
{
public:

    /**
     * Constructor.
     */
    GenericRUI();

    /**
     * Destructor - assumes the RUI task is no longer running.
     */
    virtual ~GenericRUI();


    /**
     * Entry point for the RUI task thread.
     */
    int taskSvc();

    void plugin();

    /**
     * Standard SOAP callback routine.
     */
    void Configure() throw (xdaqException);
    void Enable() throw (xdaqException);
    void Halt() throw (xdaqException);

    void sendFakeEvent(int eventNumber);

    ///  other programs can request to be sent the current event
    virtual void handleDataRequest(BufRef * ref);

protected:
    // packs into I2O format
    void sendMessage(char *data, int dataLength, int eventNumber, int targetAddress);
    // sends a package, along with a standard header and trailer added
    void addCDFAndSend(char *data, int dataLength, int eventNumber,int targetAddress);

    // prints data to the screen, for fake mode
    void printData(char *data, int dataLength);

    // Loadable reader/writer modules, assigned in sub-class Configure()
    EventReader *eventReader_;
    EventWriter *eventWriter_;

private:

    /**
     * Exported parameter - Event number
     */
    int eventNumber_;
    int maxEvents_;
    int nEvents_;

    int m_lastEventNumber; // for faking up

    /**
     * Exported parameter - Size of an event data I2O message frame.
     */
    char          taskName_[8];
    RUITask       *task_;
    volatile bool taskActivated_;
    volatile bool taskShouldRun_;
    int           ruTid_;

    // name of routine used by this FED to monitor data quality
    string dataQualityMonitor_;
    // the instance of monitor to be used by this FED
    int dataQualityMonitorInstance_;
    int dataQualityMonitorTid_;
    /// if data should be sent automatically to the monitor, prescale at this rate.
    /// A value of 10 means every 10th event is sent.  ns every event, and "0" (default) means no events
    int prescaleForPushingToDQM_;

    /// we support requests for data, using an RUIDataRequest.  Should be -1 when no request active
    int requestorTid_;

    /// the source ID in this data's header.  Not checked as of yet.
    /// only used to make empty fragments
    int sourceId_;
    /// whether or not the RUI should add a standard header & trailer to the data
    bool addCDF_;
    /// whether to send empty events if the event number is missing
    bool sendEmptyEvents_;

    /// whether to run in fake mode, and only print data to the screen
    bool fakeMode_;

    /// whether we're running with Jeremy's EVB, which has different ideas
    /// of event numbers
    bool jeremyEVB_;

    int emptyEvent_[4];
    void sendEmptyEvent(int eventNumber);
    void initializeEmptyEvent();
};

#endif
