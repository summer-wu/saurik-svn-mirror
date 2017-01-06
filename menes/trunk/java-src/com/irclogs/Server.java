package com.irclogs;

import java.io.IOException;

import javax.servlet.ServletException;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.mortbay.jetty.Connector;
import org.mortbay.jetty.Request;

import org.mortbay.jetty.bio.SocketConnector;
import org.mortbay.jetty.handler.AbstractHandler;

public class Server {

public static class Handler
    extends AbstractHandler
{
    public void handle(String target, HttpServletRequest request, HttpServletResponse response, int dispatch)
        throws IOException, ServletException
    {
        response.setContentType("text/html");
        response.setStatus(HttpServletResponse.SC_OK);
        response.getWriter().println(target);
        response.getWriter().println(dispatch);
        ((Request) request).setHandled(true);
    }
}

public static void main(String[] args)
    throws Exception
{
    org.mortbay.jetty.Server server = new org.mortbay.jetty.Server();

    Connector connector = new SocketConnector();
    connector.setHost("irclogs.com");
    connector.setPort(80);
    server.addConnector(connector);

    server.setHandler(new Handler());
    server.start();
}

}
