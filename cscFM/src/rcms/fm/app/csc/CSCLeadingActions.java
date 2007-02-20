package rcms.fm.app.csc;

import rcms.fm.fw.user.*;
import rcms.fm.fw.EventHandlerException;
import rcms.fm.fw.parameter.*;
import rcms.fm.fw.parameter.type.*;
import rcms.fm.fw.StateEnteredEvent;
import rcms.fm.resource.QualifiedGroup;
import rcms.stateFormat.StateNotification;
import rcms.statemachine.definition.Input;

import rcms.fm.resource.qualifiedresource.XdaqApplicationContainer;
import rcms.fm.resource.qualifiedresource.XdaqApplication;
import rcms.xdaqctl.XDAQParameter;

import java.util.Timer;
import java.util.TimerTask;

import rcms.util.logger.RCMSLogger;

public class CSCLeadingActions extends Level1LeadingActions {

	private CSCFunctionManager fm;

	private XDAQParameter svStateParameter;
	private XDAQParameter svTTSParameter;
	private Timer timer;
	private SetTTSTask setTTSTask;

	class SetTTSTask extends TimerTask {
		int crate, slot, bits, repeat;

		public void run() {
			System.out.println("==== " + slot + " " + bits);

			try {
				svTTSParameter.setValue("TTSCrate", "" + crate);
				svTTSParameter.setValue("TTSSlot", "" + slot);
				svTTSParameter.setValue("TTSBits", "" + bits);
				svTTSParameter.send();
				fm.xdaqSupervisor.execute(new Input("SetTTSBits"));

				fm.fireEvent(createStateNotification());
			} catch (Exception e) {
				logger.error(e);
				fm.fireEvent(Level1Inputs.ERROR);
			}

			cancel();
		}

		public void config(int crate, int slot, int bits, int repeat) {
			this.crate = crate;
			this.slot = slot;
			this.bits = bits;
			this.repeat = repeat;
		}
	}

	/*
	 *
	 */
	public CSCLeadingActions() throws EventHandlerException {
		super();
	}
	
	/*
	 *
	 */
	public void init() throws EventHandlerException {
		fm = (CSCFunctionManager)getUserFunctionManager();
	}

	/*
	 *
	 */
	public void initialize(Object o) throws UserActionException {
		logger.debug("CSCLeadingActions.initialize");

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

		XdaqApplicationContainer xdaqApps = new XdaqApplicationContainer(
				fm.getQualifiedGroup().seekQualifiedResourcesOfType(
				new XdaqApplication()));

		fm.xdaqSupervisor = new XdaqApplicationContainer(
				xdaqApps.getApplicationsOfClass("CSCSupervisor"));

		try { 
			svStateParameter = ((XdaqApplication)fm.xdaqSupervisor.getApplications().get(0)).getXDAQParameter();
			svStateParameter.select("stateName");

			svTTSParameter = ((XdaqApplication)fm.xdaqSupervisor.getApplications().get(0)).getXDAQParameter();
			svTTSParameter.select(new String[] {"TTSCrate", "TTSSlot", "TTSBits"});
		} catch (Exception e) {
			logger.error(getClass().toString() +
					"Failed to prepare XDAQ parameters.", e);

			fm.fireEvent(Level1Inputs.ERROR);
		}

		fm.fireEvent(createStateNotification());
	}

	/*
	 *
	 */
	public void ttsPrepare(Object o) throws UserActionException {
		logger.debug("CSCLeadingActions.ttsPrepare");

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("TTS preparing")));

		try {
			fm.xdaqSupervisor.execute(new Input("Configure"));
			waitForState("Configured");

			fm.xdaqSupervisor.execute(new Input("Enable"));
			waitForState("Enabled");
		} catch (Exception e) {
			logger.error(getClass().toString() +
					"Failed to TTSPrepare XDAQ.", e);

			fm.fireEvent(Level1Inputs.ERROR);
		}

		fm.fireEvent(createStateNotification());
	}

	/*
	 *
	 */
	public void ttsTest(Object o) throws UserActionException {
		logger.debug("CSCLeadingActions.ttsTest");

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("TTS testing")));

		int crate, slot, bits, repeat;
		ParameterSet<CommandParameter> parameterSet =
				getUserFunctionManager().getLastInput().getParameterSet();

		crate = 0;
		slot = getSlotNumber(
				((IntegerT)parameterSet.get(
				Level1Parameters.TTS_TEST_FED_ID)
				.getValue()).getInteger());
		bits = Integer.parseInt(
				((StringT)parameterSet.get(
				Level1Parameters.TTS_TEST_PATTERN)
				.getValue()).getString());
		if (((StringT)parameterSet.get(Level1Parameters.TTS_TEST_PATTERN)
				.getValue()).getString().equals("PATTERN")) {
			repeat = 0;
		} else {  // CYCLE
			repeat = ((IntegerT)parameterSet.get(
					Level1Parameters.TTS_TEST_SEQUENCE_REPEAT)
					.getValue()).getInteger();
		}

		if (timer == null) {
			timer = new Timer();
		}

		if (setTTSTask == null) {
			setTTSTask = new SetTTSTask();
		}
		setTTSTask.config(crate, slot, bits, repeat);

		timer.schedule(setTTSTask, 0, 10); // 10ms interval
	}

	/*
	 *
	 */
	public void halt(Object o) throws UserActionException {
		logger.debug("CSCLeadingActions.halt");

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("Halting")));

		fm.fireEvent(createStateNotification());
	}

	/* */
	private void waitForState(String target) {
		String state = "";
		
		while (!target.equals(state)) {
			try { Thread.sleep(500); } catch (Exception ignored) {}

			try {
				svStateParameter.get();
				state = svStateParameter.getValue("stateName");
			} catch (Exception e) {
				logger.error(getClass().toString() +
						"Failed to get XDAQ state.", e);

				fm.fireEvent(Level1Inputs.ERROR);
			}
		}
	}

	/* */
	private int getSlotNumber(int fed) {
		int slot = 0;

		switch (fed) {
			case 750: slot = 8; break;
			case 841: slot = 4; break;
			case 842: slot = 5; break;
			case 843: slot = 6; break;
			case 844: slot = 7; break;
			case 845: slot = 9; break;
			case 846: slot = 10; break;
			case 847: slot = 11; break;
			case 848: slot = 12; break;
			case 849: slot = 13; break;
		}

		return slot;
	}
}

// End of file
// vim: set sw=4 ts=4:
