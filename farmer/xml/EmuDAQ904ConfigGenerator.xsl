<?xml version="1.0"?>
<!-- This XSL transformation is used to generate XDAQ config file for EmuDAQ from a RUI-to-computer mapping. -->
<!-- Usage example:  -->
<!--     xsltproc <hyphen><hyphen>stringparam WRITE Y <hyphen><hyphen>stringparam BUILD N EmuDAQ904ConfigGenerator.xsl RUI-to-computer_mapping_bdg904.xml > DAQ.xml -->
<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  version="1.0" xmlns:xs="http://www.w3.org/2001/XMLSchema"
  xmlns:xc="http://xdaq.web.cern.ch/xdaq/xsd/2004/XMLConfiguration-30"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
  xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">

  <!-- Parameter SIDE is to be optionally set from the command line. -->
  <!-- If it's set to 'M', only the minus side will be generated; if 'P', only the plus side; if 'B', both sides; otherwise both sides. -->
<!--   <xsl:param name="SIDE"/> -->
  <!-- Parameter WRITE is to be set from the command line. -->
  <!-- If it is set to 'Y', the RUIs will write data to files; if 'N', they will not. -->
  <xsl:param name="WRITE">N</xsl:param>
  <!-- Parameter BUILD is to be set from the command line. -->
  <!-- If it is set to 'Y', events will be built; if 'N', events will not be built. -->
  <xsl:param name="BUILD">N</xsl:param>
  <!-- Parameter FARM is to be set from the command line. -->
  <!-- It's either DQM_TF_File or empty-->
  <xsl:param name="FARM"/>

  <xsl:param name="PORT_OFFSET">
    <xsl:if test="$FARM!='DQM_TF_File'">0</xsl:if>
    <xsl:if test="$FARM='DQM_TF_File'">10</xsl:if>
  </xsl:param>

  <xsl:output method="xml" indent="yes"/>

<!-- Generate config file for EmuDAQ -->
  <xsl:template match="RUI-to-computer_mapping">
    <xsl:processing-instruction name="xml-stylesheet" >type="text/xml" href="EmuDAQConfig.xsl"</xsl:processing-instruction>

    <xc:Partition xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
		  xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/" 
		  xmlns:xc="http://xdaq.web.cern.ch/xdaq/xsd/2004/XMLConfiguration-30">
      <xsl:comment >Generated from RUI<xsl:if test="$FARM='DQM_TF_File'">0</xsl:if>-to-computer_mapping of <xsl:value-of select="/RUI-to-computer_mapping/@date"/></xsl:comment>

      <xsl:call-template name="i2o_protocol"/>
      <xsl:call-template name="DAQManager"/>
      <xsl:call-template name="EVM_and_TA"/>
      <xsl:call-template name="RUIs"/>
      <xsl:call-template name="EmuDisplayClient"/>
      <xsl:call-template name="EmuTFDisplayClient"/>
      <xsl:call-template name="EmuMonitors"/>
      <xsl:call-template name="EmuTFMonitor"/>
<!--       <xsl:call-template name="TTCciControl"/> -->

    </xc:Partition>
  </xsl:template>

<!-- Generate I2O protocol target id mapping -->
  <xsl:template name="i2o_protocol">
    <i2o:protocol xmlns:i2o="http://xdaq.web.cern.ch/xdaq/xsd/2004/I2OConfiguration-30">

      <i2o:target tid="2000" instance="0" class="rubuilder::evm::Application"></i2o:target>
      <i2o:target tid="2001" instance="0" class="emu::daq::ta::Application"></i2o:target>

<!--       <i2o:target tid="1450" instance="0" class="EmuDisplayClient"></i2o:target> -->

      <xsl:if test="//RUI[@instance='0']">
	<xsl:comment>RUI 0 (TF)</xsl:comment>
	<i2o:target tid="1000" instance="0" class="emu::daq::rui::Application"></i2o:target>
	<i2o:target tid="1001" instance="0" class="rubuilder::ru::Application"></i2o:target>
	<i2o:target tid="1400" instance="0" class="EmuTFMonitor"></i2o:target>
      </xsl:if>

      <xsl:for-each select="//RUI[@instance!='0']">      
