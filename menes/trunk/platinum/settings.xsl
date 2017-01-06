<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text"/>

<xsl:template name="crlf"><xsl:text>
</xsl:text></xsl:template>

<xsl:template match="/">
    <xsl:text>#!/usr/bin/env bash</xsl:text>
    <xsl:call-template name="crlf"/>

    <xsl:text>set -e</xsl:text>
    <xsl:call-template name="crlf"/>
    <xsl:call-template name="crlf"/>

    <xsl:variable name="a" select="//a[contains(@href, '/sendkeys/')]"/>

    <xsl:text>export SESSIONKEY='</xsl:text>
    <xsl:choose>
        <xsl:when test="//input[@name = 'SessionKey']">
            <xsl:value-of select="(//input[@name = 'SessionKey'])[1]/@value"/>
        </xsl:when>

        <xsl:when test="$a">
            <xsl:value-of select="substring-before(substring-after($a/@href, '/sendkeys/'), '/')"/>
        </xsl:when>
    </xsl:choose>
    <xsl:text>'</xsl:text>
    <xsl:call-template name="crlf"/>

    <xsl:text>export SCREENID='</xsl:text>
    <xsl:choose>
        <xsl:when test="//input[@name = 'SCREEN_ID']">
            <xsl:value-of select="(//input[@name = 'SCREEN_ID'])[1]/@value"/>
        </xsl:when>

        <xsl:when test="$a">
            <xsl:value-of select="substring-before(substring-after(substring-after(substring-after($a/@href, '/sendkeys/'), '/'), '/'), '/')"/>
        </xsl:when>
    </xsl:choose>
    <xsl:text>'</xsl:text>
    <xsl:call-template name="crlf"/>

    <xsl:text>export TITLETAG='</xsl:text>
    <xsl:value-of select="(//title)[1]"/>
    <xsl:text>'</xsl:text>
    <xsl:call-template name="crlf"/>
</xsl:template>

</xsl:stylesheet>
