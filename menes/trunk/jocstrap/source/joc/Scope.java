package joc;

public class Scope {
    private native static long push();
    private native static void pop(long pool);

    private long pool_;

    public Scope() {
        pool_ = push();
    }

    public void close() {
        pop(pool_);
        pool_ = 0;
    }

    protected void finalize()
        throws Throwable
    {
        assert pool_ == 0;
        super.finalize();
    }
}
