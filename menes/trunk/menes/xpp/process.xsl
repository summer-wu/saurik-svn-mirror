<?xml version="1.0"?>
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:op="http://www.w3.org/2002/08/xquery-operators"
    xmlns:xf="http://www.w3.org/2002/08/xquery-functions"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:fn="http://www.w3.org/2003/11/xpath-functions"
    xmlns:xdt="http://www.w3.org/2003/11/xpath-datatypes"
    xmlns:xpp="http://www.saurik.com/menes/xpp/0.9"
    exclude-result-prefixes="xsl xs xsi fn xdt xpp"
>

<xsl:template match="xpp:attribute"/>
<xsl:template match="xpp:variable"/>

<xsl:template match="xpp:scope">
    <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xpp:value-of">
    <xsl:if test="substring(@select, 1, 1) ne '$'">
        <xsl:message select="'xpp:value-of/@select is still limited to a variable name'"/>
    </xsl:if>

    <xsl:variable name="name" select="substring(@select, 2)"/>

    <xsl:apply-templates select="ancestor::xpp:scope/child::xpp:variable[@name = $name][last()]/*"/>
</xsl:template>

<xsl:template match="@*|node()">
    <xsl:copy>
        <xsl:for-each select="xpp:attribute">
            <xsl:attribute name="{@name}">
                <xsl:value-of select="."/>
            </xsl:attribute>
        </xsl:for-each>

        <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
</xsl:template>

</xsl:stylesheet>
