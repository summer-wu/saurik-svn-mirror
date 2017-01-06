package xxc;

import java.io.*;
import java.text.*;

import antlr.*;
import antlr.collections.*;

public class SimpleAst extends CommonAST {
    /*public void initialize(Token token) {
    }

    public void initialize(AST ast) {
    }

    public void initialize(int stupid, String string) {
    }*/

    /**
     * Translates <, & , " and > to corresponding entities.
     */
    private String xmlEscape(String orig) {
        if (orig == null) return "";
        StringBuffer temp = new StringBuffer();
        StringCharacterIterator sci = new StringCharacterIterator(orig);
        for (char c = sci.first(); c != CharacterIterator.DONE;
             c = sci.next()) {

            switch (c) {
            case '<':
                temp.append("&lt;");
                break;
            case '>':
                temp.append("&gt;");
                break;
            case '\"':
                temp.append("&quot;");
                break;
            case '&':
                temp.append("&amp;");
                break;
            default:
                temp.append(c);
                break;
            }
        }
        return temp.toString();
    }

    public String getXmlText() {
        String text = getText()
            .replace('_', ':')
            .replace('0', 'o')
            .replace('1', 'l')
            .replace('2', 'u')
            .replace('3', 'e')
            .replace('4', 'a');

        StringBuffer buffer = new StringBuffer();

        for (int i = 0; i != text.length(); ++i) {
            char c = text.charAt(i);
            if (!Character.isUpperCase(c))
                buffer.append(c);
            else {
                buffer.append('-');
                buffer.append(Character.toLowerCase(c));
            }
        }

        return buffer.toString();
    }

    public void xmlSerializeNode(Writer out) throws IOException {
        if (getType() != -1)
            out.write(xmlEscape(getText()));
        else
            out.write("<" + getXmlText() + "/>");
    }

    public void xmlSerializeRootOpen(Writer out) throws IOException {
        out.write("<" + getXmlText() + ">");
    }

    public void xmlSerializeRootClose(Writer out) throws IOException {
        out.write("</" + getXmlText() + ">");
    }
};
