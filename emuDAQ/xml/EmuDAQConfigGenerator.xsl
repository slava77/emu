<?xml version="1.0"?>
<!-- This XSL transformation is used to generate XDAQ config file for EmuDAQ from a RUI-to-computer mapping. -->
<!-- Usage example:  -->
<!--     xsltproc [<hyphen><hyphen>stringparam SIDE 'P|M|B'] <hyphen><hyphen>stringparam WRITE Y <hyphen><hyphen>stringparam BUILD N EmuDAQConfigGenerator.xsl RUI-to-computer_mapping.xml > DAQ.xml -->
<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  version="1.0" xmlns:xs="http://www.w3.org/2001/XMLSchema"
  xmlns:xc="http://xdaq.web.cern.ch/xdaq/xsd/2004/XMLConfiguration-30"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
  xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">

  <!-- Parameter SIDE is to be optionally set from the command line. -->
  <!-- If it's set to 'M', only the minus side will be generated; if 'P', only the plus side; if 'B', both sides; otherwise both sides. -->
  <xsl:param name="SIDE"/>
  <!-- Parameter WRITE is to be set from the command line. -->
  <!-- If it is set to 'Y', the RUIs will write data to files; if 'N', they will not. -->
  <xsl:param name="WRITE"/>
  <!-- Parameter BUILD is to be set from the command line. -->
  <!-- If it is set to 'Y', events will be built; if 'N', events will not be built. -->
  <xsl:param name="BUILD"/>

  <xsl:output method="xml" indent="yes"/>

<!-- Generate config file for EmuDAQ -->
  <xsl:template match="RUI-to-computer_mapping">
    <xsl:processing-instruction name="xml-stylesheet" >type="text/xml" href="EmuDAQConfig.xsl"</xsl:processing-instruction>

    <xc:Partition xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
		  xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/" 
		  xmlns:xc="http://xdaq.web.cern.ch/xdaq/xsd/2004/XMLConfiguration-30">
      <xsl:comment >Generated from RUI-to-computer_mapping of <xsl:value-of select="/RUI-to-computer_mapping/@date"/></xsl:comment>

      <xsl:call-template name="i2o_protocol"/>
      <xsl:call-template name="DAQManager"/>
      <xsl:call-template name="EVM_and_TA"/>
      <xsl:call-template name="TF"/>
      <xsl:call-template name="RUIs"/>
      <xsl:call-template name="EmuDisplayClient"/>
      <xsl:call-template name="EmuMonitors"/>

    </xc:Partition>
  </xsl:template>

<!-- Generate I2O protocol target id mapping -->
  <xsl:template name="i2o_protocol">
    <i2o:protocol xmlns:i2o="http://xdaq.web.cern.ch/xdaq/xsd/2004/I2OConfiguration-30">

      <i2o:target tid="2000" instance="0" class="rubuilder::evm::Application"></i2o:target>
      <i2o:target tid="2001" instance="0" class="EmuTA"></i2o:target>

      <i2o:target tid="1450" instance="0" class="EmuDisplayClient"></i2o:target>

      <xsl:comment>RUI 0 (TF)</xsl:comment>
      <i2o:target tid="1000" instance="0" class="EmuRUI"></i2o:target>
      <i2o:target tid="1001" instance="0" class="rubuilder::ru::Application"></i2o:target>

      <xsl:for-each select="//RUI[@instance!='0']">      
	<xsl:if test="($SIDE!='P' and $SIDE!='M') or $SIDE='B' or ($SIDE='P' and number(@instance)&lt;=18) or ($SIDE='M' and number(@instance)&gt;18)">

	  <xsl:comment >RUI <xsl:value-of select="@instance"/></xsl:comment>

	  <xsl:variable name="DAQ_TID0"><xsl:value-of select="1000+10*number(@instance)+0"/></xsl:variable>
	  <xsl:variable name="DAQ_TID1"><xsl:value-of select="1000+10*number(@instance)+1"/></xsl:variable>
	  <xsl:variable name="DAQ_TID2"><xsl:value-of select="1000+10*number(@instance)+2"/></xsl:variable>
	  <xsl:variable name="DAQ_TID3"><xsl:value-of select="1000+10*number(@instance)+3"/></xsl:variable>
	  <i2o:target tid="{$DAQ_TID0}" instance="{@instance}" class="EmuRUI"></i2o:target>
	  <i2o:target tid="{$DAQ_TID1}" instance="{@instance}" class="rubuilder::ru::Application"></i2o:target>
	  <i2o:target tid="{$DAQ_TID2}" instance="{@instance}" class="rubuilder::bu::Application"></i2o:target>
	  <i2o:target tid="{$DAQ_TID3}" instance="{@instance}" class="EmuFU"></i2o:target>

	  <xsl:variable name="DQM_TID"><xsl:value-of select="1400+number(@instance)"/></xsl:variable>
	  <i2o:target tid="{$DQM_TID}" instance="{@instance}" class="EmuMonitor"></i2o:target>

	</xsl:if>
      </xsl:for-each>
    </i2o:protocol>
  </xsl:template>

