<?xml version="1.0"?>

<!-- Menes - C++ High-Level Utility Library
   - Copyright (C) 2005  Jay Freeman (saurik)
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
    xmlns:clr="http://www.saurik.com/menes/clr/0.9"
    xmlns:xuf="http://www.saurik.com/menes/xuf/0.9"
    exclude-result-prefixes="xsl xs xsi clr fn xdt xuf"
>

<xsl:output method="text"/>

<xsl:function name="clr:get-stack" as="element()?">
    <xsl:param name="stacks" as="element()*"/>
    <xsl:param name="instruction" as="element()"/>

    <!--xsl:message select="'{'"/>
    <xsl:message select="generate-id($instruction)"/>
    <xsl:message select="$stacks"/>
    <xsl:message select="$stacks[@id = generate-id($instruction)]"/>
    <xsl:message select="'}'"/-->

    <xsl:sequence select="$stacks[@id = generate-id($instruction)]"/>
</xsl:function>

<xsl:function name="clr:unary-op" as="element()">
    <xsl:param name="old-stack" as="element()"/>
    <xsl:param name="do-real" as="xs:boolean"/>

    <xsl:variable name="new-stack" select="clr:pop-any($old-stack)"/>

    <xsl:variable name="arg0" select="local-name($old-stack/*[1])"/>

    <xsl:variable name="new-slot" as="element()">
        <xsl:choose>
            <xsl:when test="$new-stack/self::error">
                <xsl:sequence select="$new-stack"/>
            </xsl:when>

            <xsl:when test="$do-real and $arg0 eq 'int32'">
                <real64/>
            </xsl:when>

            <xsl:when test="$arg0 eq 'int32'">
                <int32/>
            </xsl:when>

            <xsl:when test="$arg0 eq 'int64'">
                <int64/>
            </xsl:when>

            <xsl:when test="$arg0 eq 'native-int'">
                <native-int/>
            </xsl:when>

            <xsl:otherwise>
                <error>invalid unary operation on <xsl:value-of select="$arg0"/></error>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>

    <xsl:choose>
        <xsl:when test="$new-slot/self::error">
            <xsl:sequence select="$new-slot"/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:sequence select="clr:alter-stack($new-stack, $new-slot, ())"/>    
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="clr:compare-op" as="element()">
    <xsl:param name="old-stack" as="element()"/>

    <xsl:variable name="new-stack" select="
        clr:pop-any(
            clr:pop-any(
                $old-stack
            )
        )
    "/>

    <xsl:variable name="arg0" select="local-name($old-stack/*[1])"/>
    <xsl:variable name="arg1" select="local-name($old-stack/*[2])"/>

    <xsl:variable name="new-slot" as="element()?">
        <xsl:choose>
            <xsl:when test="$new-stack/self::error">
                <xsl:sequence select="$new-stack"/>
            </xsl:when>

            <xsl:when test="$arg0 eq 'int32' and $arg1 eq 'int32'"/>
            <xsl:when test="$arg0 eq 'int64' and $arg1 eq 'int64'"/>
            <xsl:when test="$arg0 eq 'native-int' and $arg1 eq 'int32'"/>
            <xsl:when test="$arg0 eq 'native-int' and $arg1 eq 'native-int'"/>
            <xsl:when test="$arg0 eq 'int32' and $arg1 eq 'native-int'"/>
            <xsl:when test="$arg0 eq 'real64' and $arg1 eq 'real64'"/>
            <xsl:when test="$arg0 eq 'pointer' and $arg1 eq 'pointer'"/>

            <xsl:otherwise>
                <error>invalid compare operation on <xsl:value-of select="$arg0"/> and <xsl:value-of select="$arg1"/></error>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>

    <xsl:choose>
        <xsl:when test="$new-slot/self::error">
            <xsl:sequence select="$new-slot"/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:sequence select="$new-stack"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="clr:binary-op" as="element()">
    <xsl:param name="old-stack" as="element()"/>
    <xsl:param name="do-real" as="xs:boolean"/>

    <xsl:variable name="new-stack" select="
        clr:pop-any(
            clr:pop-any(
                $old-stack
            )
        )
    "/>

    <xsl:variable name="arg0" select="local-name($old-stack/*[1])"/>
    <xsl:variable name="arg1" select="local-name($old-stack/*[2])"/>

    <xsl:variable name="new-slot" as="element()">
        <xsl:choose>
            <xsl:when test="$new-stack/self::error">
                <xsl:sequence select="$new-stack"/>
            </xsl:when>

            <xsl:when test="$do-real and $arg0 eq 'real64' and $arg1 eq 'real64'">
                <real64/>
            </xsl:when>

            <xsl:when test="$arg0 eq 'int32' and $arg1 eq 'int32'">
                <int32/>
            </xsl:when>

            <xsl:when test="$arg0 eq 'int64' and $arg1 eq 'int64'">
                <int64/>
            </xsl:when>

            <xsl:when test="$arg0 eq 'native-int' and $arg1 eq 'int32'">
                <native-int/>
            </xsl:when>

            <xsl:when test="$arg0 eq 'native-int' and $arg1 eq 'native-int'">
                <native-int/>
            </xsl:when>

            <xsl:when test="$arg0 eq 'int32' and $arg1 eq 'native-int'">
                <native-int/>
            </xsl:when>

            <xsl:otherwise>
                <error>invalid binary operation on <xsl:value-of select="$arg0"/> and <xsl:value-of select="$arg1"/></error>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>

    <xsl:choose>
        <xsl:when test="$new-slot/self::error">
            <xsl:sequence select="$new-slot"/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:sequence select="clr:alter-stack($new-stack, $new-slot, ())"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="clr:merge-stacks_" as="element()*">
    <xsl:param name="old-stack" as="element()*"/>
    <xsl:param name="new-stack" as="element()*"/>

    <xsl:if test="count($old-stack) ne 0">
        <xsl:sequence select="
            (
                clr:merge-stack-type(
                    $old-stack[1],
                    $new-stack[1]
                ),
                clr:merge-stacks_(
                    subsequence($old-stack, 2),
                    subsequence($new-stack, 2)
                )
            )
        "/>
    </xsl:if>
