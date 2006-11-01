package rcms.fm.app.csc;

import rcms.fm.fw.user.UserStateNotificationHandler;

import java.util.Properties;
import java.io.*;

import rcms.fm.app.level1template.MyStates;
import rcms.fm.app.level1template.MyInputs;
import rcms.fm.app.level1template.MyParameters;

import rcms.fm.parameter.ParameterSet;
import rcms.xdaqctl.XDAQParameter;
import rcms.fm.resource.qualifiedresource.XdaqApplication;

import rcms.statemachine.definition.Input;
import rcms.fm.fw.StateEnteredEvent;
import rcms.fm.fw.user.UserActionException;
import rcms.fm.resource.QualifiedResourceContainerException;

import rcms.util.logger.RCMSLogger;
import rcms.utilities.runinfo.*;

public class CSCEventHandler extends UserStateNotificationHandler {
	
	private RCMSLogger logger = new RCMSLogger(CSCEventHandler.class);

	private CSCFunctionManager fm;
	private ParameterSet fmParameters;

	private final static String DEFAULT_RUN_TYPE = "ME1b";
	private final static String DEFAULT_RUN_NUMBER = "10";

	private RunInfoConnector runInfoConnector;

	class CSCConfig {
		public String daqMode, tfConfig, ttcSource;
		private XDAQParameter svParameter = null;

		public void update() throws Exception {
			if (svParameter == null) {
				svParameter = ((XdaqApplication)
						fm.xdaqSupervisor.getApplications().get(0))
						.getXDAQParameter();
				svParameter.select(
						new String[] {"DAQMode", "TriggerConfig", "TTCSource"});
			}

			svParameter.get();

			daqMode = svParameter.getValue("DAQMode");
			tfConfig = svParameter.getValue("TriggerConfig");
			ttcSource = svParameter.getValue("TTCSource");
		}
	}
	CSCConfig cscConfig = new CSCConfig();

	public CSCEventHandler() throws rcms.fm.fw.EventHandlerException {
		subscribeForEvents(StateEnteredEvent.class);
		
		addAction(MyStates.CONFIGURING, "configureAction");
		addAction(MyStates.STARTING, "startAction");		
		addAction(MyStates.PAUSING, "pauseAction");
		addAction(MyStates.RESUMING, "resumeAction");
		addAction(MyStates.HALTING, "haltAction");

		addAction(MyStates.RECOVERING, "recoverAction");
		addAction(MyStates.RESETTING, "resetAction");
		addAction(MyStates.INITIALIZING, "initAction");
	}

	public void init() throws rcms.fm.fw.EventHandlerException {
		fm = (CSCFunctionManager)getUserFunctionManager();

		String dbURL = null;

		Properties p = new Properties();
		try {
			p.load(new FileInputStream(
					System.getProperties().getProperty("catalina.home") +
					"/webapps/ROOT/functionmanagers/cscfm.properties"));

			dbURL = p.getProperty("cscfm.runinfodb.url");
		} catch (Exception ignored) {}

		if (dbURL != null) {
			try {
				runInfoConnector = new RunInfoConnector(dbURL,
						p.getProperty("cscfm.runinfodb.user"),
						p.getProperty("cscfm.runinfodb.passwd"));
			} catch (Exception e) {
				System.err.println(
						"==== Failed to connect to the run info DB: " + dbURL);
				e.printStackTrace();
			}
		} else {
			runInfoConnector = fm.getRunInfoConnector();
		}
	}

	public void initAction(Object o) throws UserActionException {
		genericAction(o, "Initializing", MyInputs.SETHALT);

		fmParameters = fm.getParameterSet();
	}

