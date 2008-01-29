package rcms.fm.app.csc;

import rcms.fm.fw.parameter.FunctionManagerParameter;
import rcms.fm.fw.parameter.ParameterSet;
import rcms.fm.fw.parameter.type.*;

/**
 * Defined Level 1 Function Manager parameters.
 * 
 */
public class Level1Parameters {
 
	/**
	 * standard parameter definitions for Level 1 Function Manager
	 */

	// FM parameters
	public static final String STATE = "STATE";
	public static final String ACTION_MSG = "ACTION_MSG";
	public static final String ERROR_MSG = "ERROR_MSG";
	public static final String COMPLETION = "COMPLETION";	
        public static final String CONF_ID = "CONF_ID";

	// 'Initialize'
	public static final String SID = "SID";
	public static final String GLOBAL_CONF_KEY = "GLOBAL_CONF_KEY";

	// 'Configure'
	public static final String RUN_TYPE = "RUN_TYPE";

	// 'Start'
	public static final String RUN_NUMBER = "RUN_NUMBER";

	// for TTS test
	public static final String TTS_TEST_FED_ID = "TTS_TEST_FED_ID";
	public static final String TTS_TEST_MODE = "TTS_TEST_MODE";
	public static final String TTS_TEST_PATTERN = "TTS_TEST_PATTERN";
	public static final String TTS_TEST_SEQUENCE_REPEAT =
			"TTS_TEST_SEQUENCE_REPEAT";

	// standard level 1 parameter set
	public static final ParameterSet<FunctionManagerParameter>
			LEVEL1_PARAMETER_SET =
			new ParameterSet<FunctionManagerParameter>();
	
	static {
		/**
		 * State of the Function Manager
		 */
		LEVEL1_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(
				STATE, new StringT("")));

		/**
		 * parameters for monitoring
		 */
		LEVEL1_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(
				ACTION_MSG, new StringT("")));
		LEVEL1_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(
				ERROR_MSG, new StringT("")));
		LEVEL1_PARAMETER_SET.put(new FunctionManagerParameter<DoubleT>(
				COMPLETION, new DoubleT(0)));
		LEVEL1_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(
				CONF_ID, new StringT("")));

	}
}

// End of file
// vim: set sw=4 ts=4:
