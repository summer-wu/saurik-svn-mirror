package com.saurik;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.ResourceBundle;

import com.facebook.api.FacebookRestClient;
import com.facebook.api.ProfileField;

import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.methods.GetMethod;

import org.w3c.dom.Document;
import org.w3c.dom.NodeList;

import com.saurik.sql;

public class fbx {
    private static HttpClient http_ = new HttpClient();

    public static void SetProfileEx(FacebookRestClient fb, Connection cn, long uid)
        throws Exception
    {
        GetMethod get = new GetMethod("http://dreadnought.saurik.com/quotable/___profile_.jsp?fb_sig_user=" + uid);

        try {
            synchronized (http_) {
                http_.executeMethod(get);
            }

            String body = get.getResponseBodyAsString();
            fb.profile_setFBML(body, (int) uid);
        } finally {
            get.releaseConnection();
        }
    }

    public static void SetProfile(FacebookRestClient fb, Connection cn, long uid)
        throws Exception
    {
        GetMethod get = new GetMethod("http://dreadnought.saurik.com/quotable/___profile.jsp?fb_sig_user=" + uid);

        try {
            synchronized (http_) {
                http_.executeMethod(get);
            }

            String body = get.getResponseBodyAsString();
            fb.profile_setFBML(body, (int) uid);
        } finally {
            get.releaseConnection();
        }
    }

    public static void Verify(FacebookRestClient fb, PreparedStatement ps)
        throws Exception
    {
        EnumSet<ProfileField> fields = EnumSet.of(ProfileField.HAS_ADDED_APP);

        ResultSet rs = ps.executeQuery(); try {
            while (rs.next()) {
                long uid = rs.getLong("id");
                Boolean installed = rs.getBoolean("installed");
                if (rs.wasNull())
                    installed = null;

                ArrayList<Integer> uids = new ArrayList<Integer>();
                uids.add((int) uid);

                Document info = fb.users_getInfo(uids, fields);
                info.normalize();

                Boolean has_added_app; {
                    NodeList has_added_apps = info.getElementsByTagName("has_added_app");

                    if (has_added_apps.getLength() == 0)
                        has_added_app = null;
                    else {
                        assert has_added_apps.getLength() == 1;
                        has_added_app = Integer.valueOf(has_added_apps.item(0).getFirstChild().getNodeValue()) != 0;
                    }
                }

                if (
                    installed == null && has_added_app != null ||
                    installed != null && !installed.equals(has_added_app)
                ) {
                    System.out.println(uid + " " + installed + " " + has_added_app);
                }
            }
        } finally {
            rs.close();
        }
    }

    public static void _assert(boolean test)
        throws Exception
    {
        if (!test)
            throw new Exception();
    }

    public static void main(String[] args)
        throws Exception
    {
        Connection cn; {
            Class.forName("org.postgresql.Driver");
            cn = DriverManager.getConnection("jdbc:postgresql:facebook", "postgres", "");
        }

        FacebookRestClient fb; {
            String fb_apikey, fb_secret; {
                PreparedStatement ps = cn.prepareStatement(
                    "select \"apikey\", \"secret\" from \"application\" where \"callback\" = ?"
                );

                ps.setString(1, "quotable");

                ResultSet rs = ps.executeQuery();

                _assert(rs.next());
                fb_apikey = rs.getString("apikey");
                fb_secret = rs.getString("secret");
                _assert(!rs.next());
            }

            fb = new FacebookRestClient(fb_apikey, fb_secret, "cb54d938d912fcd1018b2e0f-3614794");
            //fb.auth_getSession(fb.auth_createToken());
        }

        cn.prepareStatement(
            "set search_path to quotable,public"
        ).executeUpdate();

        sql.Buffer qJoin = new sql.Buffer();

        qJoin.Append(
            "join (" +
                "select " +
                    "\"installation\".\"user\", " +
                    "max(\"installation\".\"at\") as \"at\" " +
                "from \"installation\" " +
                "group by \"installation\".\"user\"" +
            ") as \"installing\" on " +
                "\"installing\".\"user\" = \"user\".\"id\" " +
            "join \"installation\" on " +
                "\"installation\".\"user\" = \"installing\".\"user\" and " +
                "\"installation\".\"at\" = \"installing\".\"at\""
        );

        PreparedStatement ps; {
            sql.Buffer qSelect = new sql.Buffer();

            qSelect.Append(
                "select " +
                    "\"user\".\"id\", " +
                    "\"installation\".\"installed\" " +
                "from \"user\" "
            );

            qSelect.AppendAll(qJoin);

            qSelect.Append(
                " where \"user\".\"id\" <> -1"
            );

            ps = qSelect.Prepare(cn);
        }

        /*Verify(fb, ps);
        if (true)
            return;*/

        sql.Buffer qSelectFromJoinWhere = new sql.Buffer();
        sql.Buffer qOrder = new sql.Buffer();

        qSelectFromJoinWhere.Append(
            "select " +
                "\"user\".\"id\" " +
            "from \"quotable\".\"user\" "
        );

        qSelectFromJoinWhere.AppendAll(qJoin);

        qSelectFromJoinWhere.Append(
            " where \"installation\".\"installed\""
        );

        qOrder.Append(
            " order by \"user\".\"id\" asc"
        );

        PreparedStatement psi; {
            sql.Buffer qSelect = new sql.Buffer();
            qSelect.AppendAll(qSelectFromJoinWhere);
            qSelect.AppendAll(qOrder);
            psi = qSelect.Prepare(cn);
        }

        PreparedStatement psir; {
            sql.Buffer qSelect = new sql.Buffer();
            qSelect.AppendAll(qSelectFromJoinWhere);
            qSelect.Append(" and \"user\".\"ordering\" = 1");
            qSelect.AppendAll(qOrder);
            psir = qSelect.Prepare(cn);
        }

        for (boolean subset = false; !subset; subset = true) {
            ResultSet rs = (subset ? psir : psi).executeQuery(); try {
                while (rs.next()) {
                    long id = rs.getLong("id");
                    System.out.println(id);

                    try {
                        SetProfile(fb, cn, id);
                    } catch (Exception e) {
                        e.printStackTrace(System.out);
                    }
                }
            } finally {
                rs.close();
            }

            System.out.println("=-=-=-=-=-=-=");
            if (true)
                return;
            Thread.sleep(1000 * 60 * 5);
        }
    }
}
