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
		html += "<table cellpadding=\"5pt\" cellspacing=\"3pt\" >";
		html += "<tbody>\n";

		// row subsystems
		html += "<tr>\n";
		html += "\n<td class=\"label\">Subsystem</td>";
		for (String subsys : MyParameters.KNOWN_SUBSYSTEMS) {
			if ( checkRenderCondition(subsys) ) { 
				html += "\n<td class=\"subsys\">" + subsys + "</td>"; 
			}
		}
		html += "</tr>\n";

		// row states			
		html += "<tr>\n";
		html += "\n<td class=\"label\">State</td>";
		for (String subsys : MyParameters.KNOWN_SUBSYSTEMS) {
			if ( checkRenderCondition(subsys) ) { 
				String subdetState = fm.getParameterSet().get( subsys + "_STATE" ).getValue().toString();
				html += "\n<td style=\"font-size: 0.8em; text-align: center;\"class=\"" + subdetState + "\" id=\"" + subsys + "_STATE\" >" + subdetState + "</td>"; 
			}
		}
		html += "</tr>\n";
		
		// row run keys			
		html += "<tr>\n";
		html += "\n<td class=\"label\">Run Key</td>";
		for (String subsys : MyParameters.KNOWN_SUBSYSTEMS) {
			if ( checkRenderCondition(subsys) ) { 
				html += "\n<td style=\"font-size: 0.8em; text-align: center; \">"; 
				html += "\n<select class=\"RunDataInputTable\" name=\""+subsys+"_RUN_KEYS\" onChange=\"setAjaxRunDataParameterSelect(\'"+subsys+"_RUN_KEY\','"
					+ "rcms.fm.fw.parameter.type.StringT' ,this)\">";

				String runKeys = fm.getParameterSet().get( subsys + "_RUN_KEYS" ).getValue().toString();
				
				// make a drop down
				if(runKeys.length()>2) {
					// rm []
					runKeys = runKeys.substring(1, runKeys.length()-1);
					// split by ,
					String[] keys = runKeys.split(",");
					html += "\n<option id=\""+subsys+"_RUN_KEY\">";
					html += fm.getParameterSet().get(subsys+"_RUN_KEY").getValue().toString();
					for (String key : keys) {
						// rm "
						key = key.substring(key.indexOf("\"")+1);
						key = key.substring(0,key.indexOf("\""));
						html += "\n<option value=\"" + key + "\">" + key;
					}					
				}
				html += "</td>";
			}
		}		
		html += "</tr>\n";

		html += "</tbody>\n";	
		html += "</table>\n";	
		return html;
	} 


	private boolean checkRenderCondition(String subsys) {
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

	}
}
