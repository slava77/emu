<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/" xmlns:i2o="http://xdaq.web.cern.ch/xdaq/xsd/2004/I2OConfiguration-30" xmlns:xc="http://xdaq.web.cern.ch/xdaq/xsd/2004/XMLConfiguration-30" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xp="http://xdaq.web.cern.ch/xdaq/xsd/2005/XMLProfile-10" xmlns:fn="http://www.w3.org/2005/02/xpath-functions">

  <xsl:output method="xml" indent="yes"/> 

  <xsl:template match="EmuConfigFileList">
    <xsl:processing-instruction name="xml-stylesheet">type="text/xsl" href="EmuDAQConfig.xsl"</xsl:processing-instruction> 
    <xc:Partition xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xc="http://xdaq.web.cern.ch/xdaq/xsd/2004/XMLConfiguration-30" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
      <xsl:call-template name="protocol"/>
      <xsl:call-template name="contexts"/>
    </xc:Partition>
  </xsl:template>

  <xsl:template name="protocol">
    <!-- merge i2o:protocol elements from all files into one i2o:protocol element -->
    <i2o:protocol xmlns:i2o="http://xdaq.web.cern.ch/xdaq/xsd/2004/I2OConfiguration-30">
    <xsl:for-each select="uri">
      <xsl:variable name="URI"><xsl:value-of select="."/></xsl:variable>
      <xsl:for-each select="document($URI)/xc:Partition/i2o:protocol/i2o:target">
	<xsl:copy-of select="."/>
      </xsl:for-each>
    </xsl:for-each>      
    </i2o:protocol>
  </xsl:template>

  <xsl:template name="contexts">
    <!-- collect all xc:Context elements from all files -->
    <xsl:for-each select="uri">
      <xsl:variable name="URI"><xsl:value-of select="."/></xsl:variable>
      <xsl:for-each select="document($URI)/xc:Partition/xc:Context">
	<xsl:copy-of select="."/>
      </xsl:for-each>
    </xsl:for-each>      
  </xsl:template>

</xsl:stylesheet>