<!-- 	<xsl:if test="($SIDE!='P' and $SIDE!='M') or $SIDE='B' or ($SIDE='P' and number(@instance)&lt;=18) or ($SIDE='M' and number(@instance)&gt;18)"> -->

	  <xsl:comment >RUI <xsl:value-of select="@instance"/></xsl:comment>

	  <xsl:variable name="DAQ_TID0"><xsl:value-of select="1000+10*number(@instance)+0"/></xsl:variable>
	  <xsl:variable name="DAQ_TID1"><xsl:value-of select="1000+10*number(@instance)+1"/></xsl:variable>
	  <xsl:variable name="DAQ_TID2"><xsl:value-of select="1000+10*number(@instance)+2"/></xsl:variable>
	  <xsl:variable name="DAQ_TID3"><xsl:value-of select="1000+10*number(@instance)+3"/></xsl:variable>
	  <i2o:target tid="{$DAQ_TID0}" instance="{@instance}" class="emu::daq::rui::Application"></i2o:target>
	  <i2o:target tid="{$DAQ_TID1}" instance="{@instance}" class="rubuilder::ru::Application"></i2o:target>
	  <i2o:target tid="{$DAQ_TID2}" instance="{@instance}" class="rubuilder::bu::Application"></i2o:target>
	  <i2o:target tid="{$DAQ_TID3}" instance="{@instance}" class="emu::daq::fu::Application"></i2o:target>

	  <xsl:variable name="DQM_TID"><xsl:value-of select="1400+number(@instance)"/></xsl:variable>
	  <i2o:target tid="{$DQM_TID}" instance="{@instance}" class="EmuMonitor"></i2o:target>

<!-- 	</xsl:if> -->
      </xsl:for-each>
    </i2o:protocol>
  </xsl:template>

