package jMonitor;

import org.apache.bcel.*;
import org.apache.bcel.classfile.*;
import org.apache.bcel.generic.*;

import org.apache.oro.text.regex.*;

import java.io.*;
import java.lang.*;
import java.text.*;
import java.util.*;

public class ClassLoader extends org.apache.bcel.util.ClassLoader {
    private static final String ContextName_ = "jMonitor.EventContext";
    private static final ObjectType ContextType_ = new ObjectType(ContextName_);
    private static final Type ClassType_ = new ObjectType("java.lang.Class");
    private static final Type ClassArray_ = new ArrayType(ClassType_, 1);
    private static final Type[] OneStringArg_ = new Type[] {Type.STRING};
    private static final Type[] ContextArgs_ = new Type[] {Type.STRING, ClassArray_, new ArrayType(Type.OBJECT , 1)};

    private static final Type[] MonitorArgs_ = new Type[] {ContextType_};

    private ArrayList<Event> events_ = new ArrayList<Event>();

    public static class Event {
        String monitor_;
        int type_;
        Pattern pattern_;
    }

    public static final int Before = -1;
    public static final int Instead = 0;
    public static final int After = 1;

    public ClassLoader()
        throws FileNotFoundException, IOException, MalformedPatternException
    {
        Perl5Compiler compiler = new Perl5Compiler();

        FileInputStream file = new FileInputStream(System.getProperty("jmonitor.eventlist"));
        try {
            BufferedReader in = new BufferedReader(new InputStreamReader(file));
            String monitor = null;
            int type = 0;
            for (String line; (line = in.readLine()) != null; )
                if (line.charAt(0) == '#') {
                } else if (line.charAt(0) == '-') {
                    monitor = line.substring(1);
                    type = Before;
                } else if (line.charAt(0) == '+') {
                    monitor = line.substring(1);
                    type = After;
                } else if (line.charAt(0) == '=') {
                    monitor = line.substring(1);
                    type = Instead;
                } else {
                    Event event = new Event();
                    event.monitor_ = monitor;
                    event.type_ = type;
                    event.pattern_ = compiler.compile(line);
                    events_.add(event);
                }
        } finally {
            file.close();
        }
    }

    private void boxType(ConstantPoolGen pool, InstructionList il, InstructionFactory factory, Instruction i, Type type) {
        ObjectType boxed;
        if (type.equals(Type.INT))
            boxed = new ObjectType("java.lang.Integer");
        else {
            il.append(i);
            return;
        }

        il.append(factory.createNew(boxed));
        il.append(factory.createDup(1));
        il.append(i);
        il.append(factory.createInvoke(boxed.getClassName(), "<init>", Type.VOID, new Type[] {type}, Constants.INVOKESPECIAL));
    }

    private void buildType(StringBuffer builder, Type type) {
        builder.append(type);
    }

    private void buildTypes(StringBuffer builder, Type[] types) {
        builder.append("(");
        for (int t = 0; t != types.length; ++t) {
            if (t != 0)
                builder.append(",");
            buildType(builder, types[t]);
        }

        builder.append(")");
    }

    private String getBoxedName(String name) {
        if (name.equals("int"))
            return "java.lang.Integer";
        else
            return name;
    }

