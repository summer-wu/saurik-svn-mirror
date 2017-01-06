package org.creativestudies.platinum;

import java.io.*;
import java.net.*;
import java.security.*;
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

public class Surfer {
    /////////////////////
    // jTidy Interface
    private static Tidy tidy = new Tidy();
    //
    static {
        tidy.setQuiet(true);
        tidy.setShowWarnings(false);
        tidy.setErrout(new PrintWriter(new NullWriter()));
        tidy.setCharEncoding(Configuration.LATIN1);
        tidy.setNumEntities(true);
        tidy.setQuoteNbsp(true);
        //tidy.setXmlOut(true);
        tidy.setXHTML(true);
        tidy.setXmlSpace(true);
    }
    /////////////////////

    static {
        System.setProperty("java.protocol.handler.pkgs", "com.sun.net.ssl.internal.www.protocol");
        Security.addProvider(new com.sun.net.ssl.internal.ssl.Provider());
    }

    public static final String ENTER = "@E";
    public static final String CLEAR = "@C";
    public static final String DELETE = "@D";
    public static final String TAB = "@T";
    public static final String EOF = "@F";
    public static final String PF1 = "@1";
    public static final String PF2 = "@2";
    public static final String PF3 = "@3";
    public static final String PF4 = "@4";
    public static final String PF5 = "@5";
    public static final String PF6 = "@6";
    public static final String PF7 = "@7";
    public static final String PF8 = "@8";
    public static final String PF9 = "@9";
    public static final String PF10 = "@a";
    public static final String PF11 = "@b";
    public static final String PF12 = "@c";
    public static final String PF13 = "@d";
    public static final String PF14 = "@e";
    public static final String PF15 = "@f";
    public static final String PF16 = "@g";
    public static final String PF17 = "@h";
    public static final String PF18 = "@i";
    public static final String PF19 = "@j";
    public static final String PF20 = "@k";
    public static final String PF21 = "@l";
    public static final String PF22 = "@m";
    public static final String PF23 = "@n";
    public static final String PF24 = "@o";
    public static final String ERASEINPUT = "@A@F";

    private String key = null, screen = null;
    //private Integer cursor = null;

    public String getClockTicks() {
        return key.substring(14);
    }

    public String getSessionKey() {
        return key;
    }

    private String getInputElementString(Document doc, String name)
        throws TransformerException
    {
        Node node = XHtml.select(doc, "//x:input[@name = '" + name + "']/@value");
        return (node == null) ? null : node.getNodeValue();
    }

    private Integer getInputElementInteger(Document doc, String name)
        throws TransformerException
    {
        String value = getInputElementString(doc, name);
        return (value == null) ? null : new Integer(value);
    }

    private void getNewSettings(Document doc)
        throws TransformerException
    {
        //this.cursor = getInputElementInteger(doc, "C_POS");
        this.key = getInputElementString(doc, "SessionKey");
        this.screen = getInputElementString(doc, "SCREEN_ID");
        System.out.println("Key: " + this.key);
    }

    static int ack = 0;

    public Document clean(InputStream is)
        throws SAXException, IOException, ParserConfigurationException, TransformerException
    {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        tidy.parse(is, baos);
        is.close();

        byte[] data = baos.toByteArray();

        FileOutputStream fos = new FileOutputStream("dat" + new Integer(ack++).toString() + ".html");
        fos.write(data);
        fos.close();

        ByteArrayInputStream bais = new ByteArrayInputStream(data);
        Document doc = XHtml.factory.newDocumentBuilder().parse(new InputSource(bais));
        getNewSettings(doc);

        return doc;
    }

    public Document clean(String url) throws Exception {
        return clean(new URL(url).openStream());
    }

    public Document sendKeys(String keys) throws Exception {
        return clean("http://gnet.ucsb.edu/surfer/sendkeys/" + key + "/" + keys + "/" + screen + "/" + getClockTicks());
    }

    public Document refresh() throws Exception {
        return clean("http://gnet.ucsb.edu/surfer/refresh/" + key + "/" + getClockTicks());
    }

    public Document click(String what, int where) throws Exception {
        return clean("http://gnet.ucsb.edu/surfer/click/" + what + "?ScreenID=" + screen + "&SessionKey=" + key + "&C_POS=" + where);
    }

    public Document logOn() throws Exception {
        return clean("http://gnet.ucsb.edu/surfer/gold/login");
    }

    public Document logOff() throws Exception {
        return clean("http://gnet.ucsb.edu/surfer/LogOff?SessionKey=" + key);
    }

    public Document aid(int cursor) throws Exception {
        Properties props = new Properties();
        props.setProperty("a_key", "[pf4]");
        props.setProperty("C_POS", new Integer(cursor).toString());

        return submit(props);
    }

    // Can be removed?
    public Document submit(int cursor, Properties props) throws Exception {
        return submit(props);
    }

    public Document submit(Properties props) throws Exception {
        //props.setProperty("a_key", "");
        //props.setProperty("C_POS", new Integer(cursor).toString());
        props.setProperty("SessionKey", key);
        props.setProperty("SCREEN_ID", screen);

        HttpURLConnection connection = (HttpURLConnection) new URL("http://gnet.ucsb.edu/surfer/form/emulator/" + key).openConnection();
        connection.setDoInput(true);
        connection.setDoOutput(true);
        connection.setRequestMethod("POST");

        PrintWriter printer = new PrintWriter(new OutputStreamWriter(connection.getOutputStream()));

        boolean notFirst = false;
        for (Enumeration names = props.propertyNames(); names.hasMoreElements(); notFirst = true) {
            String name = (String) names.nextElement();
            printer.print(URLEncoder.encode(name) + "=" + URLEncoder.encode(props.getProperty(name)));

            if (notFirst)
                printer.print("&");
        }

        printer.println();
        printer.flush();
        printer.close();

        return clean(connection.getInputStream());
    }
}
