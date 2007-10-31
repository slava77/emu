<?xml version="1.0"?>
<!-- This XSL transformation is used to generate XDAQ config file for EmuDAQ from a RUI-to-chamber mapping. -->
<!-- Usage example:  -->
<!--     xsltproc EmuDAQConfigGenerator.xsl RUI-to-chamber_mapping.xml > EmuDAQ.xml -->
<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  version="1.0" xmlns:xs="http://www.w3.org/2001/XMLSchema"
  xmlns:xc="http://xdaq.web.cern.ch/xdaq/xsd/2004/XMLConfiguration-30"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
  xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">

  <xsl:output method="xml" indent="yes"/>

<!-- Generate config file for EmuDAQ -->
  <xsl:template match="RUI-to-chamber_mapping">

    <xc:Partition xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
		  xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/" 
		  xmlns:xc="http://xdaq.web.cern.ch/xdaq/xsd/2004/XMLConfiguration-30">
      <xsl:comment >Generated from RUI-to-chamber_mapping of <xsl:value-of select="/RUI-to-chamber_mapping/@date"/></xsl:comment>

      <xsl:call-template name="i2o_protocol"/>
      <xsl:call-template name="DAQManager"/>
      <xsl:call-template name="EVM_and_TA"/>
      <xsl:call-template name="TF"/>
      <xsl:call-template name="RUIs"/>

    </xc:Partition>
  </xsl:template>

<!-- Generate I2O protocol target id mapping -->
  <xsl:template name="i2o_protocol">
    <i2o:protocol xmlns:i2o="http://xdaq.web.cern.ch/xdaq/xsd/2004/I2OConfiguration-30">

      <i2o:target tid="2000" instance="0" class="EVM"></i2o:target>
      <i2o:target tid="2001" instance="0" class="EmuTA"></i2o:target>

      <xsl:comment>RUI 0 (TF)</xsl:comment>
      <i2o:target tid="1000" instance="0" class="EmuRUI"></i2o:target>
      <i2o:target tid="1001" instance="0" class="RU"></i2o:target>

      <xsl:for-each select="RUI">      
	<xsl:comment >RUI <xsl:value-of select="@instance"/> (ME<xsl:value-of select="DDU/input[@id=0]/Chamber/@endcap"/><xsl:value-of select="DDU/input[@id=0]/Chamber/@station"/>/<xsl:value-of select="DDU/input[@id=0]/Chamber/@type"/>/<xsl:value-of select="DDU/input[@id=0]/Chamber/@number"/>, ...)</xsl:comment>
	<xsl:variable name="TID0"><xsl:value-of select="1000+10*number(@instance)+0"/></xsl:variable>
	<xsl:variable name="TID1"><xsl:value-of select="1000+10*number(@instance)+1"/></xsl:variable>
	<xsl:variable name="TID2"><xsl:value-of select="1000+10*number(@instance)+2"/></xsl:variable>
	<xsl:variable name="TID3"><xsl:value-of select="1000+10*number(@instance)+3"/></xsl:variable>
	<i2o:target tid="{$TID0}" instance="{@instance}" class="EmuRUI"></i2o:target>
	<i2o:target tid="{$TID1}" instance="{@instance}" class="RU"></i2o:target>
	<i2o:target tid="{$TID2}" instance="{@instance}" class="BU"></i2o:target>
	<i2o:target tid="{$TID3}" instance="{@instance}" class="EmuFU"></i2o:target>
      </xsl:for-each>
    </i2o:protocol>
  </xsl:template>

