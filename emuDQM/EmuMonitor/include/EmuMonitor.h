#ifndef EmuMonitor_h
#define EmuMonitor_h

/** The XDAQ application charged with monitoring data quality.
 * It should receive data from a GenericFED, and feed the
 * data to whichever plotter is named in the plotterName_ variable
 \author Victor Barashko
 */
#include "EmuBaseMonitor.h"
#include "EmuPlotterInterface.h"
#include "Task.h"
#include "TaskAttributes.h"


class EmuPlotterInterface;

class EmuMonitorTimerTask: public Task
{

public:
   EmuMonitorTimerTask(): Task("EmuMonitorTimerTask") {
	timerDelay=30;
	plotter = NULL;
        }
  ~EmuMonitorTimerTask() { plotter = NULL;}

   void setTimer(int delay) { timerDelay = delay;}
   void setPlotter(EmuPlotterInterface* pl) {plotter = pl;} 

   int svc() {
	while (1) {
		sleep(timerDelay);
		if (timerDelay && plotter) {
		//	sleep(timerDelay);
			cout << "++++++++++++ Saving Histos ++++++++++++" << endl;
			plotter->SaveHistos();
		}	
	}
	return 0;
   }

private:
	int timerDelay;
	EmuPlotterInterface * plotter;

};


class EmuMonitor : public EmuBaseMonitor 
{
public:

  //! define factory method for instantion of EmuMonitor application
        XDAQ_INSTANTIATOR();
  EmuMonitor(xdaq::ApplicationStub* stub) throw (xdaq::exception::Exception);
//  EmuMonitor();
  ~EmuMonitor();

/*
  void Configure() throw (xdaqException);

  void Enable() throw (xdaqException);

  void Disable() throw (xdaqException);
*/

  xoap::MessageReference fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception);

  void ConfigureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
  void EnableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception );
  void HaltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception );

  // Web callback functions
        void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void dispatch (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception);
        void Configure(xgi::Input * in ) throw (xgi::exception::Exception);
        void Enable(xgi::Input * in ) throw (xgi::exception::Exception);
        void Halt(xgi::Input * in ) throw (xgi::exception::Exception);
        void stateMachinePage( xgi::Output * out ) throw (xgi::exception::Exception);
        void failurePage(xgi::Output * out, xgi::exception::Exception & e)  throw (xgi::exception::Exception);



  virtual void processEvent(const char * data, int dataSize, unsigned long errorFlag);

  xoap::MessageReference sendList (xoap::MessageReference msg) throw (xoap::exception::Exception);
  xoap::MessageReference sendObjects (xoap::MessageReference msg) throw (xoap::exception::Exception);

  void updateList();
  void asktoupdateList();
  void updateObjects();
  void asktoupdateObjects();

protected:
  xdata::String displayServer_;
  xdata::Integer displayServerInstance_;
  xdaq::ApplicationDescriptor* displayServerTID_;
private:
  EmuPlotterInterface * plotter_;
  EmuMonitorTimerTask * timer_;
  xdata::UnsignedLong binCheckMask_;
  xdata::UnsignedLong dduCheckMask_;
  /// decides which plotter to use
  xdata::String plotterName_;
  xdata::Integer plotterSaveTimer_;
  xdata::String plotterHistoFile_;
  xdata::Integer ddu2004_;

  toolbox::fsm::FiniteStateMachine fsm_; // application state machine
  xgi::WSM wsm_; // Web dialog state machine

};

#endif

