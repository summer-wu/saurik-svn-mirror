package jMonitor.Testing;

public class Monitor implements jMonitor.Monitor {
    public static void monitor(jMonitor.EventContext context) {
        System.out.println("monitor!");
        System.out.println(context.getEvent());
    }
}
