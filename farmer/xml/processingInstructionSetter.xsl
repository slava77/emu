<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes"/>

  <xsl:param name="XSLURI"/>

  <xsl:template match="/">
    <xsl:processing-instruction name="xml-stylesheet">type='text/xml' href='<xsl:value-of select="$XSLURI"/>'</xsl:processing-instruction>
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="*|@*|text()|comment()">
    <xsl:copy >
      <xsl:apply-templates select="*|@*|text()|comment()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
