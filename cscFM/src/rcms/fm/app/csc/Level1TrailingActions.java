package rcms.fm.app.csc;

import rcms.fm.fw.user.*;
import rcms.fm.fw.EventHandlerException;
import rcms.fm.fw.parameter.*;
import rcms.fm.fw.parameter.type.*;
import rcms.stateFormat.StateNotification;

import rcms.util.logger.RCMSLogger;

public class Level1TrailingActions extends UserEventHandler {

    RCMSLogger logger = new RCMSLogger(Level1FunctionManager.class);

	Level1FunctionManager fm;

	/*
	 *
	 */
	public Level1TrailingActions() throws EventHandlerException {
		subscribeForEvents(StateNotification.class);

		addAction(Level1States.INITIALIZING,  "initialize");
		addAction(Level1States.TTS_PREPARING, "ttsPrepare");
		addAction(Level1States.TTS_TESTING,   "ttsTest");
		addAction(Level1States.HALTING,       "halt");
	}

	/*
	 *
	 */
	public void init() throws EventHandlerException {
		fm = (Level1FunctionManager)getUserFunctionManager();
	}

	/*
	 *
	 */
	public void initialize(Object o) throws UserActionException {
		logger.debug("Level1TrailingActions.initialize");

		StateNotification notification = (StateNotification)o;

		System.out.println("==== Level1TrailingActions.initialize: " + 
				notification.getIdentifier());

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("")));

		fm.fireEvent(Level1Inputs.INITIALIZE_DONE);

		logger.debug("Level1TrailingActions.initialize ... done.");
	}

	/*
	 *
	 */
	public void ttsPrepare(Object o) throws UserActionException {
		logger.debug("Level1TrailingActions.ttsPrepare");

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("")));

		fm.fireEvent(Level1Inputs.TTS_PREPARE_DONE);

		logger.debug("Level1TrailingActions.ttsPrepare ... done.");
	}

	/*
	 *
	 */
	public void ttsTest(Object o) throws UserActionException {
		logger.debug("Level1TrailingActions.ttsTest");

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("")));

		fm.fireEvent(Level1Inputs.TTS_TEST_DONE);

		logger.debug("Level1TrailingActions.ttsTest ... done.");
	}

	/*
	 *
	 */
	public void halt(Object o) throws UserActionException {
		logger.debug("Level1TrailingActions.halt");

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("")));

		fm.fireEvent(Level1Inputs.HALT_DONE);

		logger.debug("Level1TrailingActions.halt ... done.");
	}
}

// End of file
// vim: set sw=4 ts=4:
