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

<xsl:variable name="grammar" select="/bnf:grammar"/>

<xsl:template name="produce">
    <xsl:param name="tokens"/>
    <xsl:param name="before" select="()"/>

    <xsl:variable name="token" select="$tokens[1]"/>

    <xsl:copy>
        <xsl:copy-of select="@*"/>

        <xsl:for-each select="$before">
            <xsl:message select="."/>
            <xsl:copy-of select="bnf:lambda-token($grammar, .)"/>
        </xsl:for-each>

        <xsl:if test="count($tokens) ne 0">
            <xsl:copy-of select="bnf:unlambda-token($grammar, $token)"/>
            <xsl:copy-of select="$token/following-sibling::*"/>
        </xsl:if>
    </xsl:copy>

    <xsl:if test="count($tokens) ne 0 and not(bnf:is-unlambda-token(../.., $token))">
        <xsl:call-template name="produce">
            <xsl:with-param name="tokens" select="$tokens[position() > 1]"/>
            <xsl:with-param name="before" select="$before, $tokens[1]"/>
        </xsl:call-template>
    </xsl:if>
</xsl:template>

<xsl:template match="bnf:production">
    <xsl:variable name="edges" select="*[bnf:is-match-token(.)]"/>

    <xsl:choose>
        <xsl:when test="count($edges) eq 0">
            <xsl:next-match/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:call-template name="produce">
                <xsl:with-param name="tokens" select="$edges"/>
            </xsl:call-template>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template match="@*|node()">
    <xsl:copy>
        <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
</xsl:template>

</xsl:stylesheet>
