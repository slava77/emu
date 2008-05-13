package rcms.fm.app.cscLevelOne.gui;

import java.util.HashMap;
import java.util.Map;

import javax.servlet.jsp.JspException;

import org.apache.log4j.Logger;

import rcms.fm.app.cscLevelOne.MyFunctionManager;
import rcms.fm.app.cscLevelOne.MyParameters;
import rcms.fm.ws.parameter.stubs.ParameterBean;
import rcms.gui.common.FMPilotState;
import rcms.gui.servlet.pilot.FMPilotBean;
import rcms.gui.servlet.pilot.FMPilotConstants;

public class GuiSubdetMsgPanel {

	private static final long serialVersionUID = 1L;

	private static Logger logger = Logger.getLogger(GuiSubdetMsgPanel.class);

	private MyFunctionManager fm;


	public GuiSubdetMsgPanel( MyFunctionManager fm ) {
		super();
		this.fm = fm;
	}

	/**
	 * Generates the necessary Html code.
	 *  
	 * @param sb the output string buffer.
	 * @param fmPilot the FMPilotBean.
	 */
	public String generateHtml() {


		String html = "";
		html += "<table>\n";
		html += "<tbody>\n";
		for (String subsys : MyParameters.KNOWN_SUBSYSTEMS) {
			html += "<tr>\n";
			if ( subsys != null && fm.getParameterSet().get(subsys) != null && ! fm.getParameterSet().get(subsys).getValue().toString().equals(MyParameters.AWAY) 
			) { 

				html += "\n<td class=\"subsys\">" + subsys + "</td>";

				// add action messages
				html += "\n<td> <div id=\"" + subsys + "_ACTION_MSG\"  class=\"SubdetMessageValue\">" ;
				html += "</div>";

				// add error messages		
				html += "\n<div id=\"" + subsys + "_ERROR_MSG\"  class=\"SubdetErrorMessageValue\">" ;
				html += "</div> </td>";
			}
			html += "</tr>\n";
		}
		html += "</table>\n";
		html += "</tbody>\n";

		return html;
	} 
}
