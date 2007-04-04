package rcms.fm.app.csc;

import rcms.fm.fw.user.*;
import rcms.fm.fw.EventHandlerException;
import rcms.fm.fw.parameter.*;
import rcms.fm.fw.parameter.type.*;
import rcms.stateFormat.StateNotification;
import rcms.statemachine.definition.Input;

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
		addAction(Level1States.CONFIGURING,   "configure");
		addAction(Level1States.STARTING,      "start");
		addAction(Level1States.STOPPING,      "stop");
		addAction(Level1States.PAUSING,       "pause");
		addAction(Level1States.RESUMING,      "resume");

		addAction(Level1States.RECOVERING,    "recover");
		addAction(Level1States.RESETTING,     "reset");

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
		emptyAction("Level1TrailingActions.initialize",
				Level1Inputs.INITIALIZE_DONE);
	}

	/*
	 *
	 */
	public void configure(Object o) throws UserActionException {
		emptyAction("Level1TrailingActions.configure",
				Level1Inputs.CONFIGURE_DONE);
	}

	/*
	 *
	 */
	public void start(Object o) throws UserActionException {
		emptyAction("Level1TrailingActions.start",
				Level1Inputs.START_DONE);
	}

	/*
	 *
	 */
	public void stop(Object o) throws UserActionException {
		emptyAction("Level1TrailingActions.stop",
				Level1Inputs.STOP_DONE);
	}

	/*
	 *
	 */
	public void pause(Object o) throws UserActionException {
		emptyAction("Level1TrailingActions.pause",
				Level1Inputs.PAUSE_DONE);
	}

	/*
	 *
	 */
	public void resume(Object o) throws UserActionException {
		emptyAction("Level1TrailingActions.resume",
				Level1Inputs.RESUME_DONE);
	}

	/*
	 *
	 */
	public void recover(Object o) throws UserActionException {
		emptyAction("Level1TrailingActions.recover",
				Level1Inputs.RECOVER_DONE);
	}

	/*
	 *
	 */
	public void reset(Object o) throws UserActionException {
		emptyAction("Level1TrailingActions.reset",
				Level1Inputs.RESET_DONE);
	}

	/*
	 *
	 */
	public void ttsPrepare(Object o) throws UserActionException {
		emptyAction("Level1TrailingActions.ttsPrepare",
				Level1Inputs.TTS_PREPARE_DONE);
	}

	/*
	 *
	 */
	public void ttsTest(Object o) throws UserActionException {
		emptyAction("Level1TrailingActions.ttsTest",
				Level1Inputs.TTS_TEST_DONE);
	}

	/*
	 *
	 */
	public void halt(Object o) throws UserActionException {
		emptyAction("Level1TrailingActions.halt",
				Level1Inputs.HALT_DONE);
	}

	/*
	 *
	 */
	protected void emptyAction(String title, Input input)
			throws UserActionException {

		logger.debug(title);

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("")));

		fm.fireEvent(input);

		logger.debug(title + " ... done.");
	}
}

// End of file
// vim: set sw=4 ts=4:
