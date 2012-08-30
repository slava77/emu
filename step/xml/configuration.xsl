<?xml version="1.0"?>

<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:es="http://cms.cern.ch/emu/step"
    version="1.0">

  <xsl:output method="xml" indent="yes"/>

  <xsl:param name="GROUP"/>
 

  <!-- VME crates -->

  <xsl:template match="/EmuSystem">
    <es:peripheralCrates group="{$GROUP}" progress="0">
      <xsl:apply-templates select="PeripheralCrate"/>
    </es:peripheralCrates>
  </xsl:template>

  <xsl:template match="PeripheralCrate">
    <es:peripheralCrate id="{@crateID}" label="{@label}" selected="yes">
      <xsl:apply-templates select="CSC"/>
    </es:peripheralCrate>
  </xsl:template>

  <xsl:template match="CSC">
    <es:chamber label="{@label}"/>
  </xsl:template>

  
  <!-- Test parameters -->

  <xsl:template match="/STEP_tests">
    <es:testSequence>
      <xsl:apply-templates select="test_config"/>
    </es:testSequence>
  </xsl:template>

  <xsl:template match="test_config">
    <es:test id="{translate(test,' ','')}" description="{description}" selected="yes" status="idle"/>
  </xsl:template>
  
</xsl:transform>
