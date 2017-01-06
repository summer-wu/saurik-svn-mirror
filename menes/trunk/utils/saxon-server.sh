#!/bin/sh
menes="`dirname "$0"`/.."

#$menes/utils/jMonitor.sh java-src/SaxonServer/ExitMonitor.txt \
$menes/utils/java.sh \
    -Djavax.xml.parsers.DocumentBuilderFactory=org.apache.xerces.jaxp.DocumentBuilderFactoryImpl \
    -Djavax.xml.parsers.SAXParserFactory=org.apache.xerces.jaxp.SAXParserFactoryImpl \
    -Dorg.apache.xerces.xni.parser.XMLParserConfiguration=org.apache.xerces.parsers.XIncludeParserConfiguration \
    SaxonServer.Main
