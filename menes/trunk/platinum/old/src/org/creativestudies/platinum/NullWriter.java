package org.creativestudies.platinum;

import java.io.Writer;

public class NullWriter extends Writer {
    NullWriter() {}

    public void close() {}
    public void flush() {}
    public void write(char[] cbuf) {}
    public void write(char[] cbuf, int off, int len) {};
    public void write(int c) {}
    public void write(String str) {}
    public void write(String str, int off, int len) {}
}
