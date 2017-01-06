package SaxonServer;

public class ExitMonitor
    implements jMonitor.Monitor
{
    public static void monitor(jMonitor.EventContext context) {
        throw new ExitException();
    }
}
