<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="crlf"><xsl:text>
</xsl:text></xsl:template>

<xsl:template name="trim-left">
    <xsl:param name="value"/>

    <xsl:choose>
        <xsl:when test="starts-with($value, ' ')">
            <xsl:call-template name="trim-left">
                <xsl:with-param name="value" select="substring($value, 2)"/>
            </xsl:call-template>
        </xsl:when>

        <xsl:otherwise><xsl:value-of select="$value"/></xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="trim-right">
    <xsl:param name="value"/>
    <xsl:variable name="length" select="string-length($value)"/>

    <xsl:choose>
        <xsl:when test="substring($value, $length) = ' '">
            <xsl:call-template name="trim-right">
                <xsl:with-param name="value" select="substring($value, 1, $length - 1)"/>
            </xsl:call-template>
        </xsl:when>

        <xsl:otherwise><xsl:value-of select="$value"/></xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="trim">
    <xsl:param name="value"/>

    <xsl:call-template name="trim-left">
        <xsl:with-param name="value"><xsl:call-template name="trim-right">
            <xsl:with-param name="value" select="$value"/>
        </xsl:call-template></xsl:with-param>
    </xsl:call-template>
</xsl:template>

</xsl:stylesheet>
