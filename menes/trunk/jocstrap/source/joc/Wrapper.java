package joc;

public interface Wrapper {
    public Object forwardInvocation(Object object, String selector, Object[] arguments);
    public String methodSignatureForSelector(Object object, String selector);
    public boolean respondsToSelector(Object object, String selector);
}
