<?xml version="1.0"?>
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:xxc="http://www.saurik.com/menes/xxc/0.9"
>

<xsl:output method="text"/>

<!-- XXX: byte me -->
<!-- <xsl:include href="../../menes/xpp/text.xsl"/> -->

<xsl:template name="crlf"><xsl:text>
</xsl:text></xsl:template>

<xsl:function name="xxc:get-attributes" as="element()*">
    <xsl:param name="element" as="element()"/>

    <xsl:for-each select="$element/ancestor::attribute/attributer">
        <attribute>
            <xsl:attribute name="name">
                <xsl:apply-templates select="name"/>
            </xsl:attribute>
        </attribute>
    </xsl:for-each>
</xsl:function>

<xsl:function name="xxc:in-template" as="xs:boolean">
    <xsl:param name="element" as="element()"/>

    <xsl:value-of select="count($element/ancestor::template) ne 0"/>
</xsl:function>

<xsl:template name="arguments">
    <xsl:for-each select="*">
        <xsl:if test="position() ne 1">
            <xsl:text>, </xsl:text>
        </xsl:if>

        <xsl:apply-templates select="."/>
    </xsl:for-each>
</xsl:template>

<xsl:template name="if-block">
    <xsl:choose>
        <xsl:when test="block">
            <xsl:text> </xsl:text>
            <xsl:apply-templates select="block"/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:text>;</xsl:text>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="indent">
    <xsl:param name="indent" as="xs:integer" select="0" tunnel="yes"/>

    <xsl:if test="$indent ne 0">
        <xsl:text>    </xsl:text>

        <xsl:call-template name="indent">
            <xsl:with-param name="indent" select="$indent - 1" tunnel="yes"/>
        </xsl:call-template>
    </xsl:if>
</xsl:template>

<xsl:template name="indent-next">
    <xsl:param name="indent" tunnel="yes"/>

    <xsl:call-template name="indent">
        <xsl:with-param name="indent" select="$indent + 1" tunnel="yes"/>
    </xsl:call-template>
</xsl:template>

<xsl:template name="indent-apply-next">
    <xsl:param name="indent" tunnel="yes"/>

    <xsl:call-template name="indent-next"/>

    <xsl:apply-templates>
        <xsl:with-param name="indent" select="$indent + 1" tunnel="yes"/>
    </xsl:apply-templates>
</xsl:template>

<xsl:template name="indent-block">
    <xsl:param name="indent" tunnel="yes"/>

    <xsl:for-each select="*">
        <xsl:call-template name="indent-next"/>

        <xsl:apply-templates select=".">
            <xsl:with-param name="indent" select="$indent + 1" tunnel="yes"/>
        </xsl:apply-templates>

        <xsl:call-template name="crlf"/>
    </xsl:for-each>
</xsl:template>

<xsl:template name="parameters">
    <xsl:for-each select="parameter">
        <xsl:if test="position() ne 1">
            <xsl:text>, </xsl:text>
        </xsl:if>

        <xsl:apply-templates select="type">
            <xsl:with-param name="inner" select="id"/>
        </xsl:apply-templates>

        <xsl:if test="initializer">
            <xsl:apply-templates select="initializer"/>
        </xsl:if>
    </xsl:for-each>
</xsl:template>

<xsl:template name="pre-function-modifiers">
    <xsl:variable name="attributes" select="xxc:get-attributes(.)"/>

    <xsl:if test="ancestor::region[1]/semantic = 'static'">
        <xsl:text>static </xsl:text>
    </xsl:if>

    <xsl:if test="$attributes/@name = 'Inline'">
        <xsl:text>inline </xsl:text>
    </xsl:if>

    <xsl:if test="$attributes/@name = 'ForceInline'">
        <xsl:text>_finline </xsl:text>
    </xsl:if>

    <xsl:if test="ancestor::region[1]/semantic = 'virtual'">
        <xsl:text>virtual </xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template name="post-function-modifiers">
    <xsl:if test="semantic = 'const' or ancestor::region[1]/semantic = 'const'">
        <xsl:text> const</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template name="do-before">
    <xsl:apply-templates select="preceding-sibling::*[1]"/>
</xsl:template>

<xsl:template match="/">
    <xsl:apply-templates>
        <xsl:with-param name="indent" select="-1" tunnel="yes"/>
    </xsl:apply-templates>
</xsl:template>

<xsl:template match="address-of">
    <xsl:text>(&amp;</xsl:text>
    <xsl:call-template name="do-before"/>
    <xsl:text>)</xsl:text>
</xsl:template>

<xsl:template match="application">
    <xsl:text>(</xsl:text>
    <xsl:call-template name="arguments"/>
    <xsl:text>)</xsl:text>
</xsl:template>

<xsl:template match="apply-to">
    <xsl:call-template name="do-before"/>
    <xsl:apply-templates select="application"/>
</xsl:template>

<xsl:template match="array-of">
    <xsl:param name="inner" as="xs:string?"/>
    <xsl:param name="left" as="xs:boolean"/>

    <xsl:apply-templates>
        <xsl:with-param name="inner">
            <xsl:if test="$left">
                <xsl:text>(</xsl:text>
            </xsl:if>

            <xsl:value-of select="$inner"/>

            <xsl:if test="$left">
                <xsl:text>)</xsl:text>
            </xsl:if>

            <xsl:text>[]</xsl:text>
        </xsl:with-param>

        <xsl:with-param name="left" select="false()"/>
    </xsl:apply-templates>
</xsl:template>

<xsl:template match="atomic-expression">
    <xsl:apply-templates select="*[last()]"/>
</xsl:template>

<xsl:template match="attribute">
    <xsl:apply-templates select="attributed"/>
</xsl:template>

<xsl:template match="auto">
    <xsl:choose>
        <xsl:when test="ancestor::block">
            <xsl:value-of select="storage"/>
            <xsl:text> </xsl:text>
        </xsl:when>

        <xsl:when test="ancestor::region[1]/semantic = 'static'">
            <xsl:text>static </xsl:text>
        </xsl:when>
    </xsl:choose>

    <xsl:for-each select="declarator">
        <xsl:apply-templates select="../type">
            <xsl:with-param name="inner">
                <xsl:apply-templates select="name"/>
            </xsl:with-param>
        </xsl:apply-templates>

        <xsl:apply-templates select="*[not(self::name)]"/>
        <xsl:text>;</xsl:text>
    </xsl:for-each>
</xsl:template>

<xsl:template match="binary">
    <xsl:text>(</xsl:text>
    <xsl:apply-templates select="lhs/*"/>

    <xsl:if test="@name ne ','">
        <xsl:text> </xsl:text>
    </xsl:if>

    <xsl:value-of select="
        if (@name eq '!')
            then '^'
        else if (@name eq ';')
            then ','
            else @name
    "/>

    <xsl:text> </xsl:text>
    <xsl:apply-templates select="rhs/*"/>
    <xsl:text>)</xsl:text>
</xsl:template>

<xsl:template match="block">
    <xsl:text>{</xsl:text>
    <xsl:call-template name="crlf"/>

    <xsl:call-template name="indent-block"/>

    <xsl:call-template name="indent"/>
    <xsl:text>}</xsl:text>
</xsl:template>

<xsl:template match="concrete">
    <xsl:param name="inner" as="xs:string?"/>

    <!-- XXX: this comment isn't marked enough -->
    <!-- XXX: this _should_ very _definitely_ be 1... this is a _major_ bug -->
    <xsl:if test="count(name/step) gt 2 and xxc:in-template(.)">
        <xsl:text>typename </xsl:text>
    </xsl:if>

    <xsl:apply-templates/>

    <xsl:if test="$inner">
        <xsl:text> </xsl:text>
        <xsl:value-of select="$inner"/>
    </xsl:if>
</xsl:template>

<xsl:template match="condition">
    <xsl:text>(</xsl:text>
    <xsl:apply-templates select="test/*"/>
    <xsl:text> ? </xsl:text>
    <xsl:apply-templates select="then/*"/>
    <xsl:text> : </xsl:text>
    <xsl:apply-templates select="else/*"/>
    <xsl:text>)</xsl:text>
</xsl:template>

