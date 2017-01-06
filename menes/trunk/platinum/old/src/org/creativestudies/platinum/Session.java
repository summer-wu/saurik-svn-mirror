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

public class Session {
    public Surfer surfer = new Surfer();
    private Connection db;

    private static Class driver;
    static {
        try {
            driver = Class.forName("org.postgresql.Driver");
        } catch (ClassNotFoundException e) {
        }
    }

    public Session() throws Exception {
        while (surfer.getSessionKey() == null)
            surfer.logOn();
        db = DriverManager.getConnection("jdbc:postgresql:platinum", "platinum", "zdbvi8wnvs5d8b");
        db.setAutoCommit(false);
    }

    public void logOn(String perm, String pin) throws Exception {
        Properties props = new Properties();
        props.setProperty("F_490_7", perm);
        props.setProperty("F_505_4", pin);

        surfer.submit(490, props);
    }

    public void logOff() throws Exception {
        surfer.logOff();
    }

    public void getSubjects() throws Exception {
        surfer.sendKeys("C" + Surfer.ENTER);

        Properties props = new Properties();
        String code = null;

        db.createStatement().executeUpdate("DELETE FROM Gold_Subjects");
        PreparedStatement sqlCode = db.prepareStatement("INSERT INTO Gold_Subjects (code, name) VALUES (?, ?)");

        for (Document doc = surfer.click("HELP", 491); true; doc = surfer.submit(1704, props)) {
            NodeList codes = XPathAPI.eval(doc, "//x:tr[x:td/x:a/x:b/text()[normalize-space() != '']]", XHtml.resolver).nodelist();
            for (int i = 0; i < codes.getLength(); i++) {
                NodeList parts = XPathAPI.eval(codes.item(i), "x:td/x:a/x:b/text()", XHtml.resolver).nodelist();
                if (parts.getLength() != 2)
                    break;
                try {
                    sqlCode.setString(1, code = parts.item(0).getNodeValue());
                    sqlCode.setString(2, parts.item(1).getNodeValue());
                    System.out.println(code);
                    sqlCode.executeUpdate();
                } catch (SQLException e) {
                    System.err.print(code + ": ");
                    System.err.print(e.toString());
                }
            }

            if (codes.getLength() != 10)
                break;

            props.setProperty("F_1704_8", code + "A");
        }

        db.commit();

        surfer.click("HELP", 1774);
        surfer.sendKeys(Surfer.PF2);
    }

    public void getInstructors() throws Exception {
        surfer.sendKeys("C" + Surfer.ENTER);

        Properties props = new Properties();
        String code = null, first = null;
        //boolean pageOne = true;

        PreparedStatement sqlCode = db.prepareStatement("INSERT INTO Gold_Instructors (Code, Name, Subjects) VALUES (?, ?, ?)");

        pager: for (Document doc = surfer.click("HELP", 1053); true; doc = surfer.submit(props)) {
            NodeList instructors = XPathAPI.eval(doc, "//x:tr/x:td/x:font/x:b/x:a/x:b/text()[normalize-space() != '']", XHtml.resolver).nodelist();
            for (int i = 0 /*(pageOne ? 0 : 2)*/; i < instructors.getLength(); i += 2) {
                Node instructor = instructors.item(i);
                NodeList subjects = XPathAPI.eval(instructor, "../../../../../../following-sibling::x:tr[1]/x:td[2]/x:font//text()", XHtml.resolver).nodelist();

                code = instructor.getNodeValue();
                if (i == 0)
                    first = code;
                else if (code.equals(first))
                    break pager;

                try {
                    sqlCode.setString(1, code);
                    sqlCode.setString(2, instructors.item(i+1).getNodeValue());
                    sqlCode.setString(3, subjects.item(0).getNodeValue());
                    sqlCode.executeUpdate();
                } catch (SQLException e) {
                    System.err.print(code + ": ");
                    System.err.print(e.toString());
                }
            }

            //pageOne = false;
            props.setProperty("F_1704_9", code);
        }

        surfer.click("HELP", 1774);
        surfer.sendKeys(Surfer.PF2);
    }

    public Document searchCourses(String subject, String number, boolean onlyOpen) throws Exception {
        Properties props = new Properties();
        props.setProperty("F_505_8", subject);
        props.setProperty("F_585_5", number);
        props.setProperty("F_1305_1", onlyOpen ? "Y" : "N");
        props.setProperty("F_1065_13", ""); // Instructor
        props.setProperty("F_1145_3", ""); // GE Code
        props.setProperty("F_745_7", ""); // Days
        props.setProperty("F_825_2", ""); // Hours
        props.setProperty("F_828_2", ""); // Minutes

        return surfer.submit(0, props);
    }

    public void changePin(boolean change) throws Exception {
        Properties props = new Properties();
        props.setProperty("F_1124_1", change ? "Y" : "N");

        surfer.submit(1224, props);
    }

    public void goSchedule() throws Exception {
        surfer.sendKeys("C" + Surfer.ENTER);
    }

    public static void main(String[] args) throws Exception {
        ResourceBundle config = PropertyResourceBundle.getBundle("org.creativestudies.platinum.Platinum");

        Session sess = new Session();
        sess.logOn(config.getString("perm"), config.getString("pin"));
        sess.changePin(false);

        sess.getSubjects();
        //sess.getInstructors();
        sess.logOff();
    }
}
