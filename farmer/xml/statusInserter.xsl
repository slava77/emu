<?xml version='1.0' encoding='UTF-8'?>
<xsl:stylesheet version='1.0' xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>
  <xsl:output method='xml' indent='yes'/>

  <xsl:param name='HOST'/>
  <xsl:param name='PORT'/>
  <xsl:param name='URN'/>

  <xsl:param name='SELECTED'/>
  <xsl:param name='STATE'/>

  <xsl:template match='*[@hostname=$HOST and @port=$PORT and @urn=$URN]'>
    <xsl:element name="{name()}">
      <xsl:for-each select="@*">
	<xsl:attribute name="{name()}"><xsl:value-of select="."/></xsl:attribute>
      </xsl:for-each>
      <status selected="{$SELECTED}" state="{$STATE}"/>
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>

  <xsl:template match='*|@*|text()'>
    <xsl:copy>
      <xsl:apply-templates select='*|@*|text()'/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