<xsl:template match="const">
    <xsl:param name="inner" as="xs:string?"/>

    <xsl:apply-templates>
        <xsl:with-param name="inner">
            <xsl:text>const</xsl:text>

            <xsl:if test="$inner">
                <xsl:text> </xsl:text>
                <xsl:value-of select="$inner"/>
            </xsl:if>
        </xsl:with-param>

        <xsl:with-param name="left" select="true()"/>
    </xsl:apply-templates>
</xsl:template>

<xsl:template match="constant">
    <xsl:value-of select="."/>
</xsl:template>

<xsl:template match="element-of">
    <xsl:call-template name="do-before"/>
    <xsl:text>[</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>]</xsl:text>
</xsl:template>

<xsl:template match="empty">
    <xsl:text>;</xsl:text>
    <xsl:call-template name="crlf"/>
</xsl:template>

<xsl:template match="function">
    <xsl:call-template name="pre-function-modifiers"/>

    <xsl:if test="name/step[last()]/pseudo/id = 'constructor'">
        <xsl:text>explicit </xsl:text>
    </xsl:if>

    <xsl:apply-templates select="type">
        <xsl:with-param name="inner">
            <xsl:apply-templates select="name"/>
        </xsl:with-param>
    </xsl:apply-templates>

    <xsl:call-template name="post-function-modifiers"/>

    <xsl:choose>
        <xsl:when test="block">
            <xsl:choose>
                <xsl:when test="construction">
                    <xsl:text> :</xsl:text>
                    <xsl:call-template name="crlf"/>

                    <xsl:for-each select="construction">
                        <xsl:call-template name="indent-next"/>
                        <xsl:apply-templates select="name"/>
                        <xsl:apply-templates select="application"/>

                        <xsl:if test="position() != last()">
                            <xsl:text>,</xsl:text>
                        </xsl:if>

                        <xsl:call-template name="crlf"/>
                    </xsl:for-each>

                    <xsl:call-template name="indent"/>
                </xsl:when>

                <xsl:otherwise>
                    <xsl:text> </xsl:text>
                </xsl:otherwise>
            </xsl:choose>

            <xsl:apply-templates select="block"/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:text>;</xsl:text>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template match="hierarchy">
    <xsl:call-template name="indent-block"/>
</xsl:template>

<xsl:template match="if">
    <xsl:text>if (</xsl:text>
    <xsl:apply-templates select="test"/>
    <xsl:text>)</xsl:text>

    <xsl:choose>
        <xsl:when test="then/block">
            <xsl:text> </xsl:text>
            <xsl:apply-templates select="then"/>
        </xsl:when>

        <xsl:when test="then/empty">
            <xsl:text>;</xsl:text>
        </xsl:when>

        <xsl:otherwise>
            <xsl:call-template name="crlf"/>

            <xsl:for-each select="then">
                <xsl:call-template name="indent-apply-next"/>
            </xsl:for-each>
        </xsl:otherwise>
    </xsl:choose>

    <xsl:if test="else">
        <xsl:call-template name="crlf"/>
        <xsl:call-template name="indent"/>
        <xsl:apply-templates select="else"/>
    </xsl:if>
</xsl:template>

<xsl:template match="include">
    <xsl:text>#include </xsl:text>
    <xsl:value-of select="."/>
</xsl:template>

<xsl:template match="indirect-of">
    <xsl:text>(*</xsl:text>
    <xsl:call-template name="do-before"/>
    <xsl:text>)</xsl:text>
</xsl:template>

<xsl:template match="initializer">
    <xsl:text> = </xsl:text>
    <xsl:apply-templates/>
</xsl:template>

<xsl:template match="instantiation">
    <xsl:text>&lt; </xsl:text>
    <xsl:call-template name="arguments"/>
    <xsl:text> &gt;</xsl:text>
</xsl:template>

<xsl:template match="member-of">
    <xsl:call-template name="do-before"/>
    <xsl:text>.</xsl:text>
    <xsl:apply-templates select="step"/>
</xsl:template>

<xsl:template match="members">
    <xsl:call-template name="indent-block"/>
</xsl:template>

<xsl:template match="modifier">
    <xsl:value-of select="."/>
    <xsl:text> </xsl:text>
</xsl:template>

<xsl:template match="name">
    <xsl:for-each select="step">
        <xsl:if test="position() ne 1">
            <xsl:text>::</xsl:text>
        </xsl:if>

        <xsl:apply-templates select="."/>
    </xsl:for-each>
</xsl:template>

<xsl:template match="namespace">
    <xsl:text>namespace </xsl:text>

    <xsl:if test="name">
        <xsl:apply-templates select="name"/>
        <xsl:text> </xsl:text>
    </xsl:if>

    <xsl:text>{</xsl:text>
    <xsl:call-template name="crlf"/>

    <xsl:apply-templates select="hierarchy"/>

    <xsl:call-template name="indent"/>
    <xsl:text>}</xsl:text>
</xsl:template>

<xsl:template match="new">
    <xsl:apply-templates select="new-operator"/>
    <xsl:apply-templates select="instantiation/*[1]"/>
    <xsl:apply-templates select="application"/>
</xsl:template>

<xsl:template match="new-operator">
    <xsl:text>new</xsl:text>
    <xsl:apply-templates select="application"/>
    <xsl:text> </xsl:text>
</xsl:template>

<xsl:template match="else">
    <xsl:text>else</xsl:text>

    <xsl:choose>
        <xsl:when test="block">
            <xsl:text> </xsl:text>
            <xsl:apply-templates/>
        </xsl:when>

        <xsl:when test="empty">
            <xsl:text>;</xsl:text>
        </xsl:when>

        <xsl:otherwise>
            <xsl:call-template name="crlf"/>
            <xsl:call-template name="indent-apply-next"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template match="pointer-to">
    <xsl:param name="inner" as="xs:string?"/>

    <xsl:apply-templates>
        <xsl:with-param name="inner">
            <xsl:text>*</xsl:text>
            <xsl:value-of select="$inner"/>
        </xsl:with-param>

        <xsl:with-param name="left" select="true()"/>
    </xsl:apply-templates>
</xsl:template>

<xsl:template match="reference-to">
    <xsl:param name="inner" as="xs:string?"/>

    <xsl:apply-templates>
        <xsl:with-param name="inner">
            <xsl:text>&amp;</xsl:text>
            <xsl:value-of select="$inner"/>
        </xsl:with-param>

        <xsl:with-param name="left" select="true()"/>
    </xsl:apply-templates>
</xsl:template>

<xsl:template match="region">
    <xsl:call-template name="indent"/>
    <xsl:text>  </xsl:text>
    <xsl:value-of select="access"/>
    <xsl:text>:</xsl:text>
    <xsl:call-template name="crlf"/>

    <xsl:apply-templates select="members"/>
</xsl:template>

<xsl:template match="return">
    <xsl:text>return</xsl:text>

    <xsl:if test="*">
        <xsl:text> </xsl:text>
        <xsl:apply-templates/>
    </xsl:if>

    <xsl:text>;</xsl:text>
</xsl:template>

<xsl:template match="signature">
    <xsl:param name="inner" as="xs:string?"/>
    <xsl:param name="left" as="xs:boolean"/>

    <xsl:variable name="value">
        <xsl:if test="$left">
            <xsl:text>(</xsl:text>
        </xsl:if>

        <xsl:value-of select="$inner"/>

        <xsl:if test="$left">
            <xsl:text>)</xsl:text>
        </xsl:if>

        <xsl:text>(</xsl:text>
        <xsl:call-template name="parameters"/>
        <xsl:text>)</xsl:text>
    </xsl:variable>

    <xsl:apply-templates select="type">
        <xsl:with-param name="inner" select="$value"/>
    </xsl:apply-templates>
</xsl:template>

<xsl:template match="stated">
    <xsl:apply-templates/>
    <xsl:text>;</xsl:text>
</xsl:template>

<xsl:template match="step">
    <xsl:if test="instantiation and preceding-sibling::step">
        <xsl:text>template </xsl:text>
    </xsl:if>

    <xsl:variable name="type" select="
        if (preceding-sibling::step)
            then preceding-sibling::step[1]/id
            else ancestor::typedef[1]/name/step[last()]/id
    "/>

    <xsl:choose>
        <xsl:when test="pseudo/caster">
            <xsl:text>operator </xsl:text>
            <xsl:apply-templates select="pseudo/caster/type"/>
        </xsl:when>

        <xsl:when test="pseudo/id = 'constructor'">
            <xsl:value-of select="$type"/>
        </xsl:when>

        <xsl:when test="pseudo/id = 'destructor'">
            <xsl:text>~</xsl:text>
            <xsl:value-of select="$type"/>
        </xsl:when>

        <xsl:when test="pseudo">
            <xsl:text>~</xsl:text>
            <xsl:value-of select="pseudo/id"/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:value-of select="id"/>
        </xsl:otherwise>
    </xsl:choose>

    <xsl:apply-templates select="instantiation"/>
