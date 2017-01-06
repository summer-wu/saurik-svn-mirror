package com.saurik.swill;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.PushbackReader;
import java.io.Reader;

import java.net.URL;
import java.util.Stack;

public class TemplateReader
    extends Reader
{
    private Template template_;
    private boolean debug_ = false;

    private class File_ {
        private URL url_;
        private Reader reader_;
    }

    private StringBuilder code_;

    private Stack<File_> file_ = new Stack<File_>();

    private StringBuilder buffer_ = new StringBuilder();

    private static enum Mode_ {
        Code, Comment, RegEx, String, Text
    }

    private interface Terminator_ {
        public void terminate()
            throws IOException;
    }

    private static class State_ {
        public Mode_ mode_;
        public Character delimeter_;
        public CharSequence terminator_;
    }

    private Stack<State_> state_ = new Stack<State_>(); {
        push_(Mode_.Code, null, null);
    }

    private void open_(URL url)
        throws IOException
    {
        File_ file = new File_();
        file.url_ = url;
        file.reader_ = new PushbackReader(new BufferedReader(new InputStreamReader(url.openStream())));
        file_.push(file);
    }

    public TemplateReader(Template template, URL url)
        throws IOException
    {
        this(template, url, null);
    }

    public TemplateReader(Template template, URL url, StringBuilder code)
        throws IOException
    {
        template_ = template;
        open_(url);
        code_ = code;
    }

    public int read(char[] cbuf, int off, int len)
        throws IOException
    {
        int c = read();
        if (c == -1)
            return -1;

        int i = 0; do
            cbuf[off + i++] = (char) c;
        while (
            --len != 0 &&
            (c = read()) != -1
        );

        return i;
    }

    private int read_()
        throws IOException
    {
        for (;;) {
            if (file_.empty())
                return -1;
            Reader reader = file_.peek().reader_;
            int value = reader.read();
            if (value != -1)
                return (char) value;
            else {
                reader.close();
                file_.pop();
            }
        }
    }

    private char next_()
        throws IOException
    {
        int value = read_();
        if (value == -1)
            throw new IOException();
        if (debug_)
            System.out.print((char) value);
        return (char) value;
    }

    private void push_(Mode_ mode, Character delimeter, CharSequence terminator) {
        if (debug_)
            System.out.print("<<");
        State_ child = new State_();
        child.mode_ = mode;
        child.delimeter_ = delimeter;
        child.terminator_ = terminator;
        state_.push(child);
    }

    public void inline_(char quote, char delimeter) {
        buffer_.append(quote);
        buffer_.append("+(");
        StringBuilder terminator = new StringBuilder();
        terminator.append(")+");
        terminator.append(quote);
        push_(Mode_.Code, delimeter, terminator);
    }

    public void unread_(char c)
        throws IOException
    {
        ((PushbackReader) file_.peek().reader_).unread(c);
    }

    public void special_()
        throws IOException
    {
        StringBuilder language = null;

        State_ state = state_.peek();
        char c = next_();

        special: for (;;) {
            switch (c) {
                case '{':
                    if (state.mode_ == Mode_.String) {
                        buffer_.append(state.delimeter_);
                        buffer_.append(template_.getStringBlockHeader());

                        StringBuilder terminator = new StringBuilder(); {
                            terminator.append(template_.getStringBlockFooter());
                            terminator.append(state.delimeter_);
                        }

                        push_(Mode_.Text, '}', terminator);
                    } else if (state.mode_ == Mode_.Text) {
                        buffer_.append("\"),(function(){");
                        push_(Mode_.Code, '}', "})(),_o(\"");
                    } else if (state.mode_ == Mode_.Code) {
                        buffer_.append("(function () {_o(\"");
                        StringBuilder terminator = new StringBuilder();
                        terminator.append("\");})();");
                        push_(Mode_.Text, '}', terminator);
                    } else throw new IOException();
                break;

                case '(':
                    if (language != null)
                        throw new IOException();
                    if (state.mode_ == Mode_.String)
                        inline_(state.delimeter_, ')');
                    else if (state.mode_ == Mode_.Text)
                        inline_('"', ')');
                    else if (state.mode_ == Mode_.Code) {
                        buffer_.append("_o(");
                        StringBuilder terminator = new StringBuilder();
                        terminator.append(");");
                        push_(Mode_.Code, ')', terminator);
                    } else throw new IOException();
                break;

                case '<':
                    if (language != null)
                        throw new IOException();

                    StringBuilder path = new StringBuilder(); {
                        for (;;) {
                            c = next_();
                            if (c == '>')
                                break;
                            path.append(c);
                        }
                    }

                    open_(new URL(file_.peek().url_, path.toString()));
                break;

                case '$':
                    if (language != null)
                        throw new IOException();
                    language = new StringBuilder();

                    for (;;) {
                        c = next_();

                        if (c >= 'a' && c <= 'z')
                            language.append(c);
                        else continue special;
                    }

                default:
                    if (language != null)
                        throw new IOException();
                    buffer_.append('#');
                    unread_(c);
            }

            break;
        }
    }

    public void complete_() {
        if (debug_)
            System.out.print(">>");
        state_.pop();
    }

    public int read()
        throws IOException
    {
        for (;;) {
            if (buffer_.length() != 0) {
                char value = buffer_.charAt(0);
                buffer_.deleteCharAt(0);
                if (code_ != null)
                    code_.append(value);
                return value;
            }

            char c; {
                int i = read_();
                if (i == -1)
                    return -1;
                c = (char) i;
                if (debug_)
                    System.out.print(c);
            }

            State_ state = state_.peek();

            if (state.delimeter_ != null && state.delimeter_.charValue() == c) {
                if (state.terminator_ == null)
                    buffer_.append(c);
                else
                    buffer_.append(state.terminator_);
                complete_();
            } else if (c == '#' && state.mode_ != Mode_.Comment)
                special_();
            else switch (state.mode_) {
                case Code:
                    switch (c) {
                        case '"':
                        case '\'':
                            buffer_.append(c);
                            push_(Mode_.String, c, null);
                        break;

                        case '/':
                            c = next_();

                            switch (c) {
                                case '*':
                                    push_(Mode_.Comment, null, null);
                                break;

                                case '/':
                                    push_(Mode_.Comment, '\n', null);
                                break;

                                case ' ':
                                    buffer_.append('/');
                                    buffer_.append(' ');
                                break;

                                default:
                                    buffer_.append('/');
                                    unread_(c);
                                    push_(Mode_.RegEx, '/', null);
                                break;
                            }
                        break;

                        case '{':
                            buffer_.append(c);
                            push_(Mode_.Code, '}', null);
                        break;

                        case '(':
                            buffer_.append(c);
                            push_(Mode_.Code, ')', null);
                        break;

                        default:
                            buffer_.append(c);
                        break;
                    }
                break;

                case Comment:
                    if (state.delimeter_ == null && c == '*') {
                        c = next_();
                        if (c != '/')
                            unread_(c);
                        else
                            complete_();
                    }
                break;

                case String:
                case RegEx:
                    buffer_.append(c);

                    switch (c) {
                        case '\\':
                            buffer_.append(next_());
                        break;
                    }
                break;

                case Text:
                    switch (c) {
                        case '\"':
                        case '\\':
                            buffer_.append('\\');
                            buffer_.append(c);
                        break;

                        case '\n':
                            buffer_.append("\\n");
                        break;

                        case '{':
                            buffer_.append('{');
                            push_(Mode_.Text, '}', null);
                        break;

                        default:
                            buffer_.append(c);
                        break;
                    }
                break;
            }
        }
    }

    public void close()
        throws IOException
    {
        if (!file_.empty())
            throw new IOException();
    }

    public void setDebug(boolean value) {
        debug_ = value;
    }
}

