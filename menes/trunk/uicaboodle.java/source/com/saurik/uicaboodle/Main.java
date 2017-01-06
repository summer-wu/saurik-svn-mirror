package com.saurik.uicaboodle;

import joc.*;
import obc.*;

import static joc.Pointer.*;
import static joc.Static.*;

import java.util.ArrayList;

public class Main {

static { System.loadLibrary("uicaboodle"); }

private static String[] args_;

public static String[] getArguments() {
    return args_;
}

public native static void UIApplicationMain(int argc, Pointer<Pointer<Byte>> argv, Object _class);

public static void main(String[] arga)
    throws Exception
{
    Scope scope = new Scope(); try {
        ArrayList<Pointer<Byte>> argl = new ArrayList<Pointer<Byte>>(arga.length + 1);
        ArrayList<String> args = new ArrayList<String>();

        boolean internal = false;

        for (int i = 1; i != arga.length; ++i)
            if (internal)
                args.add(arga[i]);
            else if (arga[i].equals("--"))
                internal = true;
            else
                argl.add(box(arga[i]));

        args_ = args.toArray(new String[args.size()]);
        argl.add(null);

        Pointer<Pointer<Byte>> argv = box(argl);

        Class _class = Class.forName(arga[0]);
        assert _class != null;
        UIApplicationMain(argl.size() - 1, argv, _class);

        for (Pointer arg : argl)
            free(arg);
        free(argv);
    } finally { scope.close(); }
}

}