<!-- Generate contexts for EmuDAQManager -->
  <xsl:template name="DAQManager">
    <xsl:comment>EmuDAQManager</xsl:comment>
    <xc:Context url="http://csc-daq00.cms:40200">
      <xc:Endpoint hostname="csc-daq00.cms" protocol="tcp" port="40250" service="i2o" network="tcp1"></xc:Endpoint>
      <xc:Application network="tcp1" class="pt::tcp::PeerTransportTCP" id="21">
	<properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:PeerTransportTCP">
	  <autoSize xsi:type="xsd:boolean">true</autoSize>
	  <maxPacketSize xsi:type="xsd:unsignedInt">131072</maxPacketSize>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/lib/libpttcp.so</xc:Module>
      <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libCSCSupervisor.so</xc:Module>
      <xc:Application instance="0" class="EmuDAQManager" network="tcp1" id="12">
	<properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:EmuDAQManager">
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
	  <globalMode xsi:type="xsd:boolean">true</globalMode>
	  <runDbBookingCommand xsi:type="xsd:string">/usr/java/jdk/bin/java -jar /nfshome0/cscdaq/bin/runnumberbooker.jar</runDbBookingCommand>
	  <runDbWritingCommand xsi:type="xsd:string">/usr/java/jdk/bin/java -jar /nfshome0/cscdaq/bin/runinfowriter.jar</runDbWritingCommand>
	  <runDbAddress xsi:type="xsd:string">dbc:oracle:thin:@oracms.cern.ch:10121:omds</runDbAddress>
	  <runDbUserFile xsi:type="xsd:string">/nfshome0/cscdaq/config/.runDbUser</runDbUserFile>
	  <!-- <runDbUserFile xsi:type="xsd:string">/nfshome0/cscdaq/config/.runDbTestUser</runDbUserFile> -->
	  <!-- <runDbUserFile xsi:type="xsd:string">/nfshome0/cscdaq/config/.runDbMTCCUser</runDbUserFile> -->
	  <curlCommand xsi:type="xsd:string">/usr/bin/curl -v</curlCommand>
	  <curlCookies xsi:type="xsd:string">.curlCookies</curlCookies>
	  <CMSUserFile xsi:type="xsd:string">/nfshome0/cscdaq/config/.CMSUser</CMSUserFile>
	  <eLogUserFile xsi:type="xsd:string">/nfshome0/cscdaq/config/.eLogUser</eLogUserFile>
	  <eLogURL xsi:type="xsd:string">https://cmsdaq.cern.ch/elog/CSC</eLogURL>
	  <!-- <eLogURL xsi:type="xsd:string">https://cmsdaq.cern.ch/elog/Test</eLogURL> -->
	  <postToELog xsi:type="xsd:boolean">false</postToELog>
	  <controlDQM xsi:type="xsd:boolean">true</controlDQM>
	  <hardwareMapping xsi:type="xsd:string">emu/config/EmuDAQ/RUI-to-chamber_mapping.xml</hardwareMapping>
	  <TF_FM_URL xsi:type="xsd:string">http://UNKNOWN.cms:12000</TF_FM_URL>
	  <CSC_FM_URL xsi:type="xsd:string">http://cmsrc-csc.cms:12000</CSC_FM_URL>
	  <RegexMatchingTFConfigName xsi:type="xsd:string">UNKNOWN</RegexMatchingTFConfigName>
	  <RegexMatchingCSCConfigName xsi:type="xsd:string">.*DAQ/.*</RegexMatchingCSCConfigName>
	</properties>
      </xc:Application>
      <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libEmuDAQManager.so</xc:Module>
    </xc:Context>
  </xsl:template>
  