    protected JavaClass modifyClass(JavaClass clazz) {
        String[] interfaces = clazz.getInterfaceNames();
        for (int i = 0; i != interfaces.length; ++i)
            if (interfaces[i].equals("jMonitor.Monitor") || interfaces[i].equals("jMonitor.InsteadMonitor"))
                return clazz;

        Perl5Matcher matcher = new Perl5Matcher();

        String clsname = clazz.getClassName();
        //System.out.println(clazz.getClassName());
        Method[] oldmethods = clazz.getMethods();
        ConstantPoolGen pool = new ConstantPoolGen(clazz.getConstantPool());
        InstructionFactory factory = new InstructionFactory(pool);

        ArrayList<Method> newmethods = new ArrayList<Method>();

        for (int m = 0, in = 0; m != oldmethods.length; ++m) {
            MethodGen mg = new MethodGen(oldmethods[m], clsname, pool);

            InstructionList il = mg.getInstructionList();
            if (il == null) {
                newmethods.add(oldmethods[m]);
                continue;
            }

            for (InstructionHandle ih = il.getStart(); ih != null; ih = ih.getNext()) {
                Instruction i = ih.getInstruction();
                short op = i.getOpcode();
                StringBuffer builder = new StringBuffer();

                Type retval = Type.VOID;
                Type[] argtypes = Type.NO_ARGS;

                if (i instanceof FieldInstruction) {
                    FieldInstruction fi = (FieldInstruction) i;
                    Type type = fi.getFieldType(pool);
                    ObjectType object = new ObjectType(fi.getClassName(pool));

                    if (i instanceof GETFIELD) {
                        retval = type;
                        argtypes = new Type[] {object};
                        builder.append("getfield ");
                    } else if (i instanceof GETSTATIC) {
                        retval = type;
                        builder.append("getstatic ");
                    } else if (i instanceof PUTFIELD) {
                        argtypes = new Type[] {object, type};
                        builder.append("putfield ");
                    } else if (i instanceof PUTSTATIC) {
                        argtypes = new Type[] {type};
                        builder.append("putstatic ");
                    }

                    buildType(builder, type);
                    builder.append(" ").append(fi.getClassName(pool));
                    builder.append(".").append(fi.getFieldName(pool));
                } else if (i instanceof InvokeInstruction) {
                    InvokeInstruction ii = (InvokeInstruction) i;
                    retval = ii.getReturnType(pool);
                    argtypes = ii.getArgumentTypes(pool);

                    if (i instanceof INVOKESTATIC)
                        builder.append("invokestatic ");
                    else if (i instanceof INVOKESPECIAL)
                        builder.append("invokespecial ");
                    else {
                        Type[] args = new Type[argtypes.length + 1];
                        args[0] = new ObjectType(ii.getClassName(pool));
                        for (int a = 0; a != argtypes.length; ++a)
                            args[a + 1] = argtypes[a];
                        argtypes = args;

                        if (i instanceof INVOKEVIRTUAL)
                            builder.append("invokevirtual ");
                        else if (i instanceof INVOKEINTERFACE)
                            builder.append("invokeinterface ");
                    }

                    buildType(builder, ii.getReturnType(pool));
                    builder.append(" ").append(ii.getClassName(pool));
                    builder.append(".").append(ii.getMethodName(pool));
                    buildTypes(builder, ii.getArgumentTypes(pool));
                }

                String built = builder.toString();

                ArrayList<Event> matches = new ArrayList<Event>();
                for (Iterator<Event> e = events_.iterator(); e.hasNext(); ) {
                    Event event = e.next();
                    if (matcher.contains(built, event.pattern_))
                        matches.add(event);
                }

                if (!matches.isEmpty()) {
                    String name = "_" + m + "_" + in++;
                    ih.setInstruction(factory.createInvoke(clsname, name, retval, argtypes, Constants.INVOKESTATIC));

                    String[] names = new String[argtypes.length];
                    for (int a = 0; a != argtypes.length; ++a)
                        names[a] = "a" + a;

                    InstructionList stub = new InstructionList();

                    int ctxvar = argtypes.length;

                    stub.append(factory.createNew(ContextName_));
                    stub.append(factory.createDup(1));

                    stub.append(factory.createConstant(built));

                    stub.append(factory.createConstant(new Integer(argtypes.length)));
                    stub.append(factory.createNewArray(ClassType_, (short) 1));

                    for (int a = 0; a != argtypes.length; ++a) {
                        stub.append(factory.createDup(1));
                        stub.append(factory.createConstant(new Integer(a)));
                        stub.append(factory.createConstant(getBoxedName(argtypes[a].toString())));
                        stub.append(factory.createInvoke("java.lang.Class", "forName", ClassType_, OneStringArg_, Constants.INVOKESTATIC));
                        stub.append(factory.createArrayStore(ClassType_));
                    }

                    stub.append(factory.createConstant(new Integer(argtypes.length)));
                    stub.append(factory.createNewArray(Type.OBJECT, (short) 1));

                    for (int a = 0; a != argtypes.length; ++a) {
                        stub.append(factory.createDup(1));
                        stub.append(factory.createConstant(new Integer(a)));
                        boxType(pool, stub, factory, factory.createLoad(argtypes[a], a), argtypes[a]);
                        stub.append(factory.createArrayStore(Type.OBJECT));
                    }
                    
                    stub.append(factory.createInvoke(ContextName_, "<init>", Type.VOID, ContextArgs_, Constants.INVOKESPECIAL));
                    stub.append(factory.createStore(ContextType_, ctxvar));

                    for (Iterator<Event> match = matches.iterator(); match.hasNext(); ) {
                        Event event = match.next();
                        if (event.type_ != Before)
                            continue;
                        stub.append(factory.createLoad(ContextType_, ctxvar));
                        stub.append(factory.createInvoke(event.monitor_, "monitor", Type.VOID, MonitorArgs_, Constants.INVOKESTATIC));
                    }

                    boolean instead = false;

                    for (Iterator<Event> match = matches.iterator(); match.hasNext(); ) {
                        Event event = match.next();
                        if (event.type_ != Instead)
                            continue;
                        instead = true;
                        stub.append(factory.createLoad(ContextType_, ctxvar));
                        stub.append(factory.createInvoke(event.monitor_, "monitor", Type.VOID, MonitorArgs_, Constants.INVOKESTATIC));
                    }

                    if (!instead) {
                        for (int a = 0; a != argtypes.length; ++a)
                            stub.append(factory.createLoad(argtypes[a], a));
                        stub.append(i);
                    }

                    for (Iterator<Event> match = matches.iterator(); match.hasNext(); ) {
                        Event event = match.next();
                        if (event.type_ != After)
                            continue;
                        stub.append(factory.createLoad(ContextType_, ctxvar));
                        stub.append(factory.createInvoke(event.monitor_, "monitor", Type.VOID, MonitorArgs_, Constants.INVOKESTATIC));
                    }

                    stub.append(factory.createReturn(retval));

                    int access = Constants.ACC_PUBLIC | Constants.ACC_STATIC;
                    MethodGen stubmethod = new MethodGen(access, retval, argtypes, names, name, clsname, stub, pool);
                    stubmethod.addLocalVariable("context", ContextType_, ctxvar, stub.getStart(), stub.getEnd());
                    stubmethod.setMaxStack();
                    newmethods.add(stubmethod.getMethod());

                    //System.out.println(stub);
                }

                /*System.out.println(ih);
                System.out.println(built);
                System.out.println(matches.size());*/
            }

            mg.setMaxStack();
            newmethods.add(mg.getMethod());
        }

        Method[] newarray = new Method[newmethods.size()];
        for (int m = 0; m != newarray.length; ++m)
            newarray[m] = newmethods.get(m);
        clazz.setMethods(newarray);
        clazz.setConstantPool(pool.getFinalConstantPool());
        return clazz;
    }
}
