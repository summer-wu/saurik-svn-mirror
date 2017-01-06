package com.saurik;

import java.io.IOException;
import java.io.StringReader;
import java.io.StringWriter;

import java.util.Properties;

import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

import org.w3c.dom.Document;
import org.w3c.dom.Node;

import org.apache.commons.lang.StringEscapeUtils;

import org.apache.xml.serializer.OutputPropertiesFactory;
import org.apache.xml.serializer.Method;
import org.apache.xml.serializer.Serializer;
import org.apache.xml.serializer.SerializerFactory;

import nu.validator.htmlparser.dom.HtmlDocumentBuilder;

public class html {
    public static Document Parse(String html)
        throws SAXException, IOException
    {
        Document document = new HtmlDocumentBuilder().parse(new InputSource(new StringReader(html)));
        System.out.println(document.getChildNodes().item(0).getNodeName());
        return document;
    }

    public static String Serialize(Node html)
        throws IOException
    {
        Properties properties = OutputPropertiesFactory.getDefaultMethodProperties(Method.HTML);
        Serializer serializer = SerializerFactory.getSerializer(properties);
        StringWriter writer = new StringWriter();
        serializer.setWriter(writer);
        serializer.asDOMSerializer().serialize(html);
        return writer.toString();
    }

    public static String Escape(String text) {
        return StringEscapeUtils.escapeHtml(text);
    }
}
