package joc;

public abstract class Struct<Type>
    implements joc.Boxable<Type>, java.lang.Cloneable
{
    @SuppressWarnings("unchecked")
    public joc.Pointer<Type> box() {
        return joc.Pointer.box(getFields());
    }

    @SuppressWarnings("unchecked")
    public void unbox(joc.Pointer<Type> value) {
        joc.Pointer.unbox(value, (Type) this);
    }

    public abstract Class[] getTypes();
    public abstract Object[] getFields();
    public abstract void setFields(Object[] values);
}
