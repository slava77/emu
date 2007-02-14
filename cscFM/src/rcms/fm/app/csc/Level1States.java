package rcms.fm.app.csc;

import rcms.statemachine.definition.State;

interface Level1States {

	//
	// States
	//
	State INITIAL = new State("Initial");

	State HALTED = new State("Halted");
	State HALTING = new State("Halting");
	State INITIALIZING = new State("Initializing");

	State TTS_READY = new State("TTSTestMode");
	State TTS_PREPARING = new State("PreparingTTSTestMode");
	State TTS_TESTING = new State("TestingTTS");

	State ERROR = new State("Error");
}

// End of file
// vim: set sw=4 ts=4:
