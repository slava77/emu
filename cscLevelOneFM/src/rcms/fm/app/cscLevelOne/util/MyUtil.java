package rcms.fm.app.cscLevelOne.util;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.math.BigInteger;
import java.net.URL;
import java.net.URLConnection;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

import com.sun.tools.doclets.internal.toolkit.util.Group;

import net.hep.cms.xdaqctl.XDAQMessageException;
import net.hep.cms.xdaqctl.XDAQTimeoutException;
import net.hep.cms.xdaqctl.XMASMessage;
import net.hep.cms.xdaqctl.xdata.FlashList;
import net.hep.cms.xdaqctl.xdata.SimpleItem;
import net.hep.cms.xdaqctl.xdata.TimeVal;
import net.hep.cms.xdaqctl.xdata.XDataType;
import rcms.fm.fw.parameter.CommandParameter;
import rcms.fm.fw.parameter.FunctionManagerParameter;
import rcms.fm.fw.parameter.Parameter;
import rcms.fm.fw.parameter.ParameterSet;
import rcms.fm.fw.parameter.type.DateT;
import rcms.fm.fw.parameter.type.DoubleT;
import rcms.fm.fw.parameter.type.IntegerT;
import rcms.fm.fw.parameter.type.StringT;
import rcms.fm.fw.parameter.type.VectorT;
import rcms.fm.fw.parameter.util.ParameterFactory;
import rcms.fm.fw.parameter.util.ParameterUtil;
import rcms.fm.fw.service.parameter.ParameterServiceException;
import rcms.fm.app.cscLevelOne.MyFunctionManager;
import rcms.fm.app.cscLevelOne.MyParameters;
import rcms.fm.app.cscLevelOne.MyStates;
//import rcms.fm.app.cscLevelOne.common.FileIO;
//import rcms.fm.app.cscLevelOne.common.FileIOException;
import rcms.fm.app.cscLevelOne.gui.GuiCommandPanel;
//import rcms.fm.app.cscLevelOne.gui.GuiFedTtsPanel;
import rcms.fm.app.cscLevelOne.gui.GuiStatePanel;
import rcms.fm.app.cscLevelOne.gui.GuiSubdetMsgPanel;
import rcms.fm.app.cscLevelOne.gui.GuiSubdetPanel;
//import rcms.fm.app.cscLevelOne.GuiTtsTestPanel;
import rcms.fm.resource.QualifiedResource;
import rcms.fm.resource.qualifiedresource.FunctionManager;
import rcms.resourceservice.db.resource.config.ConfigProperty;
import rcms.resourceservice.db.resource.fm.FunctionManagerResource;
import rcms.util.logger.RCMSLogger;
import rcms.utilities.elogPublisher.ElogPublisher;
import rcms.utilities.hwcfg.HWCFGInterface;
import rcms.utilities.runinfo.RunInfo;
import rcms.utilities.runinfo.RunInfoException;

public class MyUtil {



	/**
	 * <code>logger</code>: log4j logger.
	 */
	static RCMSLogger logger = new RCMSLogger(MyUtil.class);

	private MyFunctionManager functionManager = null;

	private final BlockingQueue<Vector<Object>> publisherQueue = new ArrayBlockingQueue<Vector<Object>>(2000);

	private Thread _pubThread = null;

	private Runnable _publisher = null;

	public MyUtil(MyFunctionManager functionManager) {
		super();
		this.functionManager = functionManager;

		// start publisher thread
		_publisher = new PublishConsumer(publisherQueue);
		_pubThread  = new Thread(_publisher);
		_pubThread.start();

	}

