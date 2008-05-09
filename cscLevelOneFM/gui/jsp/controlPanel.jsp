<%@ page language="java" contentType="text/html"%>
<%@ page import="java.util.*"%>
<%@ page import="rcms.gui.servlet.pilot.FMPilotConstants"%>
<%@ page isELIgnored="false"%>

<%@ taglib prefix="rcms.menu" 				uri="../tld/menu.tld"%>
<%@ taglib prefix="rcms.control" 			uri="../tld/control.tld"%>
<%@ taglib prefix="rcms.globalParameter" 	uri="../tld/globalParameter.tld"%>
<%@ taglib prefix="rcms.notification" 		uri="../tld/notification.tld"%>

<!-- Optional Section to set the visibility of available commands at a given state begin -->
<rcms.control:menuCreator/>
<html>

<head>
<meta Http-Equiv="Cache-Control" Content="no-cache">
<meta Http-Equiv="Pragma" Content="no-cache">
<meta Http-Equiv="Expires" Content="0">

<title>CSC</title>

<link rel="StyleSheet" href="../css/common.css" type="text/css" />
<link rel="StyleSheet" href="../css/control.css" type="text/css" />

<rcms.control:customResourceRenderer indentation="1" type="css" path="/css/myControlPanel.css" />
<rcms.control:customResourceRenderer indentation="1" type="js" path="/js/myControl.js" />

<rcms.control:customResourceRenderer indentation="1" type="css"
	path="/css/myControlPanel.css" />

<rcms.control:customResourceRenderer indentation="1" type="js"
	path="/js/myControl.js" />

<rcms.control:customResourceRenderer indentation="1" type="js"
	path="/js/ajaxRequest.js" />

<rcms.control:customResourceRenderer indentation="1" type="customPath"
	path="/jsp/tree.jsp?groupID=" htmlId="treeJsp" />

<rcms.control:customResourceRenderer indentation="1" type="customPath"
	path="/jsp/fedTtsConfig.jsp?groupID=" htmlId="fedTtsConfigJsp" />

<rcms.control:customResourceRenderer indentation="1" type="customPath" 	path="/html/sounds/error.wav" htmlId="errorSound" />
<rcms.control:customResourceRenderer indentation="1" type="customPath" 	path="/html/sounds/stateChanged.wav" htmlId="defaultSound" />
<rcms.control:customResourceRenderer indentation="1" type="customPath" 	path="/html/sounds/onload.wav" htmlId="onloadSound" />
<rcms.control:customResourceRenderer indentation="1" type="customPath" 	path="/html/sounds/destroy.wav" htmlId="destroySound" />

<!--  State sounds -->
<rcms.control:customResourceRenderer indentation="1" type="customPath" path="/html/sounds/destroy.wav" htmlId="InitializedSound" />
<rcms.control:customResourceRenderer indentation="1" type="customPath" path="/html/sounds/stateChanged.wav" htmlId="HaltedSound" />
<rcms.control:customResourceRenderer indentation="1" type="customPath" path="/html/sounds/stateChanged.wav" htmlId="ConfiguredSound" />
<rcms.control:customResourceRenderer indentation="1" type="customPath" path="/html/sounds/stateChanged.wav" htmlId="ReadySound" />
<rcms.control:customResourceRenderer indentation="1" type="customPath" path="/html/sounds/running.wav" htmlId="RunningSound" />
<rcms.control:customResourceRenderer indentation="1" type="customPath" path="/html/sounds/error.wav" htmlId="ErrorSound" />
<rcms.control:customResourceRenderer indentation="1" type="customPath" path="/html/sounds/stateChanged.wav" htmlId="PausedSound" />

<script type="text/javascript" src="../js/stateNotification.js"></script>
<script type="text/javascript" src="../js/common.js"></script>
<script type="text/javascript" src="../js/globalParameters.js"></script>
<script type="text/javascript" src="../js/control.js"></script>
<script type="text/javascript" src="../yui/yahoo-dom-event/yahoo-dom-event.js"></script>
<script type="text/javascript" src="../yui/animation/animation-min.js"></script>

<!-- Custom javascript section begin -->
<script type="text/javascript">
		<rcms.control:onLoadJSRenderer reloadOnStateChange="false" 
		commandButtonCssClass="MyControlButton" 
		commandParameterCheckBoxTitle="&nbsp;Show Command Parameter Section"	
		commandParameterCssClass="label_left_black" indentation="2"/>

		<rcms.control:buttonsJSRenderer indentation="2"/>
		<rcms.notification:jSRenderer indentation="2"/>
		<rcms.globalParameter:jSRenderer indentation="2"/>

</script>
</head>
<!-- Custom javascript section end -->

<!-- BODY -->
<body onLoad="myInit();" class="body">
<!--  switch back to a bigger font size -->
<font size=3> <!-- Table T1 begin -->

<p>
<rcms.globalParameter:getParameterMap fmParameterContainer="pars" />
</p>

 <!--conditional dummy span for sound support--> 
	<script type="text/javascript">
	</script>

<h2>CSC "Function Manager" Run Control</h2>

<div class="ParaTableName"> </div>

<form name="FMPilotForm" id="FMPilotForm" method="POST" action="FMPilotServlet?PAGE=/gui/jsp/controlPanel.jsp">

	<rcms.control:actionHiddenInputRenderer indentation="4" /> 
	<rcms.control:commandHiddenInputRenderer indentation="4" />
 	<rcms.notification:hiddenInputRenderer indentation="4" /> 

<table class="HeaderTable" border="0" cellpadding="3" cellspacing="3" width="100%">

	<tr>
	
	<td><button id="showStatusTableButton" class="MenuButton"
			value="Status Table" name="Status Table"
			onClick="onShowStatusTableButton()">Status Table </button>
	</td>
	
	<td class="Legend"><button value="FedTtsConfig"
			id="fedTtsConfig" class="MenuButton" name="FED TTS Configurator"
			onClick="openFedTtsConfig()">FED & TTS</button>
	</td>

	<!--table-->
	<td>
	<div> 	
	<input type="hidden" id="globalParameterName3 "name="globalParameterName3" value="" />
	<input type="hidden" id="globalParameterValue3 "name="globalParameterValue3" value="" />
	<input type="hidden" id="globalParameterType3 "name="globalParameterType3" value="" />
	</td>

<!--
	<tr>
	<td colspan="4">	
	<hr size="4"> 
	</td>
	</tr>
-->
		<td>
		<rcms.control:refreshButtonRenderer cssClass="button1"
				onClickFunction="onUpdatedRefreshButton()" name="Refresh"
				indentation="10" /></td>
				<td>
		<div id="HEARTBEAT">
		<SPACER TYPE=BLOCK HEIGHT=30 WIDTH=30>
		</div>
	</td>
<td>
		<rcms.control:createButtonRenderer cssClass="button1"
						onClickFunction="onCreateButton()" name="Create" indentation="10" />
                
		<rcms.control:attachButtonRenderer cssClass="button1"
						onClickFunction="onAttachButton()" name="Attach" indentation="10" />
		
		<rcms.control:detachButtonRenderer cssClass="button1"
						onClickFunction="onDetachButton()" name="Detach" indentation="10" />
		
		<rcms.control:destroyButtonRenderer cssClass="button1"
						onClickFunction="onDestroyButton()" name="Destroy" indentation="10" />
		</td>

	</tr>
</table>
<p></p>
${pars.GUI_STATE_PANEL_HTML}
<p></p>

<table style="width: 850px;">


	<tr>
	<td>
		<!--  COMMAND PANEL -->
		<div style="border: 1px solid white; width: 430px;" id="GUI_COMMAND_PANEL_HTML">
		${pars.GUI_COMMAND_PANEL_HTML}
		</div>
	</td>
	<td>

