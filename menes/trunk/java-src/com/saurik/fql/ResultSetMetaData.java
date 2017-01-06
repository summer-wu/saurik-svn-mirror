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
import java.sql.Types;

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

public class ResultSetMetaData
    implements java.sql.ResultSetMetaData
{
    private List<ColumnMetaData_> column_ = new ArrayList<ColumnMetaData_>();

    public ColumnMetaData_ getColumn_(int column)
        throws SQLException
    {
        if (column - 1 < 0 || column > column_.size())
            throw new SQLException();
        return column_.get(column - 1);
    }

    public ResultSetMetaData(Element response) {
        Node column = Utility.SkipTextNodes_(response.getFirstChild()).getFirstChild();
        for (;;) {
            column = Utility.SkipTextNodes_(column);
            if (column == null)
                break;
            column_.add(new ColumnMetaData_(column));
            column = column.getNextSibling();
        }
    }

    public ResultSetMetaData(ColumnMetaData_[] columns) {
        for (ColumnMetaData_ column : columns)
            column_.add(column);
    }

    public int getColumnCount()
        throws SQLException
    {
        return column_.size();
    }

    public boolean isAutoIncrement(int column)
        throws SQLException
    {
        return getColumn_(column).isAutoIncrement();
    }

    public boolean isCaseSensitive(int a0)
        throws SQLException
    {
        return false;
    }

    public boolean isSearchable(int a0)
        throws SQLException
    {
        return false;
    }

    public boolean isCurrency(int column)
        throws SQLException
    {
        return getColumn_(column).isCurrency();
    }

    public int isNullable(int column)
        throws SQLException
    {
        return getColumn_(column).isNullable();
    }

    public boolean isSigned(int column)
        throws SQLException
    {
        return getColumn_(column).isSigned();
    }

    public int getColumnDisplaySize(int column)
        throws SQLException
    {
        return getColumn_(column).getDisplaySize();
    }

    public String getColumnLabel(int column)
        throws SQLException
    {
        return getColumn_(column).getColumnLabel();
    }

    public String getColumnName(int column)
        throws SQLException
    {
        return getColumn_(column).getName();
    }

    public String getSchemaName(int a0)
        throws SQLException
    {
{System.out.println("ResultSetMetaData.java:173");     throw new RuntimeException("ResultSetMetaData.java:173");}
    }

    public int getPrecision(int column)
        throws SQLException
    {
        return getColumn_(column).getPrecision();
    }

    public int getScale(int column)
        throws SQLException
    {
        return getColumn_(column).getScale();
    }

    public String getTableName(int column)
        throws SQLException
    {
        return null;
    }

    public String getCatalogName(int column)
        throws SQLException
    {
        return null;
    }

    public int getColumnType(int column)
        throws SQLException
    {
        return getColumn_(column).getType();
    }

    public String getColumnTypeName(int column)
        throws SQLException
    {
        return getColumn_(column).getTypeName();
    }

    public boolean isReadOnly(int a0)
        throws SQLException
    {
        return true;
    }

    public boolean isWritable(int a0)
        throws SQLException
    {
        return false;
    }

    public boolean isDefinitelyWritable(int a0)
        throws SQLException
    {
        return false;
    }

    public String getColumnClassName(int a0)
        throws SQLException
    {
        return "java.lang.String";
    }

    public <T> T unwrap(Class<T> a0)
        throws SQLException
    {
{System.out.println("ResultSetMetaData.java:228");     throw new RuntimeException("ResultSetMetaData.java:228");}
    }

    public boolean isWrapperFor(Class<?> a0)
        throws SQLException
    {
{System.out.println("ResultSetMetaData.java:233");     throw new RuntimeException("ResultSetMetaData.java:233");}
    }
}