	/**
	 * Hide default constructor
	 */
	protected MyUtil() {
		super();
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#finalize()
	 */
	@Override
	protected void finalize() throws Throwable {
		super.finalize();
		destroy();
	}
	/**
	 * Put a shutdown event on the queue to close the publisher thread.
	 */
	public void destroy() throws InterruptedException {
		Vector v = new Vector();
		v.add(new ShutDownPublisher());
		publisherQueue.add(v);
	}
	/**
	 * @return Returns the xml string of element "ElementName".
	 * If not found return empty string.
	 */
	static public String getXmlRscConf(String xmlRscConf, String elementName) {

		// response string
		String response = "";

		// check the _xmlRscConf and _documentConf are filled
		if (xmlRscConf == null || xmlRscConf.equals("") ) 
			return response;

		// check for a valid argument
		if (elementName == null || elementName.equals("") ) 
			return response;
		int beginIndex = xmlRscConf.indexOf("<"+elementName+">") + elementName.length() + 2;
		int endIndex   = xmlRscConf.indexOf("</"+elementName+">");
		if (beginIndex<0 || endIndex<0) return response;
		response = xmlRscConf.substring(beginIndex, endIndex);

		return response;
	}

	/**
	 * @param ms
	 *       wait before resuming the thread.
	 */
	static public void wait(int ms) {

		try {
			Thread.sleep(ms);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}

	}


	public String myGetRscConfElement(String elementName) {

		// get the FM's resource configuration
		String myConfig = ((FunctionManagerResource) (
				functionManager.getGroup().getThisResource()
		)).getUserXml();


		// get element value		
		String elementValue = MyUtil.getXmlRscConf(myConfig, elementName);

		return elementValue;
	}

	public void prepareGlobalConfigurationKey() {

	}

	public void prepareParametersForInitialized() throws Exception {
		// check for HLT key
	}

    /*
	public void updateRunKeys() {

	}
	
	private boolean checkFedConsistency() {}

    public boolean ttsStatusActive() {}
    */
		  // check status
		  // convention is 
		  // bit mask for fed status is
		  // bit     1  /  0
	  	  // 0 SLINK ON / OFF
	   	  // 1 TTC   ON / OFF
		  // 2 & 0 SLINK NA / BROKEN
		  // 3 & 1 TTC   NA / BROKEN 
		  // 4 NO CONTROL
		  // 5 FMM status bit 0 
		  // 6 FMM status bit 1
		  // 7 FMM status bit 2
		  // 8 FMM status bit 3
		  // 9 SLINK backpressure status OFF
		  //10 SLINK backpressure status ON
	
	
	/**
	 * Set HWCFG_KEY and HWCFG_TRG_NAME parameters
	 */
	public void prepareHwcfgKey() {
		// get hwcfg element

	}

	private void setHwcfgKeyFromDaq() {

		// check daq
	}

	public void elogPublishStart() {

	}

	public void elogPublishStop() {

	}

	public void publishMyRunInfo(Parameter parameter) {
	
	}

	public void publishMyRunInfoWithHistory(FunctionManagerParameter parameter) {
	
	}

	public void publishMyRunInfoWithHistory(String name, String value) {
	
	}

	public void publishMyRunInfo(String name, String value) {
	
	}

	public void publishDynamicFlashList() {
	
	}
	public void publishStaticFlashList() {

	}
	private void addParameterToFlashlist(String name, FlashList l) {

	}
	private void addParameterToFlashlist(String name, String value, FlashList l) {

	}
	private void addTimestampToFlashlist(FlashList l) {

	}
	private void addTimestampToFlashlist(String name, Date value, FlashList l) {

	}
	private void sendFlashList(XMASMessage message) {


	}

	class ShutDownPublisher {	
	}

	/**
	 * @author alexanderoh
	 *
	 * Helper to publish runinfo and wse asynchronously.
	 * Request to publish are put on a queue and
	 * a worker thread publishes.
	 */
	class PublishConsumer implements Runnable {

		BlockingQueue<Vector> queue;

		boolean _shutdown = false;

		PublishConsumer(BlockingQueue q) { queue = q; }

		public void run() {
		}

		void consume(Object x) { 

	}

	public synchronized void publishMyRunInfoImplQueued(Parameter parameter, boolean withHistory) {

	}

	public synchronized void publishMyRunInfoImpl(Parameter parameter, boolean withHistory) {


	}

	public void setParameter(FunctionManagerParameter<StringT> parameter , String value ) {
		if (value==null) value="";
		parameter.setValue( new StringT(value));
		functionManager.
		getParameterSet().
		put(parameter);

	}

	public void setParameter(FunctionManagerParameter<DoubleT> parameter , double value ) {
		parameter.setValue( new DoubleT(value));
		functionManager.
		getParameterSet().
		put(parameter);
	}

	public void setParameter(FunctionManagerParameter<IntegerT> parameter , Integer value ) {
		if (value==null) value = new Integer(0);
		parameter.setValue( new IntegerT(value));
		functionManager.
		getParameterSet().
		put(parameter);
	}


	/**
	 * @param url
	 * @return
	 * @throws Exception
	 */
	protected String getWebPage(String url) throws Exception {
		URL theUrl = new URL(url);
		URLConnection theUrlConnection = theUrl.openConnection();
		BufferedReader in = new BufferedReader(new InputStreamReader(
				theUrlConnection.getInputStream()));

		String inputLine="";
		String result = "";

		while ((inputLine = in.readLine()) != null) {
			result += inputLine;
			result += "\n";
		}

		in.close();

		return result;
	}


	private void pollXdaqMonitorFMM() {



	}

	private void pollXdaqMonitorFRL() {



	}

	public void writeLastRunInfo() {

	}

	public void writeRunInfo(){

	}



	public boolean stateMatch(List<QualifiedResource> lqr, String targetStateString) {

		for ( QualifiedResource qr : lqr ) {
			if (!(qr.getCacheState().getStateString().equals(targetStateString))) {
				return false;
			}
		}

		return true;

	}

	/**
	 * find participating ttc partitions and return a
	 * function manager parameter TTC_PARTITIONS
	 * @param fedString
	 */

	

	    public void setDaqParameters() {

		// check that the containers are correctly populated 


	}

	    public void renderMainGui() {
		GuiCommandPanel guiCommand = new GuiCommandPanel(functionManager);
		functionManager
		.getParameterSet()
		.get(MyParameters.GUI_COMMAND_PANEL_HTML)
		.setValue(new StringT( guiCommand.generateHtml() ));

		GuiStatePanel guiState = new GuiStatePanel(functionManager);
		functionManager
		.getParameterSet()
		.get(MyParameters.GUI_STATE_PANEL_HTML)
		.setValue(new StringT( guiState.generateHtml() ));

		GuiSubdetPanel guiSubdet = new GuiSubdetPanel(functionManager);
		functionManager
		.getParameterSet()
		.get(MyParameters.GUI_SUBDET_PANEL_HTML)
		.setValue(new StringT( guiSubdet.generateHtml() ));

	    /*	
		GuiSubdetMsgPanel guiMsgSubdet = new GuiSubdetMsgPanel(functionManager);
		functionManager
		.getParameterSet()
		.get(MyParameters.GUI_SUBDET_MSG_PANEL_HTML)
		.setValue(new StringT( guiMsgSubdet.generateHtml() ));
	    */			
}

	public void renderSubdetPanel() {
		GuiSubdetPanel guiSubdet = new GuiSubdetPanel(functionManager);
		functionManager
		.getParameterSet()
		.get(MyParameters.GUI_SUBDET_PANEL_HTML)
		.setValue(new StringT( guiSubdet.generateHtml() ));

	}
	public void renderFedTtsGui() {
	    /*
		GuiFedTtsPanel guiFedTtsPanel = new GuiFedTtsPanel(functionManager);
		functionManager
		.getParameterSet()
		.get(MyParameters.GUI_FED_TTS_PANEL_HTML)
		.setValue(new StringT( guiFedTtsPanel.generateHtml() ));
	    */
	}
	public void renderTtsTesterGui() {
	    /*
		GuiTtsTestPanel guiTtsTestPanel = new GuiTtsTestPanel(functionManager);
		functionManager
		.getParameterSet()
		.get(MyParameters.GUI_TTSTEST_PANEL_HTML)
		.setValue(new StringT( guiTtsTestPanel.generateHtml() ));
	    */	
	}
	}
}

