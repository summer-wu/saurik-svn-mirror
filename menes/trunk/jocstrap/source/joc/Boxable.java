package joc;

public interface Boxable<Type> {
    public Pointer<Type> box();
    public void unbox(Pointer<Type> value);
}
