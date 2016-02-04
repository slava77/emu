package rcms.fm.app.cscL1FM;


import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import rcms.errorFormat.CMS.CMSError;
import rcms.fm.fw.StateEnteredEvent;
import rcms.fm.fw.parameter.CommandParameter;
import rcms.fm.fw.parameter.FunctionManagerParameter;
import rcms.fm.fw.parameter.ParameterSet;
import rcms.fm.fw.parameter.type.IntegerT;
import rcms.fm.fw.parameter.type.LongT;
import rcms.fm.fw.parameter.type.StringT;
import rcms.fm.fw.user.UserActionException;
import rcms.fm.fw.user.UserStateNotificationHandler;
import rcms.fm.resource.QualifiedGroup;
import rcms.fm.resource.QualifiedResource;
import rcms.fm.resource.qualifiedresource.XdaqApplication;
import rcms.fm.resource.qualifiedresource.XdaqApplicationContainer;
import rcms.stateFormat.StateNotification;
import rcms.util.logger.RCMSLogger;

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
		addAction(MyStates.COLDRESETTING,           "coldResettingAction");		
		addAction(MyStates.PAUSING,     			"pauseAction");
		addAction(MyStates.RECOVERING,  			"recoverAction");
		addAction(MyStates.RESETTING,   			"resetAction");
		addAction(MyStates.RESUMING,    			"resumeAction");
		addAction(MyStates.STARTING,    			"startAction");
		addAction(MyStates.STOPPING,    			"stopAction");
	
		addAction(MyStates.FIXINGSOFTERROR,    		"fixSoftErrorAction");
		
		addAction(MyStates.RUNNINGDEGRADED, "runningDegradedAction");                    // for testing with external inputs
		addAction(MyStates.RUNNINGSOFTERRORDETECTED, "runningSoftErrorDetectedAction");  // for testing with external inputs
		addAction(MyStates.RUNNING, "runningAction");                                    // for testing with external inputs
	}
	
	
	public void init() throws rcms.fm.fw.EventHandlerException {
		functionManager = (MyFunctionManager) getUserFunctionManager();
		qualifiedGroup  = functionManager.getQualifiedGroup();
		
		// debug
		logger.debug("init() called: functionManager=" + functionManager );
	}
	
	

	public void initAction(Object obj) throws UserActionException {
		
		if (obj instanceof StateNotification) {
			
			// triggered by State Notification from child resource
			
			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/
			
			return;
		}
		
		else if (obj instanceof StateEnteredEvent) {
			
			// triggered by entered state action
			// let's command the child resources
			
			// debug
			logger.debug("initAction called.");
			
			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("Initializing")));
		
			// get the parameters of the command
			Integer sid;
			String globalConfKey = null;
			
			try {
				ParameterSet<CommandParameter> parameterSet = getUserFunctionManager().getLastInput().getParameterSet();
				sid = ((CommandParameter<IntegerT>)parameterSet.get(MyParameters.SID)).getValue().getInteger();
				globalConfKey = ((CommandParameter<StringT>)parameterSet.get(MyParameters.GLOBAL_CONF_KEY)).getValue().toString();
			}
			catch (Exception e) {
				// go to error, we require parameters
				String errMsg = "initAction: error reading command parameters of Initialize command.";
				
				// log error
				logger.error(errMsg, e);
				
				// notify error
				sendCMSError(errMsg);
				
				//go to error state
				functionManager.fireEvent( MyInputs.SETERROR );
				return;
			}
 			
				
			// 
			// initialize qualified group
			
			//
			QualifiedGroup qg = functionManager.getQualifiedGroup();

			try {
				qg.init();
			} catch (Exception e) {
				// failed to init
				String errMsg = this.getClass().toString() + " failed to initialize resources";
			
				// send error notification
				sendCMSError(errMsg);
		
				//log error
				logger.error(errMsg,e);
			
				// go to error state
				functionManager.fireEvent(MyInputs.SETERROR);
				return;
			}


			// find xdaq applications
			List<QualifiedResource> xdaqList = qg.seekQualifiedResourcesOfType(new XdaqApplication());
			functionManager.containerXdaqApplication = new XdaqApplicationContainer(xdaqList);
			logger.debug("Application list : " + xdaqList.size() );

			// Example: find "your" applications
			// functionManager.containerYourClass = new XdaqApplicationContainer( 
			//		functionManager.containerXdaqApplication.getApplicationsOfClass("yourClass"));

			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/

			// set exported parameters
			((FunctionManagerParameter<IntegerT>)functionManager.getParameterSet().get(MyParameters.INITIALIZED_WITH_SID)).setValue(new IntegerT(sid));
			((FunctionManagerParameter<StringT>)functionManager.getParameterSet().get(MyParameters.INITIALIZED_WITH_GLOBAL_CONF_KEY)).setValue(new StringT(globalConfKey));
			
			
			 // go to HALT
			functionManager.fireEvent( MyInputs.SETHALTED );
			
			
			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("")));
			
			logger.info("initAction Executed");
		}
	}


	public void resetAction(Object obj) throws UserActionException {
		
		if (obj instanceof StateNotification) {
			
			// triggered by State Notification from child resource
			
			/************************************************
			 * PUT YOUR CODE HERE							
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
				
				/************************************************
				 * PUT YOUR CODE HERE							
				 ***********************************************/
				
				// go to Initital
				functionManager.fireEvent( MyInputs.SETHALTED );
				
				// Clean-up of the Function Manager parameters
				cleanUpFMParameters();
				
				logger.info("resetAction Executed");
		}	
	}
	
	public void recoverAction(Object obj) throws UserActionException {
		
		if (obj instanceof StateNotification) {
			
			// triggered by State Notification from child resource
			
			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/
					
			return;
		}
		
		else if (obj instanceof StateEnteredEvent) {
			
				System.out.println("Executing recoverAction");
				logger.info("Executing recoverAction");
				
				// set action
				functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("recovering")));
				
				/************************************************
				 * PUT YOUR CODE HERE							
				 ***********************************************/
				
				// leave intermediate state
				functionManager.fireEvent( MyInputs.SETHALTED );
				
				// Clean-up of the Function Manager parameters
				cleanUpFMParameters();
				
				logger.info("recoverAction Executed");
				}
	}
	
	public void configureAction(Object obj) throws UserActionException {
		
		if (obj instanceof StateNotification) {

			// triggered by State Notification from child resource
			
			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/

			return;
		}
		
		else if (obj instanceof StateEnteredEvent) {
			System.out.println("Executing configureAction");
			logger.info("Executing configureAction");
			
			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("configuring")));
			
			// get the parameters of the command
			Integer runNumber;
			String runKey = null;
			String fedEnableMask = null;
			
			try {
				ParameterSet<CommandParameter> parameterSet = getUserFunctionManager().getLastInput().getParameterSet();
				runNumber = ((CommandParameter<IntegerT>)parameterSet.get(MyParameters.RUN_NUMBER)).getValue().getInteger();
				runKey = ((CommandParameter<StringT>)parameterSet.get(MyParameters.RUN_KEY)).getValue().toString();
				fedEnableMask = ((CommandParameter<StringT>)parameterSet.get(MyParameters.FED_ENABLE_MASK)).getValue().toString();
			}
			catch (Exception e) {
				// go to error, we require parameters
				String errMsg = "configureAction: error reading command parameters of Configure command.";
				
				// log error
				logger.error(errMsg, e);
				
				// notify error
				sendCMSError(errMsg);
				
				//go to error state
				functionManager.fireEvent( MyInputs.SETERROR );
				return;
			}			
			
			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/
			
			// Set the configuration parameters in the Function Manager parameters
			((FunctionManagerParameter<IntegerT>)functionManager.getParameterSet().get(MyParameters.CONFIGURED_WITH_RUN_NUMBER)).setValue(new IntegerT(runNumber));
			((FunctionManagerParameter<StringT>)functionManager.getParameterSet().get(MyParameters.CONFIGURED_WITH_RUN_KEY)).setValue(new StringT(runKey));
			((FunctionManagerParameter<StringT>)functionManager.getParameterSet().get(MyParameters.CONFIGURED_WITH_FED_ENABLE_MASK)).setValue(new StringT(fedEnableMask));

			// leave intermediate state
			functionManager.fireEvent( MyInputs.SETCONFIGURED );
			
			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("")));

			
			
			logger.info("configureAction Executed");
		}
	}
	
	public void startAction(Object obj) throws UserActionException {
		
		if (obj instanceof StateNotification) {
			
			// triggered by State Notification from child resource
			
			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/

			return;
		}
		
		else if (obj instanceof StateEnteredEvent) {
			System.out.println("Executing startAction");
			logger.info("Executing startAction");
			
			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("starting")));

			// get the parameters of the command
			ParameterSet<CommandParameter> parameterSet = getUserFunctionManager().getLastInput().getParameterSet();

			// check parameter set
			if (parameterSet.size()==0 || parameterSet.get(MyParameters.RUN_NUMBER) == null )  {

				// go to error, we require parameters
				String errMsg = "startAction: no parameters given with start command.";
				
				// log error
				logger.error(errMsg);
				
				// notify error
				sendCMSError(errMsg);
				
				// go to error state
				functionManager.fireEvent( MyInputs.SETERROR );
				return;
			}
			
			// get the run number from the start command
			Integer runNumber = ((IntegerT)parameterSet.get(MyParameters.RUN_NUMBER).getValue()).getInteger();
			
			
			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/
			
			// Set the run number in the Function Manager parameters
			((FunctionManagerParameter<IntegerT>)functionManager.getParameterSet().get(MyParameters.STARTED_WITH_RUN_NUMBER)).setValue(new IntegerT(runNumber));

			// leave intermediate state
			functionManager.fireEvent( MyInputs.SETRUNNING );
			
			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("")));

			
			
			logger.debug("startAction Executed");
			
		}
	}
	
	public void pauseAction(Object obj) throws UserActionException {
		
		if (obj instanceof StateNotification) {
			
			// triggered by State Notification from child resource
			
			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/

			return;
		}
		
		else if (obj instanceof StateEnteredEvent) {
			
			System.out.println("Executing pauseAction");
			logger.info("Executing pauseAction");
			
			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("pausing")));
			
			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/
			
			// leave intermediate state
			functionManager.fireEvent( MyInputs.SETPAUSED );
			
			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("")));
			
			logger.debug("pausingAction Executed");
			
		}
	}
	
	public void stopAction(Object obj) throws UserActionException {
		
		if (obj instanceof StateNotification) {
			
			// triggered by State Notification from child resource
			
			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/

			return;
		}
		
		else if (obj instanceof StateEnteredEvent) {	
			System.out.println("Executing stopAction");
			logger.info("Executing stopAction");
			
			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("stopping")));
			
			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/
			
			// leave intermediate state
			functionManager.fireEvent( MyInputs.SETCONFIGURED );
			
			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("")));
			
			logger.debug("stopAction Executed");
			
		}
	}

	public void resumeAction(Object obj) throws UserActionException {
		
		if (obj instanceof StateNotification) {
			
			// triggered by State Notification from child resource
			
			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/

			return;
		}
		
		else if (obj instanceof StateEnteredEvent) {	
			System.out.println("Executing resumeAction");
			logger.info("Executing resumeAction");
			
			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("resuming")));
			
			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/
			
			// leave intermediate state
			functionManager.fireEvent( functionManager.hasSoftError() ? MyInputs.SETRESUMEDSOFTERRORDETECTED :
				                       ( functionManager.isDegraded() ? MyInputs.SETRESUMEDDEGRADED : MyInputs.SETRESUMED )  );
			
			
			// Clean-up of the Function Manager parameters
			cleanUpFMParameters();
		
			logger.debug("resumeAction Executed");
			
		}
	}
	
	public void haltAction(Object obj) throws UserActionException {
		
		if (obj instanceof StateNotification) {
			
			// triggered by State Notification from child resource
			
			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/
			
			return;
		}
		
		else if (obj instanceof StateEnteredEvent) {
			System.out.println("Executing haltAction");
			logger.info("Executing haltAction");
			
			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("halting")));
			
			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/

			// check from which state we came.
			if (functionManager.getPreviousState().equals(MyStates.TTSTEST_MODE)) {
				// when we came from TTSTestMode we need to
				// 1. give back control of sTTS to HW
			}
			
			
			// leave intermediate state
			functionManager.fireEvent( MyInputs.SETHALTED );
			
			// Clean-up of the Function Manager parameters
			cleanUpFMParameters();
			
			logger.debug("haltAction Executed");
		}
	}	
	
	public void preparingTTSTestModeAction(Object obj) throws UserActionException {

		if (obj instanceof StateNotification) {

			// triggered by State Notification from child resource

			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/

			return;
		}

		else if (obj instanceof StateEnteredEvent) {
			System.out.println("Executing preparingTestModeAction");
			logger.info("Executing preparingTestModeAction");

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("preparingTestMode")));

			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/
			// to prepare test we need to 
			// 1. configure & enable fed application
			// 2. take control of fed

			// leave intermediate state
			functionManager.fireEvent( MyInputs.SETTTSTEST_MODE );

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("")));

			logger.debug("preparingTestModeAction Executed");
		}
	}	
	
	public void testingTTSAction(Object obj) throws UserActionException {


		XdaqApplication fmm = null;
		Map attributeMap = new HashMap();

		if (obj instanceof StateNotification) {

			// triggered by State Notification from child resource

			/************************************************
			 * PUT YOUR CODE HERE							
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
			


			// debug
			logger.debug("Using parameters: fedId=" + fedId + "mode=" + mode + " pattern=" + pattern + " cycles=" + cycles );

			// find out which application controls the fedId.


			// found the correct application
			// to test we need to 
			// 1. issue the test command


			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/

			
			// leave intermediate state
			functionManager.fireEvent( MyInputs.SETTTSTEST_MODE );

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("")));

			logger.debug("preparingTestModeAction Executed");
		}
	}	

	public void coldResettingAction(Object obj) throws UserActionException {

		if (obj instanceof StateNotification) {

			// triggered by State Notification from child resource

			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/

			return;
		}

		else if (obj instanceof StateEnteredEvent) {
			System.out.println("Executing coldResettingAction");
			logger.info("Executing coldResettingAction");

			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("coldResetting")));

			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/
			// perform a cold-reset of your hardware

			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("Cold Reset completed.")));
			// leave intermediate state
			functionManager.fireEvent( MyInputs.SETHALTED );

			logger.debug("coldResettingAction Executed");
		}
	}	
	
	public void fixSoftErrorAction(Object obj) throws UserActionException {
		
		if (obj instanceof StateNotification) {
			
			// triggered by State Notification from child resource
			
			/************************************************
			 * PUT YOUR CODE HERE							
			 ***********************************************/

			return;
		}
		
		else if (obj instanceof StateEnteredEvent) {	
			System.out.println("Executing fixSoftErrorAction");
			logger.info("Executing fixSoftErrorAction");
			
			// set action
			functionManager.getParameterSet().put(new FunctionManagerParameter<StringT>(MyParameters.ACTION_MSG,new StringT("fixingSoftError")));
			
			// get the parameters of the command
			ParameterSet<CommandParameter> parameterSet = getUserFunctionManager().getLastInput().getParameterSet();

			// check parameter set
			Long triggerNumberAtPause = null;
			if (parameterSet.size()==0 || parameterSet.get(MyParameters.TRIGGER_NUMBER_AT_PAUSE) == null) {

				// go to error, we require parameters
				String warnMsg = "fixSoftErrorAction: no parameters given with fixSoftError command.";
				
				// log error
				logger.warn(warnMsg);				
				
			} else {
				triggerNumberAtPause = ((LongT)parameterSet.get(MyParameters.TRIGGER_NUMBER_AT_PAUSE).getValue()).getLong();
			}
			
			/************************************************
			 * PUT YOUR CODE HERE TO FIX THE SOFT ERROR							
			 ***********************************************/
			
			functionManager.setSoftErrorDetected(false);
			
			
			// if the soft error cannot be fixed, the FM should go to ERROR
			
			if (functionManager.hasSoftError()) 
				functionManager.fireEvent(  MyInputs.SETERROR  );
			else
				functionManager.fireEvent(  functionManager.isDegraded() ? MyInputs.SETRUNNINGDEGRADED : MyInputs.SETRUNNING  );
			
			// Clean-up of the Function Manager parameters
			cleanUpFMParameters();
		
			logger.debug("resumeAction Executed");
			
		}
	}

	//
	// for testing with external inputs.
	//
	// Here we just set our DEGRADED/SOFTERROR state according to an external trigger that sent us to this state.
	// In a real FM, an external event or periodic check will trigger the FM to change state.
	// 
	//
	public void runningDegradedAction(Object obj) throws UserActionException {
		if (obj instanceof StateEnteredEvent) {
			functionManager.setDegraded(true);
		}
	}

	//
	// for testing with external inputs  
	//
	// Here we just set our DEGRADED/SOFTERROR state according to an external trigger that sent us to this state.
	// In a real FM, an external event or periodic check will trigger the FM to change state.
	// 
	//
	public void runningSoftErrorDetectedAction(Object obj) throws UserActionException {
		if (obj instanceof StateEnteredEvent) {
			// do not touch degraded 
			functionManager.setSoftErrorDetected(true);
		}
	}

	//
	// for testing with external inputs  
	//
	// Here we just set our DEGRADED/SOFTERROR state according to an external trigger that sent us to this state.
	// In a real FM, an external event or periodic check will trigger the FM to change state.
	// 
	//
	public void runningAction(Object obj) throws UserActionException {
		if (obj instanceof StateEnteredEvent) {
			functionManager.setDegraded(false);
			functionManager.setSoftErrorDetected(false);
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
		functionManager.getParameterSet().put(new FunctionManagerParameter<IntegerT>(MyParameters.TTS_TEST_FED_ID,new IntegerT(-1)));
	}
	
}
