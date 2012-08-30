<?xml version="1.0"?>

<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    version="1.0">

  <xsl:output method="text" indent="yes"/>

  <!-- xpath to element -->
  <xsl:template name="elementXPath">
    <xsl:for-each select="(ancestor-or-self::*)"><xsl:variable name="NAME" select="name()"/>/<xsl:value-of select="name()"/>[<xsl:value-of select="1+count(preceding-sibling::*[name()=$NAME])"/>]</xsl:for-each><xsl:text>
</xsl:text>
  </xsl:template>

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <!-- elements -->
  <xsl:template match="*">
    <xsl:call-template name="elementXPath"/>
    <xsl:apply-templates select="*"/>
  </xsl:template>

</xsl:transform>
