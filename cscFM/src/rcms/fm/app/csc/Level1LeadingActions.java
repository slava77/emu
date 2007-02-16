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
	public void ttsPrepare(Object o) throws UserActionException {
		logger.debug("Level1LeadingActions.ttsPrepare");

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("TTS preparing")));

		fm.fireEvent(createStateNotification());
	}

	/*
	 *
	 */
	public void ttsTest(Object o) throws UserActionException {
		logger.debug("Level1LeadingActions.ttsTest");

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("TTS testing")));

		fm.fireEvent(createStateNotification());
	}

	/*
	 *
	 */
	public void halt(Object o) throws UserActionException {
		logger.debug("Level1LeadingActions.halt");

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("Halting")));

		fm.fireEvent(createStateNotification());
	}

	/*
	 *
	 */
	public StateNotification createStateNotification() throws UserActionException {
		StateNotification notification = new StateNotification();

		notification.setIdentifier("cscFM");

		return notification;
	}
}

// End of file
// vim: set sw=4 ts=4:
