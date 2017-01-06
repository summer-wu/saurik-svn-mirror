#!/bin/sh
menes="`dirname "$0"`/.."
JAVABLD=$menes/java-bld
JAVALIB=$menes/java-lib

java \
    -Xbootclasspath/p:$menes/java-bcp/xercesImpl.jar \
    -cp $JAVABLD:$JAVALIB:$JAVALIB/resolver.jar:$JAVALIB/saxon7.jar \
    -Djavax.xml.parsers.DocumentBuilderFactory=org.apache.xerces.jaxp.DocumentBuilderFactoryImpl \
    -Djavax.xml.parsers.SAXParserFactory=org.apache.xerces.jaxp.SAXParserFactoryImpl \
    -Dorg.apache.xerces.xni.parser.XMLParserConfiguration=org.apache.xerces.parsers.XIncludeParserConfiguration \
    SaxonServer.Main \
    -x org.apache.xml.resolver.tools.ResolvingXMLReader \
    -y org.apache.xml.resolver.tools.ResolvingXMLReader \
    -r org.apache.xml.resolver.tools.CatalogResolver \
    "$@"
