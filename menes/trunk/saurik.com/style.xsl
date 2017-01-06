<?xml version="1.0"?>
<!DOCTYPE xsl:stylesheet [
    <!ENTITY copy "&#169;">
    <!ENTITY ndash "&#8211;">
    <!ENTITY mdash "&#8212;">
    <!ENTITY nbsp "&#160;">
    <!ENTITY reg "&#174;">
    <!ENTITY shy "&#173;">
    <!ENTITY trade "&#153;">
]><xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:lxslt="http://xml.apache.org/xslt"
>

<xsl:output
    method="xhtml"
    doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN"
    doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"
    omit-xml-declaration="yes"
    indent="no"
/>

<xsl:template match="@*|node()">
    <xsl:copy>
        <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
</xsl:template>

<!--xsl:template match="text()">
    <xsl:for-each select="tokenize(., '\n')">
        <xsl:value-of select="."/><br/>
    </xsl:for-each>
</xsl:template-->

<xsl:template match="section">
    <xsl:if test="@name">
        <a name="{@name}"/>
    </xsl:if>

    <h3><xsl:value-of select="@title"/></h3>
    <xsl:call-template name="apply"/>
</xsl:template>

<xsl:template match="inset">
    <div class="inset"><div class="inset_">
        <xsl:call-template name="apply"/>
    </div></div>
</xsl:template>

<xsl:template match="blockquote">
    <xsl:copy>
        <xsl:call-template name="first"/>
        <div class="blockquote_"><xsl:call-template name="apply"/></div>
    </xsl:copy>
</xsl:template>

<xsl:template name="apply">
    <xsl:apply-templates select="element()"/>
</xsl:template>

<xsl:template match="a">
    <xsl:copy>
        <xsl:if test="matches(@href, '^[a-z0-9]*://', '')">
            <xsl:attribute name="class" select="'external'"/>
        </xsl:if>

        <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
</xsl:template>

<!--xsl:template match="text()[position() eq 1 and string-length(.) ne 1 and matches(., &quot;^'&quot;, 's')]" priority="2">
    <xsl:variable name="value">
        <span class="squo">'</span>
        <xsl:value-of select="substring(., 2)"/>
    </xsl:variable>

    <xsl:apply-templates select="$value"/>
</xsl:template>

<xsl:template match="text()[position() eq 1 and string-length(.) ne 1 and matches(., '^&quot;', 's')]" priority="2">
    <xsl:variable name="value">
        <span class="dquo">"</span>
        <xsl:value-of select="substring(., 2)"/>
    </xsl:variable>

    <xsl:apply-templates select="$value"/>
</xsl:template-->

<!--xsl:template match="text()[string-length(.) ne 1 and contains(., '&amp;')]" priority="1">
    <xsl:variable name="value">
        <xsl:value-of select="substring-before(., '&amp;')"/>
        <span class="amp">&amp;</span>
        <xsl:value-of select="substring-after(., '&amp;')"/>
    </xsl:variable>

    <xsl:apply-templates select="$value"/>
</xsl:template-->

<xsl:template match="text()[position() eq 1 and matches(., '^\n', 's')]" priority="2">
    <xsl:variable name="value" as="text()"><xsl:value-of select="substring(., 2)"/></xsl:variable>
    <xsl:apply-templates select="$value"/>
</xsl:template>

<xsl:template match="highlight">
    <span class="highlight"><xsl:apply-templates select="node()"/></span>
</xsl:template>

<xsl:template match="syntax">
    <span class="syntax"><xsl:apply-templates select="node()"/></span>
</xsl:template>

<xsl:template name="first">
    <xsl:attribute name="class" select="
        if (position() eq 1) then 'first' else 'notfirst'
    "/>
</xsl:template>

<xsl:template match="li">
    <xsl:copy>
        <xsl:if test="@title">
            <h4><xsl:value-of select="@title"/></h4>
        </xsl:if>

        <xsl:apply-templates/>
    </xsl:copy>
</xsl:template>

<xsl:template match="p|ul">
    <xsl:copy>
        <xsl:call-template name="first"/>
        <xsl:apply-templates/>
    </xsl:copy>
</xsl:template>

<xsl:template match="body">
<html><head>
    <meta name="verify-v1" content="RQZHJuPlT5MKsrp8mZfYsTC0oCFumO2asQTbgoi6sfc="/>

    <script src="http://www.google-analytics.com/urchin.js" type="text/javascript"></script>

    <script type="text/javascript">
        _uacct = "UA-2977740-1";
        urchinTracker();
    </script>

    <title><xsl:value-of select="@title"/> - Jay Freeman (saurik)</title>
    <link rel="stylesheet" type="text/css" href="/style.css"/>
    <meta name="viewport" content="width=1024"/>

    <link rel="openid.server" href="http://www.myopenid.com/server"/>
    <link rel="openid.delegate" href="http://saurik.myopenid.com/"/>
