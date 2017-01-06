package joc;

import static joc.Static.*;

public abstract class Strap
    implements Identifier
{
    private long address_;
    protected boolean incorrect_;

    public long getIdentifier() {
        return address_;
    }

    public void performSelectorOnMainThread$withObject$waitUntilDone$(joc.Selector aSelector, Object arg, byte wait) {
        System.err.println("leaking " + getClass().getName() + " due to performSelectorOnMainThread.");
    }

    public int retainCount() {
        return -1;
    }

    protected void finalize()
        throws Throwable
    {
        Scope scope = new Scope(); try {
            //System.out.println("finalize(0x" + Long.toHexString(address_)  + ":" + getClass().getName() + ")=" + retainCount());
            performSelectorOnMainThread$withObject$waitUntilDone$(new Selector("release"), null, YES);
        } finally {
            scope.close();
        }

        super.finalize();
    }

    public Strap() {
        Runtime.allocate(this);
    }
}