</xsl:template>

<xsl:template match="template">
    <xsl:apply-templates select="templater"/>
    <xsl:call-template name="crlf"/>
    <xsl:call-template name="indent"/>
    <xsl:apply-templates select="templated"/>
</xsl:template>

<xsl:template match="templater">
    <xsl:text>template &lt; </xsl:text>
    <xsl:call-template name="parameters"/>
    <xsl:text> &gt;</xsl:text>
</xsl:template>

<xsl:template match="terminator">
    <xsl:param name="inner" as="xs:string?"/>

    <xsl:value-of select="$inner"/>
</xsl:template>

<xsl:template match="type">
    <xsl:param name="inner" as="xs:string?"/>

    <xsl:apply-templates>
        <xsl:with-param name="inner" select="$inner"/>
        <xsl:with-param name="left" select="false()"/>
    </xsl:apply-templates>
</xsl:template>

<xsl:template match="typedef">
    <xsl:choose>
        <xsl:when test="type">
            <xsl:text>typedef </xsl:text>

            <xsl:apply-templates select="type">
                <xsl:with-param name="inner">
                    <xsl:apply-templates select="name"/>
                </xsl:with-param>
            </xsl:apply-templates>

            <xsl:text>;</xsl:text>
        </xsl:when>

        <xsl:when test="enum">
            <xsl:text>enum </xsl:text>
            <xsl:apply-templates select="name"/>
            <xsl:text> {</xsl:text>
            <xsl:call-template name="crlf"/>

            <xsl:for-each select="enum/declarator">
                <xsl:call-template name="indent-next"/>
                <xsl:apply-templates/>

                <xsl:if test="position() ne last()">
                    <xsl:text>,</xsl:text>
                </xsl:if>

                <xsl:call-template name="crlf"/>
            </xsl:for-each>

            <xsl:call-template name="indent"/>
            <xsl:text>};</xsl:text>
        </xsl:when>

        <xsl:otherwise>
            <xsl:value-of select="class/tag"/>
            <xsl:text> </xsl:text>
            <xsl:apply-templates select="name"/>
            <xsl:text> </xsl:text>

            <xsl:if test="class/extend">
                <xsl:text>:</xsl:text>
                <xsl:call-template name="crlf"/>

                <xsl:call-template name="indent-next"/>
                <xsl:for-each select="class/extend">
                    <xsl:if test="position() ne 1">
                        <xsl:text>, </xsl:text>
                    </xsl:if>

                    <xsl:apply-templates select="visibility"/>
                    <xsl:text> </xsl:text>
                    <xsl:apply-templates select="concrete"/>
                </xsl:for-each>

                <xsl:call-template name="crlf"/>
                <xsl:call-template name="indent"/>
            </xsl:if>

            <xsl:text>{</xsl:text>
            <xsl:call-template name="crlf"/>

            <xsl:apply-templates select="class/region"/>

            <xsl:call-template name="indent"/>
            <xsl:text>};</xsl:text>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template match="typename">
    <xsl:text>typename</xsl:text>

    <xsl:if test="*">
        <xsl:text> </xsl:text>
        <xsl:apply-templates/>
    </xsl:if>
</xsl:template>

<xsl:template match="while">
    <xsl:text>while (</xsl:text>
    <xsl:apply-templates select="test"/>
    <xsl:text>)</xsl:text>

    <xsl:choose>
        <xsl:when test="body/block">
            <xsl:text> </xsl:text>
            <xsl:apply-templates select="body"/>
        </xsl:when>

        <xsl:when test="body/empty">
            <xsl:text>;</xsl:text>
        </xsl:when>

        <xsl:otherwise>
            <xsl:call-template name="crlf"/>
            <xsl:call-template name="indent-apply-next"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

</xsl:stylesheet>
