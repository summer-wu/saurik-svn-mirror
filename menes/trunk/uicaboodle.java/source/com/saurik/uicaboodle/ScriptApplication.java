package com.saurik.uicaboodle;

import joc.*;
import obc.*;

import joc.Message;

import static joc.Pointer.*;
import static joc.Static.*;

import java.io.FileReader;
import java.io.Reader;

import java.net.URL;

import org.mozilla.javascript.Context;
import org.mozilla.javascript.ScriptableObject;

public class ScriptApplication
    extends UIApplication
{

@Message public void applicationDidFinishLaunching$(Object unused)
    throws Exception
{
    joc.Runtime.setWrapper(new ScriptableWrapper());

    Context context = Context.enter(); try {
        context.getWrapFactory().setJavaPrimitiveWrap(false);
        ScriptableObject scope = context.initStandardObjects();

        ScriptableObject.putProperty(scope, "application", Context.javaToJS(this, scope));
        ScriptableObject.putProperty(scope, "YES", YES);
        ScriptableObject.putProperty(scope, "NO", NO);
        ScriptableObject.putProperty(scope, "joc", context.evaluateString(scope, "Packages.joc", null, -1, null));
        ScriptableObject.putProperty(scope, "obc", context.evaluateString(scope, "Packages.obc", null, -1, null));

        String path = Main.getArguments()[0];
        System.out.println(path);
        Reader reader = new FileReader(path);
        context.evaluateReader(scope, reader, path, 1, null);
    } finally {
        context.exit();
    }
}

}
