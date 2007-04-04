package rcms.fm.app.csc;

import rcms.statemachine.definition.Input;

public interface Level1Inputs {

	//
	// Inputs for state transitions
	//
	Input INITIALIZE = new Input("Initialize");
	Input INITIALIZE_DONE = new Input("Initialized");

	Input CONFIGURE = new Input("Configure");
	Input CONFIGURE_DONE = new Input("Configured");

	Input START = new Input("Start");
	Input START_DONE = new Input("Started");

	Input STOP = new Input("Stop");
	Input STOP_DONE = new Input("Stopped");

	Input PAUSE = new Input("Pause");
	Input PAUSE_DONE = new Input("Paused");

	Input RESUME = new Input("Resume");
	Input RESUME_DONE = new Input("Resumed");

	Input HALT = new Input("Halt");
	Input HALT_DONE = new Input("Halted");

	Input RECOVER = new Input("Recover");
	Input RECOVER_DONE = new Input("Recovered");

	Input RESET = new Input("Reset");
	Input RESET_DONE = new Input("Resetted");  // no such english word!

	Input TTS_PREPARE = new Input("TTSTestMode");
	Input TTS_PREPARE_DONE = new Input("TTSPrepared");

	Input TTS_TEST = new Input("TestTTS");
	Input TTS_TEST_DONE = new Input("TTSTested");

	Input ERROR = new Input("Error");
}

// End of file
// vim: set sw=4 ts=4:
