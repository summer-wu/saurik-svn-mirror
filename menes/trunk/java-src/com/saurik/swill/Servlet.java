/* Swill - JavaScript/Java Templating Engine
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

package com.saurik.swill;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.Reader;
import java.io.StringReader;
import java.io.Writer;

import java.net.URL;

import java.util.Date;

import javax.servlet.ServletConfig;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.mozilla.javascript.Context;
import org.mozilla.javascript.JavaScriptException;
import org.mozilla.javascript.Scriptable;
import org.mozilla.javascript.ScriptableObject;
import org.mozilla.javascript.ImporterTopLevel;

public class Servlet
    extends HttpServlet
{

private static class Global_
    extends ImporterTopLevel
{
    public Global_(Context context) {
        initStandardObjects(context, false);
    }
}

private ServletConfig config_;
private ServletContext context_;

public void init(ServletConfig config)
    throws ServletException
{
    config_ = config;
    context_ = config.getServletContext();
}

public void service(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException
{
    String uri = request.getServletPath(); {
        String info = request.getPathInfo();
        if (info != null)
            uri += info;
    }

    URL url = context_.getResource(uri);
    if (url == null) {
        response.sendError(HttpServletResponse.SC_NOT_FOUND, uri);
        return;
    }

    PrintWriter out = response.getWriter();
    StringBuilder code = new StringBuilder();
    Reader reader = new TemplateReader(new JavaScriptTemplate(), url, code);

    Context context = Context.enter(); try {
        Scriptable scope = new Global_(context); {
            ScriptableObject.putProperty(scope, "request", Context.javaToJS(request, scope));
            ScriptableObject.putProperty(scope, "response", Context.javaToJS(response, scope));
        }

        long time = new Date().getTime();
        Object result = context.evaluateReader(scope, reader, url.toString(), 1, null);
        time = new Date().getTime() - time;

        System.out.println("<!-- render: " + time + "ms -->");
    } catch (Exception e) {
        e.printStackTrace(out);

        BufferedReader lines = new BufferedReader(new StringReader(code.toString()));
        for (int i = 1; ; ++i) {
            String line = lines.readLine();
            if (line == null)
                break;
            out.println(i + ": " + line);
        }
    } finally {
        context.exit();
    }
}

public static void main(String[] args)
    throws Exception
{
    Template template = (Template) Class.forName(args[0]).newInstance();

    StringBuilder code = new StringBuilder();
    TemplateReader reader = new TemplateReader(template, new URL(args[1]), code);

    boolean debug = args.length > 2;
    reader.setDebug(debug);

    Writer writer = new BufferedWriter(new OutputStreamWriter(System.out));

    for (;;) {
        char c; {
            int i = reader.read();
            if (i == -1)
                break;
            c = (char) i;
        }

        if (!debug)
            writer.write(c);
    }

    writer.flush();
}

}