<!-- Generate contexts for EmuDAQManager -->
  <xsl:template name="DAQManager">
    <xsl:comment>emu::daq::manager</xsl:comment>
    <xsl:variable name="HTTP_PORT" select="20200+number($PORT_OFFSET)"/>
    <xsl:variable name="I2O_PORT"><xsl:value-of select="number($HTTP_PORT)+50"/></xsl:variable>
    <xc:Context url="http://emudaq02.cern.ch:{$HTTP_PORT}">
      <xc:Module>${XDAQ_ROOT}/lib/libptatcp.so</xc:Module>
      <xc:Module>${XDAQ_ROOT}/lib/libxdaq2rc.so</xc:Module>
      <xc:Application instance="0" class="emu::daq::manager::Application" network="local" id="12" service="emudaqmanager">
	<properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:emu::daq::manager::Application">
	  <runTypes xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[24]">
	    <item xsi:type="xsd:string" soapenc:position="[0]">Monitor</item>
	    <item xsi:type="xsd:string" soapenc:position="[1]">SCA_Pedestals</item>
	    <item xsi:type="xsd:string" soapenc:position="[2]">Comparator_Thresh</item>
	    <item xsi:type="xsd:string" soapenc:position="[3]">Comparator_Spread</item>
	    <item xsi:type="xsd:string" soapenc:position="[4]">Comparator_LeftRight</item>
	    <item xsi:type="xsd:string" soapenc:position="[5]">Comparator_TimeOffset</item>
	    <item xsi:type="xsd:string" soapenc:position="[6]">Comparator_Rate</item>
	    <item xsi:type="xsd:string" soapenc:position="[7]">CFEB_Saturation</item>
	    <item xsi:type="xsd:string" soapenc:position="[8]">CFEB_Connectivity</item>
	    <item xsi:type="xsd:string" soapenc:position="[9]">CFEB_CompLogic</item>
	    <item xsi:type="xsd:string" soapenc:position="[10]">CFEB_CompThreshDAC</item>
	    <item xsi:type="xsd:string" soapenc:position="[11]">CFEB_PulseDAC</item>
	    <item xsi:type="xsd:string" soapenc:position="[12]">CFEB_NoiseMatrix</item>
	    <item xsi:type="xsd:string" soapenc:position="[13]">CFEB_Crosstalk</item>
	    <item xsi:type="xsd:string" soapenc:position="[14]">CFEB_Gains</item>
	    <item xsi:type="xsd:string" soapenc:position="[15]">AFEB_DAC</item>
	    <item xsi:type="xsd:string" soapenc:position="[16]">AFEB_TimeDelay</item>
	    <item xsi:type="xsd:string" soapenc:position="[17]">AFEB_NoiseRate</item>
	    <item xsi:type="xsd:string" soapenc:position="[18]">AFEB_Connectivity</item>
	    <item xsi:type="xsd:string" soapenc:position="[19]">AFEB_ALCT</item>
	    <item xsi:type="xsd:string" soapenc:position="[20]">AFEB_Pulse</item>
	    <item xsi:type="xsd:string" soapenc:position="[21]">HV_Scan</item>
	    <item xsi:type="xsd:string" soapenc:position="[22]">STEP</item>
	    <item xsi:type="xsd:string" soapenc:position="[23]">Debug</item>
	  </runTypes>
	  <buildEvents xsi:type="xsd:boolean"><xsl:if test="$BUILD='N'">false</xsl:if><xsl:if test="$BUILD='Y'">true</xsl:if></buildEvents>
	  <runType xsi:type="xsd:string">Monitor</runType>
	  <supervisedMode xsi:type="xsd:boolean">true</supervisedMode>
	  <runDbBookingCommand xsi:type="xsd:string">/nfshome0/cscdaq/bin/java -jar /nfshome0/cscdaq/bin/runnumberbooker.jar</runDbBookingCommand>
	  <runDbWritingCommand xsi:type="xsd:string">/nfshome0/cscdaq/bin/java -jar /nfshome0/cscdaq/bin/runinfowriter.jar</runDbWritingCommand>
	  <runDbAddress xsi:type="xsd:string">jdbc:oracle:thin:@cmsonr1-v.cms:10121/cms_rcms.cern.ch</runDbAddress>
<!-- 	  <runDbUserFile xsi:type="xsd:string">/nfshome0/cscdaq/config/.runDbUser</runDbUserFile> -->
	  <runDbUserFile xsi:type="xsd:string">/nfshome0/cscdaq/config/.runDbTestUser</runDbUserFile>
	  <!-- <runDbUserFile xsi:type="xsd:string">/nfshome0/cscdaq/config/.runDbMTCCUser</runDbUserFile> -->
	  <curlCommand xsi:type="xsd:string">/usr/bin/curl -v -k</curlCommand>
	  <curlCookies xsi:type="xsd:string">.curlCookies</curlCookies>
	  <CMSUserFile xsi:type="xsd:string">/nfshome0/cscdaq/config/.CMSUser</CMSUserFile>
	  <eLogUserFile xsi:type="xsd:string">/nfshome0/cscdaq/config/.eLogUser</eLogUserFile>
