package rcms.fm.app.csc;

import rcms.fm.fw.user.*;
import rcms.fm.fw.EventHandlerException;
import rcms.fm.fw.parameter.*;
import rcms.fm.fw.parameter.type.*;
import rcms.fm.fw.StateEnteredEvent;
import rcms.fm.resource.QualifiedGroup;
import rcms.fm.resource.QualifiedResource;
import rcms.stateFormat.StateNotification;
import rcms.statemachine.definition.Input;

import rcms.fm.resource.qualifiedresource.XdaqApplicationContainer;
import rcms.fm.resource.qualifiedresource.XdaqApplication;
import rcms.fm.resource.qualifiedresource.XdaqExecutive;
import rcms.fm.resource.qualifiedresource.JobControl;
import rcms.resourceservice.db.resource.xdaq.XdaqExecutiveResource;
import rcms.xdaqctl.XDAQParameter;

import java.util.List;
import java.util.concurrent.*;
import static java.util.concurrent.TimeUnit.*;

import rcms.util.logger.RCMSLogger;

public class CSCLeadingActions extends Level1LeadingActions {

	private CSCFunctionManager fm;

	private XDAQParameter svStateParameter;
	private XDAQParameter svTTSParameter;

	private final ScheduledExecutorService scheduler;
	private StateWatcher stateWatcher;
	private TTSSetter ttsSetter;
	private ScheduledFuture stateWatcherFuture, ttsSetterFuture;

	private class StateWatcher implements Runnable {
		private String target = "";

		public synchronized void setTarget(String target) {
			this.target = target;
		}

		public void run() {
			if (target.length() > 0) {
				try {
					svStateParameter.get();

					String state = svStateParameter.getValue("stateName");
					logger.debug("StateWatcher: " + target + "? " + state);

					if (target.equals(state)) {
						setTarget("");
						fm.fireEvent(createStateNotification());
					}
				} catch (Exception e) {
					logger.error("StateWatcher", e);
					fm.fireEvent(Level1Inputs.ERROR);
				}
			}
		}
	}

	private class TTSSetter implements Runnable {
		private int crate, slot, bits;
		private int repeat = 0;

		public synchronized void config(
				int crate, int slot, int bits, int repeat) {
			this.crate = crate;
			this.slot = slot;
			this.bits = bits;
			this.repeat = repeat * 16;

			if (this.repeat == 0) {
				this.repeat = 1;
			}
		}

		public void run() {
			if (repeat > 0) {
				try {
					logger.debug("TTSSetter: " +
							crate + " " + slot + " " + bits + " " + repeat);

					svTTSParameter.setValue("TTSCrate", "" + crate);
					svTTSParameter.setValue("TTSSlot", "" + slot);
					svTTSParameter.setValue("TTSBits", "" + bits);
					svTTSParameter.send();
					fm.xdaqSupervisor.execute(new Input("SetTTS"));

					bits = (bits + 1) % 16;  // prepare for the next shot.
					repeat--;

					if (repeat == 0) {
						fm.fireEvent(Level1Inputs.TTS_TEST_DONE);
					}
				} catch (Exception e) {
					logger.error("TTSSetter", e);
					fm.fireEvent(Level1Inputs.ERROR);
				}
			}
		}
	}

	/*
	 *
	 */
	public CSCLeadingActions() throws EventHandlerException {
		scheduler = Executors.newScheduledThreadPool(1);
		stateWatcher = new StateWatcher();
		ttsSetter = new TTSSetter();

		stateWatcherFuture = null;
		ttsSetterFuture = null;
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

		// get a user name from the first XDAQ executive
		QualifiedResource r = fm.getQualifiedGroup()
				.seekQualifiedResourcesOfType(new XdaqExecutive()).get(0);

		if (r != null) {
			String user = ((XdaqExecutiveResource)r.getResource()).getUnixUser();

			// clear up job controls
			List<QualifiedResource> l = fm.getQualifiedGroup()
					.seekQualifiedResourcesOfType(new JobControl());

			for (QualifiedResource qr: l) {
				logger.debug("killing processes of " + user);
				JobControl jc = (JobControl)qr;
				jc.init();
				jc.killUser(user);
				logger.debug("killed processes of " + user);
			}
		} else {
			logger.error("initialize(): failed to get a QualifiedResource");
		}

		// Initialize the qualified group, whatever it means.
		try {
			fm.getQualifiedGroup().init();
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
	public void configure(Object o) throws UserActionException {
		logger.debug("CSCLeadingActions.configure");

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("Configuring")));

		// set run type parameter
		String runType = "";
		try {
			XDAQParameter xdaqParam = ((XdaqApplication)
					fm.xdaqSupervisor.getApplications().get(0))
					.getXDAQParameter();

			xdaqParam.select("RunType");

			ParameterSet<CommandParameter> commandParam =
					getUserFunctionManager().getLastInput().getParameterSet();
			if (commandParam == null) {
				logger.error(getClass().toString() +
						"Failded to Configure XDAQ, no run type specified.");

				fm.fireEvent(Level1Inputs.ERROR);
			}

			runType = ((StringT)
					commandParam.get(Level1Parameters.RUN_TYPE).getValue())
					.getString();
			logger.debug(getClass().toString() + "Run type: " + runType);
			xdaqParam.setValue("RunType", runType);
			xdaqParam.send();

		} catch (Exception e) {
			logger.error(getClass().toString() +
					"Failed to set run type: " + runType, e);

			fm.fireEvent(Level1Inputs.ERROR);
		}

		// send Configure
		try {
			fm.xdaqSupervisor.execute(new Input("Configure"));

		} catch (Exception e) {
			logger.error(getClass().toString() +
					"Failed to Configure XDAQ.", e);

			fm.fireEvent(Level1Inputs.ERROR);
		}

		fm.fireEvent(createStateNotification());

		logger.debug("CSCLeadingActions.configure ... done.");
	}

