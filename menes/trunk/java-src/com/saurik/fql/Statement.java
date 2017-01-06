// Copyright {{{
/* FQL/JDBC - Level 3 JDBC Driver for Facebook's FQL
 * Copyright (C) 2007  Jay Freeman (saurik)
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
// }}}

package com.saurik.fql;

// Import {{{
import java.io.IOException;
import java.io.StringWriter;
import java.io.UnsupportedEncodingException;

import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.URL;
import java.net.URLDecoder;
import java.net.URLEncoder;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import java.sql.Array;
import java.sql.Blob;
import java.sql.Clob;
import java.sql.CallableStatement;
import java.sql.Date;
import java.sql.DriverManager;
import java.sql.DriverPropertyInfo;
import java.sql.NClob;
import java.sql.ParameterMetaData;
import java.sql.Ref;
import java.sql.RowId;
import java.sql.RowIdLifetime;
import java.sql.Savepoint;
import java.sql.Struct;
import java.sql.SQLClientInfoException;
import java.sql.SQLException;
import java.sql.SQLWarning;
import java.sql.SQLXML;
import java.sql.Time;
import java.sql.Timestamp;

import java.text.ParseException;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.Set;
import java.util.SortedMap;
import java.util.TreeMap;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.json.JSONException;
import org.json.JSONWriter;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

import org.xml.sax.SAXException;
// }}}

public class Statement
    implements java.sql.Statement
{
    private Connection connection_;
    private boolean closed_ = false;
    private ResultSet result_ = null;

    public Statement(Connection connection) {
        connection_ = connection;
    }

    public ResultSet executeQuery(String sql)
        throws SQLException
    {
        SortedMap<String, String> args = new TreeMap<String, String>();
        args.put("query", sql);
        Document document = connection_.callMethod("facebook.fql.query", args);
        return new ResultSet(document);
    }

    public int executeUpdate(String a0)
        throws SQLException
    {
{System.out.println("Statement.java:113");     throw new RuntimeException("Statement.java:113");}
    }

    public void close()
        throws SQLException
    {
        closed_ = true;
    }

    public int getMaxFieldSize()
        throws SQLException
    {
{System.out.println("Statement.java:123");     throw new RuntimeException("Statement.java:123");}
    }

    public void setMaxFieldSize(int a0)
        throws SQLException
    {
{System.out.println("Statement.java:128");     throw new RuntimeException("Statement.java:128");}
    }

    public int getMaxRows()
        throws SQLException
    {
        return 256;
    }

    public void setMaxRows(int a0)
        throws SQLException
    {
{System.out.println("Statement.java:138");     throw new RuntimeException("Statement.java:138");}
    }

    public void setEscapeProcessing(boolean a0)
        throws SQLException
    {
{System.out.println("Statement.java:143");     throw new RuntimeException("Statement.java:143");}
    }

    public int getQueryTimeout()
        throws SQLException
    {
{System.out.println("Statement.java:148");     throw new RuntimeException("Statement.java:148");}
    }

    public void setQueryTimeout(int a0)
        throws SQLException
    {
{System.out.println("Statement.java:153");     throw new RuntimeException("Statement.java:153");}
    }

    public void cancel()
        throws SQLException
    {
    }

    public SQLWarning getWarnings()
        throws SQLException
    {
        return null;
    }

    public void clearWarnings()
        throws SQLException
    {
    }

    public void setCursorName(String a0)
        throws SQLException
    {
{System.out.println("Statement.java:171");     throw new RuntimeException("Statement.java:171");}
    }

    public boolean execute(String a0)
        throws SQLException
    {
        result_ = executeQuery(a0);
        return true;
    }

    public ResultSet getResultSet()
        throws SQLException
    {
        return result_;
    }

    public int getUpdateCount()
        throws SQLException
    {
        return -1;
    }

    public boolean getMoreResults()
        throws SQLException
    {
        return false;
    }

    public void setFetchDirection(int a0)
        throws SQLException
    {
{System.out.println("Statement.java:197");     throw new RuntimeException("Statement.java:197");}
    }

    public int getFetchDirection()
        throws SQLException
    {
{System.out.println("Statement.java:202");     throw new RuntimeException("Statement.java:202");}
    }

    public void setFetchSize(int a0)
        throws SQLException
    {
{System.out.println("Statement.java:207");     throw new RuntimeException("Statement.java:207");}
    }

    public int getFetchSize()
        throws SQLException
    {
{System.out.println("Statement.java:212");     throw new RuntimeException("Statement.java:212");}
    }

    public int getResultSetConcurrency()
        throws SQLException
    {
{System.out.println("Statement.java:217");     throw new RuntimeException("Statement.java:217");}
    }

    public int getResultSetType()
        throws SQLException
    {
{System.out.println("Statement.java:222");     throw new RuntimeException("Statement.java:222");}
    }

    public void addBatch(String a0)
        throws SQLException
    {
{System.out.println("Statement.java:227");     throw new RuntimeException("Statement.java:227");}
    }

    public void clearBatch()
        throws SQLException
    {
{System.out.println("Statement.java:232");     throw new RuntimeException("Statement.java:232");}
    }

    public int[] executeBatch()
        throws SQLException
    {
{System.out.println("Statement.java:237");     throw new RuntimeException("Statement.java:237");}
    }

    public Connection getConnection()
        throws SQLException
    {
{System.out.println("Statement.java:242");     throw new RuntimeException("Statement.java:242");}
    }

    public boolean getMoreResults(int a0)
        throws SQLException
    {
{System.out.println("Statement.java:247");     throw new RuntimeException("Statement.java:247");}
    }

    public ResultSet getGeneratedKeys()
        throws SQLException
    {
{System.out.println("Statement.java:252");     throw new RuntimeException("Statement.java:252");}
    }

    public int executeUpdate(String a0, int a1)
        throws SQLException
    {
{System.out.println("Statement.java:257");     throw new RuntimeException("Statement.java:257");}
    }

    public int executeUpdate(String a0, int[] a1)
        throws SQLException
    {
{System.out.println("Statement.java:262");     throw new RuntimeException("Statement.java:262");}
    }

    public int executeUpdate(String a0, String[] a1)
        throws SQLException
    {
{System.out.println("Statement.java:267");     throw new RuntimeException("Statement.java:267");}
    }

    public boolean execute(String a0, int a1)
        throws SQLException
    {
{System.out.println("Statement.java:272");     throw new RuntimeException("Statement.java:272");}
    }

    public boolean execute(String a0, int[] a1)
        throws SQLException
    {
{System.out.println("Statement.java:277");     throw new RuntimeException("Statement.java:277");}
    }

    public boolean execute(String a0, String[] a1)
        throws SQLException
    {
{System.out.println("Statement.java:282");     throw new RuntimeException("Statement.java:282");}
    }

    public int getResultSetHoldability()
        throws SQLException
    {
{System.out.println("Statement.java:287");     throw new RuntimeException("Statement.java:287");}
    }

    public boolean isClosed()
        throws SQLException
    {
        return closed_;
    }

    public void setPoolable(boolean a0)
        throws SQLException
    {
{System.out.println("Statement.java:297");     throw new RuntimeException("Statement.java:297");}
    }

    public boolean isPoolable()
        throws SQLException
    {
{System.out.println("Statement.java:302");     throw new RuntimeException("Statement.java:302");}
    }

    public <T> T unwrap(Class<T> a0)
        throws SQLException
    {
{System.out.println("Statement.java:307");     throw new RuntimeException("Statement.java:307");}
    }

    public boolean isWrapperFor(Class<?> a0)
        throws SQLException
    {
{System.out.println("Statement.java:312");     throw new RuntimeException("Statement.java:312");}
    }
}
