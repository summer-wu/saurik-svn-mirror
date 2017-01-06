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

public class ResultSet
    implements java.sql.ResultSet
{
    private Element response_;
    private Element row_ = null;

    private ResultSetMetaData metadata_;
    private Object[][] data_;
    private int row2_;

    public ResultSet(ResultSetMetaData metadata, Object[][] data) {
        metadata_ = metadata;
        data_ = data;
        row2_ = 0;
    }

    public ResultSet(Document document)
        throws SQLException
    {
        response_ = document.getDocumentElement();

        if (!response_.getTagName().equals("fql_query_response"))
            throw new SQLException("invalid response element: " + response_.getTagName());
    }

    public boolean next()
        throws SQLException
    {
        if (data_ != null)
            if (row2_ == data_.length)
                return false;
            else
                return ++row2_ != data_.length;

        Node row = Utility.SkipTextNodes_(
            row_ == null ?
                response_.getFirstChild() :
                row_.getNextSibling()
        );

        if (row == null)
            return false;
        else {
            row_ = (Element) row;
            return true;
        }
    }

    public void close()
        throws SQLException
    {
        response_ = null;
        row_ = null;
    }

    public boolean wasNull()
        throws SQLException
    {
{System.out.println("ResultSet.java:144");     throw new RuntimeException("ResultSet.java:144");}
    }

    public String getString(int columnIndex)
        throws SQLException
    {
        if (columnIndex < 1)
            throw new SQLException();
        
        if (data_ != null) {
            Object[] row = data_[row2_];
            if (columnIndex < row.length)
                return row[columnIndex].toString();
            else
                throw new SQLException();
        }

        Node node = row_.getFirstChild();

        for (int i = 0;;) {
            node = Utility.SkipTextNodes_(node);
            if (node == null)
                throw new SQLException();
            if (++i == columnIndex)
                return node.getTextContent();
            node = node.getNextSibling();
        }
    }

    public boolean getBoolean(int columnIndex)
        throws SQLException
    {
        return Boolean.parseBoolean(getString(columnIndex));
    }

    public byte getByte(int columnIndex)
        throws SQLException
    {
        return Byte.parseByte(getString(columnIndex));
    }

    public short getShort(int columnIndex)
        throws SQLException
    {
        return Short.parseShort(getString(columnIndex));
    }

    public int getInt(int columnIndex)
        throws SQLException
    {
        return Integer.parseInt(getString(columnIndex));
    }

    public long getLong(int columnIndex)
        throws SQLException
    {
        return Long.parseLong(getString(columnIndex));
    }

    public float getFloat(int columnIndex)
        throws SQLException
    {
        return Float.parseFloat(getString(columnIndex));
    }

    public double getDouble(int columnIndex)
        throws SQLException
    {
        return Double.parseDouble(getString(columnIndex));
    }

    @Deprecated public java.math.BigDecimal getBigDecimal(int a0, int a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:207");     throw new RuntimeException("ResultSet.java:207");}
    }

    public byte[] getBytes(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:212");     throw new RuntimeException("ResultSet.java:212");}
    }

    public Date getDate(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:217");     throw new RuntimeException("ResultSet.java:217");}
    }

    public Time getTime(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:222");     throw new RuntimeException("ResultSet.java:222");}
    }

    public Timestamp getTimestamp(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:227");     throw new RuntimeException("ResultSet.java:227");}
    }

    public java.io.InputStream getAsciiStream(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:232");     throw new RuntimeException("ResultSet.java:232");}
    }

    @Deprecated public java.io.InputStream getUnicodeStream(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:237");     throw new RuntimeException("ResultSet.java:237");}
    }

    public java.io.InputStream getBinaryStream(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:242");     throw new RuntimeException("ResultSet.java:242");}
    }

    public String getString(String columnLabel)
        throws SQLException
    {
        return row_.getElementsByTagName(columnLabel).item(0).getTextContent();
    }

    public boolean getBoolean(String columnLabel)
        throws SQLException
    {
        return Boolean.parseBoolean(getString(columnLabel));
    }

    public byte getByte(String columnLabel)
        throws SQLException
    {
        return Byte.parseByte(getString(columnLabel));
    }

    public short getShort(String columnLabel)
        throws SQLException
    {
        return Short.parseShort(getString(columnLabel));
    }

    public int getInt(String columnLabel)
        throws SQLException
    {
        return Integer.parseInt(getString(columnLabel));
    }

    public long getLong(String columnLabel)
        throws SQLException
    {
        return Long.parseLong(getString(columnLabel));
    }

    public float getFloat(String columnLabel)
        throws SQLException
    {
        return Float.parseFloat(getString(columnLabel));
    }

    public double getDouble(String columnLabel)
        throws SQLException
    {
        return Double.parseDouble(getString(columnLabel));
    }

    @Deprecated public java.math.BigDecimal getBigDecimal(String a0, int a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:287");     throw new RuntimeException("ResultSet.java:287");}
    }

    public byte[] getBytes(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:292");     throw new RuntimeException("ResultSet.java:292");}
    }

    public Date getDate(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:297");     throw new RuntimeException("ResultSet.java:297");}
    }

    public Time getTime(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:302");     throw new RuntimeException("ResultSet.java:302");}
    }

    public Timestamp getTimestamp(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:307");     throw new RuntimeException("ResultSet.java:307");}
    }

    public java.io.InputStream getAsciiStream(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:312");     throw new RuntimeException("ResultSet.java:312");}
    }

    @Deprecated public java.io.InputStream getUnicodeStream(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:317");     throw new RuntimeException("ResultSet.java:317");}
    }

    public java.io.InputStream getBinaryStream(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:322");     throw new RuntimeException("ResultSet.java:322");}
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

    public String getCursorName()
        throws SQLException
    {
{System.out.println("ResultSet.java:336");     throw new RuntimeException("ResultSet.java:336");}
    }

    public ResultSetMetaData getMetaData()
        throws SQLException
    {
        return new ResultSetMetaData(response_);
    }

    public Object getObject(int columnIndex)
        throws SQLException
    {
        /* XXX: this is incorrect */
        return getString(columnIndex);
    }

    public Object getObject(String columnLabel)
        throws SQLException
    {
        return Short.parseShort(getString(columnLabel));
    }

    public int findColumn(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:357");     throw new RuntimeException("ResultSet.java:357");}
    }

    public java.io.Reader getCharacterStream(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:362");     throw new RuntimeException("ResultSet.java:362");}
    }

    public java.io.Reader getCharacterStream(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:367");     throw new RuntimeException("ResultSet.java:367");}
    }

    public java.math.BigDecimal getBigDecimal(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:372");     throw new RuntimeException("ResultSet.java:372");}
    }

    public java.math.BigDecimal getBigDecimal(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:377");     throw new RuntimeException("ResultSet.java:377");}
    }

    public boolean isBeforeFirst()
        throws SQLException
    {
{System.out.println("ResultSet.java:382");     throw new RuntimeException("ResultSet.java:382");}
    }

    public boolean isAfterLast()
        throws SQLException
    {
{System.out.println("ResultSet.java:387");     throw new RuntimeException("ResultSet.java:387");}
    }

    public boolean isFirst()
        throws SQLException
    {
{System.out.println("ResultSet.java:392");     throw new RuntimeException("ResultSet.java:392");}
    }

    public boolean isLast()
        throws SQLException
    {
{System.out.println("ResultSet.java:397");     throw new RuntimeException("ResultSet.java:397");}
    }

    public void beforeFirst()
        throws SQLException
    {
{System.out.println("ResultSet.java:402");     throw new RuntimeException("ResultSet.java:402");}
    }

    public void afterLast()
        throws SQLException
    {
{System.out.println("ResultSet.java:407");     throw new RuntimeException("ResultSet.java:407");}
    }

    public boolean first()
        throws SQLException
    {
{System.out.println("ResultSet.java:412");     throw new RuntimeException("ResultSet.java:412");}
    }

    public boolean last()
        throws SQLException
    {
{System.out.println("ResultSet.java:417");     throw new RuntimeException("ResultSet.java:417");}
    }

    public int getRow()
        throws SQLException
    {
{System.out.println("ResultSet.java:422");     throw new RuntimeException("ResultSet.java:422");}
    }

    public boolean absolute(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:427");     throw new RuntimeException("ResultSet.java:427");}
    }

    public boolean relative(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:432");     throw new RuntimeException("ResultSet.java:432");}
    }

    public boolean previous()
        throws SQLException
    {
{System.out.println("ResultSet.java:437");     throw new RuntimeException("ResultSet.java:437");}
    }

    public void setFetchDirection(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:442");     throw new RuntimeException("ResultSet.java:442");}
    }

    public int getFetchDirection()
        throws SQLException
    {
{System.out.println("ResultSet.java:447");     throw new RuntimeException("ResultSet.java:447");}
    }

    public void setFetchSize(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:452");     throw new RuntimeException("ResultSet.java:452");}
    }

    public int getFetchSize()
        throws SQLException
    {
{System.out.println("ResultSet.java:457");     throw new RuntimeException("ResultSet.java:457");}
    }

    public int getType()
        throws SQLException
    {
{System.out.println("ResultSet.java:462");     throw new RuntimeException("ResultSet.java:462");}
    }

    public int getConcurrency()
        throws SQLException
    {
{System.out.println("ResultSet.java:467");     throw new RuntimeException("ResultSet.java:467");}
    }

    public boolean rowUpdated()
        throws SQLException
    {
{System.out.println("ResultSet.java:472");     throw new RuntimeException("ResultSet.java:472");}
    }

    public boolean rowInserted()
        throws SQLException
    {
{System.out.println("ResultSet.java:477");     throw new RuntimeException("ResultSet.java:477");}
    }

    public boolean rowDeleted()
        throws SQLException
    {
{System.out.println("ResultSet.java:482");     throw new RuntimeException("ResultSet.java:482");}
    }

    public void updateNull(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:487");     throw new RuntimeException("ResultSet.java:487");}
    }

    public void updateBoolean(int a0, boolean a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:492");     throw new RuntimeException("ResultSet.java:492");}
    }

    public void updateByte(int a0, byte a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:497");     throw new RuntimeException("ResultSet.java:497");}
    }

    public void updateShort(int a0, short a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:502");     throw new RuntimeException("ResultSet.java:502");}
    }

    public void updateInt(int a0, int a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:507");     throw new RuntimeException("ResultSet.java:507");}
    }

    public void updateLong(int a0, long a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:512");     throw new RuntimeException("ResultSet.java:512");}
    }

    public void updateFloat(int a0, float a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:517");     throw new RuntimeException("ResultSet.java:517");}
    }

    public void updateDouble(int a0, double a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:522");     throw new RuntimeException("ResultSet.java:522");}
    }

    public void updateBigDecimal(int a0, java.math.BigDecimal a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:527");     throw new RuntimeException("ResultSet.java:527");}
    }

    public void updateString(int a0, String a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:532");     throw new RuntimeException("ResultSet.java:532");}
    }

    public void updateBytes(int a0, byte[] a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:537");     throw new RuntimeException("ResultSet.java:537");}
    }

    public void updateDate(int a0, Date a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:542");     throw new RuntimeException("ResultSet.java:542");}
    }

    public void updateTime(int a0, Time a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:547");     throw new RuntimeException("ResultSet.java:547");}
    }

    public void updateTimestamp(int a0, Timestamp a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:552");     throw new RuntimeException("ResultSet.java:552");}
    }

    public void updateAsciiStream(int a0, java.io.InputStream a1, int a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:557");     throw new RuntimeException("ResultSet.java:557");}
    }

    public void updateBinaryStream(int a0, java.io.InputStream a1, int a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:562");     throw new RuntimeException("ResultSet.java:562");}
    }

    public void updateCharacterStream(int a0, java.io.Reader a1, int a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:567");     throw new RuntimeException("ResultSet.java:567");}
    }

    public void updateObject(int a0, Object a1, int a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:572");     throw new RuntimeException("ResultSet.java:572");}
    }

    public void updateObject(int a0, Object a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:577");     throw new RuntimeException("ResultSet.java:577");}
    }

    public void updateNull(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:582");     throw new RuntimeException("ResultSet.java:582");}
    }

    public void updateBoolean(String a0, boolean a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:587");     throw new RuntimeException("ResultSet.java:587");}
    }

    public void updateByte(String a0, byte a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:592");     throw new RuntimeException("ResultSet.java:592");}
    }

    public void updateShort(String a0, short a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:597");     throw new RuntimeException("ResultSet.java:597");}
    }

    public void updateInt(String a0, int a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:602");     throw new RuntimeException("ResultSet.java:602");}
    }

    public void updateLong(String a0, long a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:607");     throw new RuntimeException("ResultSet.java:607");}
    }

    public void updateFloat(String a0, float a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:612");     throw new RuntimeException("ResultSet.java:612");}
    }

    public void updateDouble(String a0, double a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:617");     throw new RuntimeException("ResultSet.java:617");}
    }

    public void updateBigDecimal(String a0, java.math.BigDecimal a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:622");     throw new RuntimeException("ResultSet.java:622");}
    }

    public void updateString(String a0, String a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:627");     throw new RuntimeException("ResultSet.java:627");}
    }

    public void updateBytes(String a0, byte[] a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:632");     throw new RuntimeException("ResultSet.java:632");}
    }

    public void updateDate(String a0, Date a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:637");     throw new RuntimeException("ResultSet.java:637");}
    }

    public void updateTime(String a0, Time a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:642");     throw new RuntimeException("ResultSet.java:642");}
    }

    public void updateTimestamp(String a0, Timestamp a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:647");     throw new RuntimeException("ResultSet.java:647");}
    }

    public void updateAsciiStream(String a0, java.io.InputStream a1, int a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:652");     throw new RuntimeException("ResultSet.java:652");}
    }

    public void updateBinaryStream(String a0, java.io.InputStream a1, int a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:657");     throw new RuntimeException("ResultSet.java:657");}
    }

    public void updateCharacterStream(String a0, java.io.Reader a1, int a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:662");     throw new RuntimeException("ResultSet.java:662");}
    }

    public void updateObject(String a0, Object a1, int a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:667");     throw new RuntimeException("ResultSet.java:667");}
    }

    public void updateObject(String a0, Object a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:672");     throw new RuntimeException("ResultSet.java:672");}
    }

    public void insertRow()
        throws SQLException
    {
{System.out.println("ResultSet.java:677");     throw new RuntimeException("ResultSet.java:677");}
    }

    public void updateRow()
        throws SQLException
    {
{System.out.println("ResultSet.java:682");     throw new RuntimeException("ResultSet.java:682");}
    }

    public void deleteRow()
        throws SQLException
    {
{System.out.println("ResultSet.java:687");     throw new RuntimeException("ResultSet.java:687");}
    }

    public void refreshRow()
        throws SQLException
    {
{System.out.println("ResultSet.java:692");     throw new RuntimeException("ResultSet.java:692");}
    }

    public void cancelRowUpdates()
        throws SQLException
    {
{System.out.println("ResultSet.java:697");     throw new RuntimeException("ResultSet.java:697");}
    }

    public void moveToInsertRow()
        throws SQLException
    {
{System.out.println("ResultSet.java:702");     throw new RuntimeException("ResultSet.java:702");}
    }

    public void moveToCurrentRow()
        throws SQLException
    {
{System.out.println("ResultSet.java:707");     throw new RuntimeException("ResultSet.java:707");}
    }

    public Statement getStatement()
        throws SQLException
    {
{System.out.println("ResultSet.java:712");     throw new RuntimeException("ResultSet.java:712");}
    }

    public Object getObject(int a0, java.util.Map a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:717");     throw new RuntimeException("ResultSet.java:717");}
    }

    public Ref getRef(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:722");     throw new RuntimeException("ResultSet.java:722");}
    }

    public Blob getBlob(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:727");     throw new RuntimeException("ResultSet.java:727");}
    }

    public Clob getClob(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:732");     throw new RuntimeException("ResultSet.java:732");}
    }

    public Array getArray(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:737");     throw new RuntimeException("ResultSet.java:737");}
    }

    public Object getObject(String a0, java.util.Map a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:742");     throw new RuntimeException("ResultSet.java:742");}
    }

    public Ref getRef(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:747");     throw new RuntimeException("ResultSet.java:747");}
    }

    public Blob getBlob(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:752");     throw new RuntimeException("ResultSet.java:752");}
    }

    public Clob getClob(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:757");     throw new RuntimeException("ResultSet.java:757");}
    }

    public Array getArray(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:762");     throw new RuntimeException("ResultSet.java:762");}
    }

    public Date getDate(int a0, java.util.Calendar a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:767");     throw new RuntimeException("ResultSet.java:767");}
    }

    public Date getDate(String a0, java.util.Calendar a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:772");     throw new RuntimeException("ResultSet.java:772");}
    }

    public Time getTime(int a0, java.util.Calendar a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:777");     throw new RuntimeException("ResultSet.java:777");}
    }

    public Time getTime(String a0, java.util.Calendar a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:782");     throw new RuntimeException("ResultSet.java:782");}
    }

    public Timestamp getTimestamp(int a0, java.util.Calendar a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:787");     throw new RuntimeException("ResultSet.java:787");}
    }

    public Timestamp getTimestamp(String a0, java.util.Calendar a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:792");     throw new RuntimeException("ResultSet.java:792");}
    }

    public java.net.URL getURL(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:797");     throw new RuntimeException("ResultSet.java:797");}
    }

    public java.net.URL getURL(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:802");     throw new RuntimeException("ResultSet.java:802");}
    }

    public void updateRef(int a0, Ref a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:807");     throw new RuntimeException("ResultSet.java:807");}
    }

    public void updateRef(String a0, Ref a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:812");     throw new RuntimeException("ResultSet.java:812");}
    }

    public void updateBlob(int a0, Blob a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:817");     throw new RuntimeException("ResultSet.java:817");}
    }

    public void updateBlob(String a0, Blob a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:822");     throw new RuntimeException("ResultSet.java:822");}
    }

    public void updateClob(int a0, Clob a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:827");     throw new RuntimeException("ResultSet.java:827");}
    }

    public void updateClob(String a0, Clob a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:832");     throw new RuntimeException("ResultSet.java:832");}
    }

    public void updateArray(int a0, Array a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:837");     throw new RuntimeException("ResultSet.java:837");}
    }

    public void updateArray(String a0, Array a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:842");     throw new RuntimeException("ResultSet.java:842");}
    }

    public RowId getRowId(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:847");     throw new RuntimeException("ResultSet.java:847");}
    }

    public RowId getRowId(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:852");     throw new RuntimeException("ResultSet.java:852");}
    }

    public void updateRowId(int a0, RowId a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:857");     throw new RuntimeException("ResultSet.java:857");}
    }

    public void updateRowId(String a0, RowId a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:862");     throw new RuntimeException("ResultSet.java:862");}
    }

    public int getHoldability()
        throws SQLException
    {
{System.out.println("ResultSet.java:867");     throw new RuntimeException("ResultSet.java:867");}
    }

    public boolean isClosed()
        throws SQLException
    {
        return response_ == null;
    }

    public void updateNString(int a0, String a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:877");     throw new RuntimeException("ResultSet.java:877");}
    }

    public void updateNString(String a0, String a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:882");     throw new RuntimeException("ResultSet.java:882");}
    }

    public void updateNClob(int a0, NClob a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:887");     throw new RuntimeException("ResultSet.java:887");}
    }

    public void updateNClob(String a0, NClob a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:892");     throw new RuntimeException("ResultSet.java:892");}
    }

    public NClob getNClob(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:897");     throw new RuntimeException("ResultSet.java:897");}
    }

    public NClob getNClob(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:902");     throw new RuntimeException("ResultSet.java:902");}
    }

    public SQLXML getSQLXML(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:907");     throw new RuntimeException("ResultSet.java:907");}
    }

    public SQLXML getSQLXML(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:912");     throw new RuntimeException("ResultSet.java:912");}
    }

    public void updateSQLXML(int a0, SQLXML a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:917");     throw new RuntimeException("ResultSet.java:917");}
    }

    public void updateSQLXML(String a0, SQLXML a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:922");     throw new RuntimeException("ResultSet.java:922");}
    }

    public String getNString(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:927");     throw new RuntimeException("ResultSet.java:927");}
    }

    public String getNString(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:932");     throw new RuntimeException("ResultSet.java:932");}
    }

    public java.io.Reader getNCharacterStream(int a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:937");     throw new RuntimeException("ResultSet.java:937");}
    }

    public java.io.Reader getNCharacterStream(String a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:942");     throw new RuntimeException("ResultSet.java:942");}
    }

    public void updateNCharacterStream(int a0, java.io.Reader a1, long a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:947");     throw new RuntimeException("ResultSet.java:947");}
    }

    public void updateNCharacterStream(String a0, java.io.Reader a1, long a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:952");     throw new RuntimeException("ResultSet.java:952");}
    }

    public void updateAsciiStream(int a0, java.io.InputStream a1, long a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:957");     throw new RuntimeException("ResultSet.java:957");}
    }

    public void updateBinaryStream(int a0, java.io.InputStream a1, long a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:962");     throw new RuntimeException("ResultSet.java:962");}
    }

    public void updateCharacterStream(int a0, java.io.Reader a1, long a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:967");     throw new RuntimeException("ResultSet.java:967");}
    }

    public void updateAsciiStream(String a0, java.io.InputStream a1, long a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:972");     throw new RuntimeException("ResultSet.java:972");}
    }

    public void updateBinaryStream(String a0, java.io.InputStream a1, long a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:977");     throw new RuntimeException("ResultSet.java:977");}
    }

    public void updateCharacterStream(String a0, java.io.Reader a1, long a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:982");     throw new RuntimeException("ResultSet.java:982");}
    }

    public void updateBlob(int a0, java.io.InputStream a1, long a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:987");     throw new RuntimeException("ResultSet.java:987");}
    }

    public void updateBlob(String a0, java.io.InputStream a1, long a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:992");     throw new RuntimeException("ResultSet.java:992");}
    }

    public void updateClob(int a0, java.io.Reader a1, long a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:997");     throw new RuntimeException("ResultSet.java:997");}
    }

    public void updateClob(String a0, java.io.Reader a1, long a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:1002");      throw new RuntimeException("ResultSet.java:1002");}
    }

    public void updateNClob(int a0, java.io.Reader a1, long a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:1007");      throw new RuntimeException("ResultSet.java:1007");}
    }

    public void updateNClob(String a0, java.io.Reader a1, long a2)
        throws SQLException
    {
{System.out.println("ResultSet.java:1012");      throw new RuntimeException("ResultSet.java:1012");}
    }

    public void updateNCharacterStream(int a0, java.io.Reader a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:1017");      throw new RuntimeException("ResultSet.java:1017");}
    }

    public void updateNCharacterStream(String a0, java.io.Reader a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:1022");      throw new RuntimeException("ResultSet.java:1022");}
    }

    public void updateAsciiStream(int a0, java.io.InputStream a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:1027");      throw new RuntimeException("ResultSet.java:1027");}
    }

    public void updateBinaryStream(int a0, java.io.InputStream a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:1032");      throw new RuntimeException("ResultSet.java:1032");}
    }

    public void updateCharacterStream(int a0, java.io.Reader a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:1037");      throw new RuntimeException("ResultSet.java:1037");}
    }

    public void updateAsciiStream(String a0, java.io.InputStream a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:1042");      throw new RuntimeException("ResultSet.java:1042");}
    }

    public void updateBinaryStream(String a0, java.io.InputStream a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:1047");      throw new RuntimeException("ResultSet.java:1047");}
    }

    public void updateCharacterStream(String a0, java.io.Reader a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:1052");      throw new RuntimeException("ResultSet.java:1052");}
    }

    public void updateBlob(int a0, java.io.InputStream a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:1057");      throw new RuntimeException("ResultSet.java:1057");}
    }

    public void updateBlob(String a0, java.io.InputStream a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:1062");      throw new RuntimeException("ResultSet.java:1062");}
    }

    public void updateClob(int a0, java.io.Reader a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:1067");      throw new RuntimeException("ResultSet.java:1067");}
    }

    public void updateClob(String a0, java.io.Reader a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:1072");      throw new RuntimeException("ResultSet.java:1072");}
    }

    public void updateNClob(int a0, java.io.Reader a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:1077");      throw new RuntimeException("ResultSet.java:1077");}
    }

    public void updateNClob(String a0, java.io.Reader a1)
        throws SQLException
    {
{System.out.println("ResultSet.java:1082");      throw new RuntimeException("ResultSet.java:1082");}
    }

    public <T> T unwrap(Class<T> a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:1087");      throw new RuntimeException("ResultSet.java:1087");}
    }

    public boolean isWrapperFor(Class<?> a0)
        throws SQLException
    {
{System.out.println("ResultSet.java:1092");      throw new RuntimeException("ResultSet.java:1092");}
    }
}
