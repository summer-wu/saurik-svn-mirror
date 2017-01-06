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
    xmlns:mak="http://www.saurik.com/menes/mak/0.9"
    exclude-result-prefixes="xsl xs xsi fn xdt xpp"
>

<xsl:include href="../../menes/xpp/sequence.xsl"/>

<xsl:output method="text"/>

<xsl:param name="goal"/>

<xsl:function name="mak:get-file" as="xs:string">
    <xsl:param name="uri" as="xs:string"/>
    <xsl:sequence select="$uri"/>
</xsl:function>

<xsl:function name="mak:to-regex" as="xs:string">
    <xsl:param name="pattern" as="xs:string"/>

    <xsl:value-of select="replace($pattern, '\*', '(.*)')"/>
</xsl:function>

<xsl:variable name="matches" select="/mak:makefile/mak:match"/>

<xsl:template name="mak:find">
    <xsl:param name="uri"/>

    <xsl:variable name="match" select="$matches[matches($uri, mak:to-regex(@pattern))][1]" as="node()?"/>

    <xsl:choose>
        <xsl:when test="$match">
            <xsl:variable name="args" select="
                saxon:tokenize(
                    replace(
                        $uri,
                        mak:to-regex($match/@pattern),
                        substring(
                            string-join(
                                for $n in 0 to string-length(replace($match/@pattern, '[^\*]', ''))
                                    return string($n),
                                ',$'
                            ),
                            2
                        )
                    ),
                    ','
                )
            " as="xs:string*"/>

            <xsl:apply-templates select="$match/*" mode="mak:look">
                <xsl:with-param name="path" select="$uri"/>
                <xsl:with-param name="args" select="$args"/>
            </xsl:apply-templates>
        </xsl:when>

        <!-- XXX: this needs to check to see if $uri is a url here -->

        <!--xsl:otherwise>
            find "<xsl:value-of select="$uri"/>" -printf "%C@"
        </xsl:otherwise-->
    </xsl:choose>
</xsl:template>

<xsl:function name="mak:get-cache" as="xs:string">
    <xsl:param name="node"/>
    <xsl:param name="path"/>

    <xsl:value-of select="concat('scratch/', generate-id($node), '-', translate($path, '/', '-'))"/>
</xsl:function>

<xsl:template match="*" mode="mak:look" priority="1">
    <xsl:param name="path" as="xs:string"/>
    <xsl:param name="source" as="xs:string?"/>
    <xsl:param name="call" as="node()?"/>
    <xsl:param name="args" as="xs:string*"/>

    <xsl:variable name="preceding" select="preceding-sibling::*(:[not(self::mak:depend)]:)[1]"/>

    <xsl:next-match>
        <xsl:with-param name="path" select="$path"/>
        <xsl:with-param name="source" select="$source"/>
        <xsl:with-param name="call" select="$call"/>
        <xsl:with-param name="args" select="$args"/>

        <xsl:with-param name="cache" select="
            if (position() = last()) then
                $path
            else mak:get-cache(., $path)
        "/>

        <xsl:with-param name="input" select="
            if ($preceding) then
                mak:get-cache($preceding, $path)
            else $source
        "/>
    </xsl:next-match>
</xsl:template>

<xsl:function name="mak:timestamp" as="xs:string">
    <xsl:param name="path" as="xs:string"/>

    <xsl:value-of select="
        concat(
            '$(utils/mtime.sh &quot;',
            $path,
            '&quot; 2&gt;/dev/null || echo 0)'
        )
    "/>
</xsl:function>

<xsl:function name="mak:arg-replace" as="xs:string">
    <xsl:param name="value" as="xs:string?"/>
    <xsl:param name="args" as="xs:string"/>
    <xsl:param name="index"/>

    <xsl:variable name="replaced" select="replace($value, concat('\{', string($index), '\}'), $args[$index])"/>

    <xsl:value-of select="
        if (count($args) = $index) then
            $replaced
        else mak:arg-replace($replaced, $args, $index + 1)
    "/>
</xsl:function>

<xsl:function name="mak:arg-replace" as="xs:string">
    <xsl:param name="value" as="xs:string?"/>
    <xsl:param name="args" as="xs:string*"/>

    <xsl:value-of select="
        if ($args) then
            mak:arg-replace($value, $args, 1)
        else $value
    "/>
</xsl:function>

<xsl:function name="mak:evaluate">
    <xsl:param name="context"/>
    <xsl:param name="xpath"/>

    <xsl:for-each select="$context">
        <xsl:sequence select="saxon:evaluate($xpath)"/>
    </xsl:for-each>
</xsl:function>

<xsl:template match="mak:exec" mode="mak:look">
    <xsl:param name="path" as="xs:string"/>
    <xsl:param name="source" as="xs:string?"/>
    <xsl:param name="call" as="node()?"/>
    <xsl:param name="args" as="xs:string*"/>
    <xsl:param name="cache" as="xs:string"/>
    <xsl:param name="input" as="xs:string?"/>

    <xsl:variable name="cmd" select="@cmd"/>

    <xsl:choose>
        <xsl:when test="$input">
            if [ -e "<xsl:value-of select="$input"/>" ]; then
                input=<xsl:value-of select="mak:timestamp($input)"/>
            else
                echo "failed to find <xsl:value-of select="$input"/>"
                exit 255
            fi
        </xsl:when>

        <xsl:otherwise>
            input=0
        </xsl:otherwise>
    </xsl:choose>

    local maximum=0

    <xsl:for-each select="
        for $xpath in saxon:tokenize(@depends, ',')
            return mak:arg-replace(mak:evaluate($call, $xpath), $args)
    ">
        function make-<xsl:value-of select="."/>() {
            dummy=0
            <xsl:call-template name="mak:find">
                <xsl:with-param name="uri" select="."/>
            </xsl:call-template>
        }

        make-<xsl:value-of select="."/>

        local current=<xsl:value-of select="mak:timestamp(.)"/>
        if [ $maximum -lt $current ]; then
            local maximum=$current
        fi
    </xsl:for-each>

    if [ -e "<xsl:value-of select="$cache"/>" ]; then
        local cached=<xsl:value-of select="mak:timestamp($cache)"/>
    else
        local cached=0
    fi

    if [ $cached -ge $maximum -a $cached -ge $input ]; then
        : echo "[---] <xsl:value-of select="$cache"/>"
    else
        (
            in='<xsl:value-of select="mak:arg-replace($input, $args)"/>'
            out='<xsl:value-of select="mak:arg-replace($cache, $args)"/>'

            <xsl:for-each select="$call/@*">
                <xsl:value-of select="local-name()"/>='<xsl:value-of select="mak:arg-replace(., $args)"/>'
            </xsl:for-each>

            <xsl:for-each select="$args">
                _<xsl:value-of select="position()"/>='<xsl:value-of select="."/>'
            </xsl:for-each>

            profile "<xsl:value-of select="$cmd"/>" <xsl:value-of select="$cmd"/>
            #echo "<xsl:value-of select="$cmd"/>"
            #<xsl:value-of select="$cmd"/>
        ) || {
            rm <xsl:value-of select="$cache"/> 2>/dev/null
            exit 255
        }
    fi
</xsl:template>

<xsl:template match="*" mode="mak:look">
    <xsl:param name="path"/>
    <xsl:param name="source"/>
    <xsl:param name="cache"/>
    <xsl:param name="args"/>
    <xsl:param name="input"/>

    <xsl:apply-templates select="/mak:makefile/mak:function[resolve-QName(@name, .) = node-name(current())]/*" mode="mak:look">
        <xsl:with-param name="path" select="$cache"/>
        <xsl:with-param name="source" select="$input"/>
        <xsl:with-param name="args" select="$args"/>
        <xsl:with-param name="call" select="."/>
    </xsl:apply-templates>
</xsl:template>

<xsl:template match="text()" mode="mak:look"/>

<xsl:template match="/">
    <xsl:text>#!/usr/bin/env bash</xsl:text>

    menes="$(dirname "$0")"
    source $menes/config/cfg/menes.sh
    source $menes/utils/bashrc.sh
    source $menes/utils/profile.sh

    function go() {
        <xsl:call-template name="mak:find">
            <xsl:with-param name="uri" select="$goal"/>
        </xsl:call-template>
    }

    mkdir -p scratch
    go
</xsl:template>

</xsl:stylesheet>
