package rcms.fm.app.csc;

import rcms.fm.fw.user.UserStateNotificationHandler;

import rcms.fm.app.level1template.MyStates;
import rcms.fm.app.level1template.MyInputs;
import rcms.fm.app.level1template.MyParameters;

import rcms.statemachine.definition.Input;
import rcms.fm.fw.StateEnteredEvent;
import rcms.fm.fw.user.UserActionException;

public class CSCEventHandler extends UserStateNotificationHandler {
	
	private CSCFunctionManager fm;
	
	public CSCEventHandler() throws rcms.fm.fw.EventHandlerException {
		subscribeForEvents(StateEnteredEvent.class);
		
		addAction(MyStates.CONFIGURING, "configureAction");
		addAction(MyStates.STARTING, "startAction");		
		addAction(MyStates.PAUSING, "pauseAction");
		addAction(MyStates.RESUMING, "resumeAction");
		addAction(MyStates.HALTING, "haltAction");

		addAction(MyStates.RECOVERING, "recoverAction");
		addAction(MyStates.RESETTING, "resetAction");
		addAction(MyStates.INITIALIZING, "initAction");
	}

	public void init() throws rcms.fm.fw.EventHandlerException {
		fm = (CSCFunctionManager)getUserFunctionManager();
	}

	public void configureAction(Object o) throws UserActionException {
		genericAction(o, "Configuring", MyInputs.SETCONFIGURE);
	}
	
	public void startAction(Object o) throws UserActionException {
		genericAction(o, "Starting", MyInputs.SETSTART);
	}
	
	public void pauseAction(Object o) throws UserActionException {
		genericAction(o, "Pausing", MyInputs.SETPAUSE);
	}
	
	public void resumeAction(Object o) throws UserActionException {
		genericAction(o, "Resuming", MyInputs.SETRESUME);
	}
	
	public void haltAction(Object o) throws UserActionException {
		genericAction(o, "Halting", MyInputs.SETHALT);
	}
	
	public void resetAction(Object o) throws UserActionException {
		genericAction(o, "Resetting", MyInputs.SETRESET);
	}
	
	public void recoverAction(Object o) throws UserActionException {
		genericAction(o, "Recovering", MyInputs.SETINITIAL);
	}

	public void initAction(Object o) throws UserActionException {
		genericAction(o, "Initializing", MyInputs.SETHALT);
	}

	private void genericAction(Object o, String message, Input input) {
		if (o instanceof StateEnteredEvent) {
			fm.getParameterSet().put(MyParameters.ACTION_MSG, message);
			fm.fireEvent(input);
		}
	}
}

// End of file
// vim: sw=4 ts=4:
