<?xml version="1.0"?>
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:xxc="http://www.saurik.com/menes/xxc/0.9"
>

<xsl:output version="1.1"/>

<xsl:function name="xxc:rtl-operate" as="element()">
    <xsl:param name="lhs"/>
    <xsl:param name="rhs"/>

    <xsl:choose>
        <xsl:when test="$rhs">
            <binary name="{$rhs[1]/*[ends-with(local-name(), '-operator')]}">
                <lhs>
                    <xsl:apply-templates select="$lhs"/>
                </lhs>

                <rhs>
                    <xsl:sequence select="
                        xxc:rtl-operate(
                            $rhs[1]/*[ends-with(local-name(), '-expression')],
                            subsequence($rhs, 2)
                        )
                    "/>
                </rhs>
            </binary>
        </xsl:when>

        <xsl:otherwise>
            <xsl:apply-templates select="$lhs"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="xxc:ltr-operate" as="element()">
    <xsl:param name="lhs"/>
    <xsl:param name="rhs"/>

    <xsl:choose>
        <xsl:when test="$rhs">
            <binary name="{$rhs[last()]/*[ends-with(local-name(), '-operator')]}">
                <lhs>
                    <xsl:sequence select="
                        xxc:ltr-operate(
                            $lhs,
                            subsequence($rhs, 1, count($rhs) - 1)
                        )
                    "/>
                </lhs>

                <rhs>
                    <xsl:apply-templates select="$rhs[last()]/*[ends-with(local-name(), '-expression')]"/>
                </rhs>
            </binary>
        </xsl:when>

        <xsl:otherwise>
            <xsl:apply-templates select="$lhs"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="xxc:ternary-operate" as="element()">
    <xsl:param name="lhs"/>
    <xsl:param name="rhs"/>

    <xsl:choose>
        <xsl:when test="$rhs">
            <condition>
                <test>
                    <xsl:sequence select="
                        xxc:ltr-operate(
                            $lhs,
                            subsequence($rhs, 1, count($rhs) - 1)
                        )
                    "/>
                </test>

                <then>
                    <xsl:apply-templates select="$rhs[last()]/*[ends-with(local-name(), '-expression')][1]"/>
                </then>

                <else>
                    <xsl:apply-templates select="$rhs[last()]/*[ends-with(local-name(), '-expression')][2]"/>
                </else>
            </condition>
        </xsl:when>

        <xsl:otherwise>
            <xsl:apply-templates select="$lhs"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:template match="*[ends-with(local-name(), '-expression') and not(local-name() eq 'atomic-expression')]">
    <xsl:sequence select="
        if (local-name() eq 'condition-expression')
            then xxc:ternary-operate(*[ends-with(local-name(), '-expression')], *[ends-with(local-name(), '-operand')])
            else if (local-name() eq 'assignment-expression')
                then xxc:rtl-operate(*[ends-with(local-name(), '-expression')], *[ends-with(local-name(), '-operand')])
                else xxc:ltr-operate(*[ends-with(local-name(), '-expression')], *[ends-with(local-name(), '-operand')])
    "/>
</xsl:template>

<xsl:template match="@*|node()">
    <xsl:copy>
        <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
</xsl:template>

</xsl:stylesheet>
