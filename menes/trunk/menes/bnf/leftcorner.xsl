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

<xsl:template match="/bnf:grammar">
    <xsl:variable name="grammar" select="."/>

    <xsl:copy>
        <xsl:copy-of select="@*"/>

        <xsl:for-each select="bnf:rule">
            <xsl:variable name="recursive" select="bnf:is-left-recursive(.)" as="xs:boolean"/>

            <xsl:choose>
                <!-- Phase 4 -->
                <xsl:when test="not($recursive) and not(@bnf:left-corner = true())">
                    <xsl:copy>
                        <xsl:copy-of select="@*"/>

                        <xsl:for-each select="bnf:production">
                            <xsl:copy>
                                <xsl:copy-of select="@*"/>
                                <xsl:attribute name="bnf:lc-phase" select="'0'"/>
                                <xsl:copy-of select="*"/>
                            </xsl:copy>
                        </xsl:for-each>
                    </xsl:copy>
                </xsl:when>

                <xsl:when test="bnf:is-retained(.)">
                    <xsl:message select="concat('left-cornering rule &quot;', @name, '&quot;')"/>

                    <xsl:if test="@mode != 'back-tracking'">
                        <xsl:message select="'  invalid mode on left-cornered rule'" terminate="yes"/>
                    </xsl:if>

                    <xsl:variable name="name" select="@name"/>
                    <xsl:variable name="scope" select="if (@bnf:left-corner = true()) then @bnf:scope else ()"/>

                    <xsl:variable name="scorners" select="bnf:simple-left-corner(., false(), $scope)"/>
                    <xsl:variable name="iscorners" select="bnf:simple-left-corner(., true(), $scope)"/>

                    <xsl:if test="count($scorners) eq 0">
                        <!-- XXX: technically I should consider this a degenerate condition and just remove the rule... -->
                        <xsl:message select="concat('no simple left-corners found for &quot;', @name, '&quot;')" terminate="yes"/>
                    </xsl:if>

                    <!-- Phase 1 -->
                    <bnf:rule>
                        <xsl:copy-of select="@*"/>
                        <xsl:attribute name="mode" select="'use-prefix'"/>

                        <xsl:for-each select="$scorners">
                            <!--xsl:variable name="subtract" select="bnf:to-string(preceding-sibling::* | current())"/-->
                            <xsl:variable name="subtract" select="bnf:to-string(current())"/>
                            <!--xsl:variable name="subtract" select="generate-id(current())"/-->

                            <bnf:production bnf:lc-phase="1">
                                <!--xsl:copy-of select="preceding-sibling::*[not(bnf:is-match-token(.))]"/-->
                                <xsl:copy-of select="bnf:unlambda-token($grammar, .)"/>
                                <!--xsl:copy-of select="following-sibling::*"/-->
                                <bnf:call name="{$name}{{lc-after:{$subtract}}}"/>
                            </bnf:production>
                        </xsl:for-each>
                    </bnf:rule>

                    <!-- Phase 2 -->
                    <xsl:for-each select="$iscorners">
                        <!--xsl:variable name="subtract" select="bnf:to-string(preceding-sibling::* | current())"/-->
                        <xsl:variable name="subtract" select="bnf:to-string(current())"/>
                        <!--xsl:variable name="subtract" select="generate-id(current())"/-->

                        <xsl:variable name="prefix" select="bnf:lambda-token($grammar, preceding-sibling::*)"/>

                        <xsl:variable name="rule-name" select="concat($name, '{lc-after:', $subtract, '}')"/>
                        <xsl:variable name="prefix-name" select="concat($name, '{lc-prefix:', generate-id(current()), '}')"/>

                        <xsl:if test="$prefix">
                            <xsl:message select="concat('  adding 2a rule: ', $prefix-name)"/>

                            <bnf:rule name="{$prefix-name}" mode="add-prefix">
                                <bnf:production bnf:lc-phase="2a">
                                    <xsl:copy-of select="$prefix"/>
                                </bnf:production>
                            </bnf:rule>
                        </xsl:if>

                        <bnf:rule name="{$rule-name}">
                            <xsl:message select="concat('  adding 2b rule: ', $rule-name)"/>

                            <bnf:production bnf:lc-phase="2b">
                                <xsl:if test="$prefix">
                                    <bnf:call name="{$prefix-name}"/>
                                </xsl:if>

                                <xsl:copy-of select="following-sibling::*"/>
                                <bnf:call name="{$name}{{lc-after:({../../@name})}}"/>
                            </bnf:production>
                        </bnf:rule>
                    </xsl:for-each>

                    <!-- Phase 3 -->
                    <bnf:rule name="{$name}{{lc-after:({$name})}}">
                        <bnf:production bnf:lc-phase="3"/>
                    </bnf:rule>
                </xsl:when>

                <xsl:otherwise>
                    <xsl:message select="concat('pruning un-retained rule &quot;', @name, '&quot;')"/>

                    <xsl:if test="@mode != 'path-branching'">
                        <xsl:message select="'  unsafe prune detected!'" terminate="yes"/>
                    </xsl:if>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:for-each>
    </xsl:copy>
</xsl:template>

</xsl:stylesheet>
