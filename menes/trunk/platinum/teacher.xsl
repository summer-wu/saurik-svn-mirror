<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text"/>
<xsl:include href="strings.xsl"/>

<xsl:template name="split-class">
    <xsl:param name="classes"/>
    <xsl:variable name="rest" select="substring($classes, 10)"/>

    <xsl:text>"</xsl:text>
    <xsl:call-template name="trim-right">
        <xsl:with-param name="value" select="substring($classes, 1, 5)"/>
    </xsl:call-template>
    <xsl:text>" "</xsl:text>
    <xsl:call-template name="trim-right">
        <xsl:with-param name="value" select="substring($classes, 7, 2)"/>
    </xsl:call-template>
    <xsl:text>"</xsl:text>

    <xsl:if test="normalize-space($rest) != ''">
        <xsl:text> </xsl:text>
        <xsl:call-template name="split-class">
            <xsl:with-param name="classes" select="$rest"/>
        </xsl:call-template>
    </xsl:if>
</xsl:template>

<xsl:template match="/">
    <xsl:for-each select="//table[@width = '400']/tr[(position() + 1) mod 3 = 0 and not(td/b/input)]">
        <xsl:variable name="classes" select="substring(following-sibling::tr[1]/td[2]/font/b, 2)"/>

        <xsl:text>I "</xsl:text>
        <xsl:call-template name="trim-right">
            <xsl:with-param name="value" select="td[1]/font/b/a/b/a"/>
        </xsl:call-template>
        <xsl:text>" "</xsl:text>
        <xsl:call-template name="trim-right">
            <xsl:with-param name="value" select="substring(td[2]/font/b/a/b/a, 2)"/>
        </xsl:call-template>
        <xsl:text>" {</xsl:text>
        <xsl:if test="normalize-space($classes) != ''">
            <xsl:call-template name="split-class">
                <xsl:with-param name="classes" select="$classes"/>
            </xsl:call-template>
        </xsl:if>
        <xsl:text>}</xsl:text>
        <xsl:call-template name="crlf"/>
    </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
