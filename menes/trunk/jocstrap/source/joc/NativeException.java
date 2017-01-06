package joc;

public class NativeException
    extends Exception
    implements Identifier
{
    static final long serialVersionUID = -169231052490276083L;

    private Strap object_;

    public long getIdentifier() {
        return object_.getIdentifier();
    }

    public NativeException(Strap object) {
        object_ = object;
    }

    public Strap getNativeException() {
        return object_;
    }

    public String getMessage() {
        return (String) joc.Runtime.msgSend(object_, null, "description");
    }
}
