package rcms.fm.app.cscLevelOne;

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
 * SID						: Session Identifier
 * STATE					: State name the function manager is currently in
 * RUN_TYPE					: String identifying the global run type 
 * RUN_MODE					: String identifying the global run mode
 * GLOBAL_CONF_KEY		 	: String representing the global configuration key
 * RUN_NUMBER				: Run number of current run
 * ACTION_MSG 				: Short description of current activity, if any
 * ERROR_MSG 				: In case of an error contains a description of the error
 * COMPLETION 				: Completion of an activity can be signaled through this numerical value 0 < PROGRESS_BAR < 1
 * 
 * For more details => https://twiki.cern.ch/twiki/bin/view/CMS/StdFMParameters
 * 
 * @author Andrea Petrucci, Alexander Oh, Michele Gulmini
 *
 */
public class MyParameters {

//	subsystems
       public static final String CSC="CSC";
       public static final String CSC_EFED="CSC_EFED";
	public static final String KNOWN_SUBSYSTEMS[] =  new String[] {
	    CSC,
	    CSC_EFED
	} ; 

	public static final String KNOWN_PARAS[] =  new String[] {
		"_STATE",
		"_COMPLETION",
		"_MESSAGE",
		"_ERROR",
		"_FEDS",
		"_RUN_KEYS",
		"_RUN_KEY",
		"_URL"
	} ; 
	public static final Class KNOWN_PARA_TYPES[] =  new Class[] {
		StringT.class,
		DoubleT.class,
		StringT.class,
		StringT.class,
		StringT.class,
		StringT.class,
		StringT.class,
		StringT.class,
	} ; 
	



 
	/**
	 * standard parameter definitions for Level 1 Function Manager
	 */
	public static final String SID = "SID";

	public static final String STATE = "STATE";

	public static final String RUN_TYPE = "RUN_TYPE";

	public static final String RUN_MODE = "RUN_MODE";

	public static final String GLOBAL_CONF_KEY = "GLOBAL_CONF_KEY";

	public static final String RUN_NUMBER = "RUN_NUMBER";

	public static final String ACTION_MSG = "ACTION_MSG";

	public static final String ERROR_MSG = "ERROR_MSG";

	public static final String COMPLETION = "COMPLETION";
	
	
	// TTS testing set
	public static final String TTS_TEST_FED_ID = "TTS_TEST_FED_ID";
	
	public static final String TTS_TEST_MODE = "TTS_TEST_MODE";
	
	public static final String TTS_TEST_PATTERN = "TTS_TEST_PATTERN";
	
	public static final String TTS_TEST_SEQUENCE_REPEAT = "TTS_TEST_SEQUENCE_REPEAT";

	// for GUI
	public static final String RUN_TYPES_AVAILABLE="RUN_TYPES_AVAILABLE";
	public static final String GLOBAL_CONF_KEYS_AVAILABLE="GLOBAL_CONF_KEYS_AVAILABLE";
	public static final String GLOBAL_KEY_SETS_AVAILABLE="GLOBAL_KEY_SETS_AVAILABLE";
	public static final String HEARTBEAT="HEARTBEAT";
	public static final String GUI_COMMANDS_AVAILABLE="GUI_COMMANDS_AVAILABLE";
	public static final String GUI_SUBDET_COMMANDS_AVAILABLE="GUI_SUBDET_COMMANDS_AVAILABLE";
	public static final String GUI_DAQ_COMMANDS_AVAILABLE="GUI_DAQ_COMMANDS_AVAILABLE";
	public static final String GUI_TRG_COMMANDS_AVAILABLE="GUI_TRG_COMMANDS_AVAILABLE";
	public static final String GUI_COMMAND="GUI_COMMAND";
	public static final String GUI_EXTENDED_MODE="GUI_EXTENDED_MODE";
	public static final String GUI_BLACK_BOARD="GUI_BLACK_BOARD";
	public static final String TTS_TEST_PASSED = "TTS_TEST_PASSED";
	public static final String TTS_SEQ_READ_VAL = "TTS_SEQ_READ_VAL";
	
	// for GUI RENDERING
	public static final String GUI_COMMAND_PANEL_HTML="GUI_COMMAND_PANEL_HTML";
	public static final String GUI_FED_TTS_PANEL_HTML="GUI_FED_TTS_PANEL_HTML";
	public static final String GUI_STATE_PANEL_HTML="GUI_STATE_PANEL_HTML";
	public static final String GUI_SUBDET_PANEL_HTML="GUI_SUBDET_PANEL_HTML";
	public static final String GUI_SUBDET_MSG_PANEL_HTML="GUI_SUBDET_MSG_PANEL_HTML";
	public static final String GUI_TTSTEST_PANEL_HTML="GUI_TTSTEST_PANEL_HTML";

    public static final String LOCAL_KEYS_AVAILABLE="LOCAL_KEYS_AVAILABLE";
    public static final String RUBUFM_STATE           = "RUBUFM_STATE";
    public static final String HLTSFM_STATE           = "HLTSFM_STATE";
    public static final String SELECTED_HOSTS      = "SELECTED_HOSTS";
    public static final String SELECTED_FEDS       = "SELECTED_FEDS";
    public static final String DCS_STATUS          = "DCS_STATUS";





    //CONFIGURE PARAMETERS

    public static final String LOCAL_CONF_KEY="LOCAL_CONF_KEY";

    public static final String RUN_KEY = "RUN_KEY";

    //GUI INTERACTIVE STATES

    public static final String CSCSV_STATE            = "CSCSV_STATE";
    public static final String STATES_MODS_ALLOWED[] = new String[] {
            MyStates.INITIAL.toString(),
            MyStates.HALTED.toString(),
            MyStates.CONFIGURED.toString(),
            MyStates.TTSTEST_MODE.toString()
    };


	// values
	public static final String IN="In";
	public static final String OUT="Out";
	public static final String AWAY="Away";

	public static final String FED_ENABLE_MASK="FED_ENABLE_MASK";
	public static final String FED_TTC_MAP="FED_TTC_MAP";
	public static final String FED_NAME_MAP="FED_NAME_MAP";

	/**
	 * String to summarize test status
	 * format:
	 * [FED_ID]%[TEST_CODE]&...
	 * FED_ID   : fed id
	 * TEST_CODE: 0=not tested
	 *            1=test passed
	 *            2=test not passed
	 */
	public static final String TTS_TEST_SUMMARY = "TTS_TEST_SUMMARY";

    public static final String SUPERVISOR_URL         = "CSC_SUPERVISOR";


	// standard level 1 parameter set
	public static final ParameterSet<FunctionManagerParameter> LVL_ONE_PARAMETER_SET = new ParameterSet<FunctionManagerParameter>();
	
	static {
		/**
		 * Session Identifier
		 */
	        LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<IntegerT>(SID, new IntegerT(-1)));//, Exported.READONLY));
		/**
		 * State of the Function Manager is currently in
		 */
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(STATE, new StringT(""))); //Exported.READONLY));
		/**
		 * Run Type can be either "Fun", or "Physics"
		 */
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(RUN_TYPE, new StringT(""))); //Exported.READONLY));
		/**
		 * mode can be "Normal" or "Debug". Influences the behaviour of the top
		 * FM.
		 */
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(RUN_MODE, new StringT(""))); //Exported.READONLY));
		/**
		 * global configuration key for current run
		 */
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(GLOBAL_CONF_KEY, new StringT(""), Exported.READONLY));
		/**
		 * the run number
		 */
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<IntegerT>(RUN_NUMBER, new IntegerT(-1)));//, Exported.READONLY));
	
		
		/**
		 * parameters for monitoring
		 */
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(ACTION_MSG, new StringT(""), Exported.READONLY));
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(ERROR_MSG, new StringT(""), Exported.READONLY));
		LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<DoubleT>(COMPLETION, new DoubleT(-1), Exported.READONLY));

        /**
         * GUI parameters
         */

        LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(RUN_TYPES_AVAILABLE,new StringT(" ")/*,Exported.READONLY*/));

        //other
        LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(HEARTBEAT,new StringT("")/*,Exported.READONLY*/));
	LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(GUI_COMMAND,new StringT("")/*,Exported.READONLY*/));
        LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(GUI_COMMANDS_AVAILABLE,new StringT(" ")/*,Exported.READONLY*/));
        LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(GUI_EXTENDED_MODE, new StringT("false")/*,Exported.READONLY*/));

	LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(LOCAL_KEYS_AVAILABLE,new StringT(" ")/*,Exported.READONLY*/));
        LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(CSCSV_STATE, new StringT("")));
	LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(SELECTED_HOSTS, new StringT("")) );
	//	LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(CSC_SUPERVISOR, new StringT("")));

	}

}
