<?xml version="1.0"?>

<!-- Menes - C++ High-Level Utility Library
   - Copyright (C) 2004  Jay Freeman (saurik)
-->

<!--
   -        Redistribution and use in source and binary
   - forms, with or without modification, are permitted
   - provided that the following conditions are met:
   -
   - 1. Redistributions of source code must retain the
   -    above copyright notice, this list of conditions
   -    and the following disclaimer.
   - 2. Redistributions in binary form must reproduce the
   -    above copyright notice, this list of conditions
   -    and the following disclaimer in the documentation
   -    and/or other materials provided with the
   -    distribution.
   - 3. The name of the author may not be used to endorse
   -    or promote products derived from this software
   -    without specific prior written permission.
   -
   - THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS''
   - AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
   - BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   - MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   - ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
   - LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   - EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   - NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   - SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   - INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   - LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
   - TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
   - ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
   - ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-->

<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:fn="http://www.w3.org/2003/11/xpath-functions"
    xmlns:xdt="http://www.w3.org/2003/11/xpath-datatypes"
    xmlns:xpp="http://www.saurik.com/menes/xpp/0.9"
    xmlns:bnf="http://www.saurik.com/menes/bnf/0.9"
    exclude-result-prefixes="xsl xs xsi fn xdt xpp"
>

<xsl:include href="../../menes/bnf/utility.xsl"/>
<xsl:include href="../../menes/xpp/sequence.xsl"/>

<xsl:function name="bnf:preceding-non-tokens">
    <xsl:param name="token"/>

    <xsl:variable name="preceding" select="$token/preceding-sibling::*[last()]"/>

    <xsl:variable name="before" select="bnf:is-match-token($preceding)"/>
    <xsl:variable name="after" select="bnf:is-match-token($token)"/>

    <xsl:choose>
        <xsl:when test="count($token) eq 0"/>

        <xsl:when test="not($before) and $after">
            <xsl:sequence select="bnf:preceding-non-tokens($preceding)"/>
        </xsl:when>

        <xsl:when test="$before and not($after)">
            <xsl:sequence select="$token"/>
        </xsl:when>

        <xsl:when test="not($before) and not($after)">
            <xsl:sequence select="$token | bnf:preceding-non-tokens($preceding)"/>
        </xsl:when>
    </xsl:choose>
</xsl:function>

<!-- XXX: the leader of the output should have an unlambda for the next tokenizer and a lambda for the next boogle -->

<xsl:template match="bnf:rule">
    <xsl:variable name="rule" select="."/>

    <xsl:copy>
        <xsl:apply-templates select="@*"/>
        <xsl:copy-of select="*[not(self::bnf:production)]"/>
    </xsl:copy>

    <xsl:for-each select="bnf:production">
        <xsl:choose>
            <xsl:when test="@tokenizer != '' and (some $token in * satisfies bnf:is-match-token($token))">
                <xsl:variable name="group" select="position()"/>
                <xsl:variable name="last" select="*[last()]"/>
                <xsl:variable name="firsthit" select="*[not(bnf:is-lambda-token($rule/.., .))][1]"/>
                <xsl:variable name="tokenizer" select="@tokenizer"/>

                <xsl:for-each-group select="*" group-starting-with="*[not(bnf:is-lambda-token($rule/.., .))]">
                    <xsl:variable name="islast" select="current-group()[last()] is $last"/>
                    <xsl:variable name="isfirst" select="current-group()[1] is $firsthit"/>
                    <xsl:variable name="isprefix" select="position() eq 1 and not($isfirst)"/>
                    <xsl:variable name="isafterfirst" select="not($isprefix or $isfirst)"/>

                    <xsl:variable name="name" select="concat($rule/@name, '{', string($group), '-', string(position() - 1), '}')"/>
                    <xsl:variable name="next" select="concat(../../@name, '{', string($group), '-', string(position()), '}')"/>

                    <xsl:variable name="position" select="position()"/>
                    <xsl:variable name="current-group" select="current-group()"/>
    
                    <xsl:for-each select="
                        if (not($isafterfirst)) then
                            false()
                        else if (current-group()[1]/preceding-sibling::*[bnf:is-unlambda-token($rule/.., .)]) then
                            true()
                        else (true(), false())
                    ">
                        <xsl:variable name="suffix" select="if (.) then 'S' else 'N'"/>

                        <bnf:rule name="{if ($position eq 1) then $rule/@name else concat($name, '/', $suffix)}" bnf:tokenized="{.}">
                            <xsl:if test="not($isprefix) or $islast">
                                <bnf:production>
                                    <xsl:if test=".">
                                        <bnf:call name="{$tokenizer}"/>
                                    </xsl:if>

                                    <xsl:choose>
                                        <xsl:when test="not($isfirst and $islast)">
                                            <xsl:copy-of select="bnf:unlambda-token($rule/.., $current-group[1])"/>
                                            <xsl:copy-of select="$current-group[position() > 1]"/>
                                        </xsl:when>

                                        <xsl:otherwise>
                                            <xsl:copy-of select="$current-group"/>
                                        </xsl:otherwise>
                                    </xsl:choose>

                                    <xsl:if test="not($islast)">
                                        <bnf:call name="{$next}/S"/>
                                    </xsl:if>
                                </bnf:production>
                            </xsl:if>

                            <xsl:if test="
                                not(bnf:is-unlambda-token($rule/.., $current-group[1])) and
                                not($isfirst and $islast)
                            ">
                                <bnf:production>
                                    <xsl:copy-of select="bnf:lambda-token($rule/.., $current-group[1])"/>
                                    <xsl:copy-of select="$current-group[position() > 1]"/>

                                    <xsl:if test="not($islast)">
                                        <bnf:call name="{$next}/{$suffix}"/>
                                    </xsl:if>
                                </bnf:production>
                            </xsl:if>
                        </bnf:rule>
                    </xsl:for-each>
                </xsl:for-each-group>
            </xsl:when>

            <xsl:otherwise>
                <bnf:rule name="{$rule/@name}">
                    <xsl:apply-templates select="."/>
                </bnf:rule>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:for-each>
</xsl:template>

<xsl:template match="bnf:production/@tokenizer"/>

<xsl:template match="@*|node()">
    <xsl:copy>
        <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
</xsl:template>

</xsl:stylesheet>