</xsl:function>

<xsl:function name="clr:merge-stacks" as="element()*">
    <xsl:param name="id" as="xs:string"/>
    <xsl:param name="old-stack" as="element()?"/>
    <xsl:param name="new-stack" as="element()"/>

    <xsl:choose>
        <xsl:when test="count($old-stack) eq 0">
            <stack id="{$id}">
                <xsl:sequence select="$new-stack/*"/>
            </stack>
        </xsl:when>

        <xsl:when test="count($old-stack/*) ne count($new-stack/*)">
            <error>stack depth mismatch</error>
        </xsl:when>

        <xsl:otherwise>
            <stack id="{$id}">
                <xsl:sequence select="clr:merge-stacks_($old-stack/*, $new-stack/*)"/>
            </stack>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="clr:verify-all" as="element()+">
    <xsl:param name="these" as="element()*"/>
    <xsl:param name="new-stack" as="element()"/>
    <xsl:param name="old-stacks" as="element()*"/>

    <xsl:choose>
        <xsl:when test="count($these) eq 0">
            <xsl:sequence select="$old-stacks"/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:variable name="mod-stack" as="element()+" select="clr:verify($these[1], $new-stack, $old-stacks)"/>

            <xsl:choose>
                <xsl:when test="$mod-stack/self::stack">
                    <xsl:sequence select="
                        clr:verify-all(
                            subsequence($these, 2),
                            $new-stack,
                            $mod-stack
                        )
                    "/>
                </xsl:when>

                <xsl:otherwise>
                    <xsl:sequence select="$mod-stack"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="clr:merge-stack-types" as="element()">
    <xsl:param name="old-type" as="element()"/>
    <xsl:param name="new-type" as="element()"/>

    <!--xsl:choose>
        <xsl:otherwise-->
            <xsl:message terminate="yes" select="concat('  stack merge of ', local-name($old-type), ' and ', local-name($new-type), ' failed')"/>
        <!--/xsl:otherwise>
    </xsl:choose-->
</xsl:function>

