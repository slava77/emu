package rcms.fm.app.csc;

import rcms.fm.app.level1template.MyFunctionManager;

import rcms.fm.app.level1template.MyParameters;
import rcms.statemachine.definition.StateMachineDefinitionException;
import rcms.fm.fw.user.UserActionException;
import rcms.fm.fw.EventHandlerException;
import rcms.fm.resource.qualifiedresource.XdaqApplicationContainer;
import rcms.fm.resource.qualifiedresource.XdaqApplication;

import rcms.util.logger.RCMSLogger;

public class CSCFunctionManager extends MyFunctionManager {

    private RCMSLogger logger = new RCMSLogger(CSCFunctionManager.class);

	XdaqApplicationContainer xdaqSupervisor = null;

	public CSCFunctionManager() {
		parameterSet.put("runType", "");
	}

	private void addParameters() {
		parameterSet = MyParameters.LVL_ONE_PARAMETER_SET;
	}

	public void init()
			throws StateMachineDefinitionException,
			       EventHandlerException {
		setStateMachineDefinition(new CSCStateMachineDefinition());
		addEventHandler(new CSCEventHandler());
	}

	public void createAction() throws UserActionException {

		try {
			qualifiedGroup.init();
		} catch (Exception e) {
			logger.error("qualifiedGroup.init() failed.", e);
		}

		XdaqApplicationContainer xdaqApps = new XdaqApplicationContainer(
				qualifiedGroup.seekQualifiedResourcesOfType(
				new XdaqApplication()));

		xdaqSupervisor = new XdaqApplicationContainer(
				xdaqApps.getApplicationsOfClass("CSCSupervisor"));

		System.out.println(
				"createAction(): xdaqSupervisor: " + xdaqSupervisor.isEmpty());
		logger.info("xdaqSupervisor: " + xdaqSupervisor.isEmpty());

		super.createAction();
	}
}

// End of file
// vim: set sw=4 ts=4:
