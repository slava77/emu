<?xml version='1.0'?>
<!-- Order of specification will determine the sequence of installation. all modules are loaded prior instantiation of plugins -->
<xp:Profile xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xp="http://xdaq.web.cern.ch/xdaq/xsd/2005/XMLProfile-11">
	<!-- Compulsory  Plugins -->
	<xp:Application class="executive::Application" id="0"  service="executive" network="local">
		<properties xmlns="urn:xdaq-application:Executive" xsi:type="soapenc:Struct">
			<logUrl xsi:type="xsd:string">console</logUrl>
                	<logLevel xsi:type="xsd:string">INFO</logLevel>
                </properties>
	</xp:Application>
	<xp:Module>${XDAQ_ROOT}/lib/libb2innub.so</xp:Module>
	<xp:Module>${XDAQ_ROOT}/lib/libexecutive.so</xp:Module>
	
	<xp:Application class="pt::http::PeerTransportHTTP" id="1"  network="local">
		 <properties xmlns="urn:xdaq-application:pt::http::PeerTransportHTTP" xsi:type="soapenc:Struct">
		 	<documentRoot xsi:type="xsd:string">${XDAQ_DOCUMENT_ROOT}</documentRoot>
                        <aliasName xsi:type="xsd:string">tmp</aliasName>
                        <aliasPath xsi:type="xsd:string">/tmp</aliasPath>
                </properties>
	</xp:Application>
	<xp:Module>${XDAQ_ROOT}/lib/libpthttp.so</xp:Module>

	<xp:Application class="pt::fifo::PeerTransportFifo" id="8"  network="local"/>
	<xp:Module>${XDAQ_ROOT}/lib/libptfifo.so</xp:Module>
	
	<!-- XRelay -->
	<xp:Application class="xrelay::Application" id="4"  service="xrelay"  network="local"/>
	<xp:Module>${XDAQ_ROOT}/lib/libxrelay.so</xp:Module>
	
	<!-- HyperDAQ -->
	<xp:Application class="hyperdaq::Application" id="3"  service="hyperdaq" network="local"/>
	<xp:Module>${XDAQ_ROOT}/lib/libhyperdaq.so</xp:Module>	
		
	<!-- XPlore requires the installation of Power Pack  -->
	<xp:Application class="xplore::Application" id="9"  network="local">
	 	<properties xmlns="urn:xdaq-application:xplore::Application" xsi:type="soapenc:Struct">
			<settings xsi:type="xsd:string">${XDAQ_SETUP_ROOT}/${XDAQ_ZONE}/xplore/shortcuts.xml</settings>
			<republishInterval xsi:type="xsd:string">60</republishInterval>
                </properties>
	</xp:Application>	
	<xp:Module>/lib/libslp.so</xp:Module>
	<xp:Module>${XDAQ_ROOT}/lib/libxslp.so</xp:Module>
	<xp:Module>${XDAQ_ROOT}/lib/libxploreutils.so</xp:Module>
	<xp:Module>${XDAQ_ROOT}/lib/libxplore.so</xp:Module>  		
	
	<!-- WS-eventing -->
	<xp:Application class="ws::service::Eventing" id="400"  publish="true" network="local" group="exception,init0" service="ws-eventing">
	 	<properties xmlns="urn:xdaq-application:ws::service::Eventing" xsi:type="soapenc:Struct">
                </properties>
        </xp:Application>
	<xp:Module>${XDAQ_EXTERN_LIBRARY_PATH}/libxalanMsg.so</xp:Module>
	<xp:Module>${XDAQ_EXTERN_LIBRARY_PATH}/libxalan-c.so</xp:Module>
	<xp:Module>${XDAQ_ROOT}/lib/libxoapfilter.so</xp:Module>
	<xp:Module>${XDAQ_ROOT}/lib/libwsaddressing.so</xp:Module>
	<xp:Module>${XDAQ_ROOT}/lib/libwseventing.so</xp:Module>
	<xp:Module>${XDAQ_ROOT}/lib/libwsutils.so</xp:Module>
	<xp:Module>${XDAQ_ROOT}/lib/libwsserviceeventing.so</xp:Module>
</xp:Profile>
