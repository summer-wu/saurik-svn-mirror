<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!--
/* VcExport - Visual C++ .NET 2003 -> GNU Make
 * Copyright (C) 2003  Jay Freeman (saurik)
*/

/*
 *        Redistribution and use in source and binary
 * forms, with or without modification, are permitted
 * provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation
 *    and/or other materials provided with the
 *    distribution.
 * 3. The name of the author may not be used to endorse
 *    or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
-->

<xsl:output method="text"/>

<xsl:template name="crlf"><xsl:text>
</xsl:text></xsl:template>

<xsl:template name="tab"><xsl:text>	</xsl:text></xsl:template>

<xsl:template name="string-replace">
    <xsl:param name="string"/>
    <xsl:param name="from"/>
    <xsl:param name="to"/>

    <xsl:choose>
        <xsl:when test="contains($string, $from)">
            <xsl:value-of select="substring-before($string, $from)"/>
            <xsl:value-of select="$to"/>
            <xsl:call-template name="string-replace">
                <xsl:with-param name="string" select="substring-after($string, $from)"/>
                <xsl:with-param name="from" select="$from"/>
                <xsl:with-param name="to" select="$to"/>
            </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="$string"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="vs-variables">
    <xsl:param name="config-fullname" select="@Name"/>
    <xsl:param name="string"/>

    <xsl:variable name="config-element" select="/VisualStudioProject/Configurations/Configuration[@Name = $config-fullname]"/>

    <xsl:call-template name="string-replace">
        <xsl:with-param name="string">
            <xsl:call-template name="string-replace">
                <xsl:with-param name="string">
                    <xsl:call-template name="string-replace">
                        <xsl:with-param name="string">
                            <xsl:call-template name="string-replace">
                                <xsl:with-param name="string" select="$string"/>
                                <xsl:with-param name="from" select="'$(TargetName)'"/>
                                <!-- XXX: dude... this is SO wrong -->
                                <xsl:with-param name="to" select="/VisualStudioProject/@Name"/>
                            </xsl:call-template>
                        </xsl:with-param>
                        <xsl:with-param name="from" select="'$(ProjectName)'"/>
                        <xsl:with-param name="to" select="/VisualStudioProject/@Name"/>
                    </xsl:call-template>
                </xsl:with-param>
                <xsl:with-param name="from" select="'$(OutDir)'"/>
                <xsl:with-param name="to" select="$config-element/@OutputDirectory"/>
            </xsl:call-template>
        </xsl:with-param>
        <xsl:with-param name="from" select="'$(IntDir)'"/>
        <xsl:with-param name="to" select="$config-element/@IntermediateDirectory"/>
    </xsl:call-template>
</xsl:template>

<xsl:template name="fix-path">
    <xsl:param name="path"/>

    <xsl:variable name="fixed-slash">
        <xsl:call-template name="string-replace">
            <xsl:with-param name="string" select="$path"/>
            <xsl:with-param name="from" select="'\'"/>
            <xsl:with-param name="to" select="'/'"/>
        </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
        <xsl:when test="starts-with($fixed-slash, './') or starts-with($fixed-slash, '.\')">
            <xsl:value-of select="substring($fixed-slash, 3)"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="$fixed-slash"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="relative-path-prefix">
    <!-- XXX: this only works for .cpp files -->
    <xsl:call-template name="fix-path">
        <xsl:with-param name="path">
            <xsl:value-of select="substring(@RelativePath, 1, string-length(@RelativePath) - 4)"/>
        </xsl:with-param>
    </xsl:call-template>
</xsl:template>

<xsl:template name="compile-target">
    <xsl:param name="config-fullname"/>

    <xsl:variable name="target-fileordir">
        <xsl:call-template name="vs-variables">
            <xsl:with-param name="config-fullname" select="$config-fullname"/>

            <xsl:with-param name="string">
                <xsl:choose>
                    <xsl:when test="FileConfiguration[@Name = $config-fullname]/@ObjectFile">
                        <xsl:value-of select="FileConfiguration[@Name = $config-fullname]/@ObjectFile"/>
                    </xsl:when>
                    <xsl:otherwise>$(IntDir)/</xsl:otherwise>
                </xsl:choose>
            </xsl:with-param>
        </xsl:call-template>
    </xsl:variable>

    <xsl:value-of select="$target-fileordir"/>

    <xsl:if test="substring($target-fileordir, string-length($target-fileordir), 1) = '/'">
        <xsl:call-template name="relative-path-prefix"/><xsl:text>.obj</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template name="split-params">
    <xsl:param name="string"/>
    <xsl:param name="delimeter">;</xsl:param>
    <xsl:param name="option"/>

    <xsl:if test="$string != ''">
        <xsl:variable name="has-split" select="contains($string, $delimeter)"/>

        <xsl:variable name="define">
            <xsl:choose>
                <xsl:when test="$has-split">
                    <xsl:value-of select="substring-before($string, $delimeter)"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$string"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:text> </xsl:text>
        <xsl:value-of select="$option"/>
        <xsl:text>"</xsl:text>
        <xsl:value-of select="$define"/>
        <xsl:text>"</xsl:text>

        <xsl:if test="$has-split">
            <xsl:call-template name="split-params">
                <xsl:with-param name="delimeter" select="$delimeter"/>
                <xsl:with-param name="option" select="$option"/>
                <xsl:with-param name="string" select="substring-after($string, $delimeter)"/>
            </xsl:call-template>
        </xsl:if>
    </xsl:if>
</xsl:template>

<xsl:template match="/">
    <xsl:value-of select="VisualStudioProject/@Name"/>
    <xsl:text>:</xsl:text>
    <xsl:for-each select="VisualStudioProject/Configurations/Configuration">
        <xsl:text> </xsl:text>
        <xsl:value-of select="substring-before(@Name, '|')"/>
    </xsl:for-each>
    <xsl:call-template name="crlf"/>
    <xsl:call-template name="crlf"/>

    <xsl:for-each select="VisualStudioProject/Configurations/Configuration">
        <xsl:variable name="config-fullname" select="@Name"/>
        <xsl:variable name="config-name" select="substring-before(@Name, '|')"/>

        <xsl:variable name="linker-tool" select="Tool[@Name = 'VCLinkerTool']"/>
        <xsl:variable name="compiler-tool" select="Tool[@Name = 'VCCLCompilerTool']"/>

        <xsl:variable name="cpp-files" select="/VisualStudioProject/Files//File[substring(@RelativePath, string-length(@RelativePath) - 3, 4) = '.cpp']"/>

        <xsl:variable name="linker-target">
            <xsl:call-template name="vs-variables">
                <xsl:with-param name="string" select="$linker-tool/@OutputFile"/>
            </xsl:call-template>
        </xsl:variable>

        <xsl:value-of select="$config-name"/>
        <xsl:text>: </xsl:text>
        <xsl:value-of select="$linker-target"/>
        <xsl:call-template name="crlf"/>
        <xsl:call-template name="crlf"/>

        <xsl:value-of select="$linker-target"/>
        <xsl:text>:</xsl:text>
        <xsl:for-each select="$cpp-files">
            <xsl:text> </xsl:text>
            <xsl:call-template name="compile-target">
                <xsl:with-param name="config-fullname" select="$config-fullname"/>
            </xsl:call-template>
        </xsl:for-each>
        <xsl:call-template name="crlf"/>

        <xsl:call-template name="tab"/>
        <xsl:text>link.exe</xsl:text>

        <xsl:text> /OUT:"</xsl:text><xsl:value-of select="$linker-target"/><xsl:text>"</xsl:text>

        <xsl:choose>
            <xsl:when test="$linker-tool/@LinkIncremental = '0'"></xsl:when>
            <xsl:when test="$linker-tool/@LinkIncremental = '1'"> /INCREMENTAL:NO</xsl:when>
            <xsl:when test="$linker-tool/@LinkIncremental = '2'"> /INCREMENTAL</xsl:when>
        </xsl:choose>

        <xsl:text> /NOLOGO</xsl:text>

        <xsl:call-template name="split-params">
            <xsl:with-param name="option" select="'/LIBPATH:'"/>
            <xsl:with-param name="string" select="$linker-tool/@AdditionalLibraryDirectories"/>
        </xsl:call-template>

        <xsl:if test="$linker-tool/@GenerateDebugInformation = 'TRUE'"> /DEBUG</xsl:if>

        <xsl:variable name="linked-pdb-file">
            <xsl:call-template name="vs-variables">
                <xsl:with-param name="string">
                    <xsl:choose>
                        <xsl:when test="$linker-tool/@ProgramDatabaseFile">
                            <xsl:value-of select="$linker-tool/@ProgramDatabaseFile"/>
                        </xsl:when>
                        <xsl:otherwise>$(OutDir)/$(ProjectName).pdb</xsl:otherwise>
                    </xsl:choose>
                </xsl:with-param>
            </xsl:call-template>
        </xsl:variable>

        <xsl:text> /PDB:"</xsl:text><xsl:value-of select="$linked-pdb-file"/><xsl:text>"</xsl:text>

        <xsl:choose>
            <xsl:when test="$linker-tool/@SubSystem = '0'"></xsl:when>
            <xsl:when test="$linker-tool/@SubSystem = '1'"> /SUBSYSTEM:CONSOLE</xsl:when>
            <xsl:when test="$linker-tool/@SubSystem = '2'"> /SUBSYSTEM:WINDOWS</xsl:when>
        </xsl:choose>

        <xsl:choose>
            <xsl:when test="$linker-tool/@OptimizeReferences = '0'"></xsl:when>
            <xsl:when test="$linker-tool/@OptimizeReferences = '1'"> /OPT:NOREF</xsl:when>
            <xsl:when test="$linker-tool/@OptimizeReferences = '2'"> /OPT:REF</xsl:when>
        </xsl:choose>

        <xsl:choose>
            <xsl:when test="$linker-tool/@EnableCOMDATFolding = '0'"></xsl:when>
            <xsl:when test="$linker-tool/@EnableCOMDATFolding = '1'"> /OPT:NOICF</xsl:when>
            <xsl:when test="$linker-tool/@EnableCOMDATFolding = '2'"> /OPT:ICF</xsl:when>
        </xsl:choose>

        <xsl:choose>
            <xsl:when test="$linker-tool/@TargetMachine = '0'"></xsl:when>
            <xsl:when test="$linker-tool/@TargetMachine = '1'"> /MACHINE:X86</xsl:when>
        </xsl:choose>

        <xsl:if test="normalize-space($linker-tool/@AdditionalDependencies)">
            <xsl:text> </xsl:text>
            <xsl:value-of select="normalize-space($linker-tool/@AdditionalDependencies)"/>
        </xsl:if>

        <xsl:text> kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib</xsl:text>

        <xsl:text> $^</xsl:text>

        <xsl:call-template name="crlf"/>
        <xsl:call-template name="crlf"/>

        <xsl:for-each select="$cpp-files">
            <xsl:variable name="file-config" select="FileConfiguration[@Name = $config-fullname]"/>
            <xsl:variable name="compile-file-tool" select="$file-config/Tool[@Name = 'VCCLCompilerTool']"/>

            <xsl:variable name="pch-file">
                <xsl:call-template name="vs-variables">
                    <xsl:with-param name="config-fullname" select="$config-fullname"/>
                    <xsl:with-param name="string">
                        <xsl:choose>
                            <xsl:when test="$compiler-tool/@PrecompiledHeaderFile">
                                <xsl:value-of select="$compiler-tool/@PrecompiledHeaderFile"/>
                            </xsl:when>
                            <xsl:otherwise>$(IntDir)/$(TargetName).pch</xsl:otherwise>
                        </xsl:choose>
                    </xsl:with-param>
                </xsl:call-template>
            </xsl:variable>

            <xsl:variable name="use-pch">
                <xsl:choose>
                    <xsl:when test="$compile-file-tool/@UsePrecompiledHeader">
                        <xsl:value-of select="$compile-file-tool/@UsePrecompiledHeader"/>
                    </xsl:when>
                    <xsl:when test="$compiler-tool/@UsePrecompiledHeader">
                        <xsl:value-of select="$compiler-tool/@UsePrecompiledHeader"/>
                    </xsl:when>
                </xsl:choose>
            </xsl:variable>

            <xsl:variable name="target-name">
                <xsl:call-template name="compile-target">
                    <xsl:with-param name="config-fullname" select="$config-fullname"/>
                </xsl:call-template>
            </xsl:variable>

            <xsl:if test="$use-pch = '1'">
                <xsl:text>PCH-</xsl:text><xsl:value-of select="$pch-file"/><xsl:text>: </xsl:text><xsl:value-of select="$target-name"/>
                <xsl:call-template name="crlf"/>
            </xsl:if>

            <xsl:value-of select="$target-name"/><xsl:text>: </xsl:text>
            <xsl:call-template name="relative-path-prefix"/>
            <xsl:text>.cpp</xsl:text>

            <xsl:if test="$use-pch = '3'">
                <xsl:text> PCH-</xsl:text>
                <xsl:value-of select="$pch-file"/>
            </xsl:if>

            <xsl:call-template name="crlf"/>

            <xsl:call-template name="tab"/>
            <xsl:text>cl.exe</xsl:text>

            <xsl:variable name="compiler-optimization">
                <xsl:choose>
                    <xsl:when test="$compiler-tool/@Optimization">
                        <xsl:value-of select="$compiler-tool/@Optimization"/>
                    </xsl:when>
                    <xsl:otherwise>2</xsl:otherwise>
                </xsl:choose>
            </xsl:variable>

            <xsl:choose>
                <xsl:when test="$compiler-optimization = '0'"> /Od</xsl:when>
                <xsl:when test="$compiler-optimization = '1'"> /O1</xsl:when>
                <xsl:when test="$compiler-optimization = '2'"> /O2</xsl:when>
                <xsl:when test="$compiler-optimization = '3'"> /Ox</xsl:when>
                <xsl:when test="$compiler-optimization = '4'"></xsl:when>
            </xsl:choose>

            <xsl:call-template name="split-params">
                <xsl:with-param name="option" select="'/I '"/>
                <xsl:with-param name="string" select="$compiler-tool/@AdditionalIncludeDirectories"/>
            </xsl:call-template>

            <xsl:call-template name="split-params">
                <xsl:with-param name="option" select="'/D '"/>
                <xsl:with-param name="string" select="$compiler-tool/@PreprocessorDefinitions"/>
            </xsl:call-template>

            <xsl:if test="$compiler-tool/@MinimalRebuild = 'TRUE'"> /Gm</xsl:if>

            <xsl:variable name="exception-handling">
                <xsl:choose>
                    <xsl:when test="$compiler-tool/@ExceptionHandling">
                        <xsl:value-of select="$compiler-tool/@ExceptionHandling"/>
                    </xsl:when>
                    <xsl:otherwise>TRUE</xsl:otherwise>
                </xsl:choose>
            </xsl:variable>

            <xsl:if test="$exception-handling = 'TRUE'"> /EHsc</xsl:if>

            <xsl:choose>
                <xsl:when test="$compiler-tool/@BasicRuntimeChecks = '0'"></xsl:when>
                <xsl:when test="$compiler-tool/@BasicRuntimeChecks = '1'"> /RTCs</xsl:when>
                <xsl:when test="$compiler-tool/@BasicRuntimeChecks = '2'"> /RTCu</xsl:when>
                <xsl:when test="$compiler-tool/@BasicRuntimeChecks = '3'"> /RTC1</xsl:when>
            </xsl:choose>

            <xsl:choose>
                <xsl:when test="$compiler-tool/@RuntimeLibrary = '0'"> /MT</xsl:when>
                <xsl:when test="$compiler-tool/@RuntimeLibrary = '1'"> /MTd</xsl:when>
                <xsl:when test="$compiler-tool/@RuntimeLibrary = '2'"> /MD</xsl:when>
                <xsl:when test="$compiler-tool/@RuntimeLibrary = '3'"> /MDd</xsl:when>
                <xsl:when test="$compiler-tool/@RuntimeLibrary = '4'"> /ML</xsl:when>
                <xsl:when test="$compiler-tool/@RuntimeLibrary = '5'"> /MLd</xsl:when>
            </xsl:choose>

            <xsl:if test="$compiler-tool/@EnableFunctionLevelLinking = 'TRUE'"> /Gy</xsl:if>

            <xsl:if test="$compiler-tool/@DefaultCharIsUnsigned = 'TRUE'"> /J</xsl:if>

            <xsl:if test="$compiler-tool/@TreatWChar_tAsBuiltInType = 'TRUE'"> /Zc:wchar_t</xsl:if>

            <xsl:if test="$compiler-tool/@ForceConformanceInForLoopScope = 'TRUE'"> /Zc:wchar_t</xsl:if>

            <xsl:if test="$compiler-tool/@RuntimeTypeInfo = 'TRUE'"> /GR</xsl:if>

            <xsl:variable name="buffer-security">
                <xsl:choose>
                    <xsl:when test="$compiler-tool/@BufferSecurityCheck">
                        <xsl:value-of select="$compiler-tool/@BufferSecurityCheck"/>
                    </xsl:when>
                    <!-- XXX: technically this should be TRUE for Release and FALSE for Debug -->
                    <xsl:otherwise>TRUE</xsl:otherwise>
                </xsl:choose>
            </xsl:variable>

            <xsl:if test="$buffer-security = 'TRUE'"> /GS</xsl:if>

            <xsl:variable name="pch-header">
                <xsl:choose>
                    <xsl:when test="$compiler-tool/@PrecompiledHeaderThrough">
                        <xsl:value-of select="$compiler-tool/@PrecompiledHeaderThrough"/>
                    </xsl:when>
                    <xsl:otherwise>stdafx.h</xsl:otherwise>
                </xsl:choose>
            </xsl:variable>

            <xsl:choose>
                <xsl:when test="$use-pch = '0'"></xsl:when>
                <xsl:when test="$use-pch = '1'"> /Yc</xsl:when>
                <xsl:when test="$use-pch = '2'"> /YX</xsl:when>
                <xsl:when test="$use-pch = '3'"> /Yu</xsl:when>
            </xsl:choose>

            <!-- XXX: maybe more specific? -->
            <xsl:if test="$use-pch != '0' and $pch-header != ''">
                <xsl:text>"</xsl:text><xsl:value-of select="$pch-header"/><xsl:text>"</xsl:text>
            </xsl:if>

            <xsl:if test="$pch-file != ''">
                <xsl:text> /Fp"</xsl:text><xsl:value-of select="$pch-file"/><xsl:text>"</xsl:text>
            </xsl:if>

            <xsl:text> /Fo"</xsl:text>
            <xsl:call-template name="compile-target">
                <xsl:with-param name="config-fullname" select="$config-fullname"/>
            </xsl:call-template>
            <xsl:text>"</xsl:text>

            <xsl:variable name="compiled-pdb-file">
                <xsl:call-template name="vs-variables">
                    <xsl:with-param name="config-fullname" select="$config-fullname"/>
                    <xsl:with-param name="string">
                        <xsl:choose>
                            <xsl:when test="$compiler-tool/@ProgramDataBaseFileName">
                                <xsl:value-of select="$compiler-tool/@ProgramDataBaseFileName"/>
                            </xsl:when>
                            <xsl:otherwise>$(IntDir)/vc70.pdb</xsl:otherwise>
                        </xsl:choose>
                    </xsl:with-param>
                </xsl:call-template>
            </xsl:variable>

            <xsl:text> /Fd"</xsl:text><xsl:value-of select="$compiled-pdb-file"/><xsl:text>"</xsl:text>

            <xsl:choose>
                <xsl:when test="$compiler-tool/@WarningLevel = '0'"> /W0</xsl:when>
                <xsl:when test="$compiler-tool/@WarningLevel = '1'"> /W1</xsl:when>
                <xsl:when test="$compiler-tool/@WarningLevel = '2'"> /W2</xsl:when>
                <xsl:when test="$compiler-tool/@WarningLevel = '3'"> /W3</xsl:when>
                <xsl:when test="$compiler-tool/@WarningLevel = '4'"> /W4</xsl:when>
            </xsl:choose>

            <xsl:text> /nologo</xsl:text>

            <xsl:text> /c</xsl:text>

            <xsl:if test="$compiler-tool/@Detect64BitPortabilityProblems = 'TRUE'"> /Wp64</xsl:if>

            <xsl:choose>
                <xsl:when test="$compiler-tool/@DebugInformationFormat = '0'"></xsl:when>
                <xsl:when test="$compiler-tool/@DebugInformationFormat = '1'"> /Z7</xsl:when>
                <xsl:when test="$compiler-tool/@DebugInformationFormat = '2'"> /Zd</xsl:when>
                <xsl:when test="$compiler-tool/@DebugInformationFormat = '3'"> /Zi</xsl:when>
                <xsl:when test="$compiler-tool/@DebugInformationFormat = '4'"> /ZI</xsl:when>
            </xsl:choose>

            <xsl:variable name="compile-as">
                <xsl:choose>
                    <xsl:when test="$compiler-tool/@CompileAs">
                        <xsl:value-of select="$compiler-tool/@CompileAs"/>
                    </xsl:when>
                    <!-- XXX: should be based on file name, 1 for .c, 2 for .cpp -->
                    <xsl:otherwise>2</xsl:otherwise>
                </xsl:choose>
            </xsl:variable>

            <xsl:choose>
                <xsl:when test="$compile-as = '0'"></xsl:when>
                <xsl:when test="$compile-as = '1'"> /TC</xsl:when>
                <xsl:when test="$compile-as = '2'"> /TP</xsl:when>
            </xsl:choose>

            <xsl:text> $&lt;</xsl:text>

            <xsl:call-template name="crlf"/>
            <xsl:call-template name="crlf"/>
        </xsl:for-each>
    </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
