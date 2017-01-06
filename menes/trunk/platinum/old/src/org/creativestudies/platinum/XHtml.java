package org.creativestudies.platinum;

import java.io.*;
import java.net.*;
import java.util.*;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import javax.xml.transform.*;
import javax.xml.transform.stream.*;
import javax.xml.transform.dom.*;

import org.apache.xerces.parsers.DOMParser;
import org.apache.xml.utils.PrefixResolver;
import org.apache.xpath.XPathAPI;

import org.w3c.dom.*;
import org.w3c.dom.traversal.NodeIterator;

import org.w3c.tidy.Configuration;
import org.w3c.tidy.Tidy;
import org.xml.sax.*;

public class XHtml {
    public static final DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
    public static final PrefixResolver resolver;
    public static final Transformer serializer;

    static {
        factory.setNamespaceAware(true);

        resolver = new PrefixResolver() {
            public String getNamespaceForPrefix(String prefix) {
                if (prefix.equals("x") || prefix.equals("xhtml"))
                    return "http://www.w3.org/1999/xhtml";
                return null;
            }

            public String getNamespaceForPrefix(String prefix, Node context) {
                return getNamespaceForPrefix(prefix);
            }

            public String getBaseIdentifier() {
                return null;
            }
        };

        Transformer transformer = null;

        try {
            transformer = TransformerFactory.newInstance().newTransformer();
        } catch (TransformerConfigurationException e) {}

        transformer.setOutputProperty(OutputKeys.OMIT_XML_DECLARATION, "yes");
        serializer = transformer;
    }

    public static Node select(Node node, String path)
        throws TransformerException
    {
        //try {
            NodeList nodes = XPathAPI.eval(node, path, resolver).nodelist();
            if (nodes.getLength() > 0)
                return nodes.item(0);
            return null;
        /*} catch (TransformerException e) {
            throw new PlatinumException(e);
        }*/
    }
}
