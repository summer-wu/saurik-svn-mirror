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

public class DatabaseMetaData
    implements java.sql.DatabaseMetaData
{
    private String url_;

    public boolean allProceduresAreCallable()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:101");     throw new RuntimeException("DatabaseMetaData.java:101");}
    }

    public boolean allTablesAreSelectable()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:106");     throw new RuntimeException("DatabaseMetaData.java:106");}
    }

    public String getURL()
        throws SQLException
    {
        return url_;
    }

    public String getUserName()
        throws SQLException
    {
        return null;
    }

    public boolean isReadOnly()
        throws SQLException
    {
        return false;
    }

    public boolean nullsAreSortedHigh()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:126");     throw new RuntimeException("DatabaseMetaData.java:126");}
    }

    public boolean nullsAreSortedLow()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:131");     throw new RuntimeException("DatabaseMetaData.java:131");}
    }

    public boolean nullsAreSortedAtStart()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:136");     throw new RuntimeException("DatabaseMetaData.java:136");}
    }

    public boolean nullsAreSortedAtEnd()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:141");     throw new RuntimeException("DatabaseMetaData.java:141");}
    }

    public String getDatabaseProductName()
        throws SQLException
    {
        return "Facebook FQL";
    }

    public String getDatabaseProductVersion()
        throws SQLException
    {
        return "1.0";
    }

    public String getDriverName()
        throws SQLException
    {
        return "Facebook FQL";
    }

    public String getDriverVersion()
        throws SQLException
    {
        return "0.1";
    }

    public int getDriverMajorVersion() {
        return 0;
    }

    public int getDriverMinorVersion() {
        return 1;
    }

    public boolean usesLocalFiles()
        throws SQLException
    {
        return false;
    }

    public boolean usesLocalFilePerTable()
        throws SQLException
    {
        return false;
    }

    public boolean supportsMixedCaseIdentifiers()
        throws SQLException
    {
        return false;
    }

    public boolean storesUpperCaseIdentifiers()
        throws SQLException
    {
        return false;
    }

    public boolean storesLowerCaseIdentifiers()
        throws SQLException
    {
        return true;
    }

    public boolean storesMixedCaseIdentifiers()
        throws SQLException
    {
        return false;
    }

    public boolean supportsMixedCaseQuotedIdentifiers()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:202");     throw new RuntimeException("DatabaseMetaData.java:202");}
    }

    public boolean storesUpperCaseQuotedIdentifiers()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:207");     throw new RuntimeException("DatabaseMetaData.java:207");}
    }

    public boolean storesLowerCaseQuotedIdentifiers()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:212");     throw new RuntimeException("DatabaseMetaData.java:212");}
    }

    public boolean storesMixedCaseQuotedIdentifiers()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:217");     throw new RuntimeException("DatabaseMetaData.java:217");}
    }

    public String getIdentifierQuoteString()
        throws SQLException
    {
        return " ";
    }

    public String getSQLKeywords()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:227");     throw new RuntimeException("DatabaseMetaData.java:227");}
    }

    public String getNumericFunctions()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:232");     throw new RuntimeException("DatabaseMetaData.java:232");}
    }

    public String getStringFunctions()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:237");     throw new RuntimeException("DatabaseMetaData.java:237");}
    }

    public String getSystemFunctions()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:242");     throw new RuntimeException("DatabaseMetaData.java:242");}
    }

    public String getTimeDateFunctions()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:247");     throw new RuntimeException("DatabaseMetaData.java:247");}
    }

    public String getSearchStringEscape()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:252");     throw new RuntimeException("DatabaseMetaData.java:252");}
    }

    public String getExtraNameCharacters()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:257");     throw new RuntimeException("DatabaseMetaData.java:257");}
    }

    public boolean supportsAlterTableWithAddColumn()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:262");     throw new RuntimeException("DatabaseMetaData.java:262");}
    }

    public boolean supportsAlterTableWithDropColumn()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:267");     throw new RuntimeException("DatabaseMetaData.java:267");}
    }

    public boolean supportsColumnAliasing()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:272");     throw new RuntimeException("DatabaseMetaData.java:272");}
    }

    public boolean nullPlusNonNullIsNull()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:277");     throw new RuntimeException("DatabaseMetaData.java:277");}
    }

    public boolean supportsConvert()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:282");     throw new RuntimeException("DatabaseMetaData.java:282");}
    }

    public boolean supportsConvert(int a0, int a1)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:287");     throw new RuntimeException("DatabaseMetaData.java:287");}
    }

    public boolean supportsTableCorrelationNames()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:292");     throw new RuntimeException("DatabaseMetaData.java:292");}
    }

    public boolean supportsDifferentTableCorrelationNames()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:297");     throw new RuntimeException("DatabaseMetaData.java:297");}
    }

    public boolean supportsExpressionsInOrderBy()
        throws SQLException
    {
        return false;
    }

    public boolean supportsOrderByUnrelated()
        throws SQLException
    {
        return false;
    }

    public boolean supportsGroupBy()
        throws SQLException
    {
        return false;
    }

    public boolean supportsGroupByUnrelated()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:317");     throw new RuntimeException("DatabaseMetaData.java:317");}
    }

    public boolean supportsGroupByBeyondSelect()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:322");     throw new RuntimeException("DatabaseMetaData.java:322");}
    }

    public boolean supportsLikeEscapeClause()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:327");     throw new RuntimeException("DatabaseMetaData.java:327");}
    }

    public boolean supportsMultipleResultSets()
        throws SQLException
    {
        return false;
    }

    public boolean supportsMultipleTransactions()
        throws SQLException
    {
        return false;
    }

    public boolean supportsNonNullableColumns()
        throws SQLException
    {
        return true;
    }

    public boolean supportsMinimumSQLGrammar()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:347");     throw new RuntimeException("DatabaseMetaData.java:347");}
    }

    public boolean supportsCoreSQLGrammar()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:352");     throw new RuntimeException("DatabaseMetaData.java:352");}
    }

    public boolean supportsExtendedSQLGrammar()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:357");     throw new RuntimeException("DatabaseMetaData.java:357");}
    }

    public boolean supportsANSI92EntryLevelSQL()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:362");     throw new RuntimeException("DatabaseMetaData.java:362");}
    }

    public boolean supportsANSI92IntermediateSQL()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:367");     throw new RuntimeException("DatabaseMetaData.java:367");}
    }

    public boolean supportsANSI92FullSQL()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:372");     throw new RuntimeException("DatabaseMetaData.java:372");}
    }

    public boolean supportsIntegrityEnhancementFacility()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:377");     throw new RuntimeException("DatabaseMetaData.java:377");}
    }

    public boolean supportsOuterJoins()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:382");     throw new RuntimeException("DatabaseMetaData.java:382");}
    }

    public boolean supportsFullOuterJoins()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:387");     throw new RuntimeException("DatabaseMetaData.java:387");}
    }

    public boolean supportsLimitedOuterJoins()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:392");     throw new RuntimeException("DatabaseMetaData.java:392");}
    }

    public String getSchemaTerm()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:397");     throw new RuntimeException("DatabaseMetaData.java:397");}
    }

    public String getProcedureTerm()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:402");     throw new RuntimeException("DatabaseMetaData.java:402");}
    }

    public String getCatalogTerm()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:407");     throw new RuntimeException("DatabaseMetaData.java:407");}
    }

    public boolean isCatalogAtStart()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:412");     throw new RuntimeException("DatabaseMetaData.java:412");}
    }

    public String getCatalogSeparator()
        throws SQLException
    {
        return null;
    }

    public boolean supportsSchemasInDataManipulation()
        throws SQLException
    {
        return false;
    }

    public boolean supportsSchemasInProcedureCalls()
        throws SQLException
    {
        return false;
    }

    public boolean supportsSchemasInTableDefinitions()
        throws SQLException
    {
        return false;
    }

    public boolean supportsSchemasInIndexDefinitions()
        throws SQLException
    {
        return false;
    }

    public boolean supportsSchemasInPrivilegeDefinitions()
        throws SQLException
    {
        return false;
    }

    public boolean supportsCatalogsInDataManipulation()
        throws SQLException
    {
        return false;
    }

    public boolean supportsCatalogsInProcedureCalls()
        throws SQLException
    {
        return false;
    }

    public boolean supportsCatalogsInTableDefinitions()
        throws SQLException
    {
        return false;
    }

    public boolean supportsCatalogsInIndexDefinitions()
        throws SQLException
    {
        return false;
    }

    public boolean supportsCatalogsInPrivilegeDefinitions()
        throws SQLException
    {
        return false;
    }

    public boolean supportsPositionedDelete()
        throws SQLException
    {
        return false;
    }

    public boolean supportsPositionedUpdate()
        throws SQLException
    {
        return false;
    }

    public boolean supportsSelectForUpdate()
        throws SQLException
    {
        return false;
    }

    public boolean supportsStoredProcedures()
        throws SQLException
    {
        return false;
    }

    public boolean supportsSubqueriesInComparisons()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:492");     throw new RuntimeException("DatabaseMetaData.java:492");}
    }

    public boolean supportsSubqueriesInExists()
        throws SQLException
    {
        return true;
    }

    public boolean supportsSubqueriesInIns()
        throws SQLException
    {
        return true;
    }

    public boolean supportsSubqueriesInQuantifieds()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:507");     throw new RuntimeException("DatabaseMetaData.java:507");}
    }

    public boolean supportsCorrelatedSubqueries()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:512");     throw new RuntimeException("DatabaseMetaData.java:512");}
    }

    public boolean supportsUnion()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:517");     throw new RuntimeException("DatabaseMetaData.java:517");}
    }

    public boolean supportsUnionAll()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:522");     throw new RuntimeException("DatabaseMetaData.java:522");}
    }

    public boolean supportsOpenCursorsAcrossCommit()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:527");     throw new RuntimeException("DatabaseMetaData.java:527");}
    }

    public boolean supportsOpenCursorsAcrossRollback()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:532");     throw new RuntimeException("DatabaseMetaData.java:532");}
    }

    public boolean supportsOpenStatementsAcrossCommit()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:537");     throw new RuntimeException("DatabaseMetaData.java:537");}
    }

    public boolean supportsOpenStatementsAcrossRollback()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:542");     throw new RuntimeException("DatabaseMetaData.java:542");}
    }

    public int getMaxBinaryLiteralLength()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:547");     throw new RuntimeException("DatabaseMetaData.java:547");}
    }

    public int getMaxCharLiteralLength()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:552");     throw new RuntimeException("DatabaseMetaData.java:552");}
    }

    public int getMaxColumnNameLength()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:557");     throw new RuntimeException("DatabaseMetaData.java:557");}
    }

    public int getMaxColumnsInGroupBy()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:562");     throw new RuntimeException("DatabaseMetaData.java:562");}
    }

    public int getMaxColumnsInIndex()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:567");     throw new RuntimeException("DatabaseMetaData.java:567");}
    }

    public int getMaxColumnsInOrderBy()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:572");     throw new RuntimeException("DatabaseMetaData.java:572");}
    }

    public int getMaxColumnsInSelect()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:577");     throw new RuntimeException("DatabaseMetaData.java:577");}
    }

    public int getMaxColumnsInTable()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:582");     throw new RuntimeException("DatabaseMetaData.java:582");}
    }

    public int getMaxConnections()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:587");     throw new RuntimeException("DatabaseMetaData.java:587");}
    }

    public int getMaxCursorNameLength()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:592");     throw new RuntimeException("DatabaseMetaData.java:592");}
    }

    public int getMaxIndexLength()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:597");     throw new RuntimeException("DatabaseMetaData.java:597");}
    }

    public int getMaxSchemaNameLength()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:602");     throw new RuntimeException("DatabaseMetaData.java:602");}
    }

    public int getMaxProcedureNameLength()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:607");     throw new RuntimeException("DatabaseMetaData.java:607");}
    }

    public int getMaxCatalogNameLength()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:612");     throw new RuntimeException("DatabaseMetaData.java:612");}
    }

    public int getMaxRowSize()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:617");     throw new RuntimeException("DatabaseMetaData.java:617");}
    }

    public boolean doesMaxRowSizeIncludeBlobs()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:622");     throw new RuntimeException("DatabaseMetaData.java:622");}
    }

    public int getMaxStatementLength()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:627");     throw new RuntimeException("DatabaseMetaData.java:627");}
    }

    public int getMaxStatements()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:632");     throw new RuntimeException("DatabaseMetaData.java:632");}
    }

    public int getMaxTableNameLength()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:637");     throw new RuntimeException("DatabaseMetaData.java:637");}
    }

    public int getMaxTablesInSelect()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:642");     throw new RuntimeException("DatabaseMetaData.java:642");}
    }

    public int getMaxUserNameLength()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:647");     throw new RuntimeException("DatabaseMetaData.java:647");}
    }

    public int getDefaultTransactionIsolation()
        throws SQLException
    {
        return Connection.TRANSACTION_NONE;
    }

    public boolean supportsTransactions()
        throws SQLException
    {
        return false;
    }

    public boolean supportsTransactionIsolationLevel(int a0)
        throws SQLException
    {
        return a0 == Connection.TRANSACTION_NONE;
    }

    public boolean supportsDataDefinitionAndDataManipulationTransactions()
        throws SQLException
    {
        return false;
    }

    public boolean supportsDataManipulationTransactionsOnly()
        throws SQLException
    {
        return false;
    }

    public boolean dataDefinitionCausesTransactionCommit()
        throws SQLException
    {
        return false;
    }

    public boolean dataDefinitionIgnoredInTransactions()
        throws SQLException
    {
        return false;
    }

    public ResultSet getProcedures(String a0, String a1, String a2)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:687");     throw new RuntimeException("DatabaseMetaData.java:687");}
    }

    public ResultSet getProcedureColumns(String a0, String a1, String a2, String a3)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:692");     throw new RuntimeException("DatabaseMetaData.java:692");}
    }

    public ResultSet getTables(String a0, String a1, String a2, String[] a3)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:697");     throw new RuntimeException("DatabaseMetaData.java:697");}
    }

    public ResultSet getSchemas()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:702");     throw new RuntimeException("DatabaseMetaData.java:702");}
    }

    public ResultSet getCatalogs()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:707");     throw new RuntimeException("DatabaseMetaData.java:707");}
    }

    public ResultSet getTableTypes()
        throws SQLException
    {
        return new ResultSet(new ResultSetMetaData(new ColumnMetaData_[] {
            new ColumnMetaData_("TABLE_TYPE", Types.VARCHAR)
        }), new Object[][] {
            new Object[] {"TABLE"}
        });
    }

    public ResultSet getColumns(String a0, String a1, String a2, String a3)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:721");     throw new RuntimeException("DatabaseMetaData.java:721");}
    }

    public ResultSet getColumnPrivileges(String a0, String a1, String a2, String a3)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:726");     throw new RuntimeException("DatabaseMetaData.java:726");}
    }

    public ResultSet getTablePrivileges(String a0, String a1, String a2)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:731");     throw new RuntimeException("DatabaseMetaData.java:731");}
    }

    public ResultSet getBestRowIdentifier(String a0, String a1, String a2, int a3, boolean a4)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:736");     throw new RuntimeException("DatabaseMetaData.java:736");}
    }

    public ResultSet getVersionColumns(String a0, String a1, String a2)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:741");     throw new RuntimeException("DatabaseMetaData.java:741");}
    }

    public ResultSet getPrimaryKeys(String a0, String a1, String a2)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:746");     throw new RuntimeException("DatabaseMetaData.java:746");}
    }

    public ResultSet getImportedKeys(String a0, String a1, String a2)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:751");     throw new RuntimeException("DatabaseMetaData.java:751");}
    }

    public ResultSet getExportedKeys(String a0, String a1, String a2)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:756");     throw new RuntimeException("DatabaseMetaData.java:756");}
    }

    public ResultSet getCrossReference(String a0, String a1, String a2, String a3, String a4, String a5)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:761");     throw new RuntimeException("DatabaseMetaData.java:761");}
    }

    public ResultSet getTypeInfo()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:766");     throw new RuntimeException("DatabaseMetaData.java:766");}
    }

    public ResultSet getIndexInfo(String a0, String a1, String a2, boolean a3, boolean a4)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:771");     throw new RuntimeException("DatabaseMetaData.java:771");}
    }

    public boolean supportsResultSetType(int a0)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:776");     throw new RuntimeException("DatabaseMetaData.java:776");}
    }

    public boolean supportsResultSetConcurrency(int a0, int a1)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:781");     throw new RuntimeException("DatabaseMetaData.java:781");}
    }

    public boolean ownUpdatesAreVisible(int a0)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:786");     throw new RuntimeException("DatabaseMetaData.java:786");}
    }

    public boolean ownDeletesAreVisible(int a0)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:791");     throw new RuntimeException("DatabaseMetaData.java:791");}
    }

    public boolean ownInsertsAreVisible(int a0)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:796");     throw new RuntimeException("DatabaseMetaData.java:796");}
    }

    public boolean othersUpdatesAreVisible(int a0)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:801");     throw new RuntimeException("DatabaseMetaData.java:801");}
    }

    public boolean othersDeletesAreVisible(int a0)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:806");     throw new RuntimeException("DatabaseMetaData.java:806");}
    }

    public boolean othersInsertsAreVisible(int a0)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:811");     throw new RuntimeException("DatabaseMetaData.java:811");}
    }

    public boolean updatesAreDetected(int a0)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:816");     throw new RuntimeException("DatabaseMetaData.java:816");}
    }

    public boolean deletesAreDetected(int a0)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:821");     throw new RuntimeException("DatabaseMetaData.java:821");}
    }

    public boolean insertsAreDetected(int a0)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:826");     throw new RuntimeException("DatabaseMetaData.java:826");}
    }

    public boolean supportsBatchUpdates()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:831");     throw new RuntimeException("DatabaseMetaData.java:831");}
    }

    public ResultSet getUDTs(String a0, String a1, String a2, int[] a3)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:836");     throw new RuntimeException("DatabaseMetaData.java:836");}
    }

    public Connection getConnection()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:841");     throw new RuntimeException("DatabaseMetaData.java:841");}
    }

    public boolean supportsSavepoints()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:846");     throw new RuntimeException("DatabaseMetaData.java:846");}
    }

    public boolean supportsNamedParameters()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:851");     throw new RuntimeException("DatabaseMetaData.java:851");}
    }

    public boolean supportsMultipleOpenResults()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:856");     throw new RuntimeException("DatabaseMetaData.java:856");}
    }

    public boolean supportsGetGeneratedKeys()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:861");     throw new RuntimeException("DatabaseMetaData.java:861");}
    }

    public ResultSet getSuperTypes(String a0, String a1, String a2)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:866");     throw new RuntimeException("DatabaseMetaData.java:866");}
    }

    public ResultSet getSuperTables(String a0, String a1, String a2)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:871");     throw new RuntimeException("DatabaseMetaData.java:871");}
    }

    public ResultSet getAttributes(String a0, String a1, String a2, String a3)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:876");     throw new RuntimeException("DatabaseMetaData.java:876");}
    }

    public boolean supportsResultSetHoldability(int a0)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:881");     throw new RuntimeException("DatabaseMetaData.java:881");}
    }

    public int getResultSetHoldability()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:886");     throw new RuntimeException("DatabaseMetaData.java:886");}
    }

    public int getDatabaseMajorVersion()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:891");     throw new RuntimeException("DatabaseMetaData.java:891");}
    }

    public int getDatabaseMinorVersion()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:896");     throw new RuntimeException("DatabaseMetaData.java:896");}
    }

    public int getJDBCMajorVersion()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:901");     throw new RuntimeException("DatabaseMetaData.java:901");}
    }

    public int getJDBCMinorVersion()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:906");     throw new RuntimeException("DatabaseMetaData.java:906");}
    }

    public int getSQLStateType()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:911");     throw new RuntimeException("DatabaseMetaData.java:911");}
    }

    public boolean locatorsUpdateCopy()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:916");     throw new RuntimeException("DatabaseMetaData.java:916");}
    }

    public boolean supportsStatementPooling()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:921");     throw new RuntimeException("DatabaseMetaData.java:921");}
    }

    public RowIdLifetime getRowIdLifetime()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:926");     throw new RuntimeException("DatabaseMetaData.java:926");}
    }

    public ResultSet getSchemas(String a0, String a1)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:931");     throw new RuntimeException("DatabaseMetaData.java:931");}
    }

    public boolean supportsStoredFunctionsUsingCallSyntax()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:936");     throw new RuntimeException("DatabaseMetaData.java:936");}
    }

    public boolean autoCommitFailureClosesAllResultSets()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:941");     throw new RuntimeException("DatabaseMetaData.java:941");}
    }

    public ResultSet getClientInfoProperties()
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:946");     throw new RuntimeException("DatabaseMetaData.java:946");}
    }

    public ResultSet getFunctions(String a0, String a1, String a2)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:951");     throw new RuntimeException("DatabaseMetaData.java:951");}
    }

    public ResultSet getFunctionColumns(String a0, String a1, String a2, String a3)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:956");     throw new RuntimeException("DatabaseMetaData.java:956");}
    }

    public <T> T unwrap(Class<T> a0)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:961");     throw new RuntimeException("DatabaseMetaData.java:961");}
    }

    public boolean isWrapperFor(Class<?> a0)
        throws SQLException
    {
{System.out.println("DatabaseMetaData.java:966");     throw new RuntimeException("DatabaseMetaData.java:966");}
    }
}
