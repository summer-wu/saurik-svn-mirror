<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text"/>
<xsl:include href="strings.xsl"/>

<xsl:template match="/">
    <xsl:for-each select="//table[@width = '400']/tr[position() != 1 and td/a/b/a != '']">
        <xsl:text>S "</xsl:text>
        <xsl:call-template name="trim-right">
            <xsl:with-param name="value" select="substring(td[1]/a/b/a, 1, 5)"/>
        </xsl:call-template>
        <xsl:text>" "</xsl:text>
        <xsl:call-template name="trim-right">
            <xsl:with-param name="value" select="substring(td[1]/a/b/a, 6)"/>
        </xsl:call-template>
        <xsl:text>" "</xsl:text>
        <xsl:call-template name="trim-right">
            <xsl:with-param name="value" select="td[2]/a/b/a"/>
        </xsl:call-template>
        <xsl:text>"</xsl:text>
        <xsl:call-template name="crlf"/>
    </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
