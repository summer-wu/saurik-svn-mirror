package SaxonServer;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.OutputStreamWriter;
import java.io.Reader;
import java.io.Writer;

import java.net.ConnectException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;

public class Main extends Thread {
    public Socket socket_;

    public Main(Socket socket) {
        socket_ = socket;
    }

    public void run() {
        boolean result = true;

        try {
            Writer writer = new OutputStreamWriter(socket_.getOutputStream());
            BufferedReader reader = new BufferedReader(new InputStreamReader(socket_.getInputStream()));
            ObjectOutputStream output = new ObjectOutputStream(socket_.getOutputStream());
            String line = reader.readLine();

            boolean error = false;

            try {
                net.sf.saxon.Transform.main(line.split(" "));
            } catch (ExitException exit) {
                error = true;
                output.writeObject(null);
            } catch (Throwable thrown) {
                error = true;
                output.writeObject(thrown);
            }

            if (!error)
                output.writeObject(null);

            output.flush();
        } catch (Throwable thrown) {
        } finally {
            try {
                socket_.close();
            } catch (IOException ioe) {
            }
        }
    }

    public static void main(String[] args)
        throws Throwable
    {
        int port = 7070;

        if (args.length == 0) {
            ServerSocket server = new ServerSocket(port);
            for (;;)
                new Main(server.accept()).start();
        } else {
            try {
                Socket client = new Socket(InetAddress.getLocalHost(), port);
                Writer writer = new OutputStreamWriter(client.getOutputStream());

                for (int i = 0; i != args.length; ++i) {
                    if (i != 0)
                        writer.write(" ");
                    writer.write(args[i]);
                }

                writer.flush();
                client.shutdownOutput();

                ObjectInputStream input = new ObjectInputStream(client.getInputStream());
                Object error = input.readObject();
                if (error != null)
                    throw (Throwable) error;

                client.close();
            } catch (ConnectException e) {
                net.sf.saxon.Transform.main(args);
            }
        }
    }
}
