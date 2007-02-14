package rcms.fm.app.csc;

import rcms.fm.fw.user.*;
import rcms.fm.fw.EventHandlerException;
import rcms.statemachine.definition.StateMachineDefinitionException;

import rcms.util.logger.RCMSLogger;

public class Level1FunctionManager extends UserFunctionManager {

    private RCMSLogger logger = new RCMSLogger(Level1FunctionManager.class);

	/*
	 *
	 */
	public Level1FunctionManager() {
		// parameterSet is defined in rcms.fw.fm.user.UserFunctionManager
		parameterSet = Level1Parameters.LEVEL1_PARAMETER_SET;
	}

	/*
	 *
	 */
	public void createAction() throws UserActionException {
		logger.debug("Level1FunctionManager.createAction");
	}

	/*
	 *
	 */
	public void destroyAction() throws UserActionException {
		logger.debug("Level1FunctionManager.destroyAction");
	}

	/*
	 *
	 */
	public void init()
			throws StateMachineDefinitionException,
			       EventHandlerException {
		// states + transitions
		setStateMachineDefinition(new Level1StateMachineDefinition());

		// actions associated with states
		addEventHandler(new Level1LeadingActions());
		addEventHandler(new Level1TrailingActions());
	}
}

// End of file
// vim: set sw=4 ts=4:
