package api.jni.saxon;

import java.io.*;

import javax.xml.transform.*;
import javax.xml.transform.stream.*;

public class Saxon {
    private static TransformerFactory factory = TransformerFactory.newInstance();
    private Transformer transformer;

    public Saxon(InputStream xsl, String uri) {
        try {
            Source source = new StreamSource(xsl);
            source.setSystemId(uri);
            transformer = factory.newTransformer(source);
        } catch (Throwable e) {
            e.printStackTrace(System.out);
        }
    }

    public void Transform(InputStream in, OutputStream out) {
        try {
            transformer.transform(new StreamSource(in), new StreamResult(out));
        } catch (Throwable e) {
            e.printStackTrace(System.out);
        }
    }
}