<!-- Generate contexts for EVM and EmuTA -->
  <xsl:template name="EVM_and_TA">
    <xsl:comment >EVM and EmuTA</xsl:comment>
    <xc:Context url="http://csc-daq00.cms:40201">
      <xc:Endpoint hostname="csc-daq00.cms" protocol="tcp" port="40251" service="i2o" network="tcp1"></xc:Endpoint>
      <xc:Module>${XDAQ_ROOT}/lib/libxdaq2rc.so</xc:Module>
      <xc:Application network="tcp1" class="pt::tcp::PeerTransportTCP" id="21">
	<properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:PeerTransportTCP">
	  <autoSize xsi:type="xsd:boolean">true</autoSize>
	  <maxPacketSize xsi:type="xsd:unsignedInt">131072</maxPacketSize>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/lib/libpttcp.so</xc:Module>
      <xc:Application instance="0" class="rubuilder::evm::Application" network="tcp1" id="13">
	<properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:rubuilder::evm::Application">
	  <taClass xsi:type="xsd:string">EmuTA</taClass>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/lib/librubuilderutils.so</xc:Module>
      <xc:Module>${XDAQ_ROOT}/lib/librubuilderevm.so</xc:Module>
      <xc:Application instance="0" class="EmuTA" network="tcp1" id="17"/>
      <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libEmuTA.so</xc:Module>
    </xc:Context>
  </xsl:template>


<!-- Generate context for TrackFinder's RUI -->
  <xsl:template name="TF">

    <xsl:variable name="HTTP_HOST"><xsl:value-of select="//RUI[@instance='0']/../@alias"/></xsl:variable>
    <xsl:variable name="HTTP_PORT"><xsl:value-of select="//RUI[@instance='0']/@port"/></xsl:variable>
    <xsl:variable name="I2O_HOST"><xsl:value-of select="$HTTP_HOST"/></xsl:variable>
    <xsl:variable name="I2O_PORT"><xsl:value-of select="number($HTTP_PORT)+50"/></xsl:variable>

    <xsl:comment >RUI 0 (TF)</xsl:comment>

    <xc:Context url="http://{$HTTP_HOST}:{$HTTP_PORT}">
      <xc:Endpoint hostname="{$I2O_HOST}" protocol="tcp" port="{$I2O_PORT}" service="i2o" network="tcp1"></xc:Endpoint>
      <xc:Module>${XDAQ_ROOT}/lib/libxdaq2rc.so</xc:Module>
      <xc:Application network="tcp1" class="pt::tcp::PeerTransportTCP" id="20">
	<properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:PeerTransportTCP">
	  <autoSize xsi:type="xsd:boolean">true</autoSize>
	  <maxPacketSize xsi:type="xsd:unsignedInt">131072</maxPacketSize>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/lib/libpttcp.so</xc:Module>
      <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libEmuReadout.so</xc:Module>
      <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libEmuServer.so</xc:Module>
      <xc:Application instance="0" class="rubuilder::ru::Application" network="tcp1" id="14">
	<properties xmlns="urn:xdaq-application:rubuilder::ru::Application" xsi:type="soapenc:Struct">
	  <tolerateCSCFaults xsi:type="xsd:boolean">true</tolerateCSCFaults>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/lib/librubuilderutils.so</xc:Module>
      <xc:Module>${XDAQ_ROOT}/lib/librubuilderru.so</xc:Module>
      <xc:Application instance="0" class="EmuRUI" network="tcp1" id="18">
	<properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:EmuRUI">
	  <passDataOnToRUBuilder xsi:type="xsd:boolean">false</passDataOnToRUBuilder>
	  <inputDeviceType xsi:type="xsd:string">spy</inputDeviceType>
	  <inputDeviceName xsi:type="xsd:string">/dev/schar<xsl:value-of select="//RUI[@instance='0']/@schar"/></inputDeviceName>
	  <hardwareMnemonic xsi:type="xsd:string">TF</hardwareMnemonic>
	  <inputDataFormat xsi:type="xsd:string">DDU</inputDataFormat>
	  <pathToRUIDataOutFile xsi:type="xsd:string">/data</pathToRUIDataOutFile>
	  <ruiFileSizeInMegaBytes xsi:type="xsd:unsignedLong"><xsl:if test="$WRITE='N'">0</xsl:if><xsl:if test="$WRITE='Y'">200</xsl:if></ruiFileSizeInMegaBytes>
	  <pathToBadEventsFile xsi:type="xsd:string"></pathToBadEventsFile>
	  <clientsClassName xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[5]">
	    <item xsi:type="xsd:string" soapenc:position="[0]">EmuMonitor</item>
	  </clientsClassName>
	  <clientsInstance xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[5]">
	    <item xsi:type="xsd:unsignedLong" soapenc:position="[0]">0</item>
	  </clientsInstance>
	</properties>
      </xc:Application>
      <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libEmuRUI.so</xc:Module>
    </xc:Context>
    
  </xsl:template>

