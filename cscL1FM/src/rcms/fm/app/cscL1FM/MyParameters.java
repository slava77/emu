package rcms.fm.app.cscL1FM;

import rcms.fm.fw.parameter.FunctionManagerParameter;
import rcms.fm.fw.parameter.ParameterSet;
import rcms.fm.fw.parameter.FunctionManagerParameter.Exported;
import rcms.fm.fw.parameter.type.DoubleT;
import rcms.fm.fw.parameter.type.IntegerT;
import rcms.fm.fw.parameter.type.StringT;

/**
 * Defined Level 1 Function Manager parameters.
 * 
 * Standard parameter definitions for Level 1 Function Manager
 * 
 * STATE					: State name the function manager is currently in
 *
 * ACTION_MSG 				: Short description of current activity, if any
 * ERROR_MSG 				: In case of an error contains a description of the error
 * COMPLETION 				: Completion of an activity can be signaled through this numerical value 0 < PROGRESS_BAR < 1
 * 
 * For more details => https://twiki.cern.ch/twiki/bin/view/CMS/StdFMParameters
 * 
 * @author Andrea Petrucci, Alexander Oh, Michele Gulmini, Hannes Sakulin
 *
 */
public class MyParameters {
 
	/**
	 * standard parameter definitions for Level 1 Function Manager
	 */	

	public static final String STATE = "STATE";


	public static final String ACTION_MSG = "ACTION_MSG";

	public static final String ERROR_MSG = "ERROR_MSG";

	public static final String COMPLETION = "COMPLETION";

	

	// for HCAL LUMI only
	public static final String HCAL_LUMI_STATE = "HCAL_LUMI_STATE";
	public static final String HCAL_LUMI_KEY = "HCAL_LUMI_KEY";
	public static final String HCAL_LUMI_COMMAND = "HCAL_LUMI_COMMAND";
	
	
	// To be exported after initialize
	public static final String INITIALIZED_WITH_SID = "INITIALIZED_WITH_SID";
	public static final String INITIALIZED_WITH_GLOBAL_CONF_KEY = "INITIALIZED_WITH_GLOBAL_CONF_KEY";

	// To be exported after configure
	public static final String CONFIGURED_WITH_FED_ENABLE_MASK = "CONFIGURED_WITH_FED_ENABLE_MASK";
	public static final String CONFIGURED_WITH_RUN_KEY = "CONFIGURED_WITH_RUN_KEY";
	public static final String CONFIGURED_WITH_RUN_NUMBER = "CONFIGURED_WITH_RUN_NUMBER";
	
	// To be exported after start
	public static final String STARTED_WITH_RUN_NUMBER = "STARTED_WITH_RUN_NUMBER";
	
	
	// Command parameters
	public static final String SID = "SID";
	public static final String GLOBAL_CONF_KEY = "GLOBAL_CONF_KEY";

	public static final String RUN_NUMBER = "RUN_NUMBER";
	public static final String RUN_KEY = "RUN_KEY";

	public static final String FED_ENABLE_MASK = "FED_ENABLE_MASK";
	public static final String TRIGGER_NUMBER_AT_PAUSE = "TRIGGER_NUMBER_AT_PAUSE";

	// Command parameters for TTS testing
	public static final String TTS_TEST_FED_ID = "TTS_TEST_FED_ID";
	
	public static final String TTS_TEST_MODE = "TTS_TEST_MODE";
	
	public static final String TTS_TEST_PATTERN = "TTS_TEST_PATTERN";
	
	public static final String TTS_TEST_SEQUENCE_REPEAT = "TTS_TEST_SEQUENCE_REPEAT";
	
	
	// standard level 1 parameter set
	public static final ParameterSet<FunctionManagerParameter> LVL_ONE_PARAMETER_SET = new ParameterSet<FunctionManagerParameter>();


	
	static {

		/**
		 * State of the Function Manager is currently in
		 */
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(STATE, new StringT(""), Exported.READONLY));

		/**
		 * parameters for monitoring
		 */
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(ACTION_MSG, new StringT(""), Exported.READONLY));
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(ERROR_MSG, new StringT(""), Exported.READONLY));
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<DoubleT>(COMPLETION, new DoubleT(-1), Exported.READONLY));
		
		/**
		 * Session Identifier
		 */
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<IntegerT>(INITIALIZED_WITH_SID, new IntegerT(-1), Exported.READONLY));
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(INITIALIZED_WITH_GLOBAL_CONF_KEY, new StringT(""), Exported.READONLY));
				
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(CONFIGURED_WITH_FED_ENABLE_MASK, new StringT(""), Exported.READONLY));
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(CONFIGURED_WITH_RUN_KEY, new StringT(""), Exported.READONLY));
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<IntegerT>(CONFIGURED_WITH_RUN_NUMBER, new IntegerT(-1), Exported.READONLY));
		
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<IntegerT>(STARTED_WITH_RUN_NUMBER, new IntegerT(-1), Exported.READONLY));

		/** special - for HCAL only */
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(HCAL_LUMI_STATE, new StringT(""), Exported.READWRITE));
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(HCAL_LUMI_COMMAND, new StringT(""), Exported.READWRITE));
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(HCAL_LUMI_KEY, new StringT(""), Exported.READWRITE));
		
	}

}
