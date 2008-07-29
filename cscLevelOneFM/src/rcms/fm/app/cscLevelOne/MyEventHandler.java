package rcms.fm.app.cscLevelOne;

import static java.util.concurrent.TimeUnit.MILLISECONDS;

import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;

import rcms.errorFormat.CMS.CMSError;
import rcms.fm.fw.EventHandlerException;
import rcms.fm.fw.StateEnteredEvent;
import rcms.fm.fw.parameter.CommandParameter;
import rcms.fm.fw.parameter.FunctionManagerParameter;
import rcms.fm.fw.parameter.ParameterException;
import rcms.fm.fw.parameter.ParameterSet;
import rcms.fm.fw.parameter.type.IntegerT;
import rcms.fm.fw.parameter.type.StringT;
import rcms.fm.fw.user.UserActionException;
import rcms.fm.fw.user.UserStateNotificationHandler;
import rcms.fm.resource.QualifiedGroup;
import rcms.fm.resource.QualifiedResource;
import rcms.fm.resource.QualifiedResourceContainerException;
import rcms.fm.resource.qualifiedresource.JobControl;
import rcms.fm.resource.qualifiedresource.XdaqApplication;
import rcms.fm.resource.qualifiedresource.XdaqApplicationContainer;
import rcms.fm.resource.qualifiedresource.XdaqExecutive;
import rcms.stateFormat.StateNotification;
import rcms.statemachine.definition.Input;
import rcms.util.logger.RCMSLogger;
import rcms.xdaqctl.XDAQParameter;

// needed to store RunInfo
import rcms.utilities.runinfo.RunInfo;
import rcms.utilities.runinfo.RunInfoConnectorIF;
import rcms.utilities.runinfo.RunInfoException;
import rcms.utilities.runinfo.RunNumberData;
import rcms.utilities.runinfo.RunSequenceNumber;
import rcms.fm.fw.parameter.Parameter;

import rcms.fm.app.cscLevelOne.util.MyUtil;
/**
 * 
 * Main Event Handler class for Level 1 Function Manager.
 * 
 * @author Andrea Petrucci, Alexander Oh, Michele Gulmini
 *
 */
public class MyEventHandler extends UserStateNotificationHandler {

	/**
	 * <code>RCMSLogger</code>: RCMS log4j logger.
	 */
	static RCMSLogger logger = new RCMSLogger(MyEventHandler.class);

	MyFunctionManager functionManager = null;

	private QualifiedGroup qualifiedGroup = null;
        private RunNumberData _myRunNumberData = null;

	// parameters for tts test
	private XDAQParameter svTTSParameter = null;
	private TTSSetter ttsSetter = null;
	private ScheduledFuture ttsSetterFuture = null;
	private final ScheduledExecutorService scheduler;

  protected RunSequenceNumber runNumberGenerator = null;
  protected RunNumberData     runNumberData = null;
  protected Integer           runNumber = null;
  protected boolean           localRunNumber = false;
  protected RunInfo           runInfo = null;


	/**
	 * Handle to utility class
	 */
	private MyUtil _myUtil;
	
	public MyEventHandler() throws rcms.fm.fw.EventHandlerException {
		// this handler inherits UserStateNotificationHandler
		// so it is already registered for StateNotification events

		// Let's register also the StateEnteredEvent triggered when the FSM enters in a new state.
		subscribeForEvents(StateEnteredEvent.class);

		addAction(MyStates.INITIALIZING,			"initAction");		
		addAction(MyStates.CONFIGURING, 			"configureAction");
		addAction(MyStates.HALTING,     			"haltAction");
		addAction(MyStates.PREPARING_TTSTEST_MODE,	"preparingTTSTestModeAction");		
		addAction(MyStates.TESTING_TTS,    			"testingTTSAction");		
		addAction(MyStates.PAUSING,     			"pauseAction");
		addAction(MyStates.RECOVERING,  			"recoverAction");
		addAction(MyStates.RESETTING,   			"resetAction");
		addAction(MyStates.RESUMING,    			"resumeAction");
		addAction(MyStates.STARTING,    			"startAction");
		addAction(MyStates.STOPPING,    			"stopAction");

		scheduler = Executors.newScheduledThreadPool(1);
	}


	public void init() throws rcms.fm.fw.EventHandlerException {
		functionManager = (MyFunctionManager) getUserFunctionManager();
		qualifiedGroup  = functionManager.getQualifiedGroup();
				//Unique id for each configuration 

		int session_id = functionManager.getQualifiedGroup().getGroup().getDirectory().getId();
		functionManager.getParameterSet().put(new FunctionManagerParameter<IntegerT>(MyParameters.SID, new IntegerT(session_id)));

		 // instantiate utility
		_myUtil = new MyUtil(functionManager);

		ttsSetter = new TTSSetter();
		ttsSetterFuture = null;
		
		// debug
		logger.debug("init() called: functionManager=" + functionManager );
		// call renderers
		//_myUtil.renderMainGui();
	}

	public void initAction(Object obj) throws UserActionException {

		if (obj instanceof StateNotification) {

			// triggered by State Notification from child resource

			/************************************************
			 * PUT HERE YOUR CODE							
			 ***********************************************/

			return;
		}

		else if (obj instanceof StateEnteredEvent) {

			// triggered by entered state action
			// let's command the child resources
		    //_myUtil.setParameter(action ,"Initializing" );

			// debug
			logger.debug("initAction called.");

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("Initializing")));

			// update state
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.STATE,new StringT(
							  functionManager.getState().getStateString())));

			// initialize qualified group
			try {
				qualifiedGroup.init();
			} catch (Exception e) {
				// failed to init
				String errMsg = this.getClass().toString() + " failed to initialize resources";

				// send error notification
				sendCMSError(errMsg);

				//log error
				logger.error(errMsg,e);

				// go to error state
				functionManager.fireEvent(MyInputs.SETERROR);
			}

			// find xdaq applications
			List<QualifiedResource> xdaqList = qualifiedGroup.seekQualifiedResourcesOfType(new XdaqApplication());
			functionManager.containerXdaqApplication = new XdaqApplicationContainer(xdaqList);
			logger.debug("Application list : " + xdaqList.size() );

			// set paraterts from properties
			_myUtil.setParameterFromProperties();
			
			// render gui
			_myUtil.renderMainGui();
			
			functionManager.xdaqSupervisor = new XdaqApplicationContainer(
					functionManager.containerXdaqApplication.getApplicationsOfClass("CSCSupervisor"));

			// go to HALT
			functionManager.fireEvent( MyInputs.SETHALT );


			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("")));

			logger.info("initAction Executed");
		}
	}


	public void resetAction(Object obj) throws UserActionException {

		if (obj instanceof StateNotification) {

			// triggered by State Notification from child resource

			/************************************************
			 * PUT HERE YOUR CODE							
			 ***********************************************/

			return;
		}

		else if (obj instanceof StateEnteredEvent) {


			// triggered by entered state action
			// let's command the child resources

			// debug
			logger.debug("resetAction called.");

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("Resetting")));
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.STATE, new StringT(
                                                          functionManager.getState().getStateString())));

			// destroy xdaq
			destroyXDAQ();

			// reset qualified group
			QualifiedGroup qualifiedGroup = functionManager.getQualifiedGroup();
			qualifiedGroup.reset();

			// reinitialize qualified group
			try {
				qualifiedGroup.init();
			}
			catch (Exception e) {
				String errorMess = e.getMessage();
				logger.error(errorMess);
				throw new UserActionException(errorMess);
			}

			// go to Initital
			functionManager.fireEvent( MyInputs.SETHALT );

			// Clean-up of the Function Manager parameters
			cleanUpFMParameters();

			logger.info("resetAction Executed");
		}	
	}

	public void recoverAction(Object obj) throws UserActionException {

		if (obj instanceof StateNotification) {

			// triggered by State Notification from child resource

			/************************************************
			 * PUT HERE YOUR CODE							
			 ***********************************************/

			return;
		}

		else if (obj instanceof StateEnteredEvent) {

			System.out.println("Executing recoverAction");
			logger.info("Executing recoverAction");

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("recovering")));
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(
					MyParameters.STATE,new StringT(functionManager.getState().getStateString())));

			/************************************************
			 * PUT HERE YOUR CODE							
			 ***********************************************/

			// leave intermediate state
			functionManager.fireEvent( MyInputs.SETINITIAL );

			// Clean-up of the Function Manager parameters
			cleanUpFMParameters();

			logger.info("recoverAction Executed");
		}
	}

	public void configureAction(Object obj) throws UserActionException {

		if (obj instanceof StateNotification) {

			// triggered by State Notification from child resource

			// leave intermediate state
			// check that the csc supervisor is in configured state
			for ( XdaqApplication xdaqApp : functionManager.xdaqSupervisor.getApplications()) {
				if (xdaqApp.getCacheState().equals(MyStates.ERROR)) {
					functionManager.fireEvent(MyInputs.SETERROR);
				}
				else if (!xdaqApp.getCacheState().equals(MyStates.CONFIGURED)) return;
			}
			functionManager.fireEvent( MyInputs.SETCONFIGURE );

			return;
		}

		else if (obj instanceof StateEnteredEvent) {
			System.out.println("Executing configureAction");
			logger.info("Executing configureAction");

			// check that we have a csc supervisor to control
			if (functionManager.xdaqSupervisor.getApplications().size() == 0) {
				// nothing to control, go to configured immediately
				functionManager.fireEvent( MyInputs.SETCONFIGURE );
				return;
			}

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("configuring")));
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.STATE, new StringT("Configured")));

			//get the parameters of the command
			ParameterSet<CommandParameter> parameterSet = getUserFunctionManager().getLastInput().getParameterSet();

			// check parameter set
			if (parameterSet.size()==0 || parameterSet.get(MyParameters.RUN_TYPE) == null ||
					((StringT)parameterSet.get(MyParameters.RUN_TYPE).getValue()).equals("") )  {

				// Set default
				try {
					parameterSet.add( new CommandParameter<StringT>(MyParameters.RUN_TYPE, new StringT("Default")));
				} catch (ParameterException e) {
					logger.error("Could not default the run type to Default",e);
					functionManager.fireEvent(MyInputs.SETERROR);
				}
			}

			// get the run number from the configure command
			String runType = ((StringT)parameterSet.get(MyParameters.RUN_TYPE).getValue()).getString();

			// Set the runType in the Function Manager parameters
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.RUN_TYPE,new StringT(runType)));


			// set run type parameter			
			try {
				XDAQParameter xdaqParam = ((XdaqApplication)
						functionManager.xdaqSupervisor.getApplications().get(0))
						.getXDAQParameter();

				// select RunType of csc supervisor.
				// this parameter is used to differentiate between 
				// 1) calibration run
				// 2) global run
				// since level one is only used in global runs we hardwire
				// for the time being to global run = "Default".
				// the parameter is set to the supervisor xdaq application.
				xdaqParam.select("RunType");
				xdaqParam.setValue("RunType", "Default");
				xdaqParam.send();

			} catch (Exception e) {
				logger.error(getClass().toString() +
						"Failed to set run type Default to csc supervisor xdaq application. (Value is hardwired in the code) ", e);

				functionManager.fireEvent(MyInputs.SETERROR);
			}

			// send Configure
			try {
				functionManager.xdaqSupervisor.execute(MyInputs.CONFIGURE);

			} catch (Exception e) {
				logger.error(getClass().toString() +
						"Failed to Configure csc supervisor xdaq application.", e);

				functionManager.fireEvent(MyInputs.SETERROR);
			}

		// set action
		functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("")));
		logger.info("configureAction Executed");
		}
	}

public void startAction(Object obj) throws UserActionException {

		if (obj instanceof StateNotification) {

			// triggered by State Notification from child resource

			// leave intermediate state
			// check that the csc supervisor is in configured state
			for ( XdaqApplication xdaqApp : functionManager.xdaqSupervisor.getApplications()) {
				if (xdaqApp.getCacheState().equals(MyStates.ERROR)) {
					functionManager.fireEvent(MyInputs.SETERROR);
				}
				else if (xdaqApp.getCacheState().equals(MyStates.ERROR)) {
					functionManager.fireEvent(MyInputs.SETERROR);
				}
				else if (!xdaqApp.getCacheState().equals(MyStates.RUNNING)) return;
			}
			functionManager.fireEvent( MyInputs.SETSTART );

			return;
		}

		else if (obj instanceof StateEnteredEvent) {
			System.out.println("Executing startAction");
			logger.info("Executing startAction");

			// check that we have a csc supervisor to control
			if (functionManager.xdaqSupervisor.getApplications().size() == 0) {
				// nothing to control, go to configured immediately
				functionManager.fireEvent( MyInputs.SETSTART );
				return;
			}

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("starting")));
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.STATE, new StringT("Running")));

			// get the parameters of the command
			ParameterSet<CommandParameter> parameterSet = getUserFunctionManager().getLastInput().getParameterSet();
      runNumber = null;
      localRunNumber = false;


		// check parameter set
		if (parameterSet.size()==0 || parameterSet.get(MyParameters.RUN_NUMBER) == null )  {
		    // go to error, we require parameters
		    String errMsg = "startAction: no parameters given with start command";
		
		    // log remark
		    logger.info(errMsg);
	
		    RunInfoConnectorIF ric = functionManager.getRunInfoConnector();
		    if ( ric != null )
			{
			    if ( runNumberGenerator == null )
				{
				    logger.info("Creating RunNumberGenerator (RunSequenceNumber) ");
				    runNumberGenerator = new RunSequenceNumber(ric,functionManager.getOwner(),functionManager.getParameterSet().
									       get(MyParameters.SEQ_NAME).getValue().toString());
				}

			    if ( runNumberGenerator != null )
				{
				    runNumberData = runNumberGenerator.createRunSequenceNumber(new Integer(functionManager.getParameterSet().get(MyParameters.SID).
													   getValue().toString()));
				}

			    if ( runNumberData != null )
				{
				    runNumber =  runNumberData.getRunNumber();
				    System.out.println("Generated RunNumber\n"+runNumberData.toString());
				    localRunNumber = true;
				}
			    else
				{
				    logger.error("Error generating RunNumber");
				}
			}
		    else
			{
			    logger.error("Failed to create RunNumberGenerator");
			}
		}
		else
		    {
			// get the run number from the start command
			runNumber =((IntegerT)parameterSet.get(MyParameters.RUN_NUMBER).getValue()).getInteger();
		    }
      
		if ( runNumber != null )
		    {
			// Set the run number in the Function Manager parameters
			functionManager.getParameterSet().put(new FunctionManagerParameter<IntegerT>(MyParameters.RUN_NUMBER,new IntegerT(runNumber)));
		    }
		else
		    {
			logger.error("Cannot find and/or generator RunNumber");
		    }
    
		publishRunInfo(true);


		    /*    
		    // default to -1
		    try {
			parameterSet.add( new CommandParameter<IntegerT>(MyParameters.RUN_NUMBER, new IntegerT(-1)));
				} catch (ParameterException e) {
				    logger.error("Could not default the run number to -1",e);
				    functionManager.fireEvent(MyInputs.SETERROR);
				}
		    
		    // log this
		    logger.warn("No run number given, defaulting to -1.");
		    */
		    
		
		
		// get the run number from the start command
		//Integer runNumber = ((IntegerT)parameterSet.get(MyParameters.RUN_NUMBER).getValue()).getInteger();
		
		// Set the run number in the Function Manager parameters
		//functionManager.getParameterSet().put(new FunctionManagerParameter<IntegerT>(MyParameters.RUN_NUMBER,new IntegerT(-1)));
		
		//			set run number parameter
		try {
		    XDAQParameter xdaqParam = ((XdaqApplication)
					       functionManager.xdaqSupervisor.getApplications().get(0)).getXDAQParameter();
		    
		    xdaqParam.select("RunNumber");
		    ParameterSet<CommandParameter> commandParam = getUserFunctionManager().getLastInput().getParameterSet();
		    if (commandParam == null) {
			logger.error(getClass().toString() +"Failded to Enable XDAQ, no run # specified.");
			functionManager.fireEvent(MyInputs.SETERROR);
		    }
		    logger.debug(getClass().toString() + "Run #: " + runNumber);
		    
		    xdaqParam.setValue("RunNumber", runNumber.toString());
		    xdaqParam.send();
		    
		} catch (Exception e) {
		    logger.error(getClass().toString() + "Failed to Enable csc supervisor XDAQ application.", e);
		    
		    functionManager.fireEvent(MyInputs.SETERROR);
		}
		
		// send Enable
			try {
			    functionManager.xdaqSupervisor.execute(new Input("Start"));
			    
			} catch (Exception e) {
			    logger.error(getClass().toString() + "Failed to Enable csc supervisor XDAQ application.", e);
			    
			    functionManager.fireEvent(MyInputs.SETERROR);
			}
			
			logger.debug("CSCLeadingActions.start ... done.");
			
			
			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("")));
			
			logger.debug("startAction Executed");
			
		}
}
    
    public void pauseAction(Object obj) throws UserActionException {

		if (obj instanceof StateNotification) {

			// triggered by State Notification from child resource

			/************************************************
			 * PUT HERE YOUR CODE							
			 ***********************************************/

			return;
		}

		else if (obj instanceof StateEnteredEvent) {	
			System.out.println("Executing pauseAction");
			logger.info("Executing pauseAction");

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("pausing")));

			/************************************************
			 * PUT HERE YOUR CODE							
			 ***********************************************/

			// leave intermediate state
			functionManager.fireEvent( MyInputs.SETPAUSE );

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("")));
			
			logger.debug("pausingAction Executed");
			
		}
	}

	public void stopAction(Object obj) throws UserActionException {

		if (obj instanceof StateNotification) {

			// triggered by State Notification from child resource

			// leave intermediate state
			// check that the csc supervisor is in configured state
			for ( XdaqApplication xdaqApp : functionManager.xdaqSupervisor.getApplications()) {
				if (xdaqApp.getCacheState().equals(MyStates.ERROR)) {
					functionManager.fireEvent(MyInputs.SETERROR);
				}
				else if (!xdaqApp.getCacheState().equals(MyStates.CONFIGURED)) return;
			}
			functionManager.fireEvent( MyInputs.SETCONFIGURE );

			return;
		}

		else if (obj instanceof StateEnteredEvent) {	
			System.out.println("Executing stopAction");
			logger.info("Executing stopAction");

			// check that we have a csc supervisor to control
			if (functionManager.xdaqSupervisor.getApplications().size() == 0) {
				// nothing to control, go to configured immediately
				functionManager.fireEvent( MyInputs.SETCONFIGURE );
				return;
			}

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("stopping")));
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.STATE, new StringT("Halted")));
			
			//Run Info
			publishRunInfo(false);

			// send Stop
			try {
				functionManager.xdaqSupervisor.execute(new Input("Stop"));

			} catch (Exception e) {
				logger.error(getClass().toString() + "Failed to Stop XDAQ.", e);

				functionManager.fireEvent(MyInputs.SETERROR);
			}


			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("")));
			
			logger.debug("stopAction Executed");
			
		}
	}
	public void resumeAction(Object obj) throws UserActionException {

		if (obj instanceof StateNotification) {

			// triggered by State Notification from child resource

			/************************************************
			 * PUT HERE YOUR CODE							
			 ***********************************************/

			return;
		}

		else if (obj instanceof StateEnteredEvent) {	
			System.out.println("Executing resumeAction");
			logger.info("Executing resumeAction");

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("resuming")));

			/************************************************
			 * PUT HERE YOUR CODE							
			 ***********************************************/

			// leave intermediate state
			functionManager.fireEvent( MyInputs.SETRESUME );

			// Clean-up of the Function Manager parameters
			cleanUpFMParameters();

			logger.debug("resumeAction Executed");

		}
	}

	public void haltAction(Object obj) throws UserActionException {

		if (obj instanceof StateNotification) {

//			triggered by State Notification from child resource

			// leave intermediate state
			// check that the csc supervisor is in configured state
			for ( XdaqApplication xdaqApp : functionManager.xdaqSupervisor.getApplications()) {
				if (xdaqApp.getCacheState().equals(MyStates.ERROR)) {
					functionManager.fireEvent(MyInputs.SETERROR);
				}
				else if (!xdaqApp.getCacheState().equals(MyStates.HALTED)) return;
			}
			functionManager.fireEvent( MyInputs.SETHALT );

			return;
		}

		else if (obj instanceof StateEnteredEvent) {
			System.out.println("Executing haltAction");
			logger.info("Executing haltAction");

			// check that we have a csc supervisor to control
			if (functionManager.xdaqSupervisor.getApplications().size() == 0) {
				// nothing to control, go to configured immediately
				functionManager.fireEvent( MyInputs.SETHALT );
				return;
			}

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("halting")));

			// send Halt
			try {
				functionManager.xdaqSupervisor.execute(new Input("Halt"));

			} catch (Exception e) {
				logger.error(getClass().toString() + "Failed to Halt XDAQ.", e);

				functionManager.fireEvent(MyInputs.SETERROR);
			}


			// check from which state we came.
			if (functionManager.getPreviousState().equals(MyStates.TTSTEST_MODE)) {
				// when we came from TTSTestMode we need to
				// 1. give back control of sTTS to HW
			}


			// Clean-up of the Function Manager parameters
			cleanUpFMParameters();

			logger.debug("haltAction Executed");
		}
	}	

	public void preparingTTSTestModeAction(Object obj) throws UserActionException {

		if (obj instanceof StateNotification) {

			// triggered by State Notification from child resource

			StateNotification sn = (StateNotification)obj;
			
			if (sn.getToState().equals(MyStates.CONFIGURED.getStateString())) {
				// configure is send in state entered part (below)
				// here we receive "Configured" from supervisor.
				// send a enable now.
				try {                                                                                           
					functionManager.xdaqSupervisor.execute(new Input("Enable"));
				} catch (QualifiedResourceContainerException e) {
					logger.error("Could not enable csc supervisor.",e);
					functionManager.fireEvent(MyInputs.SETERROR);
				}
			}
			else if ( sn.getToState().equals(MyStates.RUNNING.getStateString()) ) {
				// recveived runnning state. now we can
				// leave intermediate state
				functionManager.fireEvent( MyInputs.SETTTSTEST_MODE );
			}
			else {
				// don't understand state, go to error.
				logger.error("Unexpected state notification in state " 
						+ functionManager.getState().toString() 
						+ ", received state :" 
						+ sn.getToState());
				functionManager.fireEvent(MyInputs.SETERROR);
			}
			return;
		}

		else if (obj instanceof StateEnteredEvent) {
			System.out.println("Executing preparingTestModeAction");
			logger.info("Executing preparingTestModeAction");

			// check that we have a csc supervisor to control
			if (functionManager.xdaqSupervisor.getApplications().size() == 0) {
				// nothing to control, go to configured immediately
				functionManager.fireEvent( MyInputs.SETTTSTEST_MODE );
				return;
			}

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("preparingTestMode")));

			// set run type parameter
			try {
				XDAQParameter xdaqParam = ((XdaqApplication)
						functionManager.xdaqSupervisor.getApplications().get(0))
						.getXDAQParameter();

				xdaqParam.select("RunType");
				xdaqParam.setValue("RunType", "sTTS_Test");
				xdaqParam.send();

			} catch (Exception e) {
				logger.error(getClass().toString() +
						"Failed to set run type: " + "sTTS_Test", e);

				functionManager.fireEvent(MyInputs.SETERROR);
			}

			try {
				functionManager.xdaqSupervisor.execute(new Input("Configure"));


			} catch (Exception e) {
				logger.error(getClass().toString() +
						"Failed to TTSPrepare XDAQ.", e);

				functionManager.fireEvent(MyInputs.SETERROR);
			}

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("")));

			logger.debug("preparingTestModeAction Executed");
		}
	}	

	public void testingTTSAction(Object obj) throws UserActionException {

		if (obj instanceof StateNotification) {

			// triggered by State Notification from child resource

			/************************************************
			 * PUT HERE YOUR CODE							
			 ***********************************************/

			return;
		}

		else if (obj instanceof StateEnteredEvent) {
			System.out.println("Executing testingTTSAction");
			logger.info("Executing testingTTSAction");

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("testing TTS")));

			// get the parameters of the command
			ParameterSet<CommandParameter> parameterSet = getUserFunctionManager().getLastInput().getParameterSet();

			// check parameter set
			if (parameterSet.size()==0 || parameterSet.get(MyParameters.TTS_TEST_FED_ID) == null ||
					parameterSet.get(MyParameters.TTS_TEST_MODE) == null ||
					((StringT)parameterSet.get(MyParameters.TTS_TEST_MODE).getValue()).equals("") || 
					parameterSet.get(MyParameters.TTS_TEST_PATTERN) == null ||
					((StringT)parameterSet.get(MyParameters.TTS_TEST_PATTERN).getValue()).equals("") ||
					parameterSet.get(MyParameters.TTS_TEST_SEQUENCE_REPEAT) == null)
			{

				// go to error, we require parameters
				String errMsg = "testingTTSAction: no parameters given with TestTTS command.";

				// log error
				logger.error(errMsg);

				// notify error
				sendCMSError(errMsg);

				//go to error state
				functionManager.fireEvent( MyInputs.SETERROR );

			}

			Integer fedId = ((IntegerT)parameterSet.get(MyParameters.TTS_TEST_FED_ID).getValue()).getInteger();
			String mode = ((StringT)parameterSet.get(MyParameters.TTS_TEST_MODE).getValue()).getString();
			String pattern = ((StringT)parameterSet.get(MyParameters.TTS_TEST_PATTERN).getValue()).getString();
			Integer cycles = ((IntegerT)parameterSet.get(MyParameters.TTS_TEST_SEQUENCE_REPEAT).getValue()).getInteger();


			// Set last parameters in the Function Manager parameters
			functionManager.getParameterSet().put(new FunctionManagerParameter<IntegerT>(MyParameters.TTS_TEST_FED_ID,new IntegerT(fedId)));
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.TTS_TEST_MODE,new StringT(mode)));
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.TTS_TEST_PATTERN,new StringT(pattern)));
			functionManager.getParameterSet().put(new FunctionManagerParameter<IntegerT>(MyParameters.TTS_TEST_SEQUENCE_REPEAT,new IntegerT(cycles)));

			// debug
			logger.debug("Using parameters: fedId=" + fedId + "mode=" + mode + " pattern=" + pattern + " cycles=" + cycles );

			// find out which application controls the fedId.

			int crate, slot, bits, repeat;
		
			int fedID = ((IntegerT)parameterSet.get(
					MyParameters.TTS_TEST_FED_ID).getValue())
					.getInteger();
			crate = getCrateNumber(fedID);
			slot = getSlotNumber(fedID);
			bits = Integer.parseInt(
					((StringT)parameterSet.get(
							MyParameters.TTS_TEST_PATTERN)
					.getValue()).getString());
			if (((StringT)parameterSet.get(MyParameters.TTS_TEST_PATTERN)
					.getValue()).getString().equals("PATTERN")) {
				repeat = 0;
			} else {  // CYCLE
				repeat = ((IntegerT)parameterSet.get(
						MyParameters.TTS_TEST_SEQUENCE_REPEAT)
						.getValue()).getInteger();
			}

			ttsSetter.config(crate, slot, bits, repeat);
			if (ttsSetterFuture == null) {
				ttsSetterFuture = scheduler.scheduleWithFixedDelay(
						ttsSetter, 0, 10, MILLISECONDS);
			}


			// leave intermediate state
			functionManager.fireEvent( MyInputs.SETTTSTEST_MODE );

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("")));

			logger.debug("preparingTestModeAction Executed");
		}
	}	

	@SuppressWarnings("unchecked")
	private void sendCMSError(String errMessage){

		// create a new error notification msg
		CMSError error = functionManager.getErrorFactory().getCMSError();
		error.setDateTime(new Date().toString());
		error.setMessage(errMessage);

		// update error msg parameter for GUI
		functionManager.getParameterSet().get(MyParameters.ERROR_MSG).setValue(new StringT(errMessage));

		// send error
		try {
			functionManager.getParentErrorNotifier().sendError(error);
		} catch (Exception e) {
			logger.warn(functionManager.getClass().toString() + ": Failed to send error mesage " + errMessage);
		}
	}

	private void cleanUpFMParameters() {
		// Clean-up of the Function Manager parameters
		functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("")));
		functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ERROR_MSG,new StringT("")));
		functionManager.getParameterSet().put(new FunctionManagerParameter<IntegerT>(MyParameters.RUN_NUMBER,new IntegerT(-1)));
		functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.RUN_TYPE,new StringT("")));
		functionManager.getParameterSet().put(new FunctionManagerParameter<IntegerT>(MyParameters.TTS_TEST_FED_ID,new IntegerT(-1)));
		functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.TTS_TEST_MODE,new StringT("")));
		functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.TTS_TEST_PATTERN,new StringT("")));
		functionManager.getParameterSet().put(new FunctionManagerParameter<IntegerT>(MyParameters.TTS_TEST_SEQUENCE_REPEAT,new IntegerT(-1)));
	}

public void publishRunInfo(boolean doRun)
  {
    RunInfoConnectorIF ric = functionManager.getRunInfoConnector();

    if ( ric != null )
    {
      if ( runNumberData != null )
      {
	runInfo = new RunInfo(ric,runNumberData);
      }
      else
      {
	runInfo = new RunInfo(ric,runNumber);
      }

      runInfo.setNameSpace(MyParameters.CSC_NS);
      if ( localRunNumber )
      {
	try
	{
	  runInfo.publishRunInfo("RunNumber", runNumber.toString());
	}
	catch ( RunInfoException e )
	{
	  logger.error("RunInfoException: "+e.toString());
	}
      }
    }
  }
      // Record LhcStatus info
      /*
      try
      {
	lhcStatus = LhcStatus.fetchLhcStatus();
	Class c = lhcStatus.getClass();
	Field fields [] = c.getFields();
	for ( int i=0; i<fields.length; i++)
	{
	  String fieldName = fields[i].getName();
	  Object value = fields[i].get(lhcStatus);
	  String type = fields[i].getType().getName();

	  if ( type.equals("java.lang.String") )
	  {
	    StringT sValue = new StringT((String)value);
	    Parameter<StringT> p = new Parameter<StringT>(fieldName,sValue);
	    runInfo.publishWithHistory(p);
	  }
	  else if ( type.equals("int" ) )
	  {
	    IntegerT iValue = new IntegerT((Integer)value);
	    Parameter<IntegerT> p = new Parameter<IntegerT>(fieldName,iValue);
	    runInfo.publishWithHistory(p);
	  }
	}
      }
     
      catch ( Exception e )
      {
	logger.warn(e.toString());
      }
    }
    else

    {
      logger.error("Cannot find RunInfoConnectorIF");
    }
  }
      */

	private void destroyXDAQ() throws UserActionException {
	functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.STATE,new StringT(functionManager.getState().getStateString())));
	
		// get executives and their job controls 
		List<QualifiedResource> listExecutive = qualifiedGroup
		.seekQualifiedResourcesOfType(new XdaqExecutive());


		for (QualifiedResource ex : listExecutive) {
			((XdaqExecutive)ex).killMe();
			logger.debug("killMe() called on " + ex.getResource().getURL());
		}
	}

	private void killAllXDAQ() throws UserActionException {

		// get executives and their job controls 
		List<QualifiedResource> listJC = qualifiedGroup
		.seekQualifiedResourcesOfType(new JobControl());

		for (QualifiedResource jc : listJC) {
			((JobControl)jc).killAll();
			logger.debug("killAll() called on " + jc.getResource().getURL());
		}
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
					functionManager.xdaqSupervisor.execute(new Input("SetTTS"));

					bits = (bits + 1) % 16;  // prepare for the next shot.
					repeat--;

					if (repeat == 0) {
						functionManager.fireEvent(MyInputs.SETTTSTEST_MODE);
					}
				} catch (Exception e) {
					logger.error("TTSSetter", e);
					functionManager.fireEvent(MyInputs.SETERROR);
				}
			}
		}
	}


}

// End of file
// vim: set sw=4 ts=4:
