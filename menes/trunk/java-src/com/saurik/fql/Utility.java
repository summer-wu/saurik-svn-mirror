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
import java.sql.DatabaseMetaData;
import java.sql.Date;
import java.sql.DriverManager;
import java.sql.DriverPropertyInfo;
import java.sql.NClob;
import java.sql.ParameterMetaData;
import java.sql.Ref;
import java.sql.RowId;
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

public class Utility {

public static String Sign(SortedMap<String, String> args, String secret)
    throws NoSuchAlgorithmException
{
    StringBuilder signature = new StringBuilder(); {
        StringBuilder buffer = new StringBuilder(); {
            Iterator<Map.Entry<String, String>> i = args.entrySet().iterator();
            while (i.hasNext()) {
                Map.Entry<String, String> arg = i.next();
                buffer.append(arg.getKey());
                buffer.append('=');
                buffer.append(arg.getValue());
            }

            buffer.append(secret);
        }

        MessageDigest md5 = MessageDigest.getInstance("MD5");

        for (byte b : md5.digest(buffer.toString().getBytes())) {
            signature.append(Integer.toHexString((b & 0xf0) >>> 4));
            signature.append(Integer.toHexString((b & 0x0f) >>> 0));
        }
    }

    return signature.toString();
}

public static boolean Verify(Map<String, String[]> parameterMap, String secret)
    throws NoSuchAlgorithmException
{
    String[] fb_sig = parameterMap.get("fb_sig");
    if (fb_sig == null || fb_sig.length == 0)
        return false;

    TreeMap<String, String> fbSigParameters = new TreeMap<String, String>(); {
        for (Map.Entry<String, String[]> parameter : parameterMap.entrySet()) {
            String key = parameter.getKey();
            if (key.startsWith("fb_sig_"))
                fbSigParameters.put(key.substring(7), parameter.getValue()[0]);
        }
    }

    return Sign(fbSigParameters, secret).equals(fb_sig[0]);
}

public static boolean Verify(Map<String, String[]> parameters, String query, String secret)
    throws NoSuchAlgorithmException, ParseException, UnsupportedEncodingException
{
    Map<String, ArrayList<String>> filters = new HashMap<String, ArrayList<String>>(); {
        if (query != null && query.length() != 0)
            for (String entry : query.split("&")) {
                int equal = entry.indexOf('=');
                if (equal == -1)
                    throw new ParseException("query string format invalid", -1);
                String key = entry.substring(0, equal);
                ArrayList<String> values = filters.get(key);
                if (values == null)
                    values = new ArrayList<String>();
                values.add(URLDecoder.decode(entry.substring(equal + 1), "UTF-8"));
                filters.put(key, values);
            }
    }

    Map<String, String[]> posted = new HashMap<String, String[]>(); {
        for (Map.Entry<String, String[]> entry : parameters.entrySet()) {
            String key = entry.getKey();
            String[] values = entry.getValue(); {
                ArrayList<String> filter = filters.get(key);
                if (filter != null) {
                    ArrayList<String> filtered = new ArrayList<String>();
                    int length = 0;

                    for (String value : values) {
                        int index = filter.indexOf(value);
                        if (index != -1)
                            filter.remove(index);
                        else {
                            filtered.add(value);
                            ++length;
                        }
                    }

                    values = filtered.toArray(new String[length]);
                }
            }

            posted.put(key, values);
        }
    }

    return Verify(posted, secret);
}

public static Node SkipTextNodes_(Node node) {
    for (;;) {
        if (node == null)
            return null;

        if (node.getNodeType() == Node.TEXT_NODE) {
            node = node.getNextSibling();
            continue;
        }

        break;
    }

    return node;
}

}
