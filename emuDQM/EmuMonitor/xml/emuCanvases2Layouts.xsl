<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  xmlns:fn="http://www.w3.org/2005/02/xpath-functions">

<xsl:template match="/">
def csclayout(i, p, *rows): i["Layouts/CSC Layouts/" + p] = DQMItem(layout=rows)
<xsl:apply-templates select="Canvases/Canvas"/>
</xsl:template>

<xsl:template match="Canvas">
csclayout(dqmitems,"<xsl:value-of select="Title"/>",
<xsl:variable name="prefix"><xsl:if test="string-length(Prefix) > 0"><xsl:value-of select="Prefix"/>/</xsl:if></xsl:variable>
<xsl:for-each select="./*[substring(name(),1,3) = 'Pad' and number(substring(name(),4))][position() = 1]">
  ["<xsl:value-of select="$prefix"/><xsl:value-of select="."/>"]
</xsl:for-each>
<xsl:for-each select="./*[substring(name(),1,3) = 'Pad' and number(substring(name(),4))][position() > 1]">
 ,["<xsl:value-of select="$prefix"/><xsl:value-of select="."/>"]
</xsl:for-each>
)
</xsl:template>

</xsl:stylesheet>
