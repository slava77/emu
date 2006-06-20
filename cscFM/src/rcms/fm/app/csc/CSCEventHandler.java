package rcms.fm.app.csc;

import rcms.fm.fw.user.UserStateNotificationHandler;

import rcms.fm.app.level1template.MyStates;
import rcms.fm.app.level1template.MyInputs;
import rcms.fm.app.level1template.MyParameters;

import rcms.fm.parameter.ParameterSet;
import rcms.xdaqctl.XDAQParameter;
import rcms.fm.resource.qualifiedresource.XdaqApplication;

import rcms.statemachine.definition.Input;
import rcms.fm.fw.StateEnteredEvent;
import rcms.fm.fw.user.UserActionException;
import rcms.fm.resource.QualifiedResourceContainerException;

import rcms.util.logger.RCMSLogger;

public class CSCEventHandler extends UserStateNotificationHandler {
	
	private RCMSLogger logger = new RCMSLogger(CSCEventHandler.class);

	private CSCFunctionManager fm;
	private ParameterSet fmParameters;

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

	public void initAction(Object o) throws UserActionException {
		genericAction(o, "Initializing", MyInputs.SETHALT);

		fmParameters = fm.getParameterSet();
	}

	public void configureAction(Object o) throws UserActionException {
		if (o instanceof StateEnteredEvent) {
			fm.getParameterSet().put(MyParameters.ACTION_MSG, "Configuring");

			if (!fm.xdaqSupervisor.isEmpty()) {
				/*
				ParameterSet param = new ParameterSet();
				param.put("RunType", fmParameters.get("RUN_TYPE"));
				param.put("RunNumber", fmParameters.get("RUN_NUMBER"));

				Input input = new Input("Configure");
				input.setParameters(param);
				*/

				XDAQParameter param = null;
				try {
					param = ((XdaqApplication)
							fm.xdaqSupervisor.getApplications().get(0))
							.getXDAQParameter();

					param.select(new String[] { "RunType", "RunNumber" });
					param.setValue("RunType", fmParameters.get("RUN_TYPE"));
					param.setValue("RunNumber", fmParameters.get("RUN_NUMBER"));
					param.send();
				} catch (Exception ignored) {}

				Input input = new Input("Configure");

				try {
					fm.xdaqSupervisor.execute(input);
					logger.info("Configure executed.");
				} catch (QualifiedResourceContainerException e) {
					logger.error("Configure FAILED.", e);
					throw new UserActionException("Configure FAILED.", e);
				}
			}

			fm.fireEvent(MyInputs.SETCONFIGURE);
		}
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
		xdaqCommandAction(o, "Resetting", MyInputs.SETRESET, "Reset");
	}
	
	public void recoverAction(Object o) throws UserActionException {
		genericAction(o, "Recovering", MyInputs.SETINITIAL);
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