<!-- Generate contexts for RUIs -->
  <xsl:template name="RUIs">
    <xsl:for-each select="//RUI[@instance!='0']">
      <xsl:if test="($SIDE!='P' and $SIDE!='M') or $SIDE='B' or ($SIDE='P' and number(@instance)&lt;=18) or ($SIDE='M' and number(@instance)&gt;18)">

	<xsl:variable name="HTTP_HOST"><xsl:value-of select="../@alias"/></xsl:variable>
	<xsl:variable name="HTTP_PORT"><xsl:value-of select="@port"/></xsl:variable>
	<xsl:variable name="I2O_HOST"><xsl:value-of select="../@alias"/></xsl:variable>
	<xsl:variable name="I2O_PORT"><xsl:value-of select="number(@port)+50"/></xsl:variable>

	<xsl:comment >RUI <xsl:value-of select="@instance"/></xsl:comment>
	
	<xc:Context url="http://{$HTTP_HOST}:{$HTTP_PORT}">
	  <xc:Endpoint hostname="{$I2O_HOST}" protocol="tcp" port="{$I2O_PORT}" service="i2o" network="tcp1"></xc:Endpoint>
	  <xc:Module>${XDAQ_ROOT}/lib/libxdaq2rc.so</xc:Module>
	  <xc:Application network="tcp1" class="pt::tcp::PeerTransportTCP" id="21">
	    <properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:PeerTransportTCP">
	      <autoSize xsi:type="xsd:boolean">true</autoSize>
	      <maxPacketSize xsi:type="xsd:unsignedInt">131072</maxPacketSize>
	    </properties>
	  </xc:Application>
	  <xc:Module>${XDAQ_ROOT}/lib/libpttcp.so</xc:Module>
	  <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libEmuReadout.so</xc:Module>
	  <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libEmuServer.so</xc:Module>
	  <xc:Application instance="{@instance}" class="rubuilder::ru::Application" network="tcp1" id="14">
	    <properties xmlns="urn:xdaq-application:rubuilder::ru::Application" xsi:type="soapenc:Struct">
	      <tolerateCSCFaults xsi:type="xsd:boolean">true</tolerateCSCFaults>
	    </properties>
	  </xc:Application>
	  <xc:Module>${XDAQ_ROOT}/lib/librubuilderutils.so</xc:Module>
	  <xc:Module>${XDAQ_ROOT}/lib/librubuilderru.so</xc:Module>
	  <xc:Application instance="{@instance}" class="rubuilder::bu::Application" network="tcp1" id="15"></xc:Application>
	  <xc:Module>${XDAQ_ROOT}/lib/librubuilderbu.so</xc:Module>
	  <xc:Application instance="{@instance}" class="EmuFU" network="tcp1" id="16">
	    <properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:EmuFU">
	      <buInstNb xsi:type="xsd:unsignedLong"><xsl:value-of select="@instance"/></buInstNb>
	      <pathToDataOutFile xsi:type="xsd:string">/data</pathToDataOutFile>
	      <fileSizeInMegaBytes xsi:type="xsd:unsignedLong">200</fileSizeInMegaBytes>
	      <clientsClassName xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[5]"></clientsClassName>
	    </properties>
	  </xc:Application>
	  <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libEmuFU.so</xc:Module>
	  <xc:Application instance="{@instance}" class="EmuRUI" network="tcp1" id="18">
	    <properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:EmuRUI">
	      <passDataOnToRUBuilder xsi:type="xsd:boolean">false</passDataOnToRUBuilder>
	      <inputDeviceType xsi:type="xsd:string">spy</inputDeviceType>
	      <inputDeviceName xsi:type="xsd:string">/dev/schar<xsl:value-of select="@schar"/></inputDeviceName>
	      <hardwareMnemonic xsi:type="xsd:string">chambers</hardwareMnemonic>
	      <inputDataFormat xsi:type="xsd:string">DDU</inputDataFormat>
	      <pathToRUIDataOutFile xsi:type="xsd:string">/data</pathToRUIDataOutFile>
	      <pathToBadEventsFile xsi:type="xsd:string"></pathToBadEventsFile>
	      <ruiFileSizeInMegaBytes xsi:type="xsd:unsignedLong"><xsl:if test="$WRITE='N'">0</xsl:if><xsl:if test="$WRITE='Y'">200</xsl:if></ruiFileSizeInMegaBytes>
	      <clientsClassName xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[5]">
		<item xsi:type="xsd:string" soapenc:position="[0]">EmuMonitor</item>
	      </clientsClassName>
	      <clientsInstance xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[5]">
		<item xsi:type="xsd:unsignedLong" soapenc:position="[0]"><xsl:value-of select="@instance"/></item>
	      </clientsInstance>
	      <poolSizeForClient xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[5]">
		<item xsi:type="xsd:unsignedLong" soapenc:position="[0]">0x4000000</item>
	      </poolSizeForClient>
	    </properties>
	  </xc:Application>
	  <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libEmuRUI.so</xc:Module>
	</xc:Context>

      </xsl:if>
    </xsl:for-each>
  </xsl:template>

<!-- Generate context for EmuDisplayClient -->
  <xsl:template name="EmuDisplayClient">
    <xsl:comment >EmuDisplayClient</xsl:comment>
    <xc:Context url="http://csc-dqm.cms:40550">
      <xc:Application class="EmuDisplayClient" id="1450" instance="0" network="local">
	<properties xmlns="urn:xdaq-application:EmuDisplayClient" xsi:type="soapenc:Struct">
	<monitorClass xsi:type="xsd:string">EmuMonitor</monitorClass>
        <imageFormat xsi:type="xsd:string">png</imageFormat>
        <baseDir xsi:type="xsd:string">/nfshome0/cscdqm/config/dqm</baseDir>
        <viewOnly xsi:type="xsd:boolean">true</viewOnly>
	</properties>
      </xc:Application>
      <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libEmuDisplayClient.so</xc:Module>
    </xc:Context>
  </xsl:template>

<!-- Generate contexts for EmuMonitors -->
  <xsl:template name="EmuMonitors">
    <xsl:for-each select="//RUI[@instance!='0']">
      <xsl:if test="($SIDE!='P' and $SIDE!='M') or $SIDE='B' or ($SIDE='P' and number(@instance)&lt;=18) or ($SIDE='M' and number(@instance)&gt;18)">

	<xsl:variable name="HTTP_HOST"><xsl:value-of select="../@alias"/></xsl:variable>
	<xsl:variable name="HTTP_PORT"><xsl:value-of select="number(@port)+200"/></xsl:variable>
	<xsl:variable name="I2O_HOST"><xsl:value-of select="$HTTP_HOST"/></xsl:variable>
	<xsl:variable name="I2O_PORT"><xsl:value-of select="number($HTTP_PORT)+100"/></xsl:variable>
	<xsl:variable name="SERVER_TID"><xsl:value-of select="1000+10*number(@instance)+0"/></xsl:variable>
	<xsl:variable name="APP_ID"><xsl:value-of select="1400+number(@instance)"/></xsl:variable>
	
	<xsl:comment >EmuMonitor <xsl:value-of select="@instance"/></xsl:comment>

	<xc:Context url="http://{$HTTP_HOST}:{$HTTP_PORT}">
	  <xc:Endpoint hostname="{$I2O_HOST}" protocol="tcp" port="{$I2O_PORT}" service="i2o" network="tcp1"/>
	  <xc:Application network="tcp1" class="pt::tcp::PeerTransportTCP" id="21">
	    <properties xmlns="urn:xdaq-application:pt::tcp::PeerTransportTCP" xsi:type="soapenc:Struct">
	      <autoSize xsi:type="xsd:boolean">true</autoSize>
	      <maxPacketSize xsi:type="xsd:unsignedInt">131072</maxPacketSize>
	    </properties>
	  </xc:Application>
	  <xc:Module>${XDAQ_ROOT}/lib/libpttcp.so</xc:Module>
	  <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libEmuReadout.so</xc:Module>
	  <xc:Application instance="{@instance}" class="EmuMonitor" id="{$APP_ID}" network="tcp1" group="dqm">
	    <properties xmlns="urn:xdaq-application:EmuMonitor" xsi:type="soapenc:Struct">
	      <readoutMode xsi:type="xsd:string">external</readoutMode>
	      <inputDeviceType xsi:type="xsd:string">file</inputDeviceType>
	      <inputDeviceName xsi:type="xsd:string">/data/run_data.raw</inputDeviceName>
	      <inputDataFormat xsi:type="xsd:string">DDU</inputDataFormat>
	      <nEventCredits xsi:type="xsd:unsignedInt">200</nEventCredits>
	      <prescalingFactor xsi:type="xsd:unsignedInt">1</prescalingFactor>
	      <serversClassName xsi:type="xsd:string">EmuRUI</serversClassName>
	      <serverTIDs xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[1]">
		<item xsi:type="xsd:unsignedInt" soapenc:position="[0]"><xsl:value-of select="$SERVER_TID"/></item>
	      </serverTIDs>
	      <xmlCfgFile xsi:type="xsd:string">/nfshome0/cscdqm/config/emuDQMBooking.xml</xmlCfgFile>
              <xmlCanvasesCfgFile xsi:type="xsd:string">/nfshome0/cscdqm/config/emuDQMCanvases.xml</xmlCanvasesCfgFile>
              <cscMapFile xsi:type="xsd:string">/nfshome0/cscdqm/config/csc_map.txt</cscMapFile>
              <fSaveROOTFile xsi:type="xsd:boolean">true</fSaveROOTFile>
              <outputROOTFile xsi:type="xsd:string">/nfshome0/cscdqm/results/</outputROOTFile>
              <outputImagesPath xsi:type="xsd:string">/tmp/images/</outputImagesPath>
              <useAltFileReader xsi:type="xsd:boolean">false</useAltFileReader>
              <dduCheckMask xsi:type="xsd:unsignedInt">0xFFFFDFFF</dduCheckMask>
              <binCheckMask xsi:type="xsd:unsignedInt">0xFFFB7BF6</binCheckMask>
	    </properties>
	  </xc:Application>
	  <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libEmuPlotter.so</xc:Module>
          <xc:Module>${BUILD_HOME}/${XDAQ_PLATFORM}/lib/libEmuMonitor.so</xc:Module>
	</xc:Context>
	
      </xsl:if>
    </xsl:for-each>
  </xsl:template>

</xsl:transform>
