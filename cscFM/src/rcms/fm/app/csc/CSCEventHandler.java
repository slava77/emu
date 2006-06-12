package rcms.fm.app.csc;

import rcms.fm.fw.user.UserStateNotificationHandler;

import rcms.fm.app.level1template.MyStates;
import rcms.fm.app.level1template.MyInputs;
import rcms.fm.app.level1template.MyParameters;

import rcms.statemachine.definition.Input;
import rcms.fm.fw.StateEnteredEvent;
import rcms.fm.fw.user.UserActionException;
import rcms.fm.resource.QualifiedResourceContainerException;

import rcms.util.logger.RCMSLogger;

public class CSCEventHandler extends UserStateNotificationHandler {
	
	private RCMSLogger logger = new RCMSLogger(CSCEventHandler.class);

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
		xdaqCommandAction(o, "Configuring", MyInputs.SETCONFIGURE, "Configure");
	}
	
	public void startAction(Object o) throws UserActionException {
		xdaqCommandAction(o, "Starting", MyInputs.SETSTART, "Enable");
	}
	
	public void pauseAction(Object o) throws UserActionException {
		genericAction(o, "Pausing", MyInputs.SETPAUSE);
	}
	
	public void resumeAction(Object o) throws UserActionException {
		genericAction(o, "Resuming", MyInputs.SETRESUME);
	}
	
	public void haltAction(Object o) throws UserActionException {
		xdaqCommandAction(o, "Halting", MyInputs.SETHALT, "Halt");
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

	private void xdaqCommandAction(
			Object o, String message, Input input, String command)
			throws UserActionException {
		if (o instanceof StateEnteredEvent) {
			fm.getParameterSet().put(MyParameters.ACTION_MSG, message);

			if (!fm.xdaqSupervisor.isEmpty()) {
				try {
					fm.xdaqSupervisor.execute(new Input(command));
					logger.info(command + " executed.");
				} catch (QualifiedResourceContainerException e) {
					logger.error(command + " FAILED.", e);
					throw new UserActionException(command + " FAILED.", e);
				}
			}

			fm.fireEvent(input);
		}
	}
}

// End of file
// vim: sw=4 ts=4:
