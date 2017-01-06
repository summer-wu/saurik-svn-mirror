<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:lxslt="http://xml.apache.org/xslt"
    xmlns:xhtml="http://www.w3.org/1999/xhtml"
    xmlns:cyte="http://www.wikicyte.com/2009/cyte"
    exclude-result-prefixes="cyte lxslt xhtml"
>

<xsl:template match="@*|node()" mode="cyte">
    <xsl:call-template name="cyte:copy"/>
</xsl:template>

<xsl:template name="cyte:class">
    <xsl:param name="class"/>

    <xsl:if test="$class and $class != '' or @class">
        <xsl:attribute name="class">
            <xsl:if test="$class and $class != ''">
                <xsl:value-of select="$class"/>
            </xsl:if>

            <xsl:if test="$class and $class != '' and @class">
                <xsl:text> </xsl:text>
            </xsl:if>

            <xsl:if test="@class">
                <xsl:value-of select="@class"/>
            </xsl:if>
        </xsl:attribute>
    </xsl:if>
</xsl:template>

<xsl:template match="xhtml:p[preceding-sibling::xhtml:*[1][self::xhtml:p]]" mode="cyte">
    <xsl:copy>
        <xsl:call-template name="cyte:class">
            <xsl:with-param name="class" select="'second'"/>
        </xsl:call-template>

        <xsl:apply-templates select="@*[local-name() != 'class']|node()" mode="cyte"/>
    </xsl:copy>
</xsl:template>

<xsl:template match="xhtml:p[xhtml:address or xhtml:ul]" mode="cyte">
    <xsl:apply-templates select="@*|node()" mode="cyte"/>
</xsl:template>

<xsl:template match="xhtml:h1|xhtml:h2|xhtml:h3|xhtml:h4|xhtml:h5|xhtml:h6" mode="cyte">
    <xsl:copy>
        <xsl:if test="not(preceding-sibling::*)">
            <xsl:attribute name="class">first</xsl:attribute>
        </xsl:if>

        <xsl:apply-templates select="@*|node()" mode="cyte"/>
    </xsl:copy>
</xsl:template>

<xsl:template name="cyte:body">
    <xsl:apply-templates select="/xhtml:div/xhtml:ul[@class='menucyte']" mode="cyte"/>
    <xsl:apply-templates select="/xhtml:div/xhtml:div[@class='wikicyte']/node()" mode="cyte"/>

    <!--xsl:apply-templates select="/xhtml:div/node()[not(self::cyte:*) and not(self::xhtml:a[@class = 'crumb'])]" mode="cyte"/-->
</xsl:template>

<xsl:template match="xhtml:ul" mode="cyte">
    <xsl:copy>
        <xsl:variable name="level">
            <xsl:value-of select="count(ancestor::xhtml:ul)"/>
        </xsl:variable>

        <xsl:variable name="depth">
            <xsl:for-each select=".//xhtml:ul">
                <xsl:sort select="count(ancestor::xhtml:ul)" data-type="number" order="descending"/>
                <xsl:if test="position() = 1">
                    <xsl:value-of select="count(ancestor::xhtml:ul)"/>
                </xsl:if>
            </xsl:for-each>
        </xsl:variable>

        <xsl:attribute name="class">
            <xsl:text>level-</xsl:text>
            <xsl:choose>
                <xsl:when test="$level != ''">
                    <xsl:value-of select="$level"/>
                </xsl:when>

                <xsl:otherwise>0</xsl:otherwise>
            </xsl:choose>

            <xsl:if test="$depth != ''">
                <xsl:text> depth-_</xsl:text>
            </xsl:if>

            <xsl:text> depth-</xsl:text>
            <xsl:choose>
                <xsl:when test="$depth != ''">
                    <xsl:value-of select="$depth"/>
                </xsl:when>

                <xsl:otherwise>0</xsl:otherwise>
            </xsl:choose>

            <xsl:if test="@class">
                <xsl:text> </xsl:text>
                <xsl:value-of select="@class"/>
            </xsl:if>
        </xsl:attribute>

        <xsl:apply-templates select="@*[local-name() != 'class']|node()" mode="cyte"/>
    </xsl:copy>
</xsl:template>

<xsl:template name="cyte:copy">
    <xsl:copy>
        <xsl:apply-templates select="@*|node()" mode="cyte"/>
    </xsl:copy>
</xsl:template>

</xsl:stylesheet>
