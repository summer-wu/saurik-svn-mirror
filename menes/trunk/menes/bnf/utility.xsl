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
    xmlns:saxon="http://saxon.sf.net/"
    xmlns:xpp="http://www.saurik.com/menes/xpp/0.9"
    xmlns:bnf="http://www.saurik.com/menes/bnf/0.9"
    exclude-result-prefixes="xsl xs xsi fn xdt xpp"
>

<xsl:function name="bnf:to-string" as="xs:string">
    <xsl:param name="tokens" as="element()*"/>

    <xsl:choose>
        <xsl:when test="count($tokens) eq 0">
            <xsl:value-of select="''"/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:value-of select="
                concat(
                    if ($tokens[1]/self::bnf:call) then
                        concat('(', $tokens[1]/@name, ')')
                    else if ($tokens[1]/self::bnf:range) then
                        concat('[', $tokens[1]/@begin, $tokens[1]/@end, ']')
                    else if ($tokens[1]/self::bnf:special) then
                        concat('&lt;', $tokens[1]/@type, '&gt;')
                    else if ($tokens[1]/self::bnf:string) then
                        concat('`', $tokens[1]/@value, &quot;'&quot;)
                    else
                        '?',
                    bnf:to-string(subsequence($tokens, 2))
                )
            "/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="bnf:all-to-string" as="xs:string">
    <xsl:param name="tokens"/>

    <!-- XXX: this might require _all_ tokens -->

    <xsl:sequence select="
        for $token in $tokens[bnf:is-match-token(.)] return
            bnf:to-string($token)
    "/>
</xsl:function>

<xsl:function name="bnf:direct-left-corner" saxon:memo-function="yes">
    <xsl:param name="rule"/>

    <xsl:sequence select="
        $rule/bnf:production/*[
            not(bnf:is-lambda-token($rule/.., .)) and (
                every $token in preceding-sibling::* satisfies
                    not(bnf:is-unlambda-token($rule/.., $token))
            )
        ]
    "/>
</xsl:function>

<xsl:function name="bnf:proper-left-corner" saxon:memo-function="yes">
    <xsl:param name="grammar"/>
    <xsl:param name="rules"/>
    <xsl:param name="corners"/>

    <xsl:choose>
        <xsl:when test="count($rules) eq 0">
            <xsl:sequence select="$corners"/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:variable name="direct" select="bnf:direct-left-corner($rules[1])"/>

            <xsl:sequence select="
                bnf:proper-left-corner(
                    $grammar, (
                        subsequence($rules, 2),
                        bnf:get-rule(
                            $grammar,
                            ($direct except $corners)/self::bnf:call/@name
                        )
                    ), $direct union $corners
                )
            "/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="bnf:proper-left-corner" saxon:memo-function="yes">
    <xsl:param name="rule"/>
    <!--xsl:variable name="start" select="current-time()"/-->

    <xsl:sequence select="bnf:proper-left-corner($rule/.., $rule, ())"/>

    <!--xsl:variable name="profile" select="current-time() - $start"/-->
    <!--xsl:message select="concat('bnf:proper-left-corner(', $rule/@name, ') = ', string(get-seconds-from-dayTimeDuration($profile)), 's')"/-->
</xsl:function>

<xsl:function name="bnf:rule-usage">
    <xsl:param name="grammar"/>
    <xsl:param name="check"/>
    <xsl:param name="checked"/>

    <xsl:choose>
        <xsl:when test="count($check) eq 0">
            <xsl:sequence select="$checked"/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:variable name="calls" select="
                bnf:get-rule(
                    $grammar,
                    subsequence($check, 1, 1)/bnf:production//bnf:call/@name
                ) except $checked
            "/>

            <xsl:sequence select="
                bnf:rule-usage(
                    $grammar,
                    subsequence($check, 2) union $calls,
                    $checked union $calls
                )
            "/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="bnf:rule-usage">
    <xsl:param name="rule"/>

    <xsl:sequence select="bnf:rule-usage($rule/.., $rule, $rule)"/>
</xsl:function>

<xsl:function name="bnf:simple-left-corner">
    <xsl:param name="rule" as="element()"/>
    <xsl:param name="inclusive" as="xs:boolean"/>
    <xsl:param name="scope" as="xs:string?"/>

    <xsl:param name="rules"/>
    <xsl:param name="corners"/>
    <xsl:param name="visited"/>

    <xsl:variable name="grammar" select="$rule/.."/>
    <!--xsl:message select="string-join($rules/@name, ' ')"/-->

    <xsl:choose>
        <xsl:when test="count($rules) eq 0">
            <xsl:sequence select="if ($inclusive) then $visited else $corners"/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:variable name="direct" select="bnf:direct-left-corner($rules[1]) except $visited"/>

            <xsl:variable name="recursive" select="
                ($direct except $corners)/self::bnf:call[
                    $scope and (
                        not(bnf:get-rule($grammar, @name)/@bnf:scope) or
                        starts-with(bnf:get-rule($grammar, @name)/@bnf:scope, $scope)
                    ) or bnf:proper-left-corner(
                        bnf:get-rule($grammar, @name)
                    )/self::bnf:call/@name = $rule/@name
                ]
            "/>

            <xsl:sequence select="
                bnf:simple-left-corner(
                    $rule, $inclusive, $scope, (
                        subsequence($rules, 2),
                        bnf:get-rule($grammar, $recursive/@name)
                    ), ($direct except $recursive) union $corners,
                    $direct union $visited
                )
            "/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="bnf:simple-left-corner">
    <xsl:param name="rule"/>
    <xsl:param name="inclusive" as="xs:boolean"/>
    <xsl:param name="scope" as="xs:string?"/>

    <xsl:sequence select="bnf:simple-left-corner($rule, $inclusive, $scope, $rule, (), ())"/>
</xsl:function>

<xsl:key name="bnf:rules" match="/bnf:grammar/bnf:rule" use="@name"/>

<xsl:function name="bnf:get-rule" as="element()*">
    <xsl:param name="grammar" as="element()"/>
    <xsl:param name="names"/>

    <xsl:for-each select="$grammar">
        <xsl:variable name="rules" select="key('bnf:rules', $names)"/>

        <!--xsl:if test="not($rules)">
            <xsl:message select="concat('failed rule resolution for &quot;', string-join($names, '&quot; &quot;'), '&quot;')" terminate="yes"/>
        </xsl:if-->

        <xsl:sequence select="$rules"/>
    </xsl:for-each>
</xsl:function>

<xsl:function name="bnf:check-rule" as="element()*">
    <xsl:param name="grammar" as="element()"/>
    <xsl:param name="names"/>

    <xsl:variable name="rules" select="bnf:get-rule($grammar, $names)"/>

    <xsl:if test="not($rules)">
        <xsl:message select="$names"/>
    </xsl:if>

    <xsl:sequence select="$rules"/>
</xsl:function>

<xsl:function name="bnf:is-lambda-rule" as="xs:boolean">
    <xsl:param name="grammar"/>
    <xsl:param name="check"/>
    <xsl:param name="checked"/>

    <xsl:choose>
        <xsl:when test="count($check) = 0">
            <xsl:value-of select="true()"/>
        </xsl:when>

        <xsl:when test="count($check/bnf:production/*[bnf:is-terminal-token(.)]) != 0">
            <xsl:value-of select="false()"/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:variable name="finished" select="$check union $checked"/>
            <xsl:value-of select="bnf:is-lambda-rule($grammar, bnf:get-rule($grammar, $check/bnf:production/bnf:call/@name) except $finished, $finished)"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="bnf:is-lambda-rule" as="xs:boolean" saxon:memo-function="yes">
    <xsl:param name="rule" as="element()"/>

    <xsl:value-of select="
        if ($rule/@mode = (
            'negative-assert',
            'positive-assert'
        )) then
            true()
        else
            bnf:is-lambda-rule($rule/.., $rule, ())
    "/>
</xsl:function>

<xsl:function name="bnf:is-unlambda-token" as="xs:boolean" saxon:memo-function="yes">
    <xsl:param name="grammar" as="element()"/>
    <xsl:param name="token" as="element()"/>

    <xsl:value-of select="
        $token/@bnf:unlambda = true() or
        bnf:is-terminal-token($token) or
        not($token/@bnf:lambda = true()) and (
            $token/self::bnf:call and bnf:is-unlambda-rule(bnf:get-rule($grammar, $token/@name))
        )
    "/>
</xsl:function>

<xsl:function name="bnf:is-lambda-token" as="xs:boolean" saxon:memo-function="yes">
    <xsl:param name="grammar" as="element()"/>
    <xsl:param name="token" as="element()"/>

    <xsl:value-of select="
        $token/@bnf:lambda = true() or
        not(bnf:is-match-token($token)) or
        not(bnf:is-terminal-token($token)) and
        not($token/@bnf:unlambda = true()) and (
            $token/self::bnf:call and bnf:is-lambda-rule(bnf:check-rule($grammar, $token/@name))
        )
    "/>
</xsl:function>

<xsl:function name="bnf:lambda-token">
    <xsl:param name="grammar" as="element()"/>
    <xsl:param name="tokens" as="element()*"/>

    <xsl:for-each select="$tokens">
        <xsl:copy>
            <xsl:copy-of select="@*"/>

            <xsl:if test="bnf:is-match-token(.) and not(bnf:is-lambda-token($grammar, .))">
                <xsl:attribute name="bnf:lambda" select="true()"/>
            </xsl:if>
        </xsl:copy>
    </xsl:for-each>
</xsl:function>

<xsl:function name="bnf:unlambda-token">
    <xsl:param name="grammar" as="element()"/>
    <xsl:param name="tokens" as="element()*"/>

    <xsl:for-each select="$tokens">
        <xsl:copy>
            <xsl:copy-of select="@*"/>

            <xsl:choose>
                <xsl:when test="not(bnf:is-match-token(.))">
                    <xsl:message select="'cannot assert unlambda on a non-match token'" terminate="yes"/>
                </xsl:when>

                <xsl:when test="not(bnf:is-unlambda-token($grammar, .))">
                    <xsl:attribute name="bnf:unlambda" select="true()"/>
                </xsl:when>
            </xsl:choose>
        </xsl:copy>
    </xsl:for-each>
</xsl:function>

<xsl:function name="bnf:is-degenerate-rule" as="xs:boolean" saxon:memo-function="yes">
    <xsl:param name="rule" as="element()"/>

    <xsl:sequence select="every $production in $rule/bnf:production satisfies count(bnf:is-degenerate-production($production)) ne 0"/>
</xsl:function>

<xsl:function name="bnf:is-degenerate-production" as="xs:string*" saxon:memo-function="yes">
    <xsl:param name="production" as="element()"/>

    <xsl:variable name="grammar" select="$production/../.." as="element()"/>

    <xsl:variable name="missing" select="
        for $call in $production/bnf:call return
            if (bnf:get-rule($grammar, $call/@name)) then
                ()
            else
                $call
    "/>

    <!--xsl:variable name="degenerate" select="
        for $call in $production/bnf:call except $missing return
            if (bnf:is-degenerate-rule(bnf:get-rule($grammar, $call/@name))) then
                $call
            else
                ()
    "/-->

    <xsl:variable name="lambda" select="
        for $call in $production/bnf:call[@bnf:lambda = true()] return
            if (bnf:is-unlambda-rule(bnf:get-rule($grammar, $call/@name))) then
                $call
            else
                ()
    "/>

    <xsl:variable name="unlambda" select="
        for $call in $production/bnf:call[@bnf:unlambda = true()] return
            if (bnf:is-lambda-rule(bnf:get-rule($grammar, $call/@name))) then
                $call
            else
                ()
    "/>

    <xsl:variable name="split" select="$production/bnf:call[@bnf:lambda = true() and @bnf:unlambda = true()]"/>

    <xsl:sequence select="
        (if (count($missing) eq 0) then () else concat('failed call to missing rule from &quot;', $production/../@name, '&quot; to &quot;', string-join($missing/@name, '&quot; &quot;'), '&quot;')),
        (:(if (count($degenerate) eq 0) then () else concat('failed call to degenerate rule from &quot;', $production/../@name, '&quot; to &quot;', string-join($degenerate/@name, '&quot; &quot;'), '&quot;')),:)
        (if (count($lambda) eq 0) then () else concat('invalid lambda assertion from &quot;', $production/../@name, '&quot; for &quot;', string-join($lambda/@name, '&quot; &quot;'), '&quot;')),
        (if (count($unlambda) eq 0) then () else concat('invalid unlambda assertion from &quot;', $production/../@name, '&quot; for &quot;', string-join($unlambda/@name, '&quot; &quot;'), '&quot;')),
        (if (count($split) eq 0) then () else concat('lambda/unlambda conflict from &quot;', $production/../@name, '&quot; for &quot;', string-join($split/@name, '&quot; &quot;'), '&quot;'))
    "/>
</xsl:function>

<xsl:function name="bnf:is-non-terminal-token" as="xs:boolean">
    <xsl:param name="token"/>

    <xsl:value-of select="count($token[self::bnf:call]) ne 0"/>
</xsl:function>

<xsl:function name="bnf:is-terminal-token" as="xs:boolean">
    <xsl:param name="token"/>

    <xsl:value-of select="count($token[self::bnf:range or self::bnf:special or self::bnf:string]) ne 0"/>
</xsl:function>

<xsl:function name="bnf:is-assert-token" as="xs:boolean">
    <xsl:param name="token"/>

    <xsl:value-of select="count($token[self::bnf:not]) ne 0"/>
</xsl:function>

<xsl:function name="bnf:is-match-token" as="xs:boolean">
    <xsl:param name="token" as="element()"/>

    <xsl:value-of select="bnf:is-non-terminal-token($token) or bnf:is-terminal-token($token)"/>
</xsl:function>

<xsl:function name="bnf:is-unlambda-rule" as="xs:boolean">
    <xsl:param name="grammar" as="element()"/>
    <xsl:param name="rule" as="element()"/>
    <xsl:param name="checked"/>

    <xsl:value-of select="
        if ($checked/@name = $rule/@name) then
            true()
        else if ($rule/bnf:production[count(*) eq 0]) then
            false()
        else every $production in $rule/bnf:production[
            not(some $token in * satisfies bnf:is-terminal-token($token))
        ] satisfies
            some $call in $production/bnf:call satisfies
                $call/@bnf:unlambda = true() or
                bnf:is-unlambda-rule(
                    $grammar,
                    bnf:check-rule($grammar, $call/@name),
                    $checked union $rule
                )
    "/>
</xsl:function>

<xsl:function name="bnf:is-unlambda-rule" as="xs:boolean" saxon:memo-function="yes">
    <xsl:param name="rule" as="element()"/>
    <xsl:value-of select="bnf:is-unlambda-rule($rule/.., $rule, ())"/>
</xsl:function>

<xsl:function name="bnf:is-left-recursive" as="xs:boolean">
    <xsl:param name="rule"/>
    <xsl:param name="check"/>
    <xsl:param name="checked"/>

    <xsl:value-of select="
        if (count($check) eq 0) then
            false()
        else if ($check = $rule/@name) then
            true()
        else bnf:is-left-recursive(
            $rule,
            bnf:direct-left-corner(bnf:get-rule($rule/.., $check))/self::bnf:call/@name except $checked,
            $check union $checked
        )
    "/>
</xsl:function>

<xsl:function name="bnf:is-left-recursive" as="xs:boolean" saxon:memo-function="yes">
    <xsl:param name="rule"/>

    <xsl:value-of select="
        if ($rule/@left-recursive) then
            $rule/@left-recursive
        else
            bnf:is-left-recursive(
                $rule,
                bnf:direct-left-corner($rule)/self::bnf:call/@name,
                ()
            )
    "/>
</xsl:function>

<xsl:function name="bnf:is-directly-left-recursive" as="xs:boolean">
    <xsl:param name="rule"/>

    <xsl:value-of select="bnf:direct-left-corner($rule)/self::bnf:call[@name = $rule/@name]"/>
</xsl:function>

<xsl:function name="bnf:is-indirectly-left-recursive" as="xs:boolean">
    <xsl:param name="rule"/>

    <xsl:value-of select="bnf:is-left-recursive($rule) and not(bnf:is-directly-left-recursive($rule))"/>
</xsl:function>

<xsl:function name="bnf:is-retained" as="xs:boolean">
    <xsl:param name="rule"/>

    <!-- XXX: this retains too many nodes -->
    <xsl:value-of select="
        $rule/../@goal = $rule/@name or
        $rule/../bnf:rule/bnf:production/bnf:call[
            @name = $rule/@name and (
                position() != 1 or
                not(bnf:is-left-recursive(../..))
            )
        ]
    "/>
</xsl:function>

</xsl:stylesheet>