<!-- 	  <eLogURL xsi:type="xsd:string">https://cmsdaq.cern.ch/elog/CSC</eLogURL> -->
	  <eLogURL xsi:type="xsd:string">https://cmsdaq.cern.ch/elog/Test</eLogURL>
	  <postToELog xsi:type="xsd:boolean">true</postToELog>
	  <controlDQM xsi:type="xsd:boolean">false</controlDQM>
	  <hardwareMapping xsi:type="xsd:string">emu/config/EmuDAQ/RUI-to-chamber_mapping_bdg904.xml</hardwareMapping>
	  <TF_FM_URL xsi:type="xsd:string">http://UNKNOWN.cms:12000</TF_FM_URL>
	  <CSC_FM_URL xsi:type="xsd:string">http://emudaq02.cern.ch:12000</CSC_FM_URL>
	  <RegexMatchingTFConfigName xsi:type="xsd:string">UNKNOWN</RegexMatchingTFConfigName>
	  <RegexMatchingCSCConfigName xsi:type="xsd:string">.*/Local/.*|.*/Global/.*</RegexMatchingCSCConfigName>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/lib/libtstoreclient.so</xc:Module>
      <xc:Module>${XDAQ_ROOT}/lib/libemubase.so</xc:Module>
      <xc:Module>${XDAQ_ROOT}/lib/libemudaqmanager.so</xc:Module>
    </xc:Context>
  </xsl:template>
  


<!-- Generate contexts for EVM and EmuTA -->
  <xsl:template name="EVM_and_TA">
    <xsl:comment >rubuilder::evm and emu::daq::ta</xsl:comment>
    <xsl:variable name="HTTP_PORT" select="20201+number($PORT_OFFSET)"/>
    <xsl:variable name="I2O_PORT"><xsl:value-of select="number($HTTP_PORT)+50"/></xsl:variable>
    <xc:Context url="http://emudaq02.cern.ch:{$HTTP_PORT}">
      <xc:Endpoint hostname="emudaq02.cern.ch" protocol="atcp" port="{$I2O_PORT}" service="i2o" network="atcp1"></xc:Endpoint>
      <xc:Module>${XDAQ_ROOT}/lib/libxdaq2rc.so</xc:Module>
      <xc:Application instance="0" network="atcp1" class="pt::atcp::PeerTransportATCP" id="31">
	<properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:pt::atcp::PeerTransportATCP">
	  <autoSize xsi:type="xsd:boolean">true</autoSize>
	  <maxPacketSize xsi:type="xsd:unsignedInt">196608</maxPacketSize>
	  <connectAtFirstSend xsi:type="xsd:boolean">true</connectAtFirstSend>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/lib/libptatcp.so</xc:Module>
      <xc:Application instance="0" class="rubuilder::evm::Application" network="atcp1" id="13">
	<properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:rubuilder::evm::Application">
	  <taClass xsi:type="xsd:string">emu::daq::ta::Application</taClass>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/lib/librubuilderutils.so</xc:Module>
      <xc:Module>${XDAQ_ROOT}/lib/librubuilderevm.so</xc:Module>
      <xc:Application instance="0" class="emu::daq::ta::Application" network="atcp1" id="17"/>
      <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libemudaqta.so</xc:Module>
    </xc:Context>
  </xsl:template>



