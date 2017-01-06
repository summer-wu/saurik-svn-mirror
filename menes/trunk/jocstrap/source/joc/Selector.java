package joc;

public class Selector {
    private long address_;

    public Selector(long address) {
        address_ = address;
    }

    public Selector(String name) {
        address_ = registerName(name);
    }

    public native long registerName(String name);
    public native String toString();
}
