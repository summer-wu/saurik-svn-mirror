import joc.*;
import obc.*;

import static joc.Pointer.*;
import static joc.Static.*;

import joc.Message;

import java.util.*;
import java.lang.reflect.*;

public class HelloDesktop
    extends NSValueDecoder
{

    public @Message Object XMLData(int a) {
        System.out.println("works");
        return null;
    }

public HelloDesktop() {
    System.out.println("HelloDesktop");
}

public static void main(String[] arga) {
    System.loadLibrary("jocstrap");
    Scope scope = new Scope(); try {
        Method[] methods = HelloDesktop.class.getDeclaredMethods();
        for (Method method : methods) {
            joc.Message message = method.getAnnotation(joc.Message.class);
            System.out.println("(): " + method.getName());
            System.out.println(message);
            System.out.println(joc.Runtime.getSignature(method));
        }

        NSDate cdate = new NSDate().initWithTimeIntervalSinceNow$(0);
        Date jdate = new Date();

        System.out.println(cdate.timeIntervalSince1970());
        System.out.println((double) jdate.getTime());

        NSMutableArray array = new NSMutableArray();
        System.out.println(array.getClass().getName());
        array = array.initWithCapacity$(10);
        //NSMutableArray array = (NSMutableArray) NSMutableArray.arrayWithCapacity$(10);

        array.addObject$("hello");

        array.addObject$(cdate);
        System.out.println(array.objectAtIndex$(0) instanceof NSDate);

        array.addObject$(jdate);
        System.out.println(array.objectAtIndex$(1) instanceof Date);
    } finally { scope.close(); }
}

}