<!-- Generate contexts for RUIs -->
  <xsl:template name="RUIs">
    <xsl:for-each select="//RUI">

	<xsl:variable name="HTTP_HOST"><xsl:value-of select="../@alias"/></xsl:variable>
	<xsl:variable name="HTTP_PORT"><xsl:value-of select="@port"/></xsl:variable>
	<xsl:variable name="I2O_HOST"><xsl:value-of select="../@alias"/></xsl:variable>
	<xsl:variable name="I2O_PORT"><xsl:value-of select="number(@port)+50"/></xsl:variable>

	<xsl:comment >RUI <xsl:value-of select="@instance"/></xsl:comment>
	
	<xc:Context url="http://{$HTTP_HOST}:{$HTTP_PORT}">
	  <xc:Endpoint hostname="{$I2O_HOST}" protocol="atcp" port="{$I2O_PORT}" service="i2o" network="atcp1"></xc:Endpoint>
	  <xc:Module>${XDAQ_ROOT}/lib/libxdaq2rc.so</xc:Module>
	  <xc:Application instance="0" network="atcp1" class="pt::atcp::PeerTransportATCP" id="31">
	    <properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:pt::atcp::PeerTransportATCP">
	      <autoSize xsi:type="xsd:boolean">true</autoSize>
	      <maxPacketSize xsi:type="xsd:unsignedInt">196608</maxPacketSize>
	      <connectAtFirstSend xsi:type="xsd:boolean">true</connectAtFirstSend>
	    </properties>
	  </xc:Application>
	  <xc:Module>${XDAQ_ROOT}/lib/libptatcp.so</xc:Module>
	  <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libemudaqreader.so</xc:Module>
	  <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libemudaqserver.so</xc:Module>
	  <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libemudaqwriter.so</xc:Module>
	  <xc:Application instance="{@instance}" class="rubuilder::ru::Application" network="atcp1" id="14">
	    <properties xmlns="urn:xdaq-application:rubuilder::ru::Application" xsi:type="soapenc:Struct">
	      <tolerateCSCFaults xsi:type="xsd:boolean">true</tolerateCSCFaults>
	    </properties>
	  </xc:Application>
	  <xc:Module>${XDAQ_ROOT}/lib/librubuilderutils.so</xc:Module>
	  <xc:Module>${XDAQ_ROOT}/lib/librubuilderru.so</xc:Module>
	  <xc:Application instance="{@instance}" class="rubuilder::bu::Application" network="atcp1" id="15"></xc:Application>
	  <xc:Module>${XDAQ_ROOT}/lib/librubuilderbu.so</xc:Module>
	  <xc:Application instance="{@instance}" class="emu::daq::fu::Application" network="atcp1" id="16">
	    <properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:emu::daq::fu::Application">
	      <buInstNb xsi:type="xsd:unsignedLong"><xsl:value-of select="@instance"/></buInstNb>
	      <pathToDataOutFile xsi:type="xsd:string">/data</pathToDataOutFile>
	      <fileSizeInMegaBytes xsi:type="xsd:unsignedLong">200</fileSizeInMegaBytes>
	      <clientsClassName xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[5]"></clientsClassName>
	    </properties>
	  </xc:Application>
	  <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libemudaqfu.so</xc:Module>
	  <xc:Application instance="{@instance}" class="emu::daq::rui::Application" network="atcp1" id="18">
	    <properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:emu::daq::rui::Application">
	      <passDataOnToRUBuilder xsi:type="xsd:boolean">false</passDataOnToRUBuilder>
	      <xsl:choose>
		<xsl:when test="$FARM='DQM_TF_File'">
		  <inputDeviceType xsi:type="xsd:string">file</inputDeviceType>
		  <inputDeviceName xsi:type="xsd:string">/home/cscdev/config/dqm/TF_DDU_data.raw</inputDeviceName>
		</xsl:when>
		<xsl:otherwise>
		  <inputDeviceType xsi:type="xsd:string">spy</inputDeviceType>
		  <inputDeviceName xsi:type="xsd:string">/dev/schar<xsl:value-of select="@schar"/></inputDeviceName>
		</xsl:otherwise>
	      </xsl:choose>
	      <hardwareMnemonic xsi:type="xsd:string"><xsl:if test="@instance='0'">TF</xsl:if><xsl:if test="@instance!='0'">chambers</xsl:if></hardwareMnemonic>
	      <inputDataFormat xsi:type="xsd:string">DDU</inputDataFormat>
	      <pathToRUIDataOutFile xsi:type="xsd:string">/data</pathToRUIDataOutFile>
	      <pathToBadEventsFile xsi:type="xsd:string"></pathToBadEventsFile>
	      <ruiFileSizeInMegaBytes xsi:type="xsd:unsignedLong"><xsl:if test="$WRITE='N'">0</xsl:if><xsl:if test="$WRITE='Y'">200</xsl:if></ruiFileSizeInMegaBytes>
	      <clientsClassName xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[5]">
		<item xsi:type="xsd:string" soapenc:position="[0]"><xsl:if test="@instance='0'">EmuTFMonitor</xsl:if><xsl:if test="@instance!='0'">EmuMonitor</xsl:if></item>
	      </clientsClassName>
	      <clientsInstance xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[5]">
		<item xsi:type="xsd:unsignedLong" soapenc:position="[0]"><xsl:value-of select="@instance"/></item>
	      </clientsInstance>
	      <poolSizeForClient xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[5]">
		<item xsi:type="xsd:unsignedLong" soapenc:position="[0]">0x4000000</item>
	      </poolSizeForClient>
	    </properties>
	  </xc:Application>
	  <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libemudaqrui.so</xc:Module>
	</xc:Context>

    </xsl:for-each>
  </xsl:template>

