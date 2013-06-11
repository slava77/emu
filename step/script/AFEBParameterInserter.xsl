<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes"/>

  <xsl:param name="CHAMBERLABEL"/>
  <xsl:param name="AFEBNUMBER"/>
  <xsl:param name="AFEBTHRESHOLD"/>
  <xsl:param name="AFEBFINEDELAY"/>

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <!-- Copy everything except text() since VME config XML tags have no text child nodes, only attributes -->
  <xsl:template match="*|@*|comment()|processing-instruction()">
    <xsl:copy >
      <xsl:apply-templates select="*|@*|comment()"/>
    </xsl:copy>
  </xsl:template>

  <!-- Change afeb_threshold when and where appropriate -->
  <xsl:template match="@afeb_threshold">
    <xsl:choose>
      <xsl:when test="$AFEBTHRESHOLD!='' and ../../../../@label=$CHAMBERLABEL and ../@afeb_number=$AFEBNUMBER">
	<xsl:attribute name="{name(.)}"><xsl:value-of select="$AFEBTHRESHOLD"/></xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
	<xsl:copy/>
      </xsl:otherwise>    
    </xsl:choose>
  </xsl:template>

  <!-- Change afeb_fine_delay when and where appropriate -->
  <xsl:template match="@afeb_fine_delay">
    <xsl:choose>
      <xsl:when test="$AFEBFINEDELAY!='' and ../../../../@label=$CHAMBERLABEL and ../@afeb_number=$AFEBNUMBER">
	<xsl:attribute name="{name(.)}"><xsl:value-of select="$AFEBFINEDELAY"/></xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
	<xsl:copy/>
      </xsl:otherwise>    
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>
