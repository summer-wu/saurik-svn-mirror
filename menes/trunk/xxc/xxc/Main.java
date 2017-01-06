package xxc;

import java.io.*;

import javax.xml.parsers.*;

import javax.xml.transform.*;
import javax.xml.transform.dom.*;
import javax.xml.transform.stream.*;

import org.w3c.dom.*;

import antlr.*;

public class Main {
    public static void main(String[] args) throws Exception {
        String xml;

        {
            StringWriter string = new StringWriter();
            PlusPlusCParser parser = new PlusPlusCParser(new PlusPlusCLexer(System.in));
            parser.setASTNodeClass("xxc.SimpleAst");
            parser.file();

            boolean stop = args.length > 1 && args[1].equals("-xml");
            Writer writer = stop ? new OutputStreamWriter(System.out) : (Writer) string;
            ((BaseAST) parser.getAST()).xmlSerialize(writer);

            if (stop)
                return;

            xml = string.toString();
        }

        Node ast;

        {
            TransformerFactory factory = TransformerFactory.newInstance();
            Transformer transformer = factory.newTransformer(new StreamSource(new FileReader(args[0] + "/xxc/xxc/input.xsl")));
            DOMResult dom = new DOMResult();

            boolean stop = args.length > 1 && args[1].equals("-ast");
            Result result = stop ? new StreamResult(System.out) : (Result) dom;
            transformer.transform(new StreamSource(new StringReader(xml)), result);

            if (stop)
                return;

            ast = dom.getNode();
        }

        {
            TransformerFactory factory = TransformerFactory.newInstance();
            Transformer transformer = factory.newTransformer(new StreamSource(new FileReader(args[0] + "/xxc/xxc/output.xsl")));
            transformer.transform(new DOMSource(ast), new StreamResult(System.out));
        }
    }
}