<!--  INFO TABLE -->
	<table  style="width: 800px;">

		<td colspan="2" class="paraTableName">
		<rcms.control:configurationPathRenderer titleClass="control_label1" label="Configuration:&nbsp;" contentClass="control_label2" indentation="10"/>
				
		<rcms.control:configurationKeyRenderer titleClass="control_label1" label="Configuration Keys:&nbsp;" contentClass="control_label2" indentation="10"/>

		<!--rcms.control:configurationNameRenderer titleClass="control_label1" label="Group Name:&nbsp;" contentClass="control_label2" indentation="10"/-->
		<br><br>
		<rcms.control:stateRenderer titleClass="control_label1" label="State:&nbsp;" contentClass="control_label3" indentation="10"/>
		</td>
		<td>
		<!--rcms.control:showStatusTableButtonRenderer cssClass="button1" onClickFunction="onShowStatusTableButton()" name="Status Table" indentation="10"/-->
		</td>
	</tr>

	<tr>
		<td align="center" bgcolor="#cccccc">
			<div id="commandSection">
				<rcms.control:commandButtonsRenderer cssClass="button1" indentation="11"/>
			</div>
			<br>
			<div id="commandParameterCheckBoxSection" class="control_label1">
				<!--rcms.control:commandParameterCheckboxRenderer title="&nbsp;Show Command Parameter Section" indentation="11"/-->
			</div>
		</td>
	</tr>

	<tr>
	   <td colspan="3">
		<div id="HEARTBEAT" > <SPACER TYPE=BLOCK HEIGHT=10 WIDTH=10> </div> 
	   </td>
	</tr>

	<tr>
	
	</tr>
	
	
	<tr>
	<td colspan="3">	
	<hr size="3"> 
	</td>
	</tr>
					
	</table>

		
		<tr>
			<td style="width: 120px;" class="paraTableName">SID</td>
			<td style="width: 400px;" class="paraTableValue" id="SID" colspan="5">${pars.SID}</td>
		</tr>

		<tr>
			<td style="width: 120px;" class="paraTableName">Run Number</td>
			<td style="width: 400px;" class="paraTableValue" id="RUN_NUMBER" colspan="2">${pars.RUN_NUMBER}</td>

		</tr>
	
		<tr>
			<td style="width: 110px;"  class="paraTableName">Global Key</td>
			<td style="width: 400px;" class="paraTableValue" id="GLOBAL_CONF_KEY" colspan="5">${pars.GLOBAL_CONF_KEY}</td>
		</tr>
			
		<tr>
			<td style="width: 110px;"  class="paraTableName">Action</td>
			<td style="width: 400px;" class="paraTableValue" id="ACTION_MSG" colspan="5">${pars.ACTION_MSG}</td>
		</tr>
	
		<tr>
			<td style="width: 110px;"  class="paraTableName">Error</td>
			<td style="width: 400px;" class="paraTableValue" id="ERROR_MSG" colspan="5">${pars.ERROR_MSG}</td>
		</tr>
</table>

<table  style="width: 800px;">

            <td><a class="MenuLinkEnabled" href="./DiagnosticServlet">Diagnostic Page</a></td>

            <td><a class="MenuLinkEnabled" href="../../../Collector/Collector">Logging Collector</a></td>

            <td><a class="MenuLinkEnabled" href="./MonitoringToolsServlet">Monitoring Tools</a></td>

            <td><a class="MenuLinkEnabled" href="./RunInfoServlet">Run Info</a></td>

            <td><a class="MenuLinkEnabled" href="./RunningConfigurationServlet">Running Configurations</a></td>

            <td><a class="MenuLinkEnabled" href="./LogoutServlet">Logout</a></td>

	
	</table>
	</td>
	</tr>
</table>


	<!-- Control Console end -->
		

<!--  my private stuff goes here --> <!--  embedded css -->


 <!-- MY HIDDEN PARAMETERS -->
</form>
	<div style="color: black; font-family: monospace;">
	
	<script type="text/javascript">
	<!--
	document.write(' ' + BrowserDetect.browser + ' ' + BrowserDetect.version + ' ' + BrowserDetect.OS );
	// -->
	</script>

</body>
</html>

