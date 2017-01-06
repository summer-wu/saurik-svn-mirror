package joc;

import java.lang.reflect.Method;
import java.lang.reflect.Type;

import java.lang.reflect.*;

import static joc.Pointer.*;
import static joc.Static.*;

public class Runtime {
    static { System.loadLibrary("jocstrap"); }

    public native static void allocate(Strap object);

    // XXX: move these functions out to a WrapperFactory
    private static Wrapper wrapper_ = null;

    public static void setWrapper(Wrapper wrapper) {
        wrapper_ = wrapper;
    }

    public static Wrapper getWrapper(Object object) {
        return wrapper_;
    }

    public native static String toString(obc.NSString string);

    public native static Object ffiCall(long function, Class type, Object... arguments);

    public static Object callFunction(String function, Class type, Object... arguments) {
        long handle = dlopen(null); try {
            return ffiCall(dlsym(handle, function), type, arguments);
        } finally {
            dlclose(handle);
        }
    }

    public native static Object msgSend(Object object, Class _class, String selector, Object... arguments);

    public native static long dlopen(String path);
    public native static long dlsym(long handle, String symbol);
    public native static void dlclose(long handle);

    // XXX: structure types
    // XXX: 64-bit machines
    // XXX: I don't think this belongs in runtime?
    public static String getSignature(Class type) {
        if (type == boolean.class)
            return "B";
        else if (type == byte.class)
            return "c";
        else if (type == short.class)
            return "s";
        else if (type == int.class)
            return "i";
        else if (type == long.class)
            return "q";
        else if (type == float.class)
            return "f";
        else if (type == double.class)
            return "d";
        else if (type == Pointer.class)
            return "^v";
        else if (type == Selector.class)
            return ":";
        else
            return "@";
    }

    public static String getSignature(Method method) {
        StringBuffer buffer = new StringBuffer();

        Class[] types = method.getParameterTypes();
        long[] sizes = new long[types.length];

        long offset = 8;
        for (int i = 0; i != types.length; ++i) {
            long size = sizeof(types[i]);
            sizes[i] = size;
            offset += size;
        }

        buffer.append(getSignature(method.getReturnType()));
        buffer.append(offset);

        buffer.append("@0:4");

        offset = 8;
        for (int i = 0; i != types.length; ++i) {
            Class type = types[i];
            buffer.append(getSignature(types[i]));
            buffer.append(offset);
            offset += sizes[i];
        }

        return buffer.toString();
    }
}
