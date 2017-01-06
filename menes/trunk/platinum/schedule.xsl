<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text"/>
<xsl:include href="strings.xsl"/>

<xsl:template match="/">
    <xsl:for-each select="//tr[translate(td/@bgcolor, 'GOLD', 'gold') = 'gold']">
        <xsl:variable name="head" select="current()"/>
        <xsl:variable name="class" select="td/font/b"/>
        <xsl:variable name="course" select="substring($class, 9)"/>

        <xsl:text>H "</xsl:text>
        <xsl:call-template name="trim-right">
            <xsl:with-param name="value" select="substring($class, 1, 5)"/>
        </xsl:call-template>
        <xsl:text>" "</xsl:text>
        <xsl:call-template name="trim-right">
            <xsl:with-param name="value" select="substring($class, 6, 2)"/>
        </xsl:call-template>
        <xsl:text>" "</xsl:text>
        <xsl:call-template name="trim-left">
            <xsl:with-param name="value" select="substring-before($course, ' - ')"/>
        </xsl:call-template>
        <xsl:text>" "</xsl:text>
        <xsl:call-template name="trim-right">
            <xsl:with-param name="value" select="substring-after($course, ' - ')"/>
        </xsl:call-template>
        <xsl:text>"</xsl:text>
        <xsl:call-template name="crlf"/>

        <xsl:for-each select="following-sibling::tr[
            translate(td/@bgcolor, 'GOLD', 'gold') != 'gold' and
            normalize-space(string()) != '' and
            preceding-sibling::tr[translate(td/@bgcolor, 'GOLD', 'gold') = 'gold'][1] = $head and
            count(td) > 1 and
            td[1]/font != '-----' and
            normalize-space(td[1]/font) != 'ENRL CODE'
        ]">
            <xsl:variable name="time" select="td[3]/font/b"/>

            <xsl:text>R "</xsl:text>
            <xsl:value-of select="normalize-space(td[1]/font)"/>
            <xsl:text>" "</xsl:text>
            <xsl:value-of select="normalize-space(substring(td[2]/font, 1, 3))"/>
            <xsl:text>" "</xsl:text>
            <xsl:value-of select="normalize-space(substring(td[2]/font, 5))"/>
            <xsl:text>" </xsl:text>

            <xsl:choose>
                <xsl:when test="normalize-space($time) = 'T B A'">
                    <xsl:text>"" ""</xsl:text>
                </xsl:when>

                <xsl:otherwise>
                    <xsl:text>"</xsl:text>
                    <xsl:value-of select="normalize-space(substring(td[3]/font/b, 1, 4))"/>
                    <xsl:text>" "</xsl:text>
                    <xsl:value-of select="normalize-space(substring(td[3]/font/b, 6, 5))"/>
                    <xsl:text>"</xsl:text>
                </xsl:otherwise>
            </xsl:choose>
            
            <xsl:text> "</xsl:text>
            <xsl:call-template name="trim-right">
                <xsl:with-param name="value" select="td[4]/font"/>
            </xsl:call-template>
            <xsl:text>" "</xsl:text>
            <xsl:call-template name="trim-right">
                <xsl:with-param name="value" select="substring(td[5]/font, 1, 6)"/>
            </xsl:call-template>
            <xsl:text>" "</xsl:text>
            <xsl:call-template name="trim">
                <xsl:with-param name="value" select="substring(td[5]/font, 7)"/>
            </xsl:call-template>
            <xsl:text>" "</xsl:text>
            <xsl:value-of select="normalize-space(td[6]/font/b)"/>
            <xsl:text>" "</xsl:text>
            <xsl:value-of select="normalize-space(td[7]/font)"/>
            <xsl:text>"</xsl:text>
            <xsl:call-template name="crlf"/>
        </xsl:for-each>
    </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