<!-- Generate context for EmuDisplayClient -->
  <xsl:template name="EmuDisplayClient">
    <xsl:comment >EmuDisplayClient</xsl:comment>
    <xc:Context url="http://emudaq02.cern.ch:20550">
      <xc:Application class="EmuDisplayClient" id="1450" instance="0" network="local">
	<properties xmlns="urn:xdaq-application:EmuDisplayClient" xsi:type="soapenc:Struct">
	<monitorClass xsi:type="xsd:string">EmuMonitor</monitorClass>
        <imageFormat xsi:type="xsd:string">png</imageFormat>
        <baseDir xsi:type="xsd:string">/home/cscdev/config/dqm/cscdqm</baseDir>
	<resultsDir xsi:type="xsd:string">/data/dqm/online</resultsDir>
        <viewOnly xsi:type="xsd:boolean">false</viewOnly>
        <xmlCfgFile xsi:type="xsd:string">/home/cscdev/config/dqm/emuDQMBooking.xml</xmlCfgFile>
        <xmlCanvasesCfgFile xsi:type="xsd:string">/home/cscdev/config/dqm/emuDQMCanvases.xml</xmlCanvasesCfgFile>
        <cscMapFile xsi:type="xsd:string">/home/cscdev/config/dqm/csc_map.txt</cscMapFile>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/lib/libtstoreclient.so</xc:Module>
      <xc:Module>${XDAQ_ROOT}/lib/libemubase.so</xc:Module>
      <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libemudqmcscdisplay.so</xc:Module>
    </xc:Context>
  </xsl:template>

<!-- Generate context for EmuTFDisplayClient -->
  <xsl:template name="EmuTFDisplayClient">
    <xsl:comment >EmuTFDisplayClient</xsl:comment>
    <xc:Context url="http://emudaq02.cern.ch:20570">
      <xc:Application class="EmuTFDisplayClient" id="1450" instance="0" network="local">
	<properties xmlns="urn:xdaq-application:EmuTFDisplayClient" xsi:type="soapenc:Struct">
	  <monitorClass xsi:type="xsd:string">EmuTFMonitor</monitorClass>
	  <imageFormat xsi:type="xsd:string">png</imageFormat>
	  <baseDir xsi:type="xsd:string">/home/cscdev/config/dqm/tfdqm</baseDir>
	  <viewOnly xsi:type="xsd:boolean">false</viewOnly>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/lib/libtstoreclient.so</xc:Module>
      <xc:Module>${XDAQ_ROOT}/lib/libemubase.so</xc:Module>
      <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libemudqmtfdisplay.so</xc:Module>
    </xc:Context>
  </xsl:template>

