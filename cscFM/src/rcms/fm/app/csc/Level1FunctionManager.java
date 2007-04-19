package rcms.fm.app.csc;

import rcms.fm.fw.user.*;
import rcms.fm.fw.EventHandlerException;
import rcms.statemachine.definition.StateMachineDefinitionException;
import rcms.fm.resource.*;
import rcms.fm.resource.qualifiedresource.*;

import java.util.List;

import rcms.util.logger.RCMSLogger;

public class Level1FunctionManager extends UserFunctionManager {

    RCMSLogger logger = new RCMSLogger(Level1FunctionManager.class);

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

		QualifiedGroup group = getQualifiedGroup();

		List<QualifiedResource> list;

		// destroy XDAQ executives
		list = group.seekQualifiedResourcesOfType(new XdaqExecutive());

		for (QualifiedResource r: list) {
			logger.debug("==== killing " + r.getURI());
			try {
				((XdaqExecutive)r).killMe();
			} catch (Exception e) {
				logger.error(
						"Could not destroy a XDAQ executive " + r.getURI(), e);
			}
		}

		// destroy function managers
		list = group.seekQualifiedResourcesOfType(new FunctionManager());

		for (QualifiedResource r: list) {
			logger.debug("==== killing " + r.getURI());

			FunctionManager fm = (FunctionManager)r;

			if (fm.isInitialized()) {
				try {
					fm.destroy();
				} catch (Exception e) {
					logger.error("Could not destroy a FM " + r.getURI(), e);
				}
			}
		}
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