	/*
	 *
	 */
	public void start(Object o) throws UserActionException {
		logger.debug("CSCLeadingActions.start");

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("Starting")));

		// set run number parameter
		try {
			XDAQParameter xdaqParam = ((XdaqApplication)
					fm.xdaqSupervisor.getApplications().get(0))
					.getXDAQParameter();

			xdaqParam.select("RunNumber");

			ParameterSet<CommandParameter> commandParam =
					getUserFunctionManager().getLastInput().getParameterSet();
			if (commandParam == null) {
				logger.error(getClass().toString() +
						"Failded to Enable XDAQ, no run # specified.");

				fm.fireEvent(Level1Inputs.ERROR);
			}

			String runNumber = ((IntegerT)
					commandParam.get(Level1Parameters.RUN_NUMBER).getValue())
					.getInteger().toString();
			logger.debug(getClass().toString() + "Run #: " + runNumber);

			xdaqParam.setValue("RunNumber", runNumber);
			xdaqParam.send();

		} catch (Exception e) {
			logger.error(getClass().toString() + "Failed to Enable XDAQ.", e);

			fm.fireEvent(Level1Inputs.ERROR);
		}

		// send Enable
		try {
			fm.xdaqSupervisor.execute(new Input("Enable"));

		} catch (Exception e) {
			logger.error(getClass().toString() + "Failed to Enable XDAQ.", e);

			fm.fireEvent(Level1Inputs.ERROR);
		}

		fm.fireEvent(createStateNotification());

		logger.debug("CSCLeadingActions.start ... done.");
	}

	/*
	 *
	 */
	public void stop(Object o) throws UserActionException {
		logger.debug("CSCLeadingActions.stop");

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("Stopping")));

		// send Disable
		try {
			fm.xdaqSupervisor.execute(new Input("Disable"));

		} catch (Exception e) {
			logger.error(getClass().toString() + "Failed to Disable XDAQ.", e);

			fm.fireEvent(Level1Inputs.ERROR);
		}

		fm.fireEvent(createStateNotification());

		logger.debug("CSCLeadingActions.stop ... done.");
	}

	/*
	 *
	 */
	public void ttsPrepare(Object o) throws UserActionException {
		logger.debug("CSCLeadingActions.ttsPrepare");

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("TTS preparing")));

		// set run type parameter
		try {
			XDAQParameter xdaqParam = ((XdaqApplication)
					fm.xdaqSupervisor.getApplications().get(0))
					.getXDAQParameter();

			xdaqParam.select("RunType");
			xdaqParam.setValue("RunType", "sTTS_Test");
			xdaqParam.send();

		} catch (Exception e) {
			logger.error(getClass().toString() +
					"Failed to set run type: " + "sTTS_Test", e);

			fm.fireEvent(Level1Inputs.ERROR);
		}

		try {
			fm.xdaqSupervisor.execute(new Input("Configure"));

			fm.xdaqSupervisor.execute(new Input("Enable"));
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

		int fedID = ((IntegerT)parameterSet.get(
				Level1Parameters.TTS_TEST_FED_ID).getValue())
				.getInteger();
		crate = getCrateNumber(fedID);
		slot = getSlotNumber(fedID);
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

		ttsSetter.config(crate, slot, bits, repeat);
		if (ttsSetterFuture == null) {
			ttsSetterFuture = scheduler.scheduleWithFixedDelay(
					ttsSetter, 0, 10, MILLISECONDS);
		}
	}

	/*
	 *
	 */
	public void halt(Object o) throws UserActionException {
		logger.debug("CSCLeadingActions.halt");

		fm.getParameterSet().put(new FunctionManagerParameter<StringT>(
				Level1Parameters.ACTION_MSG, new StringT("Halting")));

		// send Halt
		try {
			fm.xdaqSupervisor.execute(new Input("Halt"));

		} catch (Exception e) {
			logger.error(getClass().toString() + "Failed to Halt XDAQ.", e);

			fm.fireEvent(Level1Inputs.ERROR);
		}

		fm.fireEvent(createStateNotification());

		logger.debug("CSCLeadingActions.halt ... done.");
	}

	/* */
	private int getCrateNumber(int fed) {
		int crate = 1;

		if (fed == 760) {
			crate = 3; // TF-FED
		} else if (fed == 752 || (fed / 10) == 83) {
			crate = 2;
		}

		return crate;
	}

	/* */
	private int getSlotNumber(int fed) {
		int slot = 0;

		switch (fed) {
			case 760: slot = 2; break; // TF

			case 750: slot = 8; break; // plus 1
			case 841: slot = 4; break;
			case 842: slot = 5; break;
			case 843: slot = 6; break;
			case 844: slot = 7; break;
			case 845: slot = 9; break;
			case 846: slot = 10; break;
			case 847: slot = 11; break;
			case 848: slot = 12; break;
			case 849: slot = 13; break;

			case 752: slot = 8; break; // plus 2
			case 831: slot = 4; break;
			case 832: slot = 5; break;
			case 833: slot = 6; break;
			case 834: slot = 7; break;
			case 835: slot = 9; break;
			case 836: slot = 10; break;
			case 837: slot = 11; break;
			case 838: slot = 12; break;
			case 839: slot = 13; break;
		}

		return slot;
	}
}

// End of file
// vim: set sw=4 ts=4:
