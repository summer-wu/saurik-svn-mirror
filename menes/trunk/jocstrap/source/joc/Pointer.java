package joc;

import java.util.List;

public final class Pointer<Type> {
    private long address_;

    public Pointer(long address) {
        address_ = address;
    }

    public native static Pointer malloc(long size);
    public native static void free(Pointer value);

    public native static void memmove(Pointer dst, Pointer src, long len);
    public native static void memcpy(Pointer dst, Pointer src, long len);

    // XXX: when I accidentally thought this was templated it was useful
    // XXX: this method is dangerous in light of type erasuers
    // XXX: replace with a runtime version that works on Object
    @SuppressWarnings("unchecked")
    public static <T> Pointer<Pointer<T>> box(List<Pointer<T>> value) {
        return box(value.toArray(new Pointer[value.size()]));
    }

    public static Pointer<Byte> box(String value) {
        return box(value.getBytes());
    }

    public static <T extends Boxable<T>> Pointer<T> box(T value) {
        return value.box();
    }

    public native static <T> Pointer<Pointer<T>> box(Pointer<T>[] value);

    public native static Pointer box(Object[] value);
    public native static <T> T unbox(Pointer<T> value, Class<T> type);
    public native static <T> T unbox(Pointer<T> value, T type);

    public native static Pointer<Boolean> box(boolean[] value);
    public native static Pointer<Byte> box(byte[] value);
    public native static Pointer<Short> box(short[] value);
    public native static Pointer<Integer> box(int[] value);
    public native static Pointer<Long> box(long[] value);
    public native static Pointer<Float> box(float[] value);
    public native static Pointer<Double> box(double[] value);

    /* XXX: structure types */
    /* XXX: 64-bit machines */
    public static long sizeof(Class type) {
        if (type == boolean.class)
            return 1;
        else if (type == byte.class)
            return 1;
        else if (type == short.class)
            return 2;
        else if (type == int.class)
            return 4;
        else if (type == long.class)
            return 8;
        else if (type == float.class)
            return 4;
        else if (type == double.class)
            return 8;
        else
            return 4;
    }
}
