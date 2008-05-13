package rcms.fm.app.cscLevelOne.gui;

import rcms.fm.app.cscLevelOne.MyFunctionManager;

public class GuiStatePanel {

	private MyFunctionManager fm;
	
	public GuiStatePanel( MyFunctionManager fm ) {
		super();
		this.fm = fm;
	}
	
	public String generateHtml() {
			String state = "Faulty";
			if (fm!=null&&fm.getState()!=null) state = fm.getState().getStateString();			
			String html = "";
			html += "<div style=\"height: 20px; padding-top: 0px; padding-left: 15px;\" class=\""+ state + "\" id='currentState'>";
			html += state + "</div>";
			return html;
	}
}


