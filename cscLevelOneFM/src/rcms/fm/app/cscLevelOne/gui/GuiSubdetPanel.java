package rcms.fm.app.cscLevelOne.gui;

import org.apache.log4j.Logger;

import com.sun.corba.se.impl.copyobject.JavaStreamObjectCopierImpl;

import rcms.fm.app.cscLevelOne.MyFunctionManager;
import rcms.fm.app.cscLevelOne.MyParameters;

public class GuiSubdetPanel {
    private static final long serialVersionUID = 1L;
    
    private static Logger logger = Logger.getLogger(GuiSubdetPanel.class);
    
    private MyFunctionManager fm;
    
    public GuiSubdetPanel( MyFunctionManager fm ) {
	super();
	this.fm = fm;
    }
    
    public String generateHtml() {
	String html = "";
	
	// create a table with all subsystems
	html += "<!-- Table subdet panel begin -->";
	html += "<table cellpadding=\"5pt\" cellspacing=\"4pt\" >";
	html += "<tbody>\n";
	
	// row run keys			
	html += "<tr>\n";
	html += "\n<td class=\"label\">Calibration Keys</td>";
	html += "\n<td style=\"font-size: 0.8em; text-align: center; \">"; 
	html += "\n<select class=\"RunDataInputTable\" name=\"CSC_CALIB_KEYS\" onChange=\"setRunDataParameterSelect(\'CSC_CALIB_KEY\','"
	    + "rcms.fm.fw.parameter.type.StringT' ,this)\">";
	String calibKeys = fm.getParameterSet().get( MyParameters.CSC_CALIB_KEYS_AVAILABLE).getValue().toString();
	// make a drop down
	if(calibKeys.length()>2) {
	    // rm []
	    calibKeys = calibKeys.substring(1, calibKeys.length()-1);
	    // split by ,
	    String[] keys = calibKeys.split(",");
	    html += "\n<option id=\"CSC_CALIB_KEY\">";
	    html += fm.getParameterSet().get("CSC_CALIB_KEY").getValue().toString();
	    for (String key : keys) {
		// rm "
		key = key.substring(key.indexOf("\"")+1);
		key = key.substring(0,key.indexOf("\""));
		html += "\n<option value=\"" + key + "\">" + key;
	    }	
	    
	}
	html += "\n</select> </td>";
	
	html += "</tr>\n";
	
	html += "</tbody>\n";	
	html += "</table>\n";	
	return html;
	
	
    } 
    
    private boolean checkRenderCondition(String subsys) {
	/*
	  if ( subsys != null 
	  && fm.getParameterSet().get(subsys) != null 
	  && !fm.getParameterSet().get(subsys).getValue().toString().equals(MyParameters.AWAY) 
	  && !fm.getParameterSet().get(subsys).getValue().toString().equals(MyParameters.OUT) 
	  ) { 
	  return true;
	  }
	  else {
	  return false;
	  }
	*/
	return false;
    }
}
