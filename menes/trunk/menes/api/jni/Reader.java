package api.jni;

class Reader extends java.io.InputStream {
    private int pointer_;

    private native static int read_(int pointer);
    private native static int read_(int pointer, byte[] data, int offset, int length);

    public Reader(int pointer) {
        pointer_ = pointer;
    }

    public int read() {
        return read_(pointer_);
    }

    public int read(byte[] data, int offset, int length) {
        return read_(pointer_, data, offset, length);
    }
}
