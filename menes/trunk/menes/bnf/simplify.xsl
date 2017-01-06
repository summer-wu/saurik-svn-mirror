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

<xsl:output version="1.1"/>

<xsl:template name="get-tokenizer">
    <xsl:choose>
        <xsl:when test="@bnf:tokenizer">
            <xsl:value-of select="@bnf:tokenizer"/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:for-each select="..">
                <xsl:call-template name="get-tokenizer"/>
            </xsl:for-each>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="add-tokenizer">
    <xsl:attribute name="tokenizer">
        <xsl:call-template name="get-tokenizer"/>
    </xsl:attribute>
</xsl:template>

<xsl:function name="bnf:get-identifier" as="xs:string">
    <xsl:param name="token" as="element()"/>

    <xsl:value-of select="concat($token/ancestor::bnf:define/@name, '{simple:', generate-id($token), '}')"/>
</xsl:function>

<xsl:template match="*[not(self::bnf:*)] | bnf:block | bnf:multiple | bnf:optional | bnf:or | bnf:repeated" mode="define">
    <bnf:call name="{bnf:get-identifier(.)}">
        <xsl:apply-templates select="@bnf:*[local-name() != 'tokenizer']" mode="define"/>
    </bnf:call>
</xsl:template>

<xsl:template match="bnf:assert" mode="define">
    <bnf:call name="{bnf:get-identifier(.)}"/>
</xsl:template>

<xsl:template match="bnf:ignore" mode="define">
    <bnf:call name="{bnf:get-identifier(.)}"/>
</xsl:template>

<xsl:template match="bnf:not" mode="define">
    <bnf:call name="{bnf:get-identifier(.)}"/>
</xsl:template>

<xsl:template match="bnf:reverse" mode="define">
    <bnf:call name="{bnf:get-identifier(.)}"/>
</xsl:template>

<!--xsl:template match="*[not(self::bnf:*)]" mode="define">
    <bnf:open local="{local-name()}" space="{namespace-uri()}"/>
    <xsl:apply-templates mode="define"/>
    <bnf:close/>
</xsl:template-->

<xsl:template match="@bnf:tokenizer" mode="define"/>

<xsl:template match="bnf:rule" mode="define">
    <xsl:copy>
        <xsl:call-template name="scope"/>
        <xsl:apply-templates select="@*|node()" mode="define"/>
    </xsl:copy>
</xsl:template>

<xsl:template match="@*|node()" mode="define">
    <xsl:copy>
        <xsl:apply-templates select="@*|node()" mode="define"/>
    </xsl:copy>
</xsl:template>

<xsl:template name="least-repeated">
    <xsl:param name="left"/>

    <xsl:if test="$left ne 0">
        <bnf:call name="{bnf:get-identifier(.)}{{repeat}}"/>

        <xsl:if test="$left ne 1 and @separator != ''">
            <bnf:call name="{@separator}"/>
        </xsl:if>

        <xsl:call-template name="least-repeated">
            <xsl:with-param name="left" select="$left - 1"/>
        </xsl:call-template>
    </xsl:if>
</xsl:template>

<xsl:template name="most-repeated">
    <xsl:param name="left"/>

    <bnf:production bnf:simplified="repeated">
        <xsl:call-template name="add-tokenizer"/>

        <xsl:call-template name="least-repeated">
            <xsl:with-param name="left" select="$left"/>
        </xsl:call-template>
    </bnf:production>

    <xsl:if test="$left != 0">
        <xsl:call-template name="most-repeated">
            <xsl:with-param name="left" select="$left - 1"/>
        </xsl:call-template>
    </xsl:if>
</xsl:template>

<xsl:template name="scope">
    <xsl:attribute name="bnf:scope">
        <xsl:value-of select="string-join(for $scope in ancestor-or-self::* return generate-id($scope), '/')"/>
    </xsl:attribute>
</xsl:template>

