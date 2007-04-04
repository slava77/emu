package rcms.fm.app.csc;

import rcms.fm.fw.user.*;
import rcms.fm.fw.EventHandlerException;
import rcms.fm.fw.parameter.*;
import rcms.fm.fw.parameter.type.*;
import rcms.fm.fw.StateEnteredEvent;
import rcms.fm.resource.QualifiedGroup;
import rcms.stateFormat.StateNotification;

import rcms.util.logger.RCMSLogger;

public class Level1LeadingActions extends UserEventHandler {

	RCMSLogger logger = new RCMSLogger(Level1FunctionManager.class);

	Level1FunctionManager fm;

	/*
	 *
	 */
	public Level1LeadingActions() throws EventHandlerException {
		subscribeForEvents(StateEnteredEvent.class);

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
		logger.debug("Level1LeadingActions.initialize");

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("Initializing")));

		QualifiedGroup group = fm.getQualifiedGroup();

		// Initialize the qualified group, whatever it means.
		try {
			group.init();
		} catch (Exception e) {
			logger.error(getClass().toString() +
					"Failed to initialize resources.", e);

			fm.fireEvent(Level1Inputs.ERROR);
		}

		fm.fireEvent(createStateNotification());
	}

	/*
	 *
	 */
	public void configure(Object o) throws UserActionException {
		emptyAction("Level1LeadingActions.configure", "Confguring");
	}

	/*
	 *
	 */
	public void start(Object o) throws UserActionException {
		emptyAction("Level1LeadingActions.start", "Starting");
	}

	/*
	 *
	 */
	public void stop(Object o) throws UserActionException {
		emptyAction("Level1LeadingActions.stop", "Stopping");
	}

	/*
	 *
	 */
	public void pause(Object o) throws UserActionException {
		emptyAction("Level1LeadingActions.pause", "Pausing");
	}

	/*
	 *
	 */
	public void resume(Object o) throws UserActionException {
		emptyAction("Level1LeadingActions.resume", "Resuming");
	}

	/*
	 *
	 */
	public void recover(Object o) throws UserActionException {
		emptyAction("Level1LeadingActions.recover", "Recovering");
	}

	/*
	 *
	 */
	public void reset(Object o) throws UserActionException {
		emptyAction("Level1LeadingActions.reset", "Resetting");
	}

	/*
	 *
	 */
	public void ttsPrepare(Object o) throws UserActionException {
		emptyAction("Level1LeadingActions.ttsPrepare", "TTS preparing");
	}

	/*
	 *
	 */
	public void ttsTest(Object o) throws UserActionException {
		emptyAction("Level1LeadingActions.ttsTest", "TTS testing");
	}

	/*
	 *
	 */
	public void halt(Object o) throws UserActionException {
		emptyAction("Level1LeadingActions.halt", "Halting");
	}

	/*
	 *
	 */
	protected void emptyAction(String title, String message)
			throws UserActionException {
		logger.debug(title);

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT(message)));

		fm.fireEvent(createStateNotification());

		logger.debug(title + " ... done.");
	}

	/*
	 *
	 */
	protected StateNotification createStateNotification() throws UserActionException {
		StateNotification notification = new StateNotification();

		notification.setIdentifier("level1FM");

		return notification;
	}
}

// End of file
// vim: set sw=4 ts=4:
