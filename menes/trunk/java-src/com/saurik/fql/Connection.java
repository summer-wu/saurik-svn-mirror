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

public class Connection
    implements java.sql.Connection
{
    private boolean closed_ = false;

    private String apikey_;
    private String secret_;
    private String session_;

    private URL server_;

    public Connection(String url)
        throws SQLException
    {
        this(url, null);
    }

    public Connection(String url, Properties info)
        throws SQLException
    {
        Pattern pattern = Pattern.compile("jdbc:saurik:fql:([0-9a-z]+);secret=([0-9a-z]+)(;session=([-0-9a-z]+))?");
        Matcher matcher = pattern.matcher(url);

        if (!matcher.matches())
            throw new SQLException("invalid url");

        apikey_ = matcher.group(1);
        secret_ = matcher.group(2);
        session_ = matcher.group(4);

        try {
            server_ = new URL("http://api.facebook.com/restserver.php");
        } catch (MalformedURLException e) {
            throw new SQLException(e);
        }
    }

    // Call {{{
    public Document callMethod(String name, SortedMap<String, String> args)
        throws SQLException
    {
        args.put("method", name);
        args.put("v", "1.0");

        args.put("api_key", apikey_);

        if (session_ != null)
            args.put("session_key", session_);

        String signature; {
            try {
                signature = Utility.Sign(args, secret_);
            } catch (NoSuchAlgorithmException e) {
                throw new SQLException(e);
            }
        }

        args.put("sig", signature);

        HttpURLConnection connection; {
            try {
                connection = (HttpURLConnection) server_.openConnection();
            } catch (IOException e) {
                throw new SQLException(e);
            }

            try {
                connection.setRequestMethod("POST");
            } catch (ProtocolException e) {
                throw new SQLException(e);
            }

            connection.setDoOutput(true);

            try {
                connection.connect();
            } catch (IOException e) {
                throw new SQLException(e);
            }
        }

        {
            StringBuilder buffer = new StringBuilder(); {
                Iterator<Map.Entry<String, String>> i = args.entrySet().iterator();
                boolean ampersand = true;

                while (i.hasNext()) {
                    if (ampersand)
                        ampersand = false;
                    else
                        buffer.append('&');

                    Map.Entry<String, String> arg = i.next();
                    buffer.append(arg.getKey());
                    buffer.append('=');

                    try  {
                        buffer.append(URLEncoder.encode(arg.getValue(), "UTF-8"));
                    } catch (UnsupportedEncodingException e) {
                        throw new SQLException(e);
                    }
                }
            }

            try {
                connection.getOutputStream().write(buffer.toString().getBytes());
            } catch (IOException e) {
                throw new SQLException(e);
            }
        }

        Document document; {
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            factory.setNamespaceAware(true);

            DocumentBuilder builder; try {
                builder = factory.newDocumentBuilder();
            } catch (ParserConfigurationException e) {
                throw new SQLException(e);
            }

            try {
                document = builder.parse(connection.getInputStream());
            } catch (IOException e) {
                throw new SQLException(e);
            } catch (SAXException e) {
                throw new SQLException(e);
            }

            document.normalizeDocument();
        }

        Element response = document.getDocumentElement();
        if (response.getTagName().equals("error_response"))
            throw new SQLException(response.getElementsByTagName("error_msg").item(0).getTextContent());

        return document;
    }
    // }}}

    private String commaDelimited_(Set<Long> values) {
        StringBuilder delimited = new StringBuilder(); {
            boolean comma = false;

            for (Long value : values) {
                if (comma)
                    delimited.append(',');
                else
                    comma = true;

                delimited.append(values);
            }
        }

        return delimited.toString();
    }

    private String jsonEncode_(Map<String, String> values)
        throws SQLException
    {
        StringWriter writer = new StringWriter();
        JSONWriter json = new JSONWriter(writer);

        try {
            json.object();

            for (Map.Entry<String, String> value : values.entrySet()) {
                json.key(value.getKey());
                json.value(value.getValue());
            }

            json.endObject();
        } catch (JSONException e) {
            throw new SQLException(e);
        }

        return writer.toString();
    }

    public boolean sms_canSend(
        Long uid
    )
        throws SQLException
    {
        SortedMap<String, String> args = new TreeMap<String, String>();

        if (uid != null)
            args.put("uid", uid.toString());

        Document document = callMethod("facebook.sms.canSend", args);
        Element response = document.getDocumentElement();

        if (!response.getTagName().equals("sms_canSend_response"))
            throw new SQLException("invalid response element: " + response.getTagName());

        int value = Integer.parseInt(response.getTextContent().trim());
        return value == 0;
    }

    public Integer sms_send(
        Long uid,
        String message,
        Integer session_id,
        Boolean req_session
    )
        throws SQLException
    {
        SortedMap<String, String> args = new TreeMap<String, String>();

        if (uid != null)
            args.put("uid", uid.toString());
        if (message != null)
            args.put("message", message);
        if (session_id != null)
            args.put("session_id", session_id.toString());
        if (req_session != null)
            args.put("req_session", req_session ? "1" : "0");

        Document document = callMethod("facebook.sms.send", args);
        Element response = document.getDocumentElement();

        int value = Integer.parseInt(response.getTextContent().trim());
        return value == 0 ? null : value;
    }

    public void feed_publishTemplatizedAction(
        Long actorId,
        String titleTemplate,
        Map<String, String> titleData,
        String bodyTemplate,
        Map<String, String> bodyData,
        String bodyGeneral,
        Set<Long> targetIds
    )
        throws SQLException
    {
        SortedMap<String, String> args = new TreeMap<String, String>();

        if (actorId != null)
            args.put("actor_id", actorId.toString());
        if (titleTemplate != null)
            args.put("title_template", titleTemplate);
        if (titleData != null)
            args.put("title_data", jsonEncode_(titleData));
        if (bodyTemplate != null)
            args.put("body_template", bodyTemplate);
        if (bodyData != null)
            args.put("body_data", jsonEncode_(bodyData));
        if (bodyGeneral != null)
            args.put("body_general", bodyGeneral);
        if (targetIds != null)
            args.put("target_ids", commaDelimited_(targetIds));

        Document document = callMethod("feed.publishTemplatizedAction", args);
        Element response = document.getDocumentElement();

        if (!response.getTagName().equals("feed_publishTemplatizedAction_response"))
            throw new SQLException("invalid response element: " + response.getTagName());

        if (Integer.parseInt(response.getTextContent().trim()) == 0)
            throw new SQLException("publishTemplatizedAction permissions error");
    }

    // Warnings {{{
    public void clearWarnings()
        throws SQLException
    {
    }

    public SQLWarning getWarnings()
        throws SQLException
    {
        return null;
    }
    // }}}
    // Creation {{{
    public Array createArrayOf(String typeName, Object[] elements)
        throws SQLException
    {
        throw new SQLException("creation is not supported");
    }

    public Blob createBlob()
        throws SQLException
    {
        throw new SQLException("creation is not supported");
    }

    public Clob createClob()
        throws SQLException
    {
        throw new SQLException("creation is not supported");
    }

    public NClob createNClob()
        throws SQLException
    {
        throw new SQLException("creation is not supported");
    }

    public SQLXML createSQLXML()
        throws SQLException
    {
        throw new SQLException("creation is not supported");
    }

    public Struct createStruct(String typeName, Object[] attributes)
        throws SQLException
    {
        throw new SQLException("creation is not supported");
    }
    // }}}
    // Client Info {{{
    public Properties getClientInfo()
        throws SQLException
    {
        throw new SQLException("client info is not supported");
    }

    public String getClientInfo(String name)
        throws SQLException
    {
        throw new SQLException("client info is not supported");
    }

    public void setClientInfo(Properties properties)
        throws SQLClientInfoException
    {
        throw new SQLClientInfoException();
    }

    public void setClientInfo(String name, String value)
        throws SQLClientInfoException
    {
        throw new SQLClientInfoException();
    }
    // }}}
    // Transactions {{{
    public void commit()
        throws SQLException
    {
        throw new SQLException("transactions are not supported");
    }

    public boolean getAutoCommit()
        throws SQLException
    {
        return true;
    }

    public int getTransactionIsolation()
        throws SQLException
    {
        return Connection.TRANSACTION_NONE;
    }

    public void releaseSavepoint(Savepoint savepoint)
        throws SQLException
    {
        throw new SQLException("transactions are not supported");
    }

    public void rollback()
        throws SQLException
    {
        throw new SQLException("transactions are not supported");
    }

    public void rollback(Savepoint savepoint)
        throws SQLException
    {
        throw new SQLException("transactions are not supported");
    }

    public void setAutoCommit(boolean autoCommit)
        throws SQLException
    {
        if (!autoCommit)
            throw new SQLException("transactions are not supported");
    }

    public Savepoint setSavepoint()
        throws SQLException
    {
        throw new SQLException("transactions are not supported");
    }

    public Savepoint setSavepoint(String name)
        throws SQLException
    {
        throw new SQLException("transactions are not supported");
    }

    public void setTransactionIsolation(int level)
        throws SQLException
    {
        throw new SQLException("transactions are not supported");
    }
    // }}}
    // Wrapper {{{
    public boolean isWrapperFor(Class<?> iface)
        throws SQLException
    {
{System.out.println("Connection.java:427");     throw new RuntimeException("Connection.java:427");}
    }

    public <T> T unwrap(Class<T> iface)
        throws SQLException
    {
{System.out.println("Connection.java:432");     throw new RuntimeException("Connection.java:432");}
    }
    // }}}
    // Auto-Generated Keys {{{
    public PreparedStatement prepareStatement(String sql, int autoGeneratedKeys)
        throws SQLException
    {
{System.out.println("Connection.java:439");     throw new RuntimeException("Connection.java:439");}
    }

    public PreparedStatement prepareStatement(String sql, int[] columnIndexes)
        throws SQLException
    {
{System.out.println("Connection.java:444");     throw new RuntimeException("Connection.java:444");}
    }

    public PreparedStatement prepareStatement(String sql, String[] columnNames)
        throws SQLException
    {
{System.out.println("Connection.java:449");     throw new RuntimeException("Connection.java:449");}
    }
    // }}}
    // Advanced {{{
    public Statement createStatement(int resultSetType, int resultSetConcurrency)
        throws SQLException
    {
{System.out.println("Connection.java:456");     throw new RuntimeException("Connection.java:456");}
    }

    public Statement createStatement(int resultSetType, int resultSetConcurrency, int resultSetHoldability)
        throws SQLException
    {
{System.out.println("Connection.java:461");     throw new RuntimeException("Connection.java:461");}
    }

    public CallableStatement prepareCall(String sql, int resultSetType, int resultSetConcurrency)
        throws SQLException
    {
{System.out.println("Connection.java:466");     throw new RuntimeException("Connection.java:466");}
    }

    public CallableStatement prepareCall(String sql, int resultSetType, int resultSetConcurrency, int resultSetHoldability)
        throws SQLException
    {
{System.out.println("Connection.java:471");     throw new RuntimeException("Connection.java:471");}
    }

    public PreparedStatement prepareStatement(String sql, int resultSetType, int resultSetConcurrency)
        throws SQLException
    {
{System.out.println("Connection.java:476");     throw new RuntimeException("Connection.java:476");}
    }

    public PreparedStatement prepareStatement(String sql, int resultSetType, int resultSetConcurrency, int resultSetHoldability)
        throws SQLException
    {
{System.out.println("Connection.java:481");     throw new RuntimeException("Connection.java:481");}
    }
    // }}}

    public void close()
        throws SQLException
    {
        closed_ = true;
    }

    public Statement createStatement()
        throws SQLException
    {
        return new Statement(this);
    }

    public String getCatalog()
        throws SQLException
    {
        return null;
    }

    public int getHoldability()
        throws SQLException
    {
{System.out.println("Connection.java:502");     throw new RuntimeException("Connection.java:502");}
    }

    public DatabaseMetaData getMetaData()
        throws SQLException
    {
        return new DatabaseMetaData();
    }

    public Map<String, Class<?>> getTypeMap()
        throws SQLException
    {
{System.out.println("Connection.java:512");     throw new RuntimeException("Connection.java:512");}
    }

    public boolean isClosed()
        throws SQLException
    {
        return closed_;
    }

    public boolean isReadOnly()
        throws SQLException
    {
        return true;
    }

    public boolean isValid(int timeout)
        throws SQLException
    {
        if (timeout < 0)
            throw new SQLException("timeout < 0");
        if (isClosed())
            return false;

{System.out.println("Connection.java:531");     throw new RuntimeException("Connection.java:531");}
    }

    public String nativeSQL(String sql)
        throws SQLException
    {
{System.out.println("Connection.java:536");     throw new RuntimeException("Connection.java:536");}
    }

    public CallableStatement prepareCall(String sql)
        throws SQLException
    {
{System.out.println("Connection.java:541");     throw new RuntimeException("Connection.java:541");}
    }

    public PreparedStatement prepareStatement(String sql)
        throws SQLException
    {
        return new PreparedStatement(this, sql);
    }

    public void setCatalog(String catalog)
        throws SQLException
    {
    }

    public void setHoldability(int holdability)
        throws SQLException
    {
{System.out.println("Connection.java:555");     throw new RuntimeException("Connection.java:555");}
    }

    public void setReadOnly(boolean readonly)
        throws SQLException
    {
        if (!readonly)
            throw new SQLException("readonly != true");
    }

    public void setTypeMap(Map<String, Class<?>> map)
        throws SQLException
    {
{System.out.println("Connection.java:566");     throw new RuntimeException("Connection.java:566");}
    }
}
