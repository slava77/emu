<?xml version="1.0"?>

<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    version="1.0">

  <xsl:output method="text" indent="yes"/>


  <!-- xpath to element -->
  <xsl:template name="elementXPath">
    <xsl:for-each select="(ancestor-or-self::*)"><xsl:variable name="NAME" select="name()"/>/<xsl:value-of select="name()"/>[<xsl:value-of select="1+count(preceding-sibling::*[name()=$NAME])"/>]</xsl:for-each>
  </xsl:template>

  <!-- xpath to, and value of, attribute -->
  <xsl:template name="attributeXPath">    
    <xsl:for-each select="ancestor::*"><xsl:variable name="NAME" select="name()"/>/<xsl:value-of select="name()"/>[<xsl:value-of select="1+count(preceding-sibling::*[name()=$NAME])"/>]</xsl:for-each>/@<xsl:value-of select="name()"/><xsl:text> "</xsl:text><xsl:value-of select="."/><xsl:text>"
</xsl:text>
  </xsl:template>

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <!-- elements -->
  <xsl:template match="*">
    <xsl:apply-templates select="*|@*"/>
  </xsl:template>

  <!-- attributes -->
  <xsl:template match="@*">
    <xsl:call-template name="attributeXPath"/>
  </xsl:template>

</xsl:transform>
