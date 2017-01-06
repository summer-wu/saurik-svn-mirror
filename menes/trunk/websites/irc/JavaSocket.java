import java.applet.Applet;
import java.awt.Graphics;

public class JavaSocket extends Applet {
    public static class Socket {
        public String get() {
            return ":(";
        }
    }

    public Object newSocket() {
        return new Socket();
    }

    public void paint(Graphics g) {
        g.drawString("bork", 20, 20);
    }
}
