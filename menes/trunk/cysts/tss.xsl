<?xml version="1.0"?>

<!-- Cysts - Simulated iTunes TSS Requests
   - Copyright (C) 2009  Jay Freeman (saurik)
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
    xmlns:srk="http://saurik.com/"
    extension-element-prefixes="xs srk"
><xsl:output
    method="xml"
    omit-xml-declaration="no"
    indent="yes"
    encoding="UTF-8"
    doctype-public="-//Apple Computer//DTD PLIST 1.0//EN"
    doctype-system="http://www.apple.com/DTDs/PropertyList-1.0.dtd"
/>

<xsl:param name="ecid"/>

<xsl:template match="@*|node()">
    <xsl:copy>
        <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
</xsl:template>

<xsl:template match="data">
    <xsl:copy>
        <xsl:text>&#10;&#9;&#9;</xsl:text>
        <xsl:value-of select="replace(., '[\t\n]', '')"/>
        <xsl:text>&#10;&#9;&#9;</xsl:text>
    </xsl:copy>
</xsl:template>

<xsl:template match="/">
<plist version="1.0">
<dict>
    <key>@HostIpAddress</key>
    <string>192.168.1.100</string>
    <key>@HostPlatformInfo</key>
    <string>windows</string>
    <key>@Locality</key>
    <string>en_US</string>
    <key>@VersionInfo</key>
    <string>3.8</string>

    <!-- XXX: look up -->
    <key>ApBoardID</key>
    <integer>0</integer>
    <key>ApChipID</key>
    <integer>35104</integer>

    <key>ApECID</key>
    <integer><xsl:value-of select="$ecid"/></integer>

    <key>ApProductionMode</key>
    <true/>
    <key>ApSecurityDomain</key>
    <integer>1</integer>

    <xsl:for-each select="/plist/dict/array/dict[2]/dict[preceding-sibling::key[1] = 'Manifest']/dict[key = 'PartialDigest']">
        <xsl:apply-templates select="preceding-sibling::key[1]"/>
        <dict>
            <xsl:apply-templates/>
        </dict>
    </xsl:for-each>
</dict>
</plist>
</xsl:template>

</xsl:stylesheet>
