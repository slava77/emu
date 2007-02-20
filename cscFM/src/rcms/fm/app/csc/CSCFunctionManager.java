package rcms.fm.app.csc;

import rcms.fm.fw.EventHandlerException;
import rcms.statemachine.definition.StateMachineDefinitionException;

import rcms.fm.resource.qualifiedresource.XdaqApplicationContainer;
import rcms.fm.resource.qualifiedresource.XdaqApplication;
import rcms.fm.fw.parameter.ParameterSet;
import rcms.xdaqctl.XDAQParameter;

import rcms.util.logger.RCMSLogger;

public class CSCFunctionManager extends Level1FunctionManager {

    RCMSLogger logger = new RCMSLogger(CSCFunctionManager.class);

	XdaqApplicationContainer xdaqSupervisor = null;

	/*
	 *
	 */
	public void init()
			throws StateMachineDefinitionException,
			       EventHandlerException {
		// states + transitions
		setStateMachineDefinition(new Level1StateMachineDefinition());

		// actions associated with states
		addEventHandler(new CSCLeadingActions());
		addEventHandler(new Level1TrailingActions());
	}
}

// End of file
// vim: set sw=4 ts=4:
