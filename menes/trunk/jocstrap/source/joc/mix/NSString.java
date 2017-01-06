package joc.mix;

public abstract class NSString
    extends obc.NSObject
    implements java.lang.CharSequence
{
    public abstract short characterAtIndex$(int arg0);
    public abstract Object substringWithRange$(obc._NSRange arg0);

    public char charAt(int index) {
        return (char) characterAtIndex$(index);
    }

    public java.lang.CharSequence subSequence(int start, int end) {
        return (java.lang.CharSequence) substringWithRange$(new obc._NSRange(start, end - start));
    }

    public String toString() {
        // XXX: this cast is jacked up
        return joc.Runtime.toString((obc.NSString) this);
    }
}
