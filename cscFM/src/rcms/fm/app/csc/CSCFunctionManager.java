package rcms.fm.app.csc;

import rcms.fm.app.level1template.MyFunctionManager;

import rcms.fm.app.level1template.MyParameters;
import rcms.statemachine.definition.StateMachineDefinitionException;
import rcms.fm.fw.EventHandlerException;

public class CSCFunctionManager extends MyFunctionManager {

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
}

// End of file
// vim: set sw=4 ts=4:
