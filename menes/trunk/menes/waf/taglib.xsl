<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:waf="http://www.saurik.com/menes/waf/0.9"
>

<xsl:output method="text"/>

<xsl:include href="../../menes/xpp/string-10.xsl"/>

<xsl:template name="setup-attributes">
    <xsl:for-each select="@*">
        cse::String _<xsl:value-of select="local-name(.)"/>;

        {
            _S&lt;ios::String&gt; _out;

            <xsl:text>_out &lt;&lt; _B("</xsl:text>
            <xsl:call-template name="txt-include">
                <xsl:with-param name="text" select="."/>
            </xsl:call-template>
            <xsl:text>");</xsl:text>

            _<xsl:value-of select="local-name(.)"/> = _out;
        }
    </xsl:for-each>
</xsl:template>

<xsl:template match="waf:content" mode="content">
    <xsl:apply-templates mode="content"/>
</xsl:template>

<xsl:template match="waf:content" mode="logic">
    <xsl:apply-templates mode="content"/>
</xsl:template>

<xsl:template match="waf:logic" mode="content">
    <xsl:apply-templates mode="logic"/>
</xsl:template>

<xsl:template match="waf:logic" mode="logic">
    <xsl:apply-templates mode="logic"/>
</xsl:template>

<xsl:template match="waf:set-status" mode="logic"> {
    <xsl:call-template name="setup-attributes"/>
    _response.SetStatus(lexical_cast&lt;unsigned&gt;(_code));
} </xsl:template>

<xsl:template match="waf:add-header" mode="logic"> {
    <xsl:call-template name="setup-attributes"/>
    _response.AddHeader(_name, _value);
} </xsl:template>

<xsl:template match="waf:get-path-info" mode="logic">
    _request.uri_
</xsl:template>

<xsl:template match="waf:include" mode="content"/>
<xsl:template match="waf:include" mode="logic"/>

<xsl:template match="waf:set-pch" mode="content"/>
<xsl:template match="waf:set-pch" mode="logic"/>

<xsl:template name="txt-escape">
    <xsl:param name="text"/>

    <xsl:call-template name="string-replace">
        <xsl:with-param name="string">
            <xsl:call-template name="string-replace">
                <xsl:with-param name="string" select="$text"/>
                <xsl:with-param name="from" select="'&quot;'"/>
                <xsl:with-param name="to" select="'\&quot;'"/>
            </xsl:call-template>
        </xsl:with-param>

        <xsl:with-param name="from" select="'&#xa;'"/>
        <xsl:with-param name="to" select="'&quot;) &lt;&lt; ios::NewLineNoFlush &lt;&lt; _B(&quot;'"/>
    </xsl:call-template>
</xsl:template>

<xsl:template name="txt-include">
    <xsl:param name="text"/>

    <xsl:choose>
        <xsl:when test="contains($text, '{')">
            <xsl:call-template name="txt-escape">
                <xsl:with-param name="text" select="substring-before($text, '{')"/>
            </xsl:call-template>

            <xsl:variable name="after" select="substring-after($text, '{')"/>

            <xsl:choose>
                <xsl:when test="starts-with($after, '{')">
                    <xsl:text>{</xsl:text>

                    <xsl:call-template name="txt-include">
                        <xsl:with-param name="text" select="substring($after, 2)"/>
                    </xsl:call-template>
                </xsl:when>

                <xsl:otherwise>
                    <xsl:text>") &lt;&lt; (</xsl:text>
                    <xsl:value-of select="substring-before($after, '}')"/>
                    <xsl:text>) &lt;&lt; _B("</xsl:text>

                    <xsl:call-template name="txt-include">
                        <xsl:with-param name="text" select="substring-after($after, '}')"/>
                    </xsl:call-template>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:when>

        <xsl:otherwise>
            <xsl:call-template name="txt-escape">
                <xsl:with-param name="text" select="$text"/>
            </xsl:call-template>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="begin-element">
    <xsl:text>_out &lt;&lt; _B("&lt;</xsl:text>
    <xsl:value-of select="local-name(.)"/>

    <xsl:for-each select="@*">
        <xsl:text> </xsl:text>
        <xsl:value-of select="local-name(.)"/>
        <xsl:text>=\"</xsl:text>

        <xsl:call-template name="txt-include">
            <xsl:with-param name="text" select="."/>
        </xsl:call-template>

        <xsl:text>\"</xsl:text>
    </xsl:for-each>

    <xsl:text>&gt;");</xsl:text>
    <!--xsl:call-template name="lf"/-->
</xsl:template>

<xsl:template name="end-element">
    <xsl:text>_out &lt;&lt; _B("&lt;/</xsl:text>
    <xsl:value-of select="local-name(.)"/>
    <xsl:text>&gt;");</xsl:text>
    <!--xsl:call-template name="lf"/-->
</xsl:template>

<xsl:template match="*" mode="content">
    <xsl:call-template name="begin-element"/>
    <xsl:apply-templates mode="content"/>
    <xsl:call-template name="end-element"/>
</xsl:template>

<xsl:template match="*" mode="logic">
    <xsl:call-template name="begin-element"/>
    <xsl:apply-templates mode="content"/>
    <xsl:call-template name="end-element"/>
</xsl:template>

<xsl:template match="text()" mode="content">
    <xsl:text>_out &lt;&lt; _B("</xsl:text>

    <xsl:call-template name="txt-include">
        <xsl:with-param name="text" select="."/>
    </xsl:call-template>

    <xsl:text>");</xsl:text>
    <xsl:call-template name="lf"/>
</xsl:template>

<xsl:template match="text()" mode="logic">
    <xsl:value-of select="."/>
</xsl:template>

<xsl:template match="/">
    <xsl:choose>
        <xsl:when test="//waf:set-pch">
            #include "<xsl:value-of select="//waf:set-pch[1]/@file"/>"
        </xsl:when>

        <xsl:otherwise>
            #include "cxx/standard.hh"
        </xsl:otherwise>
    </xsl:choose>

    #include "ios/string.hpp"

    #include "net/http/request.hpp"
    #include "net/http/response.hpp"
    #include "net/http/server.hpp"

    <xsl:for-each select="//waf:include">
        #include "<xsl:value-of select="@file"/>"
    </xsl:for-each>

    #include &lt;stdio.h&gt;

    extern "C" void WafProcess(net::Http::Server &amp;server, const net::Http::Request &amp;_request, net::Http::Response &amp;_response) {
        ios::PrintWriter &amp;_out(_response);
        <xsl:apply-templates mode="content"/>
    }
</xsl:template>

</xsl:stylesheet>
