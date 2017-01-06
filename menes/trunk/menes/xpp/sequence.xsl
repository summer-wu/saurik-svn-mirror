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

<xsl:function name="xpp:contains" as="xs:boolean">
    <xsl:param name="set"/>
    <xsl:param name="element"/>

    <xsl:value-of select="
        count($set) ne 0 and (
            deep-equal($set[1], $element) or
            xpp:contains(subsequence($set, 2), $element)
        )
    "/>
</xsl:function>

<xsl:function name="xpp:deep-unique-be">
    <xsl:param name="values"/>
    <xsl:param name="result"/>

    <xsl:if test="count($values) != 0">
        <xsl:choose>
            <xsl:when test="xpp:contains($result, $values[1])">
                <xsl:copy-of select="xpp:deep-unique-be($values[position() > 1], $result)"/>
            </xsl:when>

            <xsl:otherwise>
                <xsl:copy-of select="$values[1]"/>
                <xsl:copy-of select="xpp:deep-unique-be($values[position() > 1], ($values[1], $result))"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:if>
</xsl:function>

<xsl:function name="xpp:deep-unique">
    <xsl:param name="values"/>
    <xsl:copy-of select="xpp:deep-unique-be($values, ())"/>
</xsl:function>

<xsl:function name="xpp:deep-union" as="item()*">
    <xsl:param name="lhs"/>
    <xsl:param name="rhs"/>

    <xsl:choose>
        <xsl:when test="count($rhs) = 0">
            <xsl:copy-of select="$lhs"/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:variable name="result" select="xpp:deep-union($lhs, $rhs[position() != last()])"/>

            <xsl:choose>
                <xsl:when test="xpp:contains($result, $rhs[last()])">
                    <xsl:copy-of select="$result"/>
                </xsl:when>

                <xsl:otherwise>
                    <xsl:copy-of select="$result, $rhs[last()]"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="xpp:except">
    <xsl:param name="lhs"/>
    <xsl:param name="rhs"/>

    <xsl:if test="count($lhs) != 0">
        <xsl:variable name="rest" select="xpp:except($lhs[position() > 1], $rhs)"/>

        <xsl:choose>
            <xsl:when test="xpp:contains($rhs, $lhs[1])">
                <xsl:copy-of select="$rest"/>
            </xsl:when>

            <xsl:otherwise>
                <xsl:copy-of select="$lhs[1], $rest"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:if>
</xsl:function>

</xsl:stylesheet>
