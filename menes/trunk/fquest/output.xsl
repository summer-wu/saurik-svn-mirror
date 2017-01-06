<?xml version="1.0"?>
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
>

<xsl:output method="text"/>
<xsl:include href="strings.xsl"/>

<xsl:template name="transactions">
    <xsl:param name="pend" as="xs:boolean"/>

    <xsl:for-each select="tr[not(@class = 'statement-heading-chk') and count(td) ne 1 and not(.//div[@class = 'statementcolmn3'])]">
        <xsl:choose>
            <xsl:when test="$pend">
                <xsl:text>--/--/----</xsl:text>
            </xsl:when>

            <xsl:otherwise>
                <xsl:value-of select="(.//div[@class = 'statementcolmn1']/span)[1]"/>
            </xsl:otherwise>
        </xsl:choose>

        <xsl:text> </xsl:text>

        <xsl:choose>
            <xsl:when test="(.//div[@class = 'statementcolmn2'])[1]/span">
                <xsl:text>-</xsl:text>
            </xsl:when>

            <xsl:otherwise>
                <xsl:text>+</xsl:text>
            </xsl:otherwise>
        </xsl:choose>

        <xsl:value-of select="replace(substring((.//div[@class = 'statementcolmn2']/span)[1], 2), ',', '')"/>
        <xsl:text> "</xsl:text>

        <xsl:call-template name="trim-right">
            <xsl:with-param name="value">
                <xsl:choose>
                    <xsl:when test="$pend">
                        <xsl:value-of select=".//div[@class = 'statementcolmn4']/span"/>
                    </xsl:when>

                    <xsl:otherwise>
                        <xsl:value-of select="(.//div[@class = 'statementcolmn1']/span/a)[1]"/>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:with-param>
        </xsl:call-template>

        <xsl:text>"</xsl:text>
        <xsl:call-template name="crlf"/>
    </xsl:for-each>
</xsl:template>

<xsl:template match="/">
    <xsl:for-each select="//table[@title = 'Pending Transactions']">
        <xsl:call-template name="transactions">
            <xsl:with-param name="pend" select="true()"/>
        </xsl:call-template>
    </xsl:for-each>

    <xsl:for-each select="//table[@title = 'Posted Transactions']">
        <xsl:call-template name="transactions">
            <xsl:with-param name="pend" select="false()"/>
        </xsl:call-template>
    </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
