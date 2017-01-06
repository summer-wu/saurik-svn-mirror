<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
>

<xsl:template name="lf">
    <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template name="string-replace">
    <xsl:param name="string"/>
    <xsl:param name="from"/>
    <xsl:param name="to"/>
                                                                                                                                                             
    <xsl:choose>
        <xsl:when test="contains($string, $from)">
            <xsl:value-of select="substring-before($string, $from)"/>
            <xsl:value-of select="$to"/>

            <xsl:call-template name="string-replace">
                <xsl:with-param name="string" select="substring-after($string, $from)"/>
                <xsl:with-param name="from" select="$from"/>
                <xsl:with-param name="to" select="$to"/>
            </xsl:call-template>
        </xsl:when>

        <xsl:otherwise>
            <xsl:value-of select="$string"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

</xsl:stylesheet>
