package rcms.fm.app.cscL1FM;

import rcms.statemachine.definition.Input;

/**
 * Definition of Level 1 Function Manager Commands
 * 
 * @author Andrea Petrucci, Alexander Oh, Michele Gulmini
 */
public class MyInputs {

	// Defined commands for the level 1 Function Manager

	public static final Input INITIALIZE = new Input("Initialize");

	public static final Input CONFIGURE = new Input("Configure");

	public static final Input SETCONFIGURED = new Input("SetConfigured");

	public static final Input START = new Input("Start");

	public static final Input SETRUNNING = new Input("SetRunning");
	
	public static final Input SETRUNNINGDEGRADED = new Input("SetRunningDegraded");

	public static final Input SETRUNNINGSOFTERRORDETECTED = new Input("SetRunningSoftErrorDetected");
	
	public static final Input STOP = new Input("Stop");

	public static final Input HALT = new Input("Halt");

	public static final Input SETHALTED = new Input("SetHalted");

	public static final Input PAUSE = new Input("Pause");

	public static final Input SETPAUSED = new Input("SetPaused");

	public static final Input RESUME = new Input("Resume");
	
	public static final Input SETRESUMED = new Input("SetResumed"); 

	public static final Input SETRESUMEDDEGRADED = new Input("SetResumedDegraded");

	public static final Input SETRESUMEDSOFTERRORDETECTED = new Input("SetResumedSoftErrorDectected");

	public static final Input FIXSOFTERROR = new Input("FixSoftError");
	
	public static final Input RECOVER = new Input("Recover");

	public static final Input RESET = new Input("Reset");

	public static final Input PREPARE_TTSTEST_MODE = new Input("PrepareTTSTestMode");

	public static final Input SETTTSTEST_MODE = new Input("SetTTSTestMode");

	public static final Input TEST_TTS = new Input("TestTTS");

	public static final Input COLDRESET = new Input("ColdReset");
	
	
	// Go to error
	public static final Input SETERROR = new Input("SetError");

}