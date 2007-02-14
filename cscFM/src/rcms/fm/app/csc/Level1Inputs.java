package rcms.fm.app.csc;

import rcms.statemachine.definition.Input;

public interface Level1Inputs {

	//
	// Inputs for state transitions
	//
	Input INITIALIZE = new Input("Initialize");
	Input INITIALIZE_DONE = new Input("Initialized");

	Input TTS_PREPARE = new Input("TTSTestMode");
	Input TTS_PREPARE_DONE = new Input("TTSPrepared");

	Input TTS_TEST = new Input("TestTTS");
	Input TTS_TEST_DONE = new Input("TTSTested");

	Input HALT = new Input("Halt");
	Input HALT_DONE = new Input("Halted");

	Input ERROR = new Input("Error");
}

// End of file
// vim: set sw=4 ts=4:
