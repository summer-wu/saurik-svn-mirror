package com.saurik;

import java.io.StringWriter;

import java.sql.Array;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;

import java.util.Date;
import java.util.Deque;
import java.util.LinkedList;

import org.json.JSONArray;
import org.json.JSONString;
import org.json.JSONWriter;

public class sql {

public static class Result
    implements JSONString
{
    private PreparedStatement ps_;
    private ResultSet rs_;

    public Result(PreparedStatement ps, ResultSet rs) {
        ps_ = ps;
        rs_ = rs;
    }

    public String toJSONString() {
        return toJSONString(null);
    }

    public String toJSONString(String primary) {
        try {
            StringWriter writer = new StringWriter();
            JSONWriter json = new JSONWriter(writer);

            ResultSetMetaData meta = rs_.getMetaData();

            if (primary == null)
                json.array();
            else
                json.object();

            while (Next()) {
                if (primary != null)
                    json.key(rs_.getString(primary));

                json.object();

                for (int i = 0; i != meta.getColumnCount(); ++i) {
                    json.key(meta.getColumnName(i + 1));
                    Object value = rs_.getObject(i + 1);
                    if (value instanceof Array)
                        value = new JSONArray(((Array) value).getArray());
                    json.value(value);
                }

                json.endObject();
            }

            if (primary == null)
                json.endArray();
            else
                json.endObject();

            return writer.toString();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public boolean Next()
        throws SQLException
    {
        return rs_.next();
    }

    public Object Get(String name)
        throws SQLException
    {
        return rs_.getObject(name);
    }

    public Object Get(int index)
        throws SQLException
    {
        return rs_.getObject(index);
    }

    public Boolean GetBoolean(String name)
        throws SQLException
    {
        boolean value = rs_.getBoolean(name);
        return rs_.wasNull() ? null : value;
    }

    public Integer GetInteger(String name)
        throws SQLException
    {
        int value = rs_.getInt(name);
        return rs_.wasNull() ? null : value;
    }

    public Long GetLong(String name)
        throws SQLException
    {
        long value = rs_.getLong(name);
        return rs_.wasNull() ? null : value;
    }

    public String GetString(String name)
        throws SQLException
    {
        return rs_.getString(name);
    }

    public Date GetTimestamp(String name)
        throws SQLException
    {
        return rs_.getTimestamp(name);
    }

    public void Close()
        throws SQLException
    {
        rs_.close();
        ps_.close();
    }

    public String getClassName() {
        return "Result";
    }
}

public static class Buffer {
    private StringBuffer query_;
    private Deque<Object> parameters_;

    public Buffer() {
        query_ = new StringBuffer();
        parameters_ = new LinkedList<Object>();
    }

    public void Prepend(String query) {
        query_.insert(0, query);
    }

    public void Prepend(String query, Object... parameters) {
        query_.insert(0, query);
        for (int index = 0; index != parameters.length; ++index)
            parameters_.addFirst(parameters[parameters.length - index - 1]);
    }

    public void PrependAll(String query, Object[] parameters) {
        query_.insert(0, query);
        for (int index = 0; index != parameters.length; ++index)
            parameters_.addFirst(parameters[parameters.length - index - 1]);
    }

    public void Append(String query) {
        query_.append(query);
    }

    public void Append(String query, Object... parameters) {
        query_.append(query);
        for (int index = 0; index != parameters.length; ++index)
            parameters_.addLast(parameters[index]);
    }

    public void AppendAll(String query, Object[] parameters) {
        query_.append(query);
        for (int index = 0; index != parameters.length; ++index)
            parameters_.addLast(parameters[index]);
    }

    public void AppendAll(Buffer buffer) {
        query_.append(buffer.query_);
        for (Object parameter : buffer.parameters_)
            parameters_.addLast(parameter);
    }

    public PreparedStatement Prepare(Connection connection)
        throws SQLException
    {
        PreparedStatement statement = connection.prepareStatement(query_.toString()); {
            int field = 0;
            for (Object parameter : parameters_)
                statement.setObject(++field, parameter);
        }

        return statement;
    }

    public Result Query(Connection connection)
        throws SQLException
    {
        PreparedStatement ps = Prepare(connection);

        ResultSet rs; try {
            rs = ps.executeQuery();
        } catch (SQLException e) {
            ps.close();
            throw e;
        }

        return new Result(ps, rs);
    }

    public void Modify(Connection connection)
        throws SQLException
    {
        PreparedStatement ps = Prepare(connection); try {
            ps.executeUpdate();
        } finally {
            ps.close();
        }
    }
}

public static Result Query(Connection connection, String query, Object... parameters)
    throws SQLException
{
    Buffer buffer = new Buffer();
    buffer.AppendAll(query, parameters);
    return buffer.Query(connection);
}

public static Object Get(Connection connection, String query, Object... parameters)
    throws SQLException
{
    Buffer buffer = new Buffer();
    buffer.AppendAll(query, parameters);

    Result result = buffer.Query(connection); try {
        if (!result.Next())
            return null;
        return result.Get(1);
    } finally {
        result.Close();
    }
}

public static void Modify(Connection connection, String query, Object... parameters)
    throws SQLException
{
    Buffer buffer = new Buffer();
    buffer.AppendAll(query, parameters);
    buffer.Modify(connection);
}

}
