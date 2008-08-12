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
    
    /**
     * standard parameter definitions for Level 1 Function Manager
     */
    public static final String SID = "SID";

    public static final String SEQ_NAME = "DEFAULT";

    public static final String STATE = "STATE";
    
    public static final String RUN_TYPE = "RUN_TYPE";
    
    public static final String RUN_MODE = "RUN_MODE";
    
    public static final String GLOBAL_CONF_KEY = "GLOBAL_CONF_KEY";
    
    public static final String RUN_NUMBER = "RUN_NUMBER";
    
    public static final String ACTION_MSG = "ACTION_MSG";
    
    public static final String ERROR_MSG = "ERROR_MSG";
    
    public static final String COMPLETION = "COMPLETION";
    
    public static final String CSC_NS = "CMS.CSC";

    // TTS testing set
    public static final String TTS_TEST_FED_ID = "TTS_TEST_FED_ID";
    
    public static final String TTS_TEST_MODE = "TTS_TEST_MODE";
    
    public static final String TTS_TEST_PATTERN = "TTS_TEST_PATTERN";
    
    public static final String TTS_TEST_SEQUENCE_REPEAT = "TTS_TEST_SEQUENCE_REPEAT";
    
    public static final String GUI_SUBDET_PANEL_HTML="GUI_SUBDET_PANEL_HTML";
    public static final String GUI_STATE_PANEL_HTML="GUI_STATE_PANEL_HTML";
    //CONFIGURE PARAMETERS
    public static final String CSC_CALIB_KEYS_AVAILABLE="CSC_CALIB_KEYS_AVAILABLE";
    public static final String CSC_CALIB_KEY="CSC_CALIB_KEY";
    public static final String LOCAL_CONF_KEY="LOCAL_CONF_KEY";


    
    // standard level 1 parameter set
    public static final ParameterSet<FunctionManagerParameter> LVL_ONE_PARAMETER_SET = new ParameterSet<FunctionManagerParameter>();
    
    static {
	/**
	 * Session Identifier
	 */
	LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<IntegerT>(SID, new IntegerT(-1)));//, Exported.READONLY));
	LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(SEQ_NAME, new StringT("DEFAULT"),Exported.READONLY));

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
	LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(CSC_CALIB_KEYS_AVAILABLE,new StringT("")));
	LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(CSC_CALIB_KEY,new StringT("")));

	
	LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<IntegerT>(RUN_NUMBER, new IntegerT(-1)));//, Exported.READONLY));
	LVL_ONE_PARAMETER_SET.put( new FunctionManagerParameter<StringT>(GUI_SUBDET_PANEL_HTML,new StringT(" ")));
	LVL_ONE_PARAMETER_SET.put( new FunctionManagerParameter<StringT>(GUI_STATE_PANEL_HTML,new StringT(" ")));
	
	/**
	 * parameters for monitoring
	 */
	LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(ACTION_MSG, new StringT(""), Exported.READONLY));
	LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<StringT>(ERROR_MSG, new StringT(""), Exported.READONLY));
	LVL_ONE_PARAMETER_SET.put(new FunctionManagerParameter<DoubleT>(COMPLETION, new DoubleT(-1), Exported.READONLY));
		
    }
    
}