<xsl:function name="clr:alter-stack" as="element()">
    <xsl:param name="old-stack" as="element()"/>
    <xsl:param name="to-push" as="element()*"/>
    <xsl:param name="to-pop" as="element()*"/>

    <xsl:choose>
        <xsl:when test="$old-stack/self::error">
            <xsl:sequence select="$old-stack"/>
        </xsl:when>

        <xsl:when test="count($to-pop) ne 0">
            <xsl:sequence select="
                clr:alter-stack(
                    clr:pop-type($old-stack, $to-pop[last()]),
                    $to-push,
                    subsequence($to-pop, 1, count($to-pop) - 1)
                )
            "/>
        </xsl:when>

        <xsl:when test="count($to-push) ne 0">
            <xsl:variable name="new-stack" select="
                clr:alter-stack(
                    $old-stack,
                    subsequence($to-push, 2),
                    $to-pop
                )
            "/>

            <xsl:choose>
                <xsl:when test="$new-stack/self::error">
                    <xsl:sequence select="$new-stack"/>
                </xsl:when>

                <xsl:otherwise>
                    <stack id="{$old-stack/@id}">
                        <xsl:sequence select="clr:to-stack($to-push[1])"/>
                        <xsl:sequence select="$new-stack/*"/>
                    </stack>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:when>

        <xsl:otherwise>
            <xsl:sequence select="$old-stack"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="clr:pop-type" as="element()">
    <xsl:param name="old-stack" as="element()+"/>
    <xsl:param name="to-pop" as="element()"/>

    <xsl:choose>
        <xsl:when test="
            $old-stack/self::error or
            $to-pop/intrinsic = 'void'
        ">
            <xsl:sequence select="$old-stack"/>
        </xsl:when>

        <xsl:when test="count($old-stack/*) eq 0">
            <error>stack underflow</error>
        </xsl:when>

        <xsl:otherwise>
            <stack id="{$old-stack/@id}">
                <xsl:sequence select="subsequence($old-stack/*, 2)"/>
            </stack>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="clr:pop-array" as="element()">
    <xsl:param name="old-stack" as="element()+"/>

    <xsl:choose>
        <xsl:when test="$old-stack/self::error">
            <xsl:sequence select="$old-stack"/>
        </xsl:when>

        <xsl:when test="count($old-stack/*) eq 0">
            <error>stack underflow</error>
        </xsl:when>

        <xsl:when test="$old-stack/*[1]/array">
            <stack id="{$old-stack/@id}">
                <xsl:sequence select="subsequence($old-stack/*, 2)"/>
            </stack>
        </xsl:when>

        <xsl:otherwise>
            <error>attempting to pop non-array in array context</error>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="clr:pop-any" as="element()">
    <xsl:param name="old-stack" as="element()"/>

    <xsl:choose>
        <xsl:when test="$old-stack/self::error">
            <xsl:sequence select="$old-stack"/>
        </xsl:when>

        <xsl:when test="count($old-stack/*) eq 0">
            <error>stack underflow</error>
        </xsl:when>

        <xsl:otherwise>
            <stack id="{$old-stack/@id}">
                <xsl:sequence select="subsequence($old-stack/*, 2)"/>
            </stack>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="clr:get-type-class" as="element()">
    <xsl:param name="class" as="element()"/>

    <type><xsl:sequence select="$class/name"/></type>
</xsl:function>

<xsl:function name="clr:unbox-type" as="element()?">
    <xsl:param name="type" as="element()"/>

    <xsl:choose>
        <xsl:when test="$type/module ne 'mscorlib'">
            <xsl:message terminate="yes" select="'  unknown value type'"/>
        </xsl:when>

        <xsl:when test="$type/name eq 'System.SByte'">
            <xsl:sequence select="clr:get-intrinsic('int8')"/>
        </xsl:when>

        <xsl:when test="$type/name eq 'System.Int16'">
            <xsl:sequence select="clr:get-intrinsic('int16')"/>
        </xsl:when>

        <xsl:when test="$type/name eq 'System.Int32'">
            <xsl:sequence select="clr:get-intrinsic('int32')"/>
        </xsl:when>

        <xsl:when test="$type/name eq 'System.Int64'">
            <xsl:sequence select="clr:get-intrinsic('int64')"/>
        </xsl:when>

        <xsl:when test="$type/name eq 'System.Char'">
            <xsl:sequence select="clr:get-intrinsic('char')"/>
        </xsl:when>

        <xsl:when test="$type/name eq 'System.String'">
            <xsl:sequence select="clr:get-intrinsic('string')"/>
        </xsl:when>

        <xsl:when test="$type/name eq 'System.Double'">
            <xsl:sequence select="clr:get-intrinsic('double')"/>
        </xsl:when>

        <xsl:when test="$type/name eq 'System.Byte'">
            <xsl:sequence select="clr:get-decorated-intrinsic('int8', 'unsigned')"/>
        </xsl:when>

        <xsl:when test="$type/name eq 'System.UInt16'">
            <xsl:sequence select="clr:get-decorated-intrinsic('int16', 'unsigned')"/>
        </xsl:when>

        <xsl:when test="$type/name eq 'System.UInt32'">
            <xsl:sequence select="clr:get-decorated-intrinsic('int32', 'unsigned')"/>
        </xsl:when>

        <xsl:when test="$type/name eq 'System.UInt64'">
            <xsl:sequence select="clr:get-decorated-intrinsic('int64', 'unsigned')"/>
        </xsl:when>

        <xsl:when test="$type/name eq 'System.Boolean'">
            <xsl:sequence select="clr:get-intrinsic('bool')"/>
        </xsl:when>
    </xsl:choose>
</xsl:function>

<xsl:function name="clr:get-intrinsic" as="element()">
    <xsl:param name="name" as="xs:string"/>

    <type>
        <intrinsic><xsl:value-of select="$name"/></intrinsic>
    </type>
</xsl:function>

<xsl:function name="clr:get-decorated-intrinsic" as="element()">
    <xsl:param name="name" as="xs:string"/>
    <xsl:param name="decorator" as="xs:string"/>

    <type>
        <decorator><xsl:value-of select="$decorator"/></decorator>
        <intrinsic><xsl:value-of select="$name"/></intrinsic>
    </type>
</xsl:function>

<xsl:function name="clr:to-stack" as="element()?">
    <xsl:param name="type" as="element()"/>

    <xsl:choose>
        <xsl:when test="
            $type/name or
            $type/array or
            $type/intrinsic = ('null', 'object', 'string')
        ">
            <xsl:sequence select="$type"/>
        </xsl:when>

        <xsl:when test="$type/intrinsic = 'void'"/>

        <xsl:when test="$type/intrinsic = 'int64'">
            <int64/>
        </xsl:when>

        <xsl:when test="$type/decorator = 'native'">
            <native-int/>
        </xsl:when>

        <xsl:when test="$type/intrinsic = ('real32', 'real64')">
            <real64/>
        </xsl:when>

        <xsl:when test="$type/intrinsic = ('reference')">
            <pointer/>
        </xsl:when>

        <xsl:otherwise>
            <int32/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="clr:get-encloser-type" as="element()">
    <xsl:param name="method" as="element()"/>

    <xsl:choose>
        <xsl:when test="$method/self::method-token">
            <xsl:sequence select="$method/encloser"/>
        </xsl:when>

        <xsl:when test="$method/self::method">
            <xsl:sequence select="clr:get-type-class($method/..)"/>
        </xsl:when>
    </xsl:choose>
</xsl:function>

<xsl:function name="clr:get-arguments" as="element()*">
    <xsl:param name="method" as="element()"/>

    <xsl:variable name="old-arguments" as="element()*" select="$method/arguments/element/type"/>

    <xsl:variable name="new-arguments" as="element()*">
        <xsl:choose>
            <xsl:when test="$method/convention/this-pointer = 'instance'">
                <xsl:sequence select="clr:get-encloser-type($method), $old-arguments"/>
            </xsl:when>

            <xsl:otherwise>
                <xsl:sequence select="$old-arguments"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:variable>

    <xsl:sequence select="$new-arguments"/>
</xsl:function>

<xsl:function name="clr:verify" as="element()+">
    <xsl:param name="this" as="element()"/>
    <xsl:param name="new-stack" as="element()"/>
    <xsl:param name="old-stacks" as="element()*"/>

    <xsl:variable name="method" select="$this/.."/>
    <xsl:variable name="arguments" select="clr:get-arguments($method)"/>
    <xsl:variable name="locals" select="$method/locals/element/type"/>

    <xsl:variable name="id" select="generate-id($this)"/>

    <xsl:variable name="next" as="element()?" select="$this/following-sibling::instruction[1]"/>
    <xsl:variable name="target" as="element()*" select="$method/instruction[label = $this/target]"/>

    <xsl:variable name="old-stack" as="element()?" select="clr:get-stack($old-stacks, $this)"/>
    <xsl:variable name="fix-stack" as="element()" select="clr:merge-stacks($id, $old-stack, $new-stack)"/>
    <xsl:variable name="new-stacks" as="element()+" select="$old-stacks[@id ne $id], $fix-stack"/>

    <xsl:message select="concat('   checking: ', xs:string($this/name), ' [#', $id, ']')"/>

    <xsl:choose>
        <xsl:when test="$new-stacks/self::error">
            <xsl:sequence select="$new-stacks/self::error"/>
        </xsl:when>

        <xsl:when test="deep-equal($old-stack, $fix-stack)">
            <xsl:sequence select="$new-stacks"/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:message select="concat('   reading: ', xs:string($this/name), ' [#', $id, ']')"/>

            <xsl:variable name="mod-stack" as="element()?">
                <xsl:choose>
                    <xsl:when test="$this/name eq 'nop'">
                        <xsl:sequence select="$new-stacks"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'break'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldarg.0'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $arguments[1],
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldarg.1'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $arguments[2],
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldarg.2'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $arguments[3],
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldarg.3'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $arguments[4],
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldloc.0'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $locals[1],
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldloc.1'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $locals[2],
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldloc.2'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $locals[3],
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldloc.3'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $locals[4],
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stloc.0'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                (),
                                $locals[1]
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stloc.1'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                (),
                                $locals[2]
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stloc.2'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                (),
                                $locals[3]
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stloc.3'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                (),
                                $locals[4]
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldarg.s'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $arguments[$this/index + 1],
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldarga.s'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'starg.s'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                (),
                                $arguments[$this/index + 1]
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldloc.s'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $locals[$this/index + 1],
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldloca.s'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stloc.s'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                (),
                                $locals[$this/index + 1]
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldnull'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('null'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldc.i4.m1'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('int32'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldc.i4.0'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('int32'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldc.i4.1'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('int32'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldc.i4.2'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('int32'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldc.i4.3'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('int32'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldc.i4.4'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('int32'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldc.i4.5'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('int32'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldc.i4.6'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('int32'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldc.i4.7'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('int32'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldc.i4.8'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('int32'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldc.i4.s'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('int32'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldc.i4'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('int32'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'idc.i8'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('int64'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldc.r4'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('real32'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldc.r8'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('real64'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'dup'">
                        <xsl:choose>
                            <xsl:when test="count($fix-stack/*) eq 0">
                                <error>stack underflow</error>
                            </xsl:when>

                            <xsl:otherwise>
                                <stack id="{$fix-stack/@id}">
                                    <xsl:sequence select="$fix-stack/*[1]"/>
                                    <xsl:sequence select="$fix-stack/*"/>
                                </stack>
                            </xsl:otherwise>
                        </xsl:choose>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'pop'">
                        <xsl:sequence select="clr:pop-any($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'jmp'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'call'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $this/method-token/type,
                                clr:get-arguments($this/method-token)
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'calli'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ret'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                (),
                                $method/type
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'br.s'">
                        <xsl:sequence select="$fix-stack"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'brfalse.s'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                (),
                                clr:get-intrinsic('bool')
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'brtrue.s'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                (),
                                clr:get-intrinsic('bool')
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'beq.s'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'bge.s'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'bgt.s'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ble.s'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'blt.s'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'bne.un.s'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'bge.un.s'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'bgt.un.s'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ble.un.s'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'blt.un.s'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'br'">
                        <xsl:sequence select="$fix-stack"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'brfalse'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                (),
                                clr:get-intrinsic('bool')
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'brtrue'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                (),
                                clr:get-intrinsic('bool')
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'beq'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'bge'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'bgt'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ble'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'blt'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'bne.un'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'bge.un'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'bgt.un'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ble.un'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'blt.un'">
                        <xsl:sequence select="clr:compare-op($fix-stack)"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'switch'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                (),
                                clr:get-intrinsic('int32')
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldind.i1'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldind.u1'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldind.i2'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldind.u2'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldind.i4'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldind.u4'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldind.i8'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldind.i'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldind.r4'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldind.r8'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldind.ref'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stind.ref'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stind.il'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stind.i2'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stind.i4'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stind.i8'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stind.r4'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stind.r8'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'add'">
                        <xsl:sequence select="clr:binary-op($fix-stack, true())"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'sub'">
                        <xsl:sequence select="clr:binary-op($fix-stack, true())"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'mul'">
                        <xsl:sequence select="clr:binary-op($fix-stack, true())"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'div'">
                        <xsl:sequence select="clr:binary-op($fix-stack, true())"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'div.un'">
                        <xsl:sequence select="clr:binary-op($fix-stack, false())"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'rem'">
                        <xsl:sequence select="clr:binary-op($fix-stack, true())"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'rem.un'">
                        <xsl:sequence select="clr:binary-op($fix-stack, false())"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'and'">
                        <xsl:sequence select="clr:binary-op($fix-stack, false())"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'or'">
                        <xsl:sequence select="clr:binary-op($fix-stack, false())"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'xor'">
                        <xsl:sequence select="clr:binary-op($fix-stack, false())"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'shl'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'shr'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'shr.un'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'neg'">
                        <xsl:sequence select="clr:unary-op($fix-stack, true())"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'not'">
                        <xsl:sequence select="clr:unary-op($fix-stack, false())"/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.il'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.i2'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.i4'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('int32'),
                                clr:get-intrinsic('int32')
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.i8'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.r4'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.r8'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('real64'),
                                clr:get-intrinsic('real64')
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.u4'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.u8'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'callvirt'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $this/method-token/type,
                                clr:get-arguments($this/method-token)
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'cpobj'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldobj'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldstr'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-intrinsic('string'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'newobj'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                clr:get-encloser-type($this/method-token),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'castclass'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'isinst'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:pop-any($fix-stack),
                                $this/type,
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.r.un'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'unbox'">
                        <error>unverifiable instruction!</error>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'throw'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldfld'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $this/field-token/type,
                                clr:get-encloser-type($this/field-token)
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldflda'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stfld'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                (),
                                (
                                    clr:get-encloser-type($this/field-token),
                                    $this/field-token/type
                                )
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldsfld'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $this/field-token/type,
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldsflda'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stsfld'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                (),
                                $this/field-token/type
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stobj'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.i1.un'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.i2.un'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.i4.un'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.i8.un'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.u1.un'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.u2.un'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.u4.un'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.u8.un'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.i.un'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.u.un'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'box'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $this/type,
                                clr:to-stack($this/type)
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'newarr'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldlen'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:pop-array($fix-stack),
                                clr:get-intrinsic('int32'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldelema'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldelem.i1'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:pop-array(
                                    clr:alter-stack(
                                        $fix-stack,
                                        (),
                                        clr:get-intrinsic('int')
                                    )
                                ),
                                clr:get-intrinsic('int8'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldelem.u1'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:pop-array(
                                    clr:alter-stack(
                                        $fix-stack,
                                        (),
                                        clr:get-intrinsic('int')
                                    )
                                ),
                                clr:get-decorated-intrinsic('int8', 'unsigned'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldelem.i2'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:pop-array(
                                    clr:alter-stack(
                                        $fix-stack,
                                        (),
                                        clr:get-intrinsic('int')
                                    )
                                ),
                                clr:get-intrinsic('int16'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldelem.u2'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:pop-array(
                                    clr:alter-stack(
                                        $fix-stack,
                                        (),
                                        clr:get-intrinsic('int')
                                    )
                                ),
                                clr:get-decorated-intrinsic('int16', 'unsigned'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldelem.i4'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:pop-array(
                                    clr:alter-stack(
                                        $fix-stack,
                                        (),
                                        clr:get-intrinsic('int')
                                    )
                                ),
                                clr:get-intrinsic('int32'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldelem.u4'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:pop-array(
                                    clr:alter-stack(
                                        $fix-stack,
                                        (),
                                        clr:get-intrinsic('int')
                                    )
                                ),
                                clr:get-decorated-intrinsic('int32', 'unsigned'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldelem.i8'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:pop-array(
                                    clr:alter-stack(
                                        $fix-stack,
                                        (),
                                        clr:get-intrinsic('int')
                                    )
                                ),
                                clr:get-intrinsic('int64'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldelem.i'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:pop-array(
                                    clr:alter-stack(
                                        $fix-stack,
                                        (),
                                        clr:get-intrinsic('int')
                                    )
                                ),
                                clr:get-intrinsic('int'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldelem.r4'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:pop-array(
                                    clr:alter-stack(
                                        $fix-stack,
                                        (),
                                        clr:get-intrinsic('int')
                                    )
                                ),
                                clr:get-intrinsic('real32'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldelem.r8'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:pop-array(
                                    clr:alter-stack(
                                        $fix-stack,
                                        (),
                                        clr:get-intrinsic('int')
                                    )
                                ),
                                clr:get-intrinsic('real64'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldelem.ref'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:pop-array(
                                    clr:alter-stack(
                                        $fix-stack,
                                        (),
                                        clr:get-intrinsic('int')
                                    )
                                ),
                                clr:get-intrinsic('reference'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stelem.i'">
                        <xsl:sequence select="
                            clr:pop-array(
                                clr:alter-stack(
                                    $fix-stack,
                                    (),
                                    (
                                        clr:get-intrinsic('int'),
                                        clr:get-intrinsic('int')
                                    )
                                )
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stelem.i1'">
                        <xsl:sequence select="
                            clr:pop-array(
                                clr:alter-stack(
                                    $fix-stack,
                                    (),
                                    (
                                        clr:get-intrinsic('int'),
                                        clr:get-intrinsic('int8')
                                    )
                                )
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stelem.i2'">
                        <xsl:sequence select="
                            clr:pop-array(
                                clr:alter-stack(
                                    $fix-stack,
                                    (),
                                    (
                                        clr:get-intrinsic('int'),
                                        clr:get-intrinsic('int16')
                                    )
                                )
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stelem.i4'">
                        <xsl:sequence select="
                            clr:pop-array(
                                clr:alter-stack(
                                    $fix-stack,
                                    (),
                                    (
                                        clr:get-intrinsic('int'),
                                        clr:get-intrinsic('int32')
                                    )
                                )
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stelem.i8'">
                        <xsl:sequence select="
                            clr:pop-array(
                                clr:alter-stack(
                                    $fix-stack,
                                    (),
                                    (
                                        clr:get-intrinsic('int'),
                                        clr:get-intrinsic('int64')
                                    )
                                )
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stelem.r4'">
                        <xsl:sequence select="
                            clr:pop-array(
                                clr:alter-stack(
                                    $fix-stack,
                                    (),
                                    (
                                        clr:get-intrinsic('int'),
                                        clr:get-intrinsic('real32')
                                    )
                                )
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stelem.r8'">
                        <xsl:sequence select="
                            clr:pop-array(
                                clr:alter-stack(
                                    $fix-stack,
                                    (),
                                    (
                                        clr:get-intrinsic('int'),
                                        clr:get-intrinsic('real64')
                                    )
                                )
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stelem.ref'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldelem.any'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stelem.any'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'unbox.any'">
                        <error>unverifiable instruction!</error>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.i1'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.u1'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.i2'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.u2'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.i4'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.u4'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.i8'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.u8'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'refanyval'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ckfinite'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'mkrefany'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldtoken'">
                        <error>unverifiable instruction!</error>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.u2'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.u1'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.i'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.i'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.ovf.u'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'add.ovf'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'add.ovf.un'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'mul.ovf'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'mul.ovf.un'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'sub.ovf'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'sub.ovf.un'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'endfinally'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'leave'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'leave.s'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stind.i'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'conv.u'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'arglist'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ceq'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:compare-op($fix-stack),
                                clr:get-intrinsic('bool'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'cgt'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:compare-op($fix-stack),
                                clr:get-intrinsic('bool'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'cgt.un'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:compare-op($fix-stack),
                                clr:get-intrinsic('bool'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'clt'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:compare-op($fix-stack),
                                clr:get-intrinsic('bool'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'clt.un'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                clr:compare-op($fix-stack),
                                clr:get-intrinsic('bool'),
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldftn'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldvirtftn'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldarg'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $arguments[$this/index + 1],
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldarga'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'starg'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                (),
                                $arguments[$this/index + 1]
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldloc'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                $locals[$this/index + 1],
                                ()
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'ldloca'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'stloc'">
                        <xsl:sequence select="
                            clr:alter-stack(
                                $fix-stack,
                                (),
                                $locals[$this/index + 1]
                            )
                        "/>
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'localloc'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'endfilter'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'unaligned.'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'volatile.'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'tail.'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'initobj'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'constrained.'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'cpblk'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'initblk'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'no.'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'rethrow'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'sizeof'">
                    </xsl:when>
    
                    <xsl:when test="$this/name eq 'refanytype'">
                    </xsl:when>
    
                    <xsl:otherwise>
                        <error>unknown instruction!</error>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:variable>

            <xsl:choose>
                <xsl:when test="$mod-stack/self::stack">
                    <xsl:message select="concat('      stack: ', string-join(for $slot in $mod-stack/* return local-name($slot), ','))"/>

                    <xsl:sequence select="
                        clr:verify-all(
                            (
                                if ($this/name = (
                                    'br.s',
                                    'br',
                                    'throw',
                                    'ret',
                                    'jmp',
                                    'leave.s',
                                    'leave'
                                )) then
                                    ()
                                else $next,
                                $target
                            ),
                            $mod-stack,
                            $new-stacks
                        )
                    "/>
                </xsl:when>

                <xsl:when test="count($mod-stack) eq 0">
                    <error>unimplemented instruction :(</error>
                </xsl:when>

                <xsl:otherwise>
                    <xsl:sequence select="$mod-stack"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:function name="clr:verify" as="xs:string">
    <xsl:param name="method" as="element()"/>

    <xsl:message select="concat('verifying: ', $method/name)"/>

    <xsl:variable name="new-stack" as="element()">
        <stack/>
    </xsl:variable>

    <xsl:variable name="result" as="element()+" select="clr:verify($method/instruction[1], $new-stack, ())"/>

    <xsl:choose>
        <xsl:when test="$result/self::stack">
            <xsl:sequence select="'success!'"/>
        </xsl:when>

        <xsl:when test="$result/self::error">
            <xsl:sequence select="concat('error!: ', $result)"/>
        </xsl:when>
    </xsl:choose>
</xsl:function>

<xsl:function name="clr:parse-hex" as="xs:integer">
    <xsl:param name="hex" as="xs:string"/>

    <xsl:variable name="hex-digit" as="xs:string" select="substring($hex, string-length($hex))"/>

    <xsl:choose>
        <xsl:when test="string-length($hex) eq 0">
            <xsl:value-of select="0"/>
        </xsl:when>

        <xsl:otherwise>
            <xsl:value-of select="
                (
                    if ($hex-digit eq '0') then
                        0
                    else if ($hex-digit eq '1') then
                        1
                    else if ($hex-digit eq '2') then
                        2
                    else if ($hex-digit eq '3') then
                        3
                    else if ($hex-digit eq '4') then
                        4
                    else if ($hex-digit eq '5') then
                        5
                    else if ($hex-digit eq '6') then
                        6
                    else if ($hex-digit eq '7') then
                        7
                    else if ($hex-digit eq '8') then
                        8
                    else if ($hex-digit eq '9') then
                        9
                    else if ($hex-digit = ('a', 'A')) then
                        10
                    else if ($hex-digit = ('b', 'B')) then
                        11
                    else if ($hex-digit = ('c', 'C')) then
                        12
                    else if ($hex-digit = ('d', 'D')) then
                        13
                    else if ($hex-digit = ('e', 'E')) then
                        14
                    else if ($hex-digit = ('f', 'F')) then
                        15
                    else ()
                ) + clr:parse-hex(substring($hex, 1, string-length($hex) - 1)) * 16
            "/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:function>

<xsl:template match="/">
    <xsl:for-each select="//method">
        <xsl:message select="concat('class-name: ', ../name)"/>
        <xsl:message select="concat('method-name: ', ../name)"/>

        <xsl:message select="
            concat(
                'signature: (',
                normalize-space(
                    string-join(
                        arguments/element/type,
                        ','
                    )
                ),
                ')'
            )
        "/>

        <xsl:for-each select="instruction">
            <xsl:message select="
                concat(
                    '  ',
                    xs:string(
                        clr:parse-hex(
                            substring(label, 4)
                        )
                    ),
                    ': ',
                    name,
                    ' [',
                    string-join(
                        for $hex in (
                            following-sibling::instruction[1]/label,
                            target
                        ) return
                            xs:string(
                                clr:parse-hex(
                                    substring($hex, 4)
                                )
                            ),
                        ','
                    ),
                    ']'
                )
            "/>
        </xsl:for-each>

        <xsl:message/>
    </xsl:for-each>

    <xsl:for-each select="//method">
        <xsl:message/>
        <xsl:message select="clr:verify(.)"/>
    </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