<!-- Generate contexts for EmuDAQManager -->
  <xsl:template name="DAQManager">
    <xsl:comment>EmuDAQManager</xsl:comment>
    <xc:Context url="http://csc-c2d07-08.cms:40200">
      <!--       <xc:Endpoint hostname="10.177.32.08" protocol="tcp" port="40250" service="i2o" network="tcp1"></xc:Endpoint> -->
      <xc:Endpoint hostname="csc-c2d07-08.cms" protocol="tcp" port="40250" service="i2o" network="tcp1"></xc:Endpoint>
      <xc:Module>${XDAQ_ROOT}/RunControl/tools/xdaq2rc/lib/linux/x86/libxdaq2rc.so</xc:Module>
      <xc:Application network="tcp1" class="PeerTransportTCP" id="21">
	<properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:PeerTransportTCP">
	  <autoSize xsi:type="xsd:boolean">true</autoSize>
	  <maxPacketSize xsi:type="xsd:unsignedInt">131072</maxPacketSize>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/daq/pt/tcp/lib/linux/x86/libpttcp.so</xc:Module>
      <xc:Module>${XDAQ_ROOT}/emu/cscSV/lib/linux/x86/libCSCSupervisor.so</xc:Module>
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
	  <buildEvents xsi:type="xsd:boolean">false</buildEvents>
	  <runType xsi:type="xsd:string">Monitor</runType>
	  
	  <runDbBookingCommand xsi:type="xsd:string">/nfshome0/cscdaq/bin/java -jar /nfshome0/cscdaq/bin/runnumberbooker.jar</runDbBookingCommand>
	  <runDbWritingCommand xsi:type="xsd:string">/nfshome0/cscdaq/bin/java -jar /nfshome0/cscdaq/bin/runinfowriter.jar</runDbWritingCommand>
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
	  <peripheralCrateConfigFiles xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[0]">
	    <!-- 	    <item xsi:type="xsd:string" soapenc:position="[0]"></item> -->
	    <!-- 	    <item xsi:type="xsd:string" soapenc:position="[1]"></item> -->
	  </peripheralCrateConfigFiles>
	  <postToELog xsi:type="xsd:boolean">false</postToELog>
	  <controlDQM xsi:type="xsd:boolean">true</controlDQM>
	  <hardwareMapping xsi:type="xsd:string">emuConfig/EmuDAQ/RUI-to-chamber_mapping.xml</hardwareMapping>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/emu/emuDAQ/emuDAQManager/lib/linux/x86/libEmuDAQManager.so</xc:Module>
    </xc:Context>
  </xsl:template>
  


<!-- Generate contexts for EVM and EmuTA -->
  <xsl:template name="EVM_and_TA">
    <xsl:comment >EVM and EmuTA</xsl:comment>
    <xc:Context url="http://csc-c2d07-08.cms:40201">
      <!-- <xc:Endpoint hostname="10.177.32.08" protocol="tcp" port="40251" service="i2o" network="tcp1"></xc:Endpoint> -->
      <xc:Endpoint hostname="csc-c2d07-08.cms" protocol="tcp" port="40251" service="i2o" network="tcp1"></xc:Endpoint>
      <xc:Module>${XDAQ_ROOT}/RunControl/tools/xdaq2rc/lib/linux/x86/libxdaq2rc.so</xc:Module>
      <xc:Application network="tcp1" class="PeerTransportTCP" id="21">
	<properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:PeerTransportTCP">
	  <autoSize xsi:type="xsd:boolean">true</autoSize>
	  <maxPacketSize xsi:type="xsd:unsignedInt">131072</maxPacketSize>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/daq/pt/tcp/lib/linux/x86/libpttcp.so</xc:Module>
      <xc:Application instance="0" class="EVM" network="tcp1" id="13">
	<properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:EVM">
	  <taClass xsi:type="xsd:string">EmuTA</taClass>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/daq/evb/evm/lib/linux/x86/libEVM.so</xc:Module>
      <xc:Application instance="0" class="EmuTA" network="tcp1" id="17">
	<properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:EmuTA">
	  <runNumber xsi:type="xsd:unsignedLong">57005</runNumber>
	  <maxNumTriggers xsi:type="xsd:integer">-1</maxNumTriggers>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/emu/emuDAQ/emuTA/lib/linux/x86/libEmuTA.so</xc:Module>
    </xc:Context>
  </xsl:template>