<xsl:template match="bnf:grammar">
    <xsl:copy>
        <xsl:apply-templates select="@*" mode="define"/>

        <xsl:for-each-group select="bnf:define" group-by="@name">
            <bnf:rule name="{@name}" original="true">
                <xsl:call-template name="scope"/>
                <xsl:apply-templates select="current-group()/@*" mode="define"/>

                <xsl:for-each select="current-group()">
                    <bnf:production>
                        <xsl:call-template name="add-tokenizer"/>
                        <xsl:apply-templates mode="define"/>
                    </bnf:production>
                </xsl:for-each>
            </bnf:rule>
        </xsl:for-each-group>

        <xsl:for-each select=".//bnf:or">
            <bnf:rule name="{bnf:get-identifier(.)}">
                <xsl:call-template name="scope"/>
                <xsl:apply-templates select="@*" mode="define"/>

                <xsl:for-each select="*">
                    <bnf:production bnf:simplified="or">
                        <xsl:call-template name="add-tokenizer"/>
                        <xsl:apply-templates select="." mode="define"/>
                    </bnf:production>
                </xsl:for-each>
            </bnf:rule>
        </xsl:for-each>

        <xsl:for-each select=".//*[not(self::bnf:*)]">
            <bnf:rule name="{bnf:get-identifier(.)}"> <!-- local-name="{local-name()}" namespace-uri="{namespace-uri()}" -->
                <xsl:call-template name="scope"/>
                <xsl:apply-templates select="@bnf:*" mode="define"/>

                <bnf:production tokenizer="" bnf:simplified="element">
                    <xsl:call-template name="add-tokenizer"/>
                    <bnf:open local="{local-name()}" space="{namespace-uri()}"/>

                    <xsl:for-each select="
                        @*[
                            namespace-uri() ne 'http://www.saurik.com/menes/bnf/0.9' or
                            local-name() = ('precedence')
                        ]
                    ">
                        <bnf:attribute local="{local-name()}" space="{namespace-uri()}" value="{.}"/>
                    </xsl:for-each>

                    <xsl:apply-templates mode="define"/>
                    <bnf:close local="{local-name()}" space="{namespace-uri()}"/>
                </bnf:production>
            </bnf:rule>
        </xsl:for-each>

        <xsl:for-each select=".//bnf:assert | .//bnf:block | .//bnf:ignore | .//bnf:not | .//bnf:reverse">
            <bnf:rule name="{bnf:get-identifier(.)}" mode="{
                if (self::bnf:assert) then
                    'positive-assert'
                else if (self::bnf:ignore) then
                    'ignore-data'
                else if (self::bnf:not) then
                    'negative-assert'
                else if (self::bnf:reverse) then
                    'reverse-input'
                else
                    'path-branching'
            }">
                <xsl:call-template name="scope"/>

                <bnf:production bnf:simplified="{local-name()}">
                    <xsl:call-template name="add-tokenizer"/>
                    <xsl:apply-templates mode="define"/>
                </bnf:production>
            </bnf:rule>
        </xsl:for-each>

        <xsl:for-each select=".//bnf:repeated">
            <xsl:call-template name="repeated">
                <xsl:with-param name="least" select="if (@least) then @least else 0"/>
                <xsl:with-param name="most" select="@most"/>
                <xsl:with-param name="mode" select="@mode"/>
            </xsl:call-template>
        </xsl:for-each>

        <xsl:for-each select=".//bnf:multiple">
            <xsl:call-template name="repeated">
                <xsl:with-param name="least" select="1"/>
                <xsl:with-param name="most" select="()"/>
                <xsl:with-param name="mode" select="@mode"/>
            </xsl:call-template>
        </xsl:for-each>

        <xsl:for-each select=".//bnf:optional">
            <xsl:call-template name="repeated">
                <xsl:with-param name="least" select="0"/>
                <xsl:with-param name="most" select="1"/>
                <xsl:with-param name="mode" select="@mode"/>
            </xsl:call-template>
        </xsl:for-each>
    </xsl:copy>
</xsl:template>

<xsl:template name="repeated">
    <xsl:param name="least"/>
    <xsl:param name="most"/>
    <xsl:param name="mode"/>

    <!-- default mode should be given a name other than default -->

    <xsl:variable name="token" select="."/>
    <xsl:variable name="id" select="bnf:get-identifier($token)"/>
    <xsl:variable name="mode" select="if ($mode) then $mode else 'default'"/>

    <xsl:if test="not($mode = ('default', 'greedy', 'lazy', 'possessive'))">
        <xsl:message select="concat('unknown repetition mode: ', $mode)"/>
    </xsl:if>

    <bnf:rule name="{$id}">
        <xsl:call-template name="scope"/>

        <bnf:production bnf:simplified="repeated">
            <xsl:call-template name="add-tokenizer"/>

            <xsl:call-template name="least-repeated">
                <xsl:with-param name="left" select="$least"/>
            </xsl:call-template>

            <bnf:call name="{$id}{{repeat:{@separator != '' and $least ne 0}}}"/>
        </bnf:production>
    </bnf:rule>

    <xsl:for-each select="
        if (@separator != '') then
            if ($least eq 0) then
                true(), false()
            else
                true()
        else
            false()
    ">
        <bnf:rule name="{$id}{{repeat:{.}}}" mode="{
            if ($mode = ('greedy', 'lazy')) then
                'back-tracking'
            else if ($mode = 'possessive') then
                'short-circuit'
            else
                'path-branching'
        }">
            <xsl:choose>
                <xsl:when test="$most">
                    <xsl:for-each select="$token">
                        <xsl:call-template name="most-repeated">
                            <xsl:with-param name="left" select="$most - $least"/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>

                <xsl:otherwise>
                    <xsl:if test="$mode eq 'lazy'">
                        <bnf:production bnf:simplified="repeated"/>
                    </xsl:if>

                    <bnf:production bnf:simplified="repeated">
                        <xsl:for-each select="$token">
                            <xsl:call-template name="add-tokenizer"/>
                        </xsl:for-each>

                        <xsl:if test=".">
                            <bnf:call name="{$token/@separator}"/>
                        </xsl:if>

                        <bnf:call name="{$id}{{repeat}}"/>
                        <bnf:call name="{$id}{{repeat:{$token/@separator != ''}}}"/>
                    </bnf:production>

                    <xsl:if test="$mode ne 'lazy'">
                        <bnf:production bnf:simplified="repeated"/>
                    </xsl:if>
                </xsl:otherwise>
            </xsl:choose>
        </bnf:rule>
    </xsl:for-each>

    <bnf:rule name="{$id}{{repeat}}">
        <bnf:production bnf:simplified="repeated">
            <xsl:call-template name="add-tokenizer"/>
            <xsl:apply-templates mode="define"/>
        </bnf:production>
    </bnf:rule>
</xsl:template>

</xsl:stylesheet>
