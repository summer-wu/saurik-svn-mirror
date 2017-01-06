package com.saurik;

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;

public class io {
    public static void splice(InputStream in, OutputStream out)
        throws IOException
    {
        byte[] data = new byte[1024];
        int size;

        while ((size = in.read(data)) != -1)
            out.write(data, 0, size);
    }
}
