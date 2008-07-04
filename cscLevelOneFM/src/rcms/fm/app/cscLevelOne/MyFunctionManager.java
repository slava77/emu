package rcms.fm.app.cscLevelOne;

import java.math.BigInteger;
import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collection;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TimeZone;

import rcms.fm.context.RCMSConstants;
import rcms.fm.fw.parameter.CommandParameter;
import rcms.fm.fw.parameter.FunctionManagerParameter;
import rcms.fm.fw.parameter.Parameter;
import rcms.fm.fw.parameter.ParameterException;
import rcms.fm.fw.parameter.ParameterSet;
import rcms.fm.fw.parameter.type.DateT;
import rcms.fm.fw.parameter.type.IntegerT;
import rcms.fm.fw.parameter.type.StringT;
import rcms.fm.fw.user.UserActionException;
import rcms.fm.fw.user.UserFunctionManager;
import rcms.fm.app.cscLevelOne.util.MyUtil;
import rcms.fm.resource.CommandException;
import rcms.fm.resource.QualifiedGroup;
import rcms.fm.resource.QualifiedResource;
import rcms.fm.resource.QualifiedResourceContainer;
import rcms.fm.resource.StateVector;
import rcms.fm.resource.StateVectorCalculation;
import rcms.fm.resource.qualifiedresource.FunctionManager;
import rcms.fm.resource.qualifiedresource.JobControl;
import rcms.fm.resource.qualifiedresource.Subsystem;
import rcms.fm.resource.qualifiedresource.XdaqApplicationContainer;
import rcms.fm.resource.qualifiedresource.XdaqExecutive;

import rcms.ns.event.NotificationEvent;
import rcms.ns.utils.NotificationHelper;
import rcms.ns.utils.NotificationParameterContainer;

import rcms.statemachine.definition.Input;
import rcms.statemachine.definition.State;
import rcms.statemachine.definition.StateMachineDefinitionException;

import rcms.util.logger.RCMSLogger;
import rcms.util.logsession.LogSessionConnector;
import rcms.util.logsession.LogSessionException;

import rcms.utilities.elogPublisher.ElogPublisher;
import rcms.utilities.hwcfg.HWCFGInterface;
import rcms.utilities.hwcfg.HardwareConfigurationException;
import rcms.utilities.hwcfg.InvalidNodeTypeException;
import rcms.utilities.hwcfg.PathNotFoundException;
import rcms.utilities.hwcfg.dp.DAQSlice;
import rcms.utilities.hwcfg.dp.DAQSliceGenericHost;
import rcms.utilities.hwcfg.dp.FEDWithConnectivity;
import rcms.utilities.hwcfg.dp.RU;
import rcms.utilities.hwcfg.eq.EquipmentSet;
import rcms.utilities.hwcfg.eq.FMMFMMLink;
import rcms.utilities.hwcfg.eq.FMMTriggerLink;
import rcms.utilities.runinfo.RunInfo;
import rcms.utilities.runinfo.RunNumberData;



/**
 * Example of Function Machine for controlling an Level 1 Function Manager.
 * 
 * @author Andrea Petrucci, Alexander Oh, Michele Gulmini
 */
public class MyFunctionManager extends UserFunctionManager {

	/**
	 * <code>RCMSLogger</code>: RCMS log4j Logger
	 */
	static RCMSLogger logger = new RCMSLogger(MyFunctionManager.class);

	/**
	 * define some containers
	 */
	public XdaqApplicationContainer containerXdaqApplication = null;

	/**
	 * define specific application containers
	 */
	public XdaqApplicationContainer cEVM = null;

	/**
	 * <code>containerXdaqExecutive</code>: container of XdaqExecutive in the
	 * running Group.
	 */
	public XdaqApplicationContainer containerXdaqExecutive = null;

	/**
	 * <code>containerFunctionManager</code>: container of FunctionManagers
	 * in the running Group.
	 */
	public QualifiedResourceContainer containerFunctionManager = null;

	/**
	 * <code>containerJobControl</code>: container of JobControl in the
	 * running Group.
	 */
	public QualifiedResourceContainer containerJobControl = null;

	/**
	 * <code>calcState</code>: Calculated State.
	 */
	public State calcState = null;

	public XdaqApplicationContainer xdaqSupervisor = null;

	// utlitity functions handle
	public MyUtil _myUtil;

	/**
	 * Instantiates an MyFunctionManager.
	 */
	public MyFunctionManager() {
		//
		// Any State Machine Implementation must provide the framework
		// with some information about itself.
		//

		// make the parameters available
		addParameters();

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see rcms.statemachine.user.UserStateMachine#createAction()
	 */
	public void createAction(ParameterSet<CommandParameter> pars) throws UserActionException {
		//
		// This method is called by the framework when the Function Manager is
		// created.

		System.out.println("createAction called.");
		logger.debug("createAction called.");

		System.out.println("createAction executed.");
		logger.debug("createAction executed.");

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see rcms.statemachine.user.UserStateMachine#destroyAction()
	 */
	public void destroyAction() throws UserActionException {
		//
		// This method is called by the framework when the Function Manager is
		// destroyed.
		//

		System.out.println("destroyAction called");
		logger.debug("destroyAction called");

		logger.debug("cscFM destroyAction");

		QualifiedGroup group = getQualifiedGroup();

		List<QualifiedResource> list;

		// destroy XDAQ executives
		list = group.seekQualifiedResourcesOfType(new XdaqExecutive());

		for (QualifiedResource r: list) {
			logger.debug("==== killing " + r.getURI());
			try {
				((XdaqExecutive)r).killMe();
			} catch (Exception e) {
				logger.error(
						"Could not destroy a XDAQ executive " + r.getURI(), e);
			}
		}

		// destroy function managers
		list = group.seekQualifiedResourcesOfType(new FunctionManager());

		for (QualifiedResource r: list) {
			logger.debug("==== killing " + r.getURI());

			FunctionManager fm = (FunctionManager)r;

			if (fm.isInitialized()) {
				try {
					fm.destroy();
				} catch (Exception e) {
					logger.error("Could not destroy a FM " + r.getURI(), e);
				}
			}
		}
		
		System.out.println("destroyAction executed");
		logger.debug("destroyAction executed");
	}

	/**
	 * add parameters to parameterSet. After this they are accessible.
	 */
	private void addParameters() {

		// add parameters to parameter Set so they are visible.
		parameterSet = MyParameters.LVL_ONE_PARAMETER_SET;

	}

	public void init() throws StateMachineDefinitionException,
			rcms.fm.fw.EventHandlerException {

		//instantiate utility
		_myUtil = new MyUtil(this);

		//
		// Set first of all the State Machine Definition
		//
		setStateMachineDefinition(new MyStateMachineDefinition());

		//
		// Add event handler
		//
		addEventHandler(new MyEventHandler());

		//
		// Add error handler
		//
		addEventHandler(new MyErrorHandler());
		// call renderers
	       	_myUtil.renderMainGui();



	}
}
