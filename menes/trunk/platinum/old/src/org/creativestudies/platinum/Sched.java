package org.creativestudies.platinum;

import java.io.*;
import java.net.*;
import java.sql.*;
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

public class Sched {
    public static void main(String[] args) throws Exception {
        ResourceBundle config = PropertyResourceBundle.getBundle("org.creativestudies.platinum.Platinum");

        Session sess = new Session();
        sess.logOn(config.getString("perm"), config.getString("pin"));
        sess.changePin(false);
        sess.surfer.sendKeys("S" + Surfer.ENTER);
        sess.logOff();
    }
}