<!--Generate context for EmuTFMonitor-->
  <xsl:template name="EmuTFMonitor">
    <xsl:comment >EmuTFMonitor</xsl:comment>
    <xc:Context url="http://emudaq02.cern.ch:20500">
      <xc:Endpoint hostname="emudaq02.cern.ch" protocol="atcp" port="20600" service="i2o" network="atcp1"/>
      <xc:Module>/opt/xdaq/lib/libxdaq2rc.so</xc:Module>
      <xc:Application instance="0" network="atcp1" class="pt::atcp::PeerTransportATCP" id="31">
	<properties xmlns="urn:xdaq-application:pt::atcp::PeerTransportATCP" xsi:type="soapenc:Struct">
	<autoSize xsi:type="xsd:boolean">true</autoSize>
	<maxPacketSize xsi:type="xsd:unsignedInt">196608</maxPacketSize>
	<connectAtFirstSend xsi:type="xsd:boolean">true</connectAtFirstSend>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/lib/libptatcp.so</xc:Module>
      <xc:Module>${XDAQ_ROOT}/lib/libemudaqreader.so</xc:Module>
      <xc:Application instance="0" class="EmuTFMonitor" id="1400" network="atcp1" group="dqm">
	<properties xmlns="urn:xdaq-application:EmuTFMonitor" xsi:type="soapenc:Struct">
	  <readoutMode xsi:type="xsd:string">external</readoutMode>
	  <inputDeviceType xsi:type="xsd:string">file</inputDeviceType>
	  <!-- inputDeviceName xsi:type="xsd:string">/csc_data/csc_00070088_EmuRUI01_Monitor_000.raw</inputDeviceName-->
	  <inputDeviceName xsi:type="xsd:string">/data/dqm/csc_00123997_EmuRUI00_Monitor_013.raw</inputDeviceName>
	  <inputDataFormat xsi:type="xsd:string">DDU</inputDataFormat>
	  <nEventCredits xsi:type="xsd:unsignedInt">200</nEventCredits>
	  <prescalingFactor xsi:type="xsd:unsignedInt">1</prescalingFactor>
	  <serversClassName xsi:type="xsd:string">emu::daq::rui::Application</serversClassName>
	  <serverTIDs xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[1]">
	    <item xsi:type="xsd:unsignedInt" soapenc:position="[0]">1000</item>
	  </serverTIDs>
	  <xmlCfgFile xsi:type="xsd:string">/home/cscdev/config/dqm/CSCTF_histograms.xml</xmlCfgFile>
	  <xmlCanvasesCfgFile xsi:type="xsd:string">/home/cscdev/config/dqm/CSCTF_canvases.xml</xmlCanvasesCfgFile>
	  <cscMapFile xsi:type="xsd:string">/home/cscdev/config/dqm/csc_map.txt</cscMapFile>
	  <fSaveROOTFile xsi:type="xsd:boolean">true</fSaveROOTFile>
	  <outputROOTFile xsi:type="xsd:string">/data/dqm/</outputROOTFile>
	  <outputImagesPath xsi:type="xsd:string">/tmp/images/</outputImagesPath>
	  <useAltFileReader xsi:type="xsd:boolean">false</useAltFileReader>
	  <dduCheckMask xsi:type="xsd:unsignedInt">0xFFFFDFFF</dduCheckMask>
	  <binCheckMask xsi:type="xsd:unsignedInt">0xFFFB7BF6</binCheckMask>
	</properties>
      </xc:Application>
      <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libemudqmtfanalyzer.so</xc:Module>
      <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libemudqmtfmonitor.so</xc:Module>
    </xc:Context>
  </xsl:template>

<!-- Generate contexts for EmuMonitors -->
  <xsl:template name="EmuMonitors">
    <xsl:for-each select="//RUI[@instance!='0']">
