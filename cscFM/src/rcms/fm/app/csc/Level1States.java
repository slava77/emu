package rcms.fm.app.csc;

import rcms.statemachine.definition.State;

interface Level1States {

	//
	// States
	//
	State INITIAL = new State("Initial");
	State INITIALIZING = new State("Initializing");

	State CONFIGURING = new State("Configuring");
	State CONFIGURED = new State("Configured");

	State STARTING = new State("Starting");
	State RUNNING = new State("Running");

	State STOPPING = new State("Stopping");

	State PAUSING = new State("Pausing");
	State PAUSED = new State("Paused");

	State RESUMING = new State("Resuming");

	State HALTING = new State("Halting");
	State HALTED = new State("Halted");

	State RECOVERING = new State("Recovering");
	State RESETTING = new State("Resetting");

	State TTS_READY = new State("TTSTestMode");
	State TTS_PREPARING = new State("PreparingTTSTestMode");
	State TTS_TESTING = new State("TestingTTS");

	State ERROR = new State("Error");
}

// End of file
// vim: set sw=4 ts=4:
