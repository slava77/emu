package rcms.fm.app.cscLevelOne.gui;


import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import org.apache.log4j.Logger;

import rcms.fm.app.cscLevelOne.MyFunctionManager;
import rcms.fm.app.cscLevelOne.MyInputs;
import rcms.fm.app.cscLevelOne.MyParameters;
import rcms.fm.app.cscLevelOne.MyStates;
import rcms.fm.ws.command.stubs.TransitionArrayBean;
import rcms.fm.ws.command.stubs.TransitionBean;
import rcms.gui.common.FMPilotState;
import rcms.gui.common.InputCommand;
import rcms.gui.servlet.pilot.FMPilotBean;
import rcms.gui.tag.Tag;
import rcms.statemachine.definition.Input;
import rcms.statemachine.definition.State;
import rcms.statemachine.definition.Transition;

public class GuiCommandPanel extends Tag{
    /**
     * 
     */
    private static final long serialVersionUID = 1L;
    
    private String cssClass = null;
    
    private static String effectCommon = "";
    
    private static String cssCommon = "font-family: sans-serif; font-style: normal; font-size: 0.8em; position: relative;top: 0px; left: 0px; height: 30px; width: 100px; border-width:5px; background-color:#303030";
    
    private static Logger logger = Logger.getLogger(GuiCommandPanel.class);
    
    // 
    // store rendering data for commands on the GUI
    // ad-hoc format is:
    // LABEL, CSS-CLASS, X-POS, Y-POS, EXTENDED-MODE-ONLY, SHOW-IN-INITIAL
    private static String[][] commandRenderData = {
	{ 
	    MyInputs.INITIALIZE.getInputString(),
	    "font-family: sans-serif; font-style: normal; font-size: 1.8em; position: relative;top: 0px; left: 0px; height: 60px; width:  200px" + 
	    "; background-color: black",
	    "0",
	    "0",
	    "false",
	    "true"
	},
	
	{
	    MyInputs.CONFIGURE.getInputString(),cssCommon + 
	    "; border-color: green",
	    "1",
	    "1",
	    "false",
	    "false"
	},	
	
	{
	    MyInputs.START.getInputString(),cssCommon + 
	    "; border-color: green",
	    "3",
	    "1",
	    "false",
	    "false"
	},
	{
	    MyInputs.PAUSE.getInputString(),cssCommon + 
	    "; border-color: blue",
	    "0",
	    "2",
	    "false",
	    "false"
	},
	{
	    MyInputs.RESUME.getInputString(),cssCommon + 
	    "; border-color: blue",
	    "1",
	    "2",
	    "false",
	    "false"
	},
	{
	    MyInputs.STOP.getInputString(),cssCommon + 
	    "; border-color: blue",
	    "2",
	    "2",
	    "false",
	    "false"
	},
	{
	    MyInputs.HALT.getInputString(),cssCommon + 
	    "; border-color: blue",
	    "3",
	    "2",
	    "false",
	    "false"
	},		
	
	{
	    MyInputs.RECOVER.getInputString(),cssCommon + 
	    "; border-color: blue",
	    "1",
	    "3",
	    "true",
	    "false"
	},
	
	{
	    MyInputs.RESET.getInputString(),cssCommon + 
	    "; border-color: red",
	    "3",
	    "3",
	    "true",
	    "false"
	},
	
	{
	    MyInputs.TTSTEST_MODE.getInputString(),cssCommon + 
	    "; border-color: blue",
	    "2",
	    "4",
	    "true",
	    "false"
	},
	{
	    MyInputs.TEST_TTS.getInputString(),cssCommon + 
	    "; border-color: blue",
	    "3",
	    "4",
	    "true",
	    "false"
	},
	{
	    "padder",
	    cssCommon ,
	    "6",
	    "2",
	    "true",
	    "false"
	},
    };
    
    private Map<String, String[]> commandRenderDataMap = new HashMap<String, String[]>();
    
    private Map<String, String> commandRenderEnabledMap = new HashMap<String, String>();
    
    private MyFunctionManager fm;
    
    public GuiCommandPanel( MyFunctionManager fm ) {
	super();
	this.fm = fm;
	// initialize command render data map
	for (int i=0; i< commandRenderData.length ; i++ ) {
	    commandRenderDataMap.put( commandRenderData[i][0], commandRenderData[i] );
	}
    }
    
    /**
     * Generates the necessary Html code.
     *  
     * @param sb the output string buffer.
     * @param fmPilot the FMPilotBean.
     */
    public String generateHtml() {
	
	String html = "";
	String state = MyStates.INITIAL.getStateString();
	State st = fm.getState();
	if (st!=null) state = st.getStateString();
	
	String css = this.cssClass != null ? this.cssClass : "";
	Collection<Input> inputs = fm.getStateMachineDefinition().getStateMachineDefinition().getInputs();
	
	for (Input inp : inputs) {
	    if (isCommandEnabled(state, inp.getInputString())) {
		commandRenderEnabledMap.put(inp.getInputString(),"true");
	    }
	    else {
		commandRenderEnabledMap.put(inp.getInputString(),"false");				
	    }
	}
	
	// 
	// INITIAL state
	//
	boolean isInitial = false;
	if (state.equals("Initial")) {
	    isInitial = true;
	}
	
	// loop over cmdData and put in 6x2 grid
	int irow = 0;
	int icol = 0;
	for (int i=0; i<commandRenderData.length-1; i++) {
	    String [] cmdData = (String[]) commandRenderData[i];
	    Integer myRow = Integer.valueOf( cmdData[3] );
	    Integer myCol = Integer.valueOf( cmdData[2] );					
	    
	    // 
	    // check mode, some buttons are only displayed if GUI_EXTENDED_MODE is "true"
	    //
	    if ( 
		(
		 ( commandRenderData[i][4].equals("true") 
		   &&  // only in extended gui mode
		   ( (String)(fm.getParameterSet().get(MyParameters.GUI_EXTENDED_MODE).getValue().toString() ) ).equals("true") // is extended gui mode 
		   ) || commandRenderData[i][4].equals("false") 
		 )
		&& 
		( isInitial == "true".equals(commandRenderData[i][5]) ) // handle initial state flag
		) 
		{
		    if (myRow.intValue() > irow) {
			html +=  "<p>";							
			irow++;
			icol=0;
		    }
		    // PADDING OF LEFT SIDE BUTTONS WITH DUMMY BUTTONS
		    while ( myCol.intValue() > icol && icol < 7) {
			html += "<button type=\"button\" id=\"dummy\" style=\"" 
			    + commandRenderData[commandRenderData.length-1][1] + ""
			    + "\" value=\"\" disabled\">"
			    + commandRenderData[i][0] + "</button>";	
			icol++;
		    }
		    if (  
			commandRenderEnabledMap.containsKey(commandRenderData[i][0]) && 
			commandRenderEnabledMap.get( commandRenderData[i][0]).equals("false")) {
			// INACTIVE BUTTONS
			html +=  
			    "<button disabled=\"true\" type=\"button\" id=\"" + commandRenderData[i][0] + "Button\" style=\"" 
			    + "color: grey;"+ commandRenderData[i][1] + ""  							                       
			    + "\" value=\""
			    + commandRenderData[i][0] + ""
			    + "\" onClick=\"onClickMyCommandButton('" 
			    + commandRenderData[i][0] + ""
			    + "',this); "
			    + "\"" + ">" 
			    + commandRenderData[i][0] + "</button>";
		    }
		    else {
			// ACTIVE BUTTONS
			html +=  
			    "<button type=\"button\" id=\"" + commandRenderData[i][0] + "Button\" style=\"" 
			    + "color: yellow;" +commandRenderData[i][1] + "" 					                       
			    + "\" value=\"" + commandRenderData[i][0] + ""
			    + "\" onClick=\"onClickMyCommandButton('" 
			    + commandRenderData[i][0]  + "',this); " + effectCommon
			    + "\" "
			    +">"
			    + commandRenderData[i][0] + "</button>";
		    }
		    icol++;
		}
	}
	return html;
    }
    
    
    
    /**
     * @param state				The state
     * @param inputCommandName  The name of the command input
     * @return Returns true if command input is enabled
     */
    public boolean isCommandEnabled(String state, String inputCommandName) {
	boolean result = false;
	Collection<Transition> transitions = fm.getStateMachineDefinition() != null ? fm.getStateMachineDefinition().getStateMachineDefinition().getTransitions(): null;
	if (transitions != null) {
	    try {
		for (Transition trans : transitions) {
		    if (
			(
			 trans.getInput().getInputString().equals(inputCommandName) &&
			 ( trans.getFrom().getStateString().equals(state) || trans.getFrom().equals(State.ANYSTATE) )
			 )							
			) {
			result = true;
			break;
		    }
		}
	    } catch (NullPointerException e) {
		e.printStackTrace();
	    }
	}
	return result;
    }
    
}


