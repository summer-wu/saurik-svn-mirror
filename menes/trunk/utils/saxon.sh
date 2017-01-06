#!/bin/bash
menes="`dirname "$0"`/.."
javabld="$menes/java-bld"
javalib="$menes/java-lib"

if [[ $OSTYPE == 'msys' ]]; then
    CPS=";"
else
    CPS=":"
fi

java \
    -Xbootclasspath/p:$menes/java-bcp/xercesImpl.jar \
    -classpath "$javabld$CPS$javalib$CPS$javalib/resolver.jar$CPS$javalib/saxon7.jar" \
    -Djavax.xml.parsers.DocumentBuilderFactory=org.apache.xerces.jaxp.DocumentBuilderFactoryImpl \
    -Djavax.xml.parsers.SAXParserFactory=org.apache.xerces.jaxp.SAXParserFactoryImpl \
    -Dorg.apache.xerces.xni.parser.XMLParserConfiguration=org.apache.xerces.parsers.XIncludeParserConfiguration \
    net.sf.saxon.Transform \
    -x org.apache.xml.resolver.tools.ResolvingXMLReader \
    -y org.apache.xml.resolver.tools.ResolvingXMLReader \
    -r org.apache.xml.resolver.tools.CatalogResolver \
    "$@"
