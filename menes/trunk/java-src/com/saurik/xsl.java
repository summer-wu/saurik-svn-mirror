package com.saurik;

import java.io.Reader;
import java.io.Writer;

import javax.xml.transform.stream.StreamSource;
import javax.xml.transform.stream.StreamResult;

import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;

public class xsl {
    private static TransformerFactory factory_;

    static {
        factory_ = TransformerFactory.newInstance();
    }

    public static TransformerFactory getFactory() {
        return factory_;
    }

    public static void Transform(Reader xsl, Reader src, Writer dst)
        throws TransformerException
    {
        Transformer transformer = factory_.newTransformer(new StreamSource(xsl));
        transformer.transform(new StreamSource(src), new StreamResult(dst));
    }
}