<!--       <xsl:if test="($SIDE!='P' and $SIDE!='M') or $SIDE='B' or ($SIDE='P' and number(@instance)&lt;=18) or ($SIDE='M' and number(@instance)&gt;18)"> -->

	<xsl:variable name="HTTP_HOST"><xsl:value-of select="../@alias"/></xsl:variable>
	<xsl:variable name="HTTP_PORT"><xsl:value-of select="number(@port)+200"/></xsl:variable>
	<xsl:variable name="I2O_HOST"><xsl:value-of select="$HTTP_HOST"/></xsl:variable>
	<xsl:variable name="I2O_PORT"><xsl:value-of select="number($HTTP_PORT)+100"/></xsl:variable>
	<xsl:variable name="SERVER_TID"><xsl:value-of select="1000+10*number(@instance)+0"/></xsl:variable>
	<xsl:variable name="APP_ID"><xsl:value-of select="1400+number(@instance)"/></xsl:variable>
	
	<xsl:comment >EmuMonitor <xsl:value-of select="@instance"/></xsl:comment>

	
	<xc:Context url="http://{$HTTP_HOST}:{$HTTP_PORT}">
	  <xc:Endpoint hostname="{$I2O_HOST}" protocol="atcp" port="{$I2O_PORT}" service="i2o" network="atcp1"/>
	  <xc:Application instance="{@instance}" network="atcp1" class="pt::atcp::PeerTransportATCP" id="31">
	    <properties xmlns="urn:xdaq-application:pt::atcp::PeerTransportATCP" xsi:type="soapenc:Struct">
	      <autoSize xsi:type="xsd:boolean">true</autoSize>
	      <maxPacketSize xsi:type="xsd:unsignedInt">196608</maxPacketSize>
	      <connectAtFirstSend xsi:type="xsd:boolean">true</connectAtFirstSend>
	    </properties>
	  </xc:Application>
	  <xc:Module>${XDAQ_ROOT}/lib/libptatcp.so</xc:Module>
	  <xc:Module>${XDAQ_ROOT}/lib/libemudaqreader.so</xc:Module>
	  <xc:Application instance="{@instance}" class="EmuMonitor" id="{$APP_ID}" network="atcp1" group="dqm">
	    <properties xmlns="urn:xdaq-application:EmuMonitor" xsi:type="soapenc:Struct">
	      <readoutMode xsi:type="xsd:string">external</readoutMode>
	      <inputDeviceType xsi:type="xsd:string">file</inputDeviceType>
	      <inputDeviceName xsi:type="xsd:string">/data/run_data.raw</inputDeviceName>
	      <inputDataFormat xsi:type="xsd:string">DDU</inputDataFormat>
	      <nEventCredits xsi:type="xsd:unsignedInt">200</nEventCredits>
	      <prescalingFactor xsi:type="xsd:unsignedInt">1</prescalingFactor>
	      <serversClassName xsi:type="xsd:string">emu::daq::rui::Application</serversClassName>
	      <serverTIDs xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[1]">
		<item xsi:type="xsd:unsignedInt" soapenc:position="[0]"><xsl:value-of select="$SERVER_TID"/></item>
	      </serverTIDs>
	      <xmlCfgFile xsi:type="xsd:string">/home/cscdev/config/dqm/emuDQMBooking.xml</xmlCfgFile>
              <xmlCanvasesCfgFile xsi:type="xsd:string">/home/cscdev/config/dqm/emuDQMCanvases.xml</xmlCanvasesCfgFile>
              <cscMapFile xsi:type="xsd:string">/home/cscdev/config/dqm/csc_map.txt</cscMapFile>
              <fSaveROOTFile xsi:type="xsd:boolean">true</fSaveROOTFile>
              <outputROOTFile xsi:type="xsd:string">/data/dqm/</outputROOTFile>
              <outputImagesPath xsi:type="xsd:string">/tmp/images/</outputImagesPath>
              <useAltFileReader xsi:type="xsd:boolean">false</useAltFileReader>
              <dduCheckMask xsi:type="xsd:unsignedInt">0xFFFFDFFF</dduCheckMask>
              <binCheckMask xsi:type="xsd:unsignedInt">0x16EBF7F6</binCheckMask>
	    </properties>
	  </xc:Application>
	  <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libemudqmcscanalyzer.so</xc:Module>
          <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libemudqmcscmonitor.so</xc:Module>
	</xc:Context>
<!--       </xsl:if> -->
    </xsl:for-each>
  </xsl:template>

<!-- Generate context for TTCciControl of TF just for getting the trigger sources -->
  <xsl:template name="TTCciControl">
    <xsl:comment>TTCciControl (TF)</xsl:comment>
    <xc:Context url="http://emudaq02.cern.ch:20110">
      <xc:Application class="TTCciControl" id="42" instance="2" network="local"/>
    </xc:Context>
  </xsl:template>
</xsl:transform>
