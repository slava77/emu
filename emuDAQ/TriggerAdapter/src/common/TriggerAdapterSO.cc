//-----------------------------------------------------------------------
// $Id: TriggerAdapterSO.cc,v 2.0 2005/07/08 12:15:41 geurts Exp $
// $Log: TriggerAdapterSO.cc,v $
// Revision 2.0  2005/07/08 12:15:41  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#include "TriggerAdapterSO.hh"
#include "TriggerAdapter.hh"

void TriggerAdapterSO::init()
{
    U16           localHost       = xdaq::getHostId();
    int           typeIndex       = 0;
    int           numInstances    = 0;
    int           instanceIndex   = 0;
    I2O_TID       tId             = 0;
    U16           targetHost      = 0;
    xdaqPluggable *plugin         = 0;
    char*         appClassnames[] = {"TriggerAdapter","end"};


    // While there are more types of xdaq application that can be instantiated
    while(strcmp(appClassnames[typeIndex], "end") != 0)
    {
        numInstances = xdaq::getNumInstances(appClassnames[typeIndex]);
        // For each instance
        for(instanceIndex=0; instanceIndex<numInstances; instanceIndex++)
        {
            // Get the target host on which the instance should be instantiated
            tId        = xdaq::getTid(appClassnames[typeIndex], instanceIndex);
            targetHost = xdaq::getHostId(tId);
            // Instantiate and load the instance if this is the target host
            if(localHost == targetHost)
            {
                plugin = createXdaqApp(appClassnames[typeIndex]);
                xdaq::load(plugin);
            }
        }

        typeIndex++;
    }
}


xdaqPluggable* TriggerAdapterSO::createXdaqApp(const char *classname)
{
    if(strcmp(classname, "TriggerAdapter" ) == 0) return new TriggerAdapter();

    cout << "ERROR in TriggerAdapterSO::createXdaqApp()\n";
    cout << "Requested to create an unknown event builder application\n";
    cout << "Application classname = " << classname << "\n";
    cout << flush;
    exit(-1);
}


void TriggerAdapterSO::shutdown()
{
}

extern "C" {
  void * init_TriggerAdapter() {
    return ((void*) new TriggerAdapterSO() );
  }
}
