package com.saurik.uicaboodle;

import joc.Wrapper;

import java.util.ResourceBundle;

import org.mozilla.javascript.Function;
import org.mozilla.javascript.Scriptable;
import org.mozilla.javascript.ScriptableObject;

import org.mozilla.javascript.NativeJavaObject;

public class ScriptableWrapper
    implements Wrapper
{

private ResourceBundle bundle_ = ResourceBundle.getBundle("com.saurik.uicaboodle.ScriptableWrapper");

public static String mangleSelector_(String selector) {
    return selector.replace(":", "$");
}

public Object forwardInvocation(Object object, String selector, Object[] arguments) {
    //System.out.println("#forwardInvocation(" + object + "[" + object.getClass() + "], " + selector + ", [" + arguments.length + "])");
    Object value = ScriptableObject.callMethod((Scriptable) object, mangleSelector_(selector), arguments);

    /* XXX: maybe remove this? */
    if (value instanceof NativeJavaObject)
        value = ((NativeJavaObject) value).unwrap();

    //System.out.println("= " + value + " (" + value.getClass() + ")");
    return value;
}

public String methodSignatureForSelector(Object object, String selector) {
    //System.out.println("#methodSignatureForSelector(" + object + "[" + object.getClass() + "], " + selector + ")");
    Scriptable scriptable = (Scriptable) object;
    String mangled = mangleSelector_(selector);
    return scriptable.get(mangled, scriptable) instanceof Function ?
        bundle_.getString(mangled) : null;
}

public boolean respondsToSelector(Object object, String selector) {
    Scriptable scriptable = (Scriptable) object;
    return scriptable.get(mangleSelector_(selector), scriptable) instanceof Function;
}

}
