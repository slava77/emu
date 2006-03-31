<?xml version="1.0" encoding="UTF-8"?>

<!DOCTYPE stylesheet [
<!ENTITY i2osch  "http://xdaq.web.cern.ch/xdaq/xsd/2004/I2OConfiguration-30" >
<!ENTITY xcsch   "http://xdaq.web.cern.ch/xdaq/xsd/2004/XMLConfiguration-30" >
<!ENTITY soapsch "http://schemas.xmlsoap.org/soap/encoding/" >
<!ENTITY xsisch  "http://www.w3.org/2001/XMLSchema-instance" >
]>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:soapenc="&soapsch;">

  <xsl:template match="@*|node()">
    <xsl:copy>
      <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template xmlns:xc="&xcsch;" match="xc:Partition">
    <html>
      <head>
	<style type="text/css">
	  body{
	  background-color: #333333;
	  font-size: small
	  }

	  td, th{
	  border-width: 0px 0px 0px 0px;
	  padding: 2px 2px 2px 2px;
	  }

	  table.protocol{
	  background-color: #ffccff
	  }
	  td.protocol{
	  color: #880088;
	  background-color: transparent;
	  }
	  th.protocol{
	  color: #880088;
	  background-color: #ff88ff;
	  }

	  table.context{
	  background-color: #ccccff
	  }
	  td.context{
	  color: #000088;
	  background-color: transparent;
	  }
	  th.context{
	  color: #000088;
	  background-color: #8888ff;
	  }

	  table.app{
	  background-color: #ccffcc
	  }
	  td.app{
	  color: #008800;
	  background-color: transparent;
	  }
	  th.app{
	  color: #008800;
	  background-color: #88ff88;
	  }

	  table.module{
	  background-color: #ffffcc
	  }
	  td.module{
	  color: #888800;
	  background-color: transparent;
	  }
	  th.module{
	  color: #888800;
	  background-color: #ffff88;
	  }

	  table.endp{
	  background-color: #ffffee
	  }
	  td.endp{
	  color: #884400;
	  background-color: transparent;
	  }
	  th.endp{
	  color: #884400;
	  background-color: #ffdd88;
	  }

	  table.alias, table.unicast{
	  background-color: #dddddd
	  }
	  td.alias, td.unicast{
	  color: #000000;
	  background-color: transparent;
	  }
	  th.alias, th.unicast{
	  color: #000000;
	  background-color: #bbbbbb;
	  }

	  table.prop{
	  background-color: #ffeeee
	  }
	  td.prop{
	  color: #880000;
	  background-color: transparent;
	  }
	  th.prop{
	  color: #880000;
	  background-color: #ffccbb;
	  text-align: left
	  }
	</style>
	<title>EmuDAQ Configuration</title>
      </head>
      <body>
        <xsl:apply-templates xmlns:i2o="&i2osch;" select="i2o:protocol"/>
	<xsl:apply-templates xmlns:xc="&xcsch;"   select="xc:Context"/>
      </body>
    </html>
  </xsl:template>

  <xsl:template xmlns:i2o="&i2osch;" match="i2o:protocol">
    <table class="protocol">
      <tr>
	<th class="protocol">
	  <xsl:attribute name="rowspan">
	    <xsl:value-of select="count(child::i2o:target)+1"/>
	  </xsl:attribute> 
	  I2O targets
	</th>
	<th class="protocol">class</th>
	<th class="protocol">instance</th>
	<th class="protocol">tid</th>
      </tr>
      <xsl:for-each select="child::i2o:target">
	<tr>
	  <td class="protocol"><xsl:value-of select="attribute::class"/></td>
	  <td class="protocol"><xsl:value-of select="attribute::instance"/></td>
	  <td class="protocol"><xsl:value-of select="attribute::tid"/></td>
	</tr>
      </xsl:for-each>
    </table>
  </xsl:template>


  <xsl:template xmlns:xc="&xcsch;" match="xc:Context">
    <br/>
    <table class="context">
      <tr>
	<th class="context" style="font-size: medium">
	  <xsl:attribute name="rowspan">
	    <xsl:value-of select="count(child::xc:Application)+count(child::xc:Endpoint)+count(child::xc:Module)+1"/>
	  </xsl:attribute> 
	  <xsl:value-of select="substring-after(attribute::url,'http://')"/>
	</th>
      </tr>
      <xsl:call-template xmlns:xc="&xcsch;" name="xc:Endpoint"/>
      <xsl:call-template xmlns:xc="&xcsch;" name="xc:Alias"/>
      <xsl:apply-templates xmlns:xc="&xcsch;" select="xc:Application"/>
      <xsl:call-template xmlns:xc="&xcsch;" name="xc:Module"/>
    </table>
  </xsl:template>

  <xsl:template xmlns:xc="&xcsch;" match="xc:Application">
    <tr>
      <td class="context">
	<table class="app">
	  <tr>
	    <th class="app" rowspan="4">
	      <xsl:value-of select="attribute::class"/>
	    </th>
	    <th class="app">instance</th>
	    <th class="app">id</th>
	    <th class="app">network</th>
	    <th class="app">resident</th>
	  </tr>
	  <tr>
	    <td class="app" style="text-align: center"><xsl:value-of select="attribute::instance"/></td>
	    <td class="app" style="text-align: center"><xsl:value-of select="attribute::id"/></td>
	    <td class="app" style="text-align: center"><xsl:value-of select="attribute::network"/></td>
	    <td class="app" style="text-align: center"><xsl:value-of select="attribute::resident"/></td>
	  </tr>
	  <xsl:call-template name="xc:Unicast"/>
	  <xsl:for-each select="./*[name()!='xc:Unicast']">
	    <tr>
	      <td class="app" colspan="4">
		<xsl:call-template name="properties"/>
	      </td>
	    </tr>
	  </xsl:for-each>
	</table>
      </td>
    </tr>
  </xsl:template>

  <xsl:template xmlns:xc="&xcsch;" name="xc:Module">
    <xsl:if test="count(child::xc:Module)>0">
      <tr>
	<td class="context">
	  <table class="module">
	    <tr>
	      <th class="module">
		<xsl:attribute name="rowspan">
		  <xsl:value-of select="count(child::xc:Module)+1"/>
		</xsl:attribute>
		Modules
	      </th>
	    </tr>
	    <xsl:for-each select="child::xc:Module">
	      <tr>
		<td class="module">
		  <xsl:value-of select="."/>
		</td>
	      </tr>
	    </xsl:for-each>
	  </table>
	</td>
      </tr>
    </xsl:if>
  </xsl:template>

  <xsl:template xmlns:xc="&xcsch;" name="xc:Endpoint">
    <xsl:if test="count(child::xc:Endpoint)>0">
      <tr>
	<td class="context">
	  <table class="endp">
	    <tr>
	      <th class="endp">
		<xsl:attribute name="rowspan">
		  <xsl:value-of select="count(child::xc:Endpoint)+1"/>
		</xsl:attribute>
		End points
	      </th>
	      <th class="endp">protocol</th>
	      <th class="endp">service</th>
	      <th class="endp">hostname</th>
	      <th class="endp">port</th>
	      <th class="endp">network</th>
	    </tr>
	    <xsl:for-each select="child::xc:Endpoint">
	      <tr>
		<td class="endp" style="text-align: center"><xsl:value-of select="attribute::protocol"/></td>
		<td class="endp" style="text-align: center"><xsl:value-of select="attribute::service"/></td>
		<td class="endp" style="text-align: center"><xsl:value-of select="attribute::hostname"/></td>
		<td class="endp" style="text-align: center"><xsl:value-of select="attribute::port"/></td>
		<td class="endp" style="text-align: center"><xsl:value-of select="attribute::network"/></td>
	      </tr>
	    </xsl:for-each>
	  </table>
	</td>
      </tr>
    </xsl:if>
  </xsl:template>
  
  <xsl:template xmlns:xc="&xcsch;" name="xc:Alias">
    <xsl:if test="count(child::xc:Alias)>0">
      <tr>
	<td class="context">
	  <table class="alias">
	    <tr>
	      <th class="alias">
		<xsl:attribute name="rowspan">
		  <xsl:value-of select="count(child::xc:Alias)+1"/>
		</xsl:attribute>
		Alias
	      </th>
	    </tr>
	    <xsl:for-each xmlns:xc="&xcsch;" select="child::xc:Alias">
	      <tr>
		<th class="alias" style="text-align: left"><xsl:value-of select="attribute::network"/></th>
		<td class="alias" style="text-align: left"><xsl:value-of select="."/></td>
	      </tr>
	    </xsl:for-each>
	  </table>
	</td>
      </tr>
    </xsl:if>
  </xsl:template>

  <xsl:template xmlns:xc="&xcsch;" name="xc:Unicast">
    <xsl:if test="child::xc:Unicast">
      <tr>
	<td class="app" colspan="4">
	  <table class="unicast">
	    <tr>
	      <th class="unicast">
		<xsl:attribute name="rowspan">
		  <xsl:value-of select="count(child::xc:Unicast)+1"/>
		</xsl:attribute>
		Unicast
	      </th>
	      <th class="unicast">class</th>
	      <th class="unicast">instance</th>
	      <th class="unicast">id</th>
	      <th class="unicast">network</th>
	    </tr>
	    <xsl:for-each select="child::xc:Unicast">
	      <tr>
		<td class="unicast" style="text-align: center"><xsl:value-of select="attribute::class"/></td>
		<td class="unicast" style="text-align: center"><xsl:value-of select="attribute::instance"/></td>
		<td class="unicast" style="text-align: center"><xsl:value-of select="attribute::id"/></td>
		<td class="unicast" style="text-align: center"><xsl:value-of select="attribute::network"/></td>
	      </tr>
	    </xsl:for-each>
	  </table>
	</td>
      </tr>
    </xsl:if>
  </xsl:template>

  <xsl:template name="properties">
    <table class="prop" width="100%">
      <xsl:for-each select="./*">
	<tr>
	  <xsl:choose>
	    <xsl:when xmlns:soapenc="&soapsch;" xmlns:xsi="&xsisch;" test="name()='item'">
	      <th class="prop"><xsl:value-of select="attribute::soapenc:position"/></th>
	    </xsl:when>
	    <xsl:otherwise>
	      <th class="prop"><xsl:value-of select="name()"/></th>
	    </xsl:otherwise>
	  </xsl:choose>
	  <xsl:choose>
	    <xsl:when xmlns:soapenc="&soapsch;" xmlns:xsi="&xsisch;" test="starts-with(attribute::xsi:type,'soapenc:Array')">
	      <td class="prop" colspan="2" align="right"><xsl:call-template name="properties"/></td>
	    </xsl:when>
	    <xsl:otherwise>
	      <td class="prop"><xsl:value-of xmlns:xsi="&xsisch;" select="substring-after(attribute::xsi:type,'xsd:')"/></td>
	      <td class="prop" style="text-align:right; font-weight:bold" width="100%"><xsl:value-of select="."/></td>
	    </xsl:otherwise>
	  </xsl:choose>
	</tr>
      </xsl:for-each>
    </table>
  </xsl:template>

</xsl:stylesheet>