	public void configureAction(Object o) throws UserActionException {
		if (o instanceof StateEnteredEvent) {
			fm.getParameterSet().put(MyParameters.ACTION_MSG, "Configuring");

			if (!fm.xdaqSupervisor.isEmpty()) {
				XDAQParameter param = null;
				try {
					param = ((XdaqApplication)
							fm.xdaqSupervisor.getApplications().get(0))
							.getXDAQParameter();

					param.select(new String[] { "RunType", "RunNumber" });
					if (fmParameters.get("RUN_TYPE").equals("")) {
						fmParameters.put("RUN_TYPE", DEFAULT_RUN_TYPE);
					}
					param.setValue("RunType", fmParameters.get("RUN_TYPE"));
					if (fmParameters.get("RUN_NUMBER").equals("")) {
						fmParameters.put("RUN_NUMBER", DEFAULT_RUN_NUMBER);
					}
					param.setValue("RunNumber", fmParameters.get("RUN_NUMBER"));
					param.send();
				} catch (Exception ignored) {}

				Input input = new Input("Configure");

				try {
					fm.xdaqSupervisor.execute(input);
					logger.info("Configure executed.");
				} catch (QualifiedResourceContainerException e) {
					logger.error("Configure FAILED.", e);
					throw new UserActionException("Configure FAILED.", e);
				}
			}

			fm.fireEvent(MyInputs.SETCONFIGURE);
		}
	}

	public void startAction(Object o) throws UserActionException {
		if (o instanceof StateEnteredEvent) {
			fm.getParameterSet().put(MyParameters.ACTION_MSG, "Starting");

			// Enable the supervisor
			if (!fm.xdaqSupervisor.isEmpty()) {
				try {
					fm.xdaqSupervisor.execute(new Input("Enable"));
					logger.info("Enable" + " executed.");
				} catch (QualifiedResourceContainerException e) {
					logger.error("Enable" + " FAILED.", e);
					throw new UserActionException("Enable" + " FAILED.", e);
				}
			}

			// Store CSC run information to the DB
			try {
				cscConfig.update();
				RunInfo i = new RunInfo(runInfoConnector,
						new Integer(fmParameters.get("RUN_NUMBER")));
				i.setNameSpace("CMS.CSC");

				i.publishRunInfo("DAQ.mode", cscConfig.daqMode);
				i.publishRunInfo("TF.config", cscConfig.tfConfig);
				i.publishRunInfo("TTC.source", cscConfig.ttcSource);
				i.publishRunInfo("run_type", "Monitor");
			} catch (Exception e) {
				logger.error("Enable" + " FAILED.", e);
				throw new UserActionException("Enable" + " FAILED.", e);
			}

			fm.fireEvent(MyInputs.SETSTART);
		}
	}

	public void pauseAction(Object o) throws UserActionException {
		genericAction(o, "Pausing", MyInputs.SETPAUSE);
	}
	
	public void resumeAction(Object o) throws UserActionException {
		genericAction(o, "Resuming", MyInputs.SETRESUME);
	}
	
	public void haltAction(Object o) throws UserActionException {
		xdaqCommandAction(o, "Halting", MyInputs.SETHALT, "Halt");
	}
	
	public void resetAction(Object o) throws UserActionException {
		xdaqCommandAction(o, "Resetting", MyInputs.SETRESET, "Reset");
	}
	
	public void recoverAction(Object o) throws UserActionException {
		genericAction(o, "Recovering", MyInputs.SETINITIAL);
	}

	private void genericAction(Object o, String message, Input input) {
		if (o instanceof StateEnteredEvent) {
			fm.getParameterSet().put(MyParameters.ACTION_MSG, message);
			fm.fireEvent(input);
		}
	}

	private void xdaqCommandAction(
			Object o, String message, Input input, String command)
			throws UserActionException {
		if (o instanceof StateEnteredEvent) {
			fm.getParameterSet().put(MyParameters.ACTION_MSG, message);

			if (!fm.xdaqSupervisor.isEmpty()) {
				try {
					fm.xdaqSupervisor.execute(new Input(command));
					logger.info(command + " executed.");
				} catch (QualifiedResourceContainerException e) {
					logger.error(command + " FAILED.", e);
					throw new UserActionException(command + " FAILED.", e);
				}
			}

			fm.fireEvent(input);
		}
	}
}

// End of file
// vim: sw=4 ts=4:
