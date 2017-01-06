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

public class PreparedStatement
    extends Statement
    implements java.sql.PreparedStatement
{
    private String[] sql_;
    private Object[] parameters_;

    public PreparedStatement(Connection connection, String sql) {
        super(connection);

        sql_ = sql.split("\\?");
        parameters_ = new Object[sql_.length - 1];
    }

    private String getStatement_() {
        StringBuilder sql = new StringBuilder(); {
            for (int i = 0; i != sql_.length; ++i) {
                if (i != 0) {
                    Object parameter = parameters_[i - 1];

                    if (parameter instanceof Double)
                        parameter = new Integer((int) (double) (Double) parameter);
                    else if (parameter instanceof Float)
                        parameter = new Integer((int) (float) (Float) parameter);

                    if (parameter instanceof Integer)
                        sql.append(parameter);
                    else {
                        sql.append("'");
                        sql.append(parameter.toString().replace("'", "''"));
                        sql.append("'");
                    }
                }

                sql.append(sql_[i]);
            }
        }

        return sql.toString();
    }

    public ResultSet executeQuery()
        throws SQLException
    {
        return super.executeQuery(getStatement_());
    }

    public int executeUpdate()
        throws SQLException
    {
{System.out.println("PreparedStatement.java:134");     throw new RuntimeException("PreparedStatement.java:134");}
    }

    public void setNull(int parameterIndex, int sqlType)
        throws SQLException
    {
        setObject(parameterIndex, null, sqlType);
    }

    public void setBoolean(int parameterIndex, boolean x)
        throws SQLException
    {
        setObject(parameterIndex, x);
    }

    public void setByte(int parameterIndex, byte x)
        throws SQLException
    {
        setObject(parameterIndex, x);
    }

    public void setShort(int parameterIndex, short x)
        throws SQLException
    {
        setObject(parameterIndex, x);
    }

    public void setInt(int parameterIndex, int x)
        throws SQLException
    {
        setObject(parameterIndex, x);
    }

    public void setLong(int parameterIndex, long x)
        throws SQLException
    {
        setObject(parameterIndex, x);
    }

    public void setFloat(int parameterIndex, float x)
        throws SQLException
    {
        setObject(parameterIndex, x);
    }

    public void setDouble(int parameterIndex, double x)
        throws SQLException
    {
        setObject(parameterIndex, x);
    }

    public void setBigDecimal(int parameterIndex, java.math.BigDecimal x)
        throws SQLException
    {
        setObject(parameterIndex, x);
    }

    public void setString(int parameterIndex, String x)
        throws SQLException
    {
        setObject(parameterIndex, x);
    }

    public void setBytes(int parameterIndex, byte[] x)
        throws SQLException
    {
        setObject(parameterIndex, x);
    }

    public void setDate(int parameterIndex, Date x)
        throws SQLException
    {
        setObject(parameterIndex, x);
    }

    public void setTime(int parameterIndex, Time x)
        throws SQLException
    {
        setObject(parameterIndex, x);
    }

    public void setTimestamp(int parameterIndex, Timestamp x)
        throws SQLException
    {
        setObject(parameterIndex, x);
    }

    public void setAsciiStream(int a0, java.io.InputStream a1, int a2)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:209");     throw new RuntimeException("PreparedStatement.java:209");}
    }

    @Deprecated public void setUnicodeStream(int a0, java.io.InputStream a1, int a2)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:214");     throw new RuntimeException("PreparedStatement.java:214");}
    }

    public void setBinaryStream(int a0, java.io.InputStream a1, int a2)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:219");     throw new RuntimeException("PreparedStatement.java:219");}
    }

    public void clearParameters()
        throws SQLException
    {
        for (int i = 0; i != parameters_.length; ++i)
            parameters_[i] = null;
    }

    public void setObject(int parameterIndex, Object x, int targetSqlType)
        throws SQLException
    {
        setObject(parameterIndex, x);
    }

    public void setObject(int parameterIndex, Object x)
        throws SQLException
    {
        parameters_[parameterIndex - 1] = x;
    }

    public boolean execute()
        throws SQLException
    {
{System.out.println("PreparedStatement.java:240");     throw new RuntimeException("PreparedStatement.java:240");}
    }

    public void addBatch()
        throws SQLException
    {
{System.out.println("PreparedStatement.java:245");     throw new RuntimeException("PreparedStatement.java:245");}
    }

    public void setCharacterStream(int a0, java.io.Reader a1, int a2)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:250");     throw new RuntimeException("PreparedStatement.java:250");}
    }

    public void setRef(int a0, Ref a1)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:255");     throw new RuntimeException("PreparedStatement.java:255");}
    }

    public void setBlob(int a0, Blob a1)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:260");     throw new RuntimeException("PreparedStatement.java:260");}
    }

    public void setClob(int a0, Clob a1)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:265");     throw new RuntimeException("PreparedStatement.java:265");}
    }

    public void setArray(int a0, Array a1)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:270");     throw new RuntimeException("PreparedStatement.java:270");}
    }

    public ResultSetMetaData getMetaData()
        throws SQLException
    {
{System.out.println("PreparedStatement.java:275");     throw new RuntimeException("PreparedStatement.java:275");}
    }

    public void setDate(int a0, Date a1, java.util.Calendar a2)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:280");     throw new RuntimeException("PreparedStatement.java:280");}
    }

    public void setTime(int a0, Time a1, java.util.Calendar a2)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:285");     throw new RuntimeException("PreparedStatement.java:285");}
    }

    public void setTimestamp(int a0, Timestamp a1, java.util.Calendar a2)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:290");     throw new RuntimeException("PreparedStatement.java:290");}
    }

    public void setNull(int a0, int a1, String a2)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:295");     throw new RuntimeException("PreparedStatement.java:295");}
    }

    public void setURL(int parameterIndex, java.net.URL x)
        throws SQLException
    {
        setObject(parameterIndex, x);
    }

    public ParameterMetaData getParameterMetaData()
        throws SQLException
    {
{System.out.println("PreparedStatement.java:305");     throw new RuntimeException("PreparedStatement.java:305");}
    }

    public void setRowId(int a0, RowId a1)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:310");     throw new RuntimeException("PreparedStatement.java:310");}
    }

    public void setNString(int a0, String a1)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:315");     throw new RuntimeException("PreparedStatement.java:315");}
    }

    public void setNCharacterStream(int a0, java.io.Reader a1, long a2)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:320");     throw new RuntimeException("PreparedStatement.java:320");}
    }

    public void setNClob(int a0, NClob a1)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:325");     throw new RuntimeException("PreparedStatement.java:325");}
    }

    public void setClob(int a0, java.io.Reader a1, long a2)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:330");     throw new RuntimeException("PreparedStatement.java:330");}
    }

    public void setBlob(int a0, java.io.InputStream a1, long a2)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:335");     throw new RuntimeException("PreparedStatement.java:335");}
    }

    public void setNClob(int a0, java.io.Reader a1, long a2)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:340");     throw new RuntimeException("PreparedStatement.java:340");}
    }

    public void setSQLXML(int a0, SQLXML a1)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:345");     throw new RuntimeException("PreparedStatement.java:345");}
    }

    public void setObject(int a0, Object a1, int a2, int a3)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:350");     throw new RuntimeException("PreparedStatement.java:350");}
    }

    public void setAsciiStream(int a0, java.io.InputStream a1, long a2)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:355");     throw new RuntimeException("PreparedStatement.java:355");}
    }

    public void setBinaryStream(int a0, java.io.InputStream a1, long a2)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:360");     throw new RuntimeException("PreparedStatement.java:360");}
    }

    public void setCharacterStream(int a0, java.io.Reader a1, long a2)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:365");     throw new RuntimeException("PreparedStatement.java:365");}
    }

    public void setAsciiStream(int a0, java.io.InputStream a1)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:370");     throw new RuntimeException("PreparedStatement.java:370");}
    }

    public void setBinaryStream(int a0, java.io.InputStream a1)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:375");     throw new RuntimeException("PreparedStatement.java:375");}
    }

    public void setCharacterStream(int a0, java.io.Reader a1)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:380");     throw new RuntimeException("PreparedStatement.java:380");}
    }

    public void setNCharacterStream(int a0, java.io.Reader a1)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:385");     throw new RuntimeException("PreparedStatement.java:385");}
    }

    public void setClob(int a0, java.io.Reader a1)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:390");     throw new RuntimeException("PreparedStatement.java:390");}
    }

    public void setBlob(int a0, java.io.InputStream a1)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:395");     throw new RuntimeException("PreparedStatement.java:395");}
    }

    public void setNClob(int a0, java.io.Reader a1)
        throws SQLException
    {
{System.out.println("PreparedStatement.java:400");     throw new RuntimeException("PreparedStatement.java:400");}
    }
}