</head><body><div id="page" class="clearfix">

<xsl:comment> google_ad_section_start </xsl:comment>
<div id="content">
    <div id="header" class="clearfix">
        <div id="publication">
            <h1><a href="/">The Realm of the Avatar</a></h1>
        </div>

        <div id="donate">
            <form action="https://www.paypal.com/cgi-bin/webscr" method="post">
                <input type="hidden" name="cmd" value="_donations"/>
                <input type="hidden" name="business" value="saurik@saurik.com"/>
                <input type="hidden" name="item_name" value="Jay Freeman (saurik) - The Realm of the Avatar"/>
                <input type="hidden" name="no_shipping" value="0"/>
                <input type="hidden" name="logo_custom" value="http://www.saurik.com/images/donate.jpg"/>
                <input type="hidden" name="no_note" value="1"/>
                <input type="hidden" name="currency_code" value="USD"/>
                <input type="hidden" name="tax" value="0"/>
                <input type="hidden" name="lc" value="US"/>
                <input type="hidden" name="bn" value="PP-DonationsBF"/>
                <input type="image" src="https://www.paypal.com/en_US/i/btn/x-click-but21.gif" border="0" name="submit" alt="Make A Donation" title="Donate Using PayPal" width="110" height="23"/>
            </form>
        </div>
    </div>

    <div id="copy">
        <xsl:if test="@digg">
            <a class="social">
                <xsl:attribute name="href">
                    <xsl:value-of select="@digg"/>
                </xsl:attribute>

                <img src="http://digg.com/img/badges/16x16-digg-guy.gif" width="16" height="16" alt="Digg!"/>
            </a>
        </xsl:if>

        <!--div class="numeral">#20</div-->
        <div class="h2"><h2><xsl:value-of select="@title"/></h2></div>
        <xsl:apply-templates select="element()"/>
    </div>

    <div id="copyright">Copyright &copy; 2008&ndash;2009 &mdash; Jay Freeman (saurik)</div>

    <div id="lowerad">
        <script type="text/javascript"><![CDATA[
            google_ad_client = "pub-8308638528676558";
            google_ad_slot = "4313692540";
            google_ad_width = 728;
            google_ad_height = 90;
        ]]></script>

        <script type="text/javascript" src="http://pagead2.googlesyndication.com/pagead/show_ads.js"></script>
    </div>
</div>
<xsl:comment> google_ad_section_end </xsl:comment>

