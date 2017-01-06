package api.jni;

class Writer extends java.io.OutputStream {
    private int pointer_;

    private native static void write_(int pointer, int value);
    private native static void write_(int pointer, byte[] data, int offset, int length);

    public Writer(int pointer) {
        pointer_ = pointer;
    }

    public void write(int value) {
        write_(pointer_, value);
    }

    public void write(byte[] data, int offset, int length) {
        write_(pointer_, data, offset, length);
    }
}
