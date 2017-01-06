package jMonitor;

public class TraceMonitor
    implements Monitor
{
    public static void monitor(EventContext context) {
        System.out.println(context.getEvent());
        Object[] args = context.getArguments();
        for (int i = 0; i != args.length; ++i)
            System.out.println(i + ": " + args[i]);
        try {
            throw new Exception("Trace Monitor");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