<div id="sidebar">
    <div id="contact">
        <div id="name">Jay Freeman (saurik)</div>
        <div id="title">Technology Consultant</div>
        <div id="email"><a href="mailto:saurik@saurik.com">saurik@saurik.com</a></div>
        <div id="phone"><a href="tel:+1-805-895-7209">(805) 895-7209</a></div>
        <div id="twitter"><a href="http://twitter.com/saurik" style="color: #44f; border-color: #44f">Follow me on Twitter!</a></div>
        <!--a href="aim:goim?screenname=saurik"><img/></a-->
        <div id="college"><a href="http://www.ccs.ucsb.edu/" style="color: #f44; border-color: #f44">The College of Creative Studies</a></div>
    </div>

    <div id="upperad">
        <script type="text/javascript"><![CDATA[
            google_ad_client = "pub-8308638528676558";
            google_ad_slot = "0949456002";
            google_ad_width = 234;
            google_ad_height = 60;
        ]]></script>

        <script type="text/javascript" src="http://pagead2.googlesyndication.com/pagead/show_ads.js"></script>
    </div>

    <div id="image">
        <img src="/jay-1.jpg" width="150" height="300" style="border: 1px solid #aaa"/>
    </div>

    <div id="menu">
        <h4>Recent Articles</h4>

        <ol>
        <!--li><a href="/id/12">A Ruby Garbage Collection Bug</a></li-->
        <!--li><a href="/id/11">JocStrapping Java to Objective-C</a></li-->
            <li><a href="/id/12">Caching Apple's Signature Server</a></li>
            <li><a href="/id/11">Tail Call: The New Twitter @Reply</a></li>
            <li><a href="/id/10">Debian &amp; Android Together on G1</a></li>
            <li><a href="/id/9">iPhone Theming on WinterBoard</a></li>
            <li><a href="/id/8">Bypassing iPhone Code Signature</a></li>
            <li><a href="/id/7">How to Host a Cydia Repository</a></li>
            <!--li><a href="/id/6">CTIA Wireless 2008 - Las Vegas</a></li-->
            <li><a href="/id/5">iPhone Applications in Python</a></li>
            <li><a href="/id/4">Upgrading the iPhone Toolchain</a></li>
            <!--li><a href="/id/3">Solving the iPhone's DNS Issues</a></li-->
            <!--li><a href="/id/2">Packaging Issues with AppTapp</a></li-->
            <li><a href="/id/1">Bringing Debian APT to the iPhone</a></li>
        </ol>
    </div>

    <xsl:if test="@sidead = 'true'">
        <div id="sidead">
            <script type="text/javascript"><![CDATA[
                google_ad_client = "pub-8308638528676558";
                google_ad_slot = "9479065311";
                google_ad_width = 160;
                google_ad_height = 600;
            ]]></script>

            <script type="text/javascript" src="http://pagead2.googlesyndication.com/pagead/show_ads.js"></script>
        </div>

        <div class="amazon">
            <h4>On iPhone Development:</h4>
            <iframe src="http://rcm.amazon.com/e/cm?t=saurik-20&amp;o=1&amp;p=8&amp;l=as1&amp;asins=0321555457&amp;fc1=000000&amp;IS2=1&amp;lt1=_blank&amp;lc1=28A848&amp;bc1=FFFFFF&amp;bg1=FFFFFF&amp;f=ifr&amp;nou=1" style="width:120px;height:240px;" scrolling="no" marginwidth="0" marginheight="0" frameborder="0"></iframe>
        </div>

        <div class="amazon">
            <iframe src="http://rcm.amazon.com/e/cm?t=saurik-20&amp;o=1&amp;p=8&amp;l=as1&amp;asins=0596155190&amp;fc1=000000&amp;IS2=1&amp;lt1=_blank&amp;lc1=28A848&amp;bc1=FFFFFF&amp;bg1=FFFFFF&amp;f=ifr&amp;nou=1" style="width:120px;height:240px;" scrolling="no" marginwidth="0" marginheight="0" frameborder="0"></iframe>
        </div>

        <!--div class="amazon">
            <h4>On APT Repositories:</h4>
            <iframe src="http://rcm.amazon.com/e/cm?t=saurik-20&amp;o=1&amp;p=8&amp;l=as1&amp;asins=1593270690&amp;fc1=000000&amp;IS2=1&amp;lt1=_blank&amp;lc1=28A848&amp;bc1=FFFFFF&amp;bg1=FFFFFF&amp;f=ifr&amp;nou=1" style="width:120px;height:240px;" scrolling="no" marginwidth="0" marginheight="0" frameborder="0"></iframe>
        </div>

        <div class="amazon">
            <h4>On ARM Architecture:</h4>
            <iframe src="http://rcm.amazon.com/e/cm?t=saurik-20&amp;o=1&amp;p=8&amp;l=as1&amp;asins=0201737191&amp;fc1=000000&amp;IS2=1&amp;lt1=_blank&amp;lc1=28A848&amp;bc1=FFFFFF&amp;bg1=FFFFFF&amp;f=ifr&amp;nou=1" style="width:120px;height:240px;" scrolling="no" marginwidth="0" marginheight="0" frameborder="0"></iframe>
        </div>

        <div class="amazon">
            <iframe src="http://rcm.amazon.com/e/cm?t=saurik-20&amp;o=1&amp;p=8&amp;l=as1&amp;asins=1558608745&amp;fc1=000000&amp;IS2=1&amp;lt1=_blank&amp;lc1=28A848&amp;bc1=FFFFFF&amp;bg1=FFFFFF&amp;f=ifr&amp;nou=1" style="width:120px;height:240px;" scrolling="no" marginwidth="0" marginheight="0" frameborder="0"></iframe>
        </div>

        <div class="amazon">
            <h4>On Using the iPhone:</h4>
            <iframe src="http://rcm.amazon.com/e/cm?t=saurik-20&amp;o=1&amp;p=8&amp;l=as1&amp;asins=1590599268&amp;fc1=000000&amp;IS2=1&amp;lt1=_blank&amp;lc1=28A848&amp;bc1=FFFFFF&amp;bg1=FFFFFF&amp;f=ifr&amp;nou=1" style="width:120px;height:240px;" scrolling="no" marginwidth="0" marginheight="0" frameborder="0"></iframe>
        </div-->

        <!--div id="image">
            <img style="border: 1px solid #aaaaaa" src="/jay-1.jpg" width="150" height="300"/>
        </div-->
    </xsl:if>
</div>

</div></body></html>
</xsl:template>

</xsl:stylesheet>