<!-- Generate context for TrackFinder's RUI -->
  <xsl:template name="TF">

    <xsl:comment >RUI 0 (TF)</xsl:comment>

    <xc:Context url="http://csc-c2d07-02.cms:40300">
      <!-- <xc:Endpoint hostname="10.177.32.2" protocol="tcp" port="40350" service="i2o" network="tcp1"></xc:Endpoint> -->
      <xc:Endpoint hostname="csc-c2d07-02.cms" protocol="tcp" port="40350" service="i2o" network="tcp1"></xc:Endpoint>
      <xc:Module>${XDAQ_ROOT}/RunControl/tools/xdaq2rc/lib/linux/x86/libxdaq2rc.so</xc:Module>
      <xc:Application network="tcp1" class="PeerTransportTCP" id="20">
	<properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:PeerTransportTCP">
	  <autoSize xsi:type="xsd:boolean">true</autoSize>
	  <maxPacketSize xsi:type="xsd:unsignedInt">131072</maxPacketSize>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/daq/pt/tcp/lib/linux/x86/libpttcp.so</xc:Module>
      <xc:Module>${XDAQ_ROOT}/emu/emuDAQ/emuReadout/lib/linux/x86/libEmuReadout.so</xc:Module>
      <xc:Module>${XDAQ_ROOT}/emu/emuDAQ/emuUtil/lib/linux/x86/libEmuServer.so</xc:Module>
      <xc:Application instance="0" class="RU" network="tcp1" id="14">
	<!-- 	    <properties xmlns="urn:xdaq-application:RU" xsi:type="soapenc:Struct"> -->
	<!-- 	      <tolerateCSCFaults xsi:type="xsd:boolean">true</tolerateCSCFaults> -->
	<!-- 	    </properties> -->
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/daq/evb/ru/lib/linux/x86/libRU.so</xc:Module>
      <xc:Application instance="0" class="EmuRUI" network="tcp1" id="18">
	<properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:RUI">
	  <passDataOnToRUBuilder xsi:type="xsd:boolean">false</passDataOnToRUBuilder>
	  <inputDeviceType xsi:type="xsd:string">spy</inputDeviceType>
	  <inputDeviceName xsi:type="xsd:string">/dev/schar2</inputDeviceName>
	  <hardwareMnemonic xsi:type="xsd:string">TF</hardwareMnemonic>
	  <inputDataFormat xsi:type="xsd:string">DDU</inputDataFormat>
	  <pathToRUIDataOutFile xsi:type="xsd:string">/data</pathToRUIDataOutFile>
	  <ruiFileSizeInMegaBytes xsi:type="xsd:unsignedLong">200</ruiFileSizeInMegaBytes>
	  <pathToBadEventsFile xsi:type="xsd:string"></pathToBadEventsFile>
	  <clientsClassName xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[5]">
	    <item xsi:type="xsd:string" soapenc:position="[0]">EmuMonitor</item>
	  </clientsClassName>
	  <clientsInstance xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[5]">
	    <item xsi:type="xsd:unsignedLong" soapenc:position="[0]">0</item>
	  </clientsInstance>
	</properties>
      </xc:Application>
      <xc:Module>${XDAQ_ROOT}/emu/emuDAQ/emuRUI/lib/linux/x86/libEmuRUI.so</xc:Module>
    </xc:Context>
    
  </xsl:template>

<!-- Generate contexts for RUIs -->
  <xsl:template name="RUIs">
    <xsl:for-each select="RUI">

      <xsl:variable name="HW_MNEMONIC">ME<xsl:value-of select="DDU/input[@id=0]/Chamber/@endcap"/><xsl:value-of select="DDU/input[@id=0]/Chamber/@station"/>/<xsl:value-of select="DDU/input[@id=0]/Chamber/@type"/>/<xsl:value-of select="DDU/input[@id=0]/Chamber/@number"/>, ...</xsl:variable>
      <!-- <xsl:variable name="I2O_HOST">10.177.32.<xsl:value-of select="number(substring-after(@host,'csc-c2d07-'))"/></xsl:variable> -->
      <xsl:variable name="I2O_HOST"><xsl:value-of select="@host"/>.cms</xsl:variable>
      <xsl:variable name="I2O_PORT"><xsl:value-of select="number(@port)+50"/></xsl:variable>

      <xsl:comment >RUI <xsl:value-of select="@instance"/> (<xsl:value-of select="$HW_MNEMONIC"/>)</xsl:comment>

	<xc:Context url="http://{@host}.cms:{@port}">
	  <xc:Endpoint hostname="{$I2O_HOST}" protocol="tcp" port="{$I2O_PORT}" service="i2o" network="tcp1"></xc:Endpoint>
	  <xc:Module>${XDAQ_ROOT}/RunControl/tools/xdaq2rc/lib/linux/x86/libxdaq2rc.so</xc:Module>
	  <xc:Application network="tcp1" class="PeerTransportTCP" id="21">
	    <properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:PeerTransportTCP">
	      <autoSize xsi:type="xsd:boolean">true</autoSize>
	      <maxPacketSize xsi:type="xsd:unsignedInt">131072</maxPacketSize>
	    </properties>
	  </xc:Application>
	  <xc:Module>${XDAQ_ROOT}/daq/pt/tcp/lib/linux/x86/libpttcp.so</xc:Module>
	  <xc:Module>${XDAQ_ROOT}/emu/emuDAQ/emuReadout/lib/linux/x86/libEmuReadout.so</xc:Module>
	  <xc:Module>${XDAQ_ROOT}/emu/emuDAQ/emuUtil/lib/linux/x86/libEmuServer.so</xc:Module>
	  <xc:Application instance="{@instance}" class="RU" network="tcp1" id="14">
<!-- 	    <properties xmlns="urn:xdaq-application:RU" xsi:type="soapenc:Struct"> -->
<!-- 	      <tolerateCSCFaults xsi:type="xsd:boolean">true</tolerateCSCFaults> -->
<!-- 	    </properties> -->
	  </xc:Application>
	  <xc:Module>${XDAQ_ROOT}/daq/evb/ru/lib/linux/x86/libRU.so</xc:Module>
	  <xc:Application instance="{@instance}" class="BU" network="tcp1" id="15"></xc:Application>
	  <xc:Module>${XDAQ_ROOT}/daq/evb/bu/lib/linux/x86/libBU.so</xc:Module>
	  <xc:Application instance="{@instance}" class="EmuFU" network="tcp1" id="16">
	    <properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:FU">
	      <buInstNb xsi:type="xsd:unsignedLong"><xsl:value-of select="@instance"/></buInstNb>
	      <pathToDataOutFile xsi:type="xsd:string">/data</pathToDataOutFile>
	      <fileSizeInMegaBytes xsi:type="xsd:unsignedLong">200</fileSizeInMegaBytes>
	      <clientsClassName xsi:type="soapenc:Array" soapenc:arrayType="xsd:ur-type[5]"></clientsClassName>
	    </properties>
	  </xc:Application>
	  <xc:Module>${XDAQ_ROOT}/emu/emuDAQ/emuFU/lib/linux/x86/libEmuFU.so</xc:Module>
	  <xc:Application instance="{@instance}" class="EmuRUI" network="tcp1" id="18">
	    <properties xsi:type="soapenc:Struct" xmlns="urn:xdaq-application:RUI">
	      <passDataOnToRUBuilder xsi:type="xsd:boolean">false</passDataOnToRUBuilder>
	      <inputDeviceType xsi:type="xsd:string">spy</inputDeviceType>
	      <inputDeviceName xsi:type="xsd:string">/dev/schar<xsl:value-of select="@schar"/></inputDeviceName>
	      <hardwareMnemonic xsi:type="xsd:string"><xsl:value-of select="$HW_MNEMONIC"/></hardwareMnemonic>
	      <inputDataFormat xsi:type="xsd:string">DDU</inputDataFormat>
	      <pathToRUIDataOutFile xsi:type="xsd:string">/data</pathToRUIDataOutFile>
	      <pathToBadEventsFile xsi:type="xsd:string"></pathToBadEventsFile>
	      <ruiFileSizeInMegaBytes xsi:type="xsd:unsignedLong">200</ruiFileSizeInMegaBytes>
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
	  <xc:Module>${XDAQ_ROOT}/emu/emuDAQ/emuRUI/lib/linux/x86/libEmuRUI.so</xc:Module>
	</xc:Context>

      </xsl:for-each>
  </xsl:template>


</xsl:transform>
