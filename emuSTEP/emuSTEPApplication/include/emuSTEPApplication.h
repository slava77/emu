#ifndef _emuSTEPApplication_h_
#define _emuSTEPApplication_h_

//#include "EmuApplication.h"
#include "emu/soap/Messenger.h"

#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"



#include "emuSTEPApplicationV.h"

#include "xgi/Method.h"
#include "xgi/WSM.h"
#include "xgi/Utils.h"
#include "xgi/Input.h"
#include "xgi/Output.h"

#include "toolbox/fsm/FiniteStateMachine.h"
#include "toolbox/task/WorkLoop.h"
#include "toolbox/BSem.h"
#include "xdata/Vector.h"
#include "xdata/Bag.h"
#include "xdata/String.h"
#include "xdata/UnsignedLong.h"

#include "cgicc/HTMLClasses.h"
#include <iostream>
#include <vector>

#include "emuSTEPConfigRead.h"
#include "test_config_struct.h"

//class emuSTEPApplication : public EmuApplication
class emuSTEPApplication : public xdaq::Application
{
	class StateTable;
    friend class StateTable;

 public:
    XDAQ_INSTANTIATOR();

	emuSTEPApplication(xdaq::ApplicationStub * s)
		throw (xdaq::exception::Exception);

	void setParameter(
        string klass, string name, string type, string value);

	string getParameter(string klass, string name, string type);

	void sendCommand(string command, string klass)
		throw (xoap::exception::Exception, xdaq::exception::Exception);

	xoap::MessageReference createParameterSetSOAP(
        string klass, string name, string type, string value);

	xoap::MessageReference createCommandSOAP(string command);

	void analyzeReply(
		xoap::MessageReference message, xoap::MessageReference reply,
		xdaq::ApplicationDescriptor *app);

	std::string toString(long int i);

	std::string toString(long int i, int mindig);
	std::string toHexString(long int i, int mindig);

	xoap::MessageReference createParameterGetSOAP(
        string klass, string name, string type);

	xoap::MessageReference createParameterGetSOAP2(
        string klass, int length, string names[], string types[]);

	xoap::MessageReference createCommandSOAPWithAttr(
        string command, std::map<string, string> attr);

	string extractParameter(
		xoap::MessageReference message, string name);

	string getCGIParameter(xgi::Input *in, string name);

	void Combo
		(
			xgi::Output * out, // output stream
			string varname,  // variable name to return
			string selected, // previously selected value
			string txt[],    // text labels
			string val[],    // values to return
			int n			 // number of items
			);

	void Button(xgi::Output * out, string name);
	void FormButton(xgi::Output * out, string name, string command);

	void TextField
		(
			xgi::Output * out, 
			string varname, 	// name of variable
			string text,        // contents of the text field
			int size			// size
			);

	void br(xgi::Output * out);
	void Label(xgi::Output * out, string name);
	void Checkbox
	(
		xgi::Output * out, 
		string varname, 	// name of variable
		bool defsel      // default selectionC
	);


	void beginForm(xgi::Output * out, string command);
	void endForm(xgi::Output * out);
	void beginFieldSet(xgi::Output * out, string name);
	void endFieldSet(xgi::Output * out);


	std::set<xdaq::ApplicationDescriptor *> getAppsList(string klass);

	class StateTable
		{
		public:
			void addApplication(emuSTEPApplication *sv, string klass);
			void refresh() ;
			string getState(string klass, unsigned int instance);
			void webOutput(xgi::Output *out, string sv_state)
                throw (xgi::exception::Exception);

		private:
			xoap::MessageReference createStateSOAP(string klass);
			string extractState(xoap::MessageReference message, string klass);

			emuSTEPApplication *sv_;
			vector<pair<xdaq::ApplicationDescriptor *, string> > table_;
		} state_table_;

	emuSTEPConfigRead config_reader;

protected:
	void stateChanged(toolbox::fsm::FiniteStateMachine &fsm)
			throw (toolbox::fsm::exception::Exception);


};

class Table
{
public:
	Table(int rows, int cols, int border = 0, bool hcenter = false);
	xgi::Output *cell(int row, int col);
	void flush(xgi::Output * out);

	xgi::Output dummy, v[1000]; // using vector<xgi::Output> fails in compilation with impenetrable errors
	
	int r, c, b;
	bool hc;
};

#endif
