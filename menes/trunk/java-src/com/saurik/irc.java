package com.saurik;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.Writer;

import java.net.Socket;
import java.net.UnknownHostException;

import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import com.saurik.html;
import com.saurik.url;

public class irc {

private static final String Dot_ = "&#xb7;";
private static final String Bullet_ = "&#x2022;";
private static final String Prefix_ = Dot_ + Bullet_ + Dot_;

public static interface Listener {
    void onCommand(Connection.User sender, List<String> args);

    void onCtcp(Connection.User sender, String target, String command, String args, boolean response)
        throws Exception;

    void onLeave(String channel);
}

// String Utilities {{{
public static boolean isChannel(String name) {
    switch (name.charAt(0)) {
        case '&':
        case '#':
        case '+':
        case '!':
            return true;

        default:
            return false;
    }
}

public static String toLower(String mixed) {
    return mixed.toLowerCase().
        replace("[", "{").
        replace("]", "}").
        replace("|", "\\").
        replace("~", "^");
}
// }}}

public static boolean hasOne(String Data, char C) {
    int Loc = Data.indexOf(C);
    return Loc != -1 && Data.indexOf(C, Loc + 1) == -1;
}

public static String getStyle(Modes modes) {
    if (modes.has('o'))
        return "Op";
    else if (modes.has('h'))
        return "Half";
    else if (modes.has('v'))
        return "Voice";
    return "User";
}

// Colorization {{{
private static String getNumber(String string, int offset) {
    String current = "";

    for (int i = offset, _end = offset + 2 < string.length() ? offset + 2 : string.length(); i != _end; ++i) {
        char next = string.charAt(i);
        if (next < '0' || next > '9')
            break;
        current += next;
    }

    return current.length() == 0 ? null : current;
}

public static String colorize(String data) {
    StringBuilder buffer = new StringBuilder();
    int spans = 0;

    String foreground = null;
    String background = null;

    boolean bold = false;
    boolean fixed = false;
    boolean italic = false;
    boolean symbol = false;
    boolean underline = false;

    for (int loc = 0; loc < data.length(); ++loc) {
        boolean span = false;

        switch (data.charAt(loc)) {
            case 2:
                bold = !bold;
                span = true;
            break;

            case 3:
                foreground = getNumber(data, loc + 1);
                background = null;

                if (foreground != null) {
                    loc += foreground.length();
                    if (foreground.length() == 1)
                        foreground = '0' + foreground;

                    if (
                        loc != data.length() - 1 &&
                        data.charAt(loc + 1) == ','
                    ) {
                        ++loc;

                        background = getNumber(data, loc + 1);
                        if (background != null) {
                            loc += background.length();
                            if (background.length() == 1)
                                background = '0' + background;
                        }
                    }
                }

                span = true;
            break;

            case 17:
                fixed = !fixed;
                span = true;
            break;

            case 18:
                symbol = !symbol;
                span = true;
            break;

            case 22:
                italic = !italic;
                span = true;
            break;

            case 31:
                underline = !underline;
                span = true;
            break;

            case '&':
                buffer.append("&amp;");
            break;

            case '<':
                buffer.append("&lt;");
            break;

            case '>':
                buffer.append("&gt;");
            break;

            case '\"':
                buffer.append("&quot;");
            break;

            case ' ':
                if ((loc + 1) != data.length() && data.charAt(loc + 1) == ' ') {
                    loc++;
                    buffer.append(" &nbsp;");
                } else {
                    buffer.append(" ");
                }
            break;

            default:
                if (data.length() - 7 > loc && data.substring(loc, loc + 7).equalsIgnoreCase("http://")) {
                    int Pos = loc + 7;
                    char C;

                    do {
                        C = data.charAt(Pos);
                    } while (
                        ++Pos < data.length() &&
                        C != ' ' &&
                        C != '"' &&
                        C != '\'' &&
                        C != ':' &&
                        C != '(' &&
                        C != ')' &&
                        C != '<' &&
                        C != '>' &&
                        C != ';' &&
                        C != '!'
                    );

                    if (Pos != data.length())
                        --Pos;

                    do C = data.charAt(--Pos);
                    while (C == '.' || C == ',');

                    ++Pos;
                    buffer.append("<a target='_blank' href='");
                    buffer.append(data.substring(loc, Pos));
                    buffer.append("'>");
                    buffer.append(html.Escape(data.substring(loc, Pos)));
                    buffer.append("</a>");
                    loc = Pos - 1;
                } else {
                    buffer.append(data.charAt(loc));
                }
            break;
        }

        if (span) {
            for (int i = 0; i != spans; ++i)
                buffer.append("</span>");
            spans = 0;

            ++spans;

            buffer.append("<span class=\"mircO");

            if (foreground != null) {
                buffer.append(" mircKf");
                buffer.append(foreground);
            }

            if (background != null) {
                buffer.append(" mircKb");
                buffer.append(background);
            }

            if (bold)
                buffer.append(" mircB");

            if (fixed)
                buffer.append(" mircF");

            if (italic)
                buffer.append(" mircI");

            if (symbol)
                buffer.append(" mircS");

            if (underline)
                buffer.append(" mircU");

            buffer.append("\">");
        }
    }

    for (int i = 0; i != spans; ++i)
        buffer.append("</span>");

    return buffer.toString();
}
// }}}

public static class Exception
    extends java.lang.Exception
{
    public Exception(String message) {
        super(message);
    }
}

// irc.Modes {{{
public static class Modes {
    private Map<Character, String> modes_ = new HashMap<Character, String>();

    public boolean has(char mode) {
        return modes_.containsKey(mode);
    }

    protected void set(char mode, boolean value) {
        if (value)
            modes_.put(mode, "");
        else
            modes_.remove(mode);
    }

    protected void set(char mode, String value) {
        if (value == null)
            modes_.remove(mode);
        else
            modes_.put(mode, value);
    }

    protected void clear() {
        modes_.clear();
    }
}
// }}}

public static class Connection
    extends Thread
{
// irc.Connection.Channel {{{
    public class Channel {
        private String name_;
        private Map<User, Modes> users_ = new HashMap<User, Modes>();
        private Modes modes_ = new Modes();
        protected boolean rescan_ = false;

        public Channel(String name) {
            name_ = name;
        }

        public String getName() {
            return name_;
        }

        public Map<User, Modes> getUsers() {
            return users_;
        }

        public Modes getModes() {
            return modes_;
        }

        protected void addUser(User user, Modes modes) {
            if (modes == null)
                modes = new Modes();
            users_.put(user, modes);
        }

        protected void delUser(User user) {
            users_.remove(user);
        }

        protected void clearUsers() {
            users_.clear();
        }
    }
// }}}
// irc.Connection.User {{{
    public class User {
        public String nick_, ident_, host_;

        public User(String nick, String ident, String host) {
            nick_ = nick;
            ident_ = ident;
            host_ = host;
        }

        public String getNick() {
            return nick_;
        }

        public String getIdent() {
            return ident_;
        }

        public String getHost() {
            return host_;
        }

        public String getFull() {
            String full = nick_;
            if (ident_ != null)
                full += "!" + ident_;
            if (host_ != null)
                full += "@" + host_;
            return full;
        }
    }
// }}}

    private Socket socket_;
    private OutputStreamWriter sout_;
    private BufferedReader sin_;
    private Listener listener_;

    private Map<String, User> users_ = new HashMap<String, User>();

    private String nick_;
    private Map<String, Channel> channels_ = new HashMap<String, Channel>();

    public Connection(String host, int port, String nick, String name)
        throws UnknownHostException, IOException, Exception
    {
        socket_ = new Socket(host, port);
        sout_ = new OutputStreamWriter(socket_.getOutputStream());
        sin_ = new BufferedReader(new InputStreamReader(socket_.getInputStream()), 256);

        send("USER", nick, nick, nick, name);
        send("NICK", nick);
        flush();
    }

    // Command Output {{{
    public boolean isConnected() {
        return socket_ != null;
    }

    private void onDisconnect() {
        socket_ = null;
        sout_ = null;
        sin_ = null;
    }

    public void send(String command) {
        try {
            sout_.write(command + "\n");
        } catch (IOException e) {
            onDisconnect();
        }
    }

    public void send(String command, String... args)
        throws Exception
    {
        StringBuilder builder = new StringBuilder(); {
            builder.append(command);

            boolean colon = false;

            for (String arg : args) {
                if (colon)
                    throw new irc.Exception("invalid argument sequence");

                builder.append(" ");

                if (
                    arg.length() == 0 ||
                    arg.indexOf(" ") != -1 ||
                    arg.charAt(0) == ':'
                ) {
                    builder.append(":");
                    colon = true;
                }

                builder.append(arg);
            }
        }

        send(builder.toString());
    }

    public void flush() {
        try {
            sout_.flush();
        } catch (IOException e) {
            onDisconnect();
        }
    }
    // }}}
    // Event Listeners {{{
    private Set<Listener> listeners_ = new HashSet<Listener>();

    public void addListener(Listener listener) {
        listeners_.add(listener);
    }

    public Set<Listener> getListeners() {
        return listeners_;
    }

    public void removeListener(Listener listener) {
        listeners_.remove(listener);
    }
    // }}}
    // Protocl Parsing {{{
    public List<String> parse(String line) {
        List<String> args = new ArrayList<String>();

        for (int offset = 0;;)
            if (line.charAt(offset) == ':') {
                args.add(line.substring(offset + 1));
                break;
            } else {
                int space = line.indexOf(' ', offset);

                if (space == -1) {
                    args.add(line.substring(offset));
                    break;
                } else {
                    args.add(line.substring(offset, space));
                    offset = space + 1;
                }
            }

        return args;
    }

    public String scan(User sender, String target, boolean response, String text)
        throws Exception
    {
        for (;;) {
            int start = text.indexOf('\u0001');
            if (start == -1)
                break;

            int end = text.indexOf('\u0001', start + 1);
            if (end == -1)
                break;

            String ctcp = text.substring(start + 1, end);
            text = text.substring(0, start) + text.substring(end + 1);

            int cut = ctcp.indexOf(' ');

            onCtcp(
                sender,
                target,
                cut == -1 ? ctcp : ctcp.substring(0, cut),
                cut == -1 ? null : ctcp.substring(cut + 1),
                response
            );
        }

        return text.equals("") ? null : text;
    }
    // }}}

    public Map<String, Channel> getChannels() {
        return channels_;
    }

    public Channel getChannel(String name) {
        Channel channel = channels_.get(toLower(name));
        System.out.println("getChannel(" + name + ") == " + (channel != null));
        return channel;
    }

    public String getNick() {
        return nick_;
    }

    public User getUser(String full) {
        String nick = null;
        String ident = null;
        String host = null; {
            int bang = full.indexOf('!');
            if (bang == -1)
                nick = full;
            else {
                nick = full.substring(0, bang);
                int at = full.lastIndexOf('@');
                ident = full.substring(bang + 1, at);
                host = full.substring(at + 1);
            }
        }

        String key = toLower(nick);
        User user = users_.get(key);

        if (user == null) {
            user = new User(nick, ident, host);
            users_.put(key, user);
        } else {
            if (user.ident_ == null)
                user.ident_ = ident;
            if (user.host_ == null)
                user.host_ = host;
        }

        return user;
    }

    private void onLeave(String channel) {
        channels_.remove(channel.toLowerCase());

        for (Listener listener : listeners_)
            listener.onLeave(channel);
    }

    private void onCommand(String line, User sender, List<String> args)
        throws Exception
    {
        String cmd = args.get(0);

        if (false) {
        // 00.* {{{
        } else if (cmd.regionMatches(0, "00", 0, 2)) {
            nick_ = args.get(1);
        // }}}
        // 353 {{{
        } else if (cmd.equals("353")) {
            String name = args.get(3);
            String nicks = args.get(4);

            // XXX: global case
            Channel channel = getChannel(name);
            if (channel != null) {
                if (!channel.rescan_) {
                    channel.clearUsers();
                    channel.rescan_ = true;
                }

                for (int last = 0, space = nicks.indexOf(' '); space != -1; space = nicks.indexOf(' ', (last = space + 1))) {
                    String part = nicks.substring(last, space);
                    Modes modes = new Modes();

                    scan: for (;;) {
                        switch (part.charAt(0)) {
                            case '@': modes.set('o', true); break;
                            case '%': modes.set('h', true); break;
                            case '+': modes.set('v', true); break;
                            case '~': modes.set('q', true); break;
                            case '&': modes.set('a', true); break;
                            default: break scan;
                        }

                        part = part.substring(1);
                    }

                    channel.addUser(getUser(part), modes);
                }
            }
        // }}}
        // 366 {{{
        } else if (cmd.equals("366")) {
            // XXX: * case
            String target = args.get(2);
            if (!target.equals("*")) {
                Channel channel = getChannel(args.get(2));
                channel.rescan_ = false;
            }
        // }}}
        // JOIN {{{
        } else if (cmd.equals("JOIN")) {
            String channame = args.get(1);
            Channel channel = null;

            if (nick_.equalsIgnoreCase(sender.getNick())) {
                channel = new Channel(channame);
                channels_.put(channame.toLowerCase(), channel);
                send("MODE", channame);
            }

            if (channel == null)
                channel = channels_.get(channame.toLowerCase());

            channel.addUser(sender, null);
        // }}}
        // KICK {{{
        } else if (cmd.equals("KICK")) {
            Channel channel = getChannel(args.get(1));
            channel.delUser(getUser(args.get(2)));
        // }}}
        // MODE {{{
        } else if (cmd.equals("MODE")) {
            String target = args.get(1);

            if (isChannel(target)) {
                Channel channel = getChannel(target);
                Map<User, Modes> users = channel.getUsers();

                parse: for (int i = 2; i < args.size(); ++i) {
                    String part = args.get(i);

                    boolean set;
                    switch (part.charAt(0)) {
                        case '+': set = true; break;
                        case '-': set = false; break;

                        default:
                            throw new Exception("accidentally interpreted '" + part + "' as a mode");
                    }

                    for (int c = 1; c < part.length(); ++c) {
                        char mode = part.charAt(c);
                        switch (mode) {
                            case 'h':
                            case 'o':
                            case 'v':
                                User user = getUser(args.get(++i));
                                Modes modes = (Modes) users.get(user);
                                if (modes == null)
                                    throw new Exception("modes missing for " + user.getNick() + " on " + target);
                                modes.set(mode, set);
                            break;
                        }
                    }
                }
            } else {
                for (int i = 3; i < args.size(); ++i) {
                    String mode = args.get(i);
                }
            }
        // }}}
        // NOTICE {{{
        } else if (cmd.equals("NOTICE")) {
            args.set(2, scan(sender, args.get(1), true, args.get(2)));
        // }}}
        // PING {{{
        } else if (cmd.equals("PING")) {
            send("PONG " + line.substring(5));
            return;
        // }}}
        // PRIVMSG {{{
        } else if (cmd.equals("PRIVMSG")) {
            args.set(2, scan(sender, args.get(1), false, args.get(2)));
        // }}}
        // QUIT {{{
        } else if (cmd.equals("QUIT")) {
            users_.remove(toLower(sender.getNick()));

            for (Channel channel : channels_.values())
                channel.delUser(sender);
        // }}}
        }

        for (Listener listener : listeners_)
            listener.onCommand(sender, args);

        if (false) {
        // KICK {{{
        } else if (cmd.equals("KICK")) {
            if (nick_.equalsIgnoreCase(args.get(2)))
                onLeave(args.get(1));
        // }}}
        // NICK {{{
        } else if (cmd.equals("NICK")) {
            String nick = args.get(1);
            if (nick_.equalsIgnoreCase(sender.getNick()))
                nick_ = nick;

            String key = toLower(sender.getNick());
            User user = users_.get(key);
            users_.remove(key);
            user.nick_ = nick;
            users_.put(toLower(nick), user);
        // }}}
        // PART {{{
        } else if (cmd.equals("PART")) {
            String channame = args.get(1);
            if (nick_.equalsIgnoreCase(sender.getNick()))
                onLeave(channame);
            else {
                Channel channel = channels_.get(channame.toLowerCase());
                channel.delUser(sender);
            }
        // }}}
        }
    }

    private void onCtcp(User sender, String target, String command, String args, boolean response)
        throws Exception
    {
        for (Listener listener : listeners_)
            listener.onCtcp(sender, target, command, args, response);
    }

    public void run() {
        try {
            for (String line; (line = sin_.readLine()) != null; ) {
                System.out.println(line);

                if (line.charAt(0) != ':')
                    onCommand(line, null, parse(line));
                else {
                    int space = line.indexOf(' ');
                    if (space == -1)
                        throw new Exception("degenerate irc command");
                    onCommand(line, getUser(line.substring(1, space)), parse(line.substring(space + 1)));
                }

                flush();
            }
        } catch (IOException e) {
        } catch (irc.Exception e) {
        }

        onDisconnect();
    }
}

// Viewer {{{
public static class Viewer
    implements Listener
{
    private Connection connection_;
    private PrintWriter out_;

    private String channel_, query_;
    private boolean whois_ = false;

    public Viewer(Connection connection, Writer out) {
        connection_ = connection;
        out_ = new PrintWriter(out);
    }

    public void flush() {
        out_.flush();
    }

    public void print(String data) {
        out_.print(data);
    }

    public void println() {
        out_.println("<br/>");
    }

    public void println(String data) {
        out_.println(data + "<br/>");
    }

    public void printAll(List<String> args, int Start) {
        for (; Start < args.size() - 1; Start++)
            print(html.Escape(args.get(Start)) + " ");
        if (Start == args.size() - 1)
            print(html.Escape(args.get(Start)));
    }

    public void query(String query) {
        if (query_ != null) {
            println(Prefix_ + " Ending conversation with <span class='nickname'>" + query_ + "</span>");
            query_ = null;
        }

        if (query != null && !query.equals("")) {
            println(Prefix_ + " Starting conversation with <span class='nickname'>" + query + "</span>");
            query_ = query;
        }
    }

    private void update() {
    }

    private void redirect(String channel) {
        channel_ = channel;
        if (channel != null)
            println(Prefix_ + " You are now talking to channel <span class='channel'>" + html.Escape(channel) + "</span>");
        update();
    }

    public void onCommand(Connection.User sender, List<String> args) {
        String cmd = args.get(0);

        if (false) {
        // ERROR {{{
        } else if (cmd.equals("ERROR")) {
            println("<span class='msgError'>" + Prefix_ + " Error: " + html.Escape(args.get(1)) + "</span>");
        // }}}
        // NOTICE {{{
        } else if (cmd.equals("NOTICE")) {
            String Msg = args.get(2);
            if (Msg != null) {
                if (sender == null) {
                    if ((args.get(2)).startsWith("*** "))
                        Msg = Msg.substring(4);
                } else {
                    print("<span class='marker'>-</span><span class='nickname'>" + html.Escape(sender.getNick()) + "</span><span class='marker'>-</span> ");
                }
                println("<span class='msgNotice'>" + colorize(Msg) + "</span>");
            }
        // }}}
        // PRIVMSG {{{
        } else if (cmd.equals("PRIVMSG")) {
            String Msg = args.get(2);
            if (Msg != null) {
                String target = args.get(1);
                boolean direct = connection_.getNick().equalsIgnoreCase(target);

                if (direct) {
                    print("<span class='marker'>&lt;</span><span class='shadow'>-</span> <span class='marker'>*</span><span class='privDirect'>");
                } else {
                    Connection.Channel channel = connection_.getChannel(target);
                    Modes modes = (Modes) channel.getUsers().get(sender);
                    print("<span class='marker'>&lt;</span><span class='priv" + getStyle(modes) + "'>");
                }

                print(html.Escape(sender.getNick()) + "</span>");
                if (isChannel(target) && !direct && !channel_.equalsIgnoreCase(target)) {
                    print("<span class='marker'>:</span><span class='channel'>" + html.Escape(target) + "</span>");
                }

                println("<span class='marker'>" + (direct ? "*" : "&gt;") + "</span> " + colorize(Msg));
            }
        // }}}
        // TOPIC {{{
        } else if (cmd.equals("TOPIC")) {
            String target = args.get(1);
            Connection.Channel channel = connection_.getChannel(target);
            Modes modes = (Modes) channel.getUsers().get(sender);

            println("<span class='msgMode'>" + Prefix_ + " Topic on</span> <span class='channel'>" + html.Escape(target) + "</span> <span class='msgMode'>changed to</span> <span class='topic'>\"" + colorize(args.get(2)) + "\"</span> <span class='msgMode'>by</span> <span class='mode" + getStyle(modes) + "'>" + sender.getNick() + "</span>");
        // }}}
        // JOIN {{{
        } else if (cmd.equals("JOIN")) {
            String Chan = args.get(1);
            if (sender.getNick().equals(connection_.getNick())) {
                println("<span class='msgMoving'>" + Prefix_ + " You have joined</span> <span class='channel'>" + html.Escape(Chan) + "</span>");
                redirect(Chan);
            } else {
                print("<span class='msgMoving'>" + Prefix_ + " Join:</span> <span class='nickname'>" + html.Escape(sender.getNick()) + "</span>");
                if (sender.getIdent() != null) {
                    println(" <span class='msgMoving'>(" + html.Escape(sender.getIdent()) + "@" + html.Escape(sender.getHost()) + ")</span>");
                } else println();
            }

            update();
        // }}}
        // PART {{{
        } else if (cmd.equals("PART")) {
            if (sender.getNick().equals(connection_.getNick())) {
                println("<span class='msgMoving'>" + Prefix_ + " You have parted</span> <span class='channel'>" + html.Escape(args.get(1)) + "</span>");
            } else {
                print("<span class='msgMoving'>" + Prefix_ + " Part:</span> <span class='nickname'>" + html.Escape(sender.getNick()) + "</span>");
                if (sender.getIdent() != null) {
                    println(" <span class='msgMoving'>(" + html.Escape(sender.getIdent()) + "@" + html.Escape(sender.getHost()) + ")</span>");
                } else println();
            }

            update();
        // }}}
        // QUIT {{{
        } else if (cmd.equals("QUIT")) {
            println("<span class='msgQuit'>" + Prefix_ + " Quit:</span> <span class='nickname'>" + sender.getNick() + "</span> <span class='msgQuit'>(" + colorize(args.get(1)) + ")</span>");

            update();
        // }}}
        // KICK {{{
        } else if (cmd.equals("KICK")) {
            if (connection_.getNick().equals(args.get(2)))
                print("<span class='msgMoving'>" + Prefix_ + " You have");
            else
                print("<span class='msgMoving'>" + Prefix_ + " " + html.Escape(args.get(2)));
            println(" been kicked from</span> <span class='channel'>" + html.Escape(args.get(1)) + "</span> <span class='msgMoving'>by</span> <span class='channel'>" + sender.getNick() + "</span> <span class='msgMoving'>(" + colorize(args.get(3)) + ")</span>");

            update();
        // }}}
        // MODE {{{
        } else if (cmd.equals("MODE")) {
            String target = args.get(1);

            print("<span class='msgMode'>" + Prefix_ + " Mode change</span> <span class='mode'>\"");
            printAll(args, 2);
            print("\"</span> <span class='msgMode'>on</span> ");

            if (isChannel(target)) {
                Connection.Channel channel = connection_.getChannel(target);
                Modes modes = (Modes) channel.getUsers().get(sender);
                println("<span class='channel'>" + target + "</span> <span class='msgMode'>by</span> <span class='mode" + getStyle(modes) + "'>" + sender.getNick() + "</span>");
            } else {
                println("<span class='nickname'>" + target + "</span>");
            }

            update();
        // }}}
        // NICK {{{
        } else if (cmd.equals("NICK")) {
            println("<span class='msgMode'>" + Prefix_ + "</span> <span class='nickname'>" + html.Escape(sender.getNick()) + "</span> <span class='msgMode'>is now known as</span> <span class='nickname'>" + html.Escape(args.get(1)) + "</span>");
            update();
        // }}}
        // 014 {{{
        } else if (cmd.equals("014")) {
            print(Prefix_ + " ");
            printAll(args, 2);
            println();
        // }}}
        // 241 {{{
        } else if (cmd.equals("241")) {
            print("<font color='red'>" + Prefix_ + "</font> ");
            printAll(args, 2);
            println();
        // }}}
        // 301: AWAY (Message) {{{
        } else if (cmd.equals("301")) {
            if (whois_) {
                print(Prefix_ + " &nbsp; ");
            } else {
                print(Prefix_ + " <span class='nickname'>" + html.Escape(args.get(2)) + "</span>" );
            }
            println(" is away: (" + html.Escape(args.get(3)) + ")");
        // }}}
        // 305: "You are no longer marked as being away" {{{
        } else if (cmd.equals("305")) {
            println(Prefix_ + " " + html.Escape(args.get(2)));
        // }}}
        // 306: "You have been marked as being away" {{{
        } else if (cmd.equals("306")) {
            println(Prefix_ + " " + html.Escape(args.get(2)));
        // }}}
        // 310: WHOIS (IRCop) {{{
        } else if (cmd.equals("310")) {
            print(Prefix_ + " &nbsp; ");
            printAll(args, 3);
            println();
        // }}}
        } else if (cmd.equals("311")) { // WHOIS (Address)
            whois_ = true;
            String Address = html.Escape(args.get(3)) + "@" + html.Escape(args.get(4));
            if (hasOne(args.get(4), '.'))
                Address = "<a href=\"mailto:" + Address + "\">" + Address + "</a>";
            println(Prefix_ + " <span class='nickname'>" + html.Escape(args.get(2)) + "</span> is " + Address + " (" + html.Escape(args.get(6)) + ")");

        } else if (cmd.equals("312")) { // WHOIS (Server)
            println(Prefix_ + " &nbsp; on IRC via <span class='server'>" + html.Escape(args.get(3)) + "</span> (" + html.Escape(args.get(4)) + ")");

        } else if (cmd.equals("313")) { // WHOIS (IRCop)
            print(Prefix_ + " &nbsp; ");
            printAll(args, 3);
            println();

        } else if (cmd.equals("317")) { // WHOIS (Times)
            int Idle = Integer.parseInt(args.get(3), 10);
            println(Prefix_ + " &nbsp; has been idle " + (Idle / 3600) + " hours " + ((Idle / 60) % 60) + " mins " + (Idle % 60) + " secs");
            println(Prefix_ + " &nbsp; signed on " + (new Date(Long.parseLong(args.get(4)) * 1000)).toString());

        } else if (cmd.equals("318")) { // "End of /WHOIS list."
            whois_ = false;
            println(Prefix_ + " " + html.Escape(args.get(3)));

        } else if (cmd.equals("319")) { // WHOIS (Channels)
            println(Prefix_ + " &nbsp; on channels: <span class='channel'>" + html.Escape(args.get(3)) + "</span>");

        } else if (cmd.equals("321")) { // LIST Head
            println(Prefix_ + " " + html.Escape(args.get(2)) + " " + html.Escape(args.get(3)));

        } else if (cmd.equals("322")) { // LIST Data
            println(Prefix_ + " &nbsp; <span class='channel'>" + html.Escape(args.get(2)) + "</span>(" + html.Escape(args.get(3)) + "): " + html.Escape(args.get(4)));

        } else if (cmd.equals("323")) { // "End of /LIST"
            println(Prefix_ + " " + html.Escape(args.get(2)));

        } else if (cmd.equals("324")) { // MODE (Channel)
            print(Prefix_ + " Mode on <span class='channel'>" + args.get(2) + "</span> is <span class='mode'>\"");
            printAll(args, 3);
            println("\"</span>");

        } else if (cmd.equals("329")) { // Channel Info
            println(Prefix_ + " <span class='channel'>" + args.get(2) + "</span> was created on " + (new Date(Long.parseLong(args.get(3)) * 1000).toString()));
            // XXX: This yields three fields, TS, Password TS, Opleess TS...

        } else if (cmd.equals("332")) { // TOPIC Text
            println(Prefix_ + " Topic for <span class='channel'>" + args.get(2) + "</span> is <span class='topic'>\"" + args.get(3) + "\"</span>");

        } else if (cmd.equals("333")) { // TOPIC Info
            println(Prefix_ + " Topic for <span class='channel'>" + args.get(2) + "</span> was set by <span class='nickname'>" + args.get(3) + "</span> on " + (new Date(Long.parseLong(args.get(4)) * 1000).toString()));

        } else if (cmd.equals("353")) {
            println(Prefix_ + " <span class='channel'>" + html.Escape(args.get(3)) + "</span> <span class='nickname'>" + html.Escape(args.get(4)) + "</span>");

        } else if (cmd.equals("366")) { // "End of /NAMES list"
            println(Prefix_ + " <span class='channel'>" + html.Escape(args.get(2)) + "</span> " + html.Escape(args.get(3)));

        } else if (cmd.equals("376")) { // "End of /MOTD command"
            print(Prefix_ + " ");
            printAll(args, 2);
            println();

        } else if (cmd.equals("378")) { // MOTD
            print(Prefix_ + " &nbsp; ");
            printAll(args, 3);
            println();

        } else if (cmd.equals("379")) {
            print(Prefix_ + " &nbsp; ");
            printAll(args, 3);
            println();

        } else if (cmd.equals("401")) { // "No such nick"
            // XXX: deal with * case for UnderNet
            println("<span class='msgError'>" + Prefix_ + "</span> <span class='nickname'>" + html.Escape(args.get(2)) + "</span/> " + html.Escape(args.get(3)));

        } else if (cmd.equals("421")) { // "Unknown command"
            println("<span class='msgError'>" + Prefix_ + "</span> <span class='msgError'>" + html.Escape(args.get(3)) + ": " + html.Escape(args.get(2))  + "</span>");

        } else if (cmd.equals("422")) { // "MOTD File is missing"
            print("<span class='msgError'>" + Prefix_ + "</span> ");
            printAll(args, 2);
            println();

        } else if (cmd.equals("433")) { // "Nickame is already in use"
            println("<span class='msgError'>" + Prefix_ + "</span> <span class='nickname'>" + html.Escape(args.get(2)) + "</span> " + html.Escape(args.get(3)));

        } else if (cmd.equals("482")) { // "You're not channel operator"
            println("<span class='msgError'>" + Prefix_ + "</span> <span class='channel'>" + html.Escape(args.get(2)) + "</span><span class='msgError'>: " + html.Escape(args.get(3)) + "</span>");

        } else if (
            cmd.regionMatches(0, "00", 0, 2) ||
            cmd.regionMatches(0, "25", 0, 2) ||
            cmd.regionMatches(0, "26", 0, 2) ||
            cmd.regionMatches(0, "37", 0, 2)
        ) {
            print(Prefix_ + " ");
            printAll(args, 2);
            println();

        } else {
            print("<span class='msgError'>Unknown Response: ");
            if (sender != null)
                print(html.Escape(sender.getFull()) + " ");
            println(html.Escape(args.toString()) + "</span>");
        }
    }

    public void onCtcp(Connection.User sender, String target, String cmd, String args, boolean response)
        throws Exception
    {
        if (response) {
            if (cmd.equals("PING")) {
                long diff = (new Date()).getTime() - Long.parseLong(args);
                println("<span class='msgCTCP'>" + Prefix_ + " CTCP PING reply from</span> <span class='nickname'>" + sender.getNick() + "</span><span class='msgCTCP'>: " + (((double) diff) / 1000) + " seconds</span>");
            }
        } else {
            if (cmd.equals("PING")) {
                connection_.send("NOTICE", sender.getNick(), '\u0001' + "PING " + args + '\u0001');
                println("<span class='msgCTCP'>" + Prefix_ + " CTCP PING (On) " + args + " from</span> <span class='nickname'>" + sender.getNick() + "</span>");
            } else if (cmd.equals("ACTION")) {
                boolean direct = connection_.getNick().equalsIgnoreCase(target);
                if (direct) {
                    print("<span class='marker'>&lt;</span><span class='shadow'>-</span> <span class='marker'>*</span> <span class='actnDirect'>");
                } else {
                    Connection.Channel channel = connection_.getChannel(target);
                    Modes modes = (Modes) channel.getUsers().get(sender);
                    print("<span class='marker'>" + Bullet_ + "</span> <span class='actn" + getStyle(modes) + "'>");
                }

                print(sender.getNick() + "</span>");
                if (isChannel(target) && !direct && !channel_.equalsIgnoreCase(target)) {
                    print("<span class='marker'>:</span><span class='channel'>" + html.Escape(target) + "</span>");
                }
                println(" <span class='msgAction'>" + args + "</span>");
            }
        }
    }

    public void process(String Data)
        throws Exception
    {
        if (!Data.startsWith("/")) {
            if (query_ == null) {
                if (channel_ == null) {
                    error("No current channel.");
                    return;
                } else {
                    Data = "/msg " + channel_ + " " + Data;
                }
            } else {
                Data = "/msg " + query_ + " " + Data;
            }
        }

        String cmd = Data.substring(1, (Data + " ").indexOf(' ')).toUpperCase();
        String args = null;
        if (Data.indexOf(' ') != -1)
            args = Data.substring(cmd.length() + 2);

        if (cmd.equals("MSG")) {
            int Loc = (args + " ").indexOf(' ');
            String target = args.substring(0, Loc);
            connection_.send("PRIVMSG", target, args.substring(Loc + 1));
            if (isChannel(target)) {
                print("<span class='marker'>&lt;</span><span class='privOrigin'>" + html.Escape(connection_.getNick()) + "</span>");
                if (!channel_.equalsIgnoreCase(target)) {
                    print("<span class='marker'>:</span><span class='channel'>" + target + "</span>");
                }
                print("<span class='marker'>&gt;</span> ");
            } else {
                print("<span class='shadow'>-</span><span class='marker'>&gt;</span> <span class='marker'>*</span><span class='privTarget'>" + target + "</span><span class='marker'>*</span> ");
           }
            println(colorize(args.substring(Loc + 1)));
        } else if (cmd.equals("ME")) {
            String target = null;
            if (query_ != null) {
                target = query_;
                print("<span class='shadow'>-</span><span class='marker'>&gt;</span> <span class='marker'>*</span> ");
            // XXX: This just plain sucks...
            } else if (channel_ != null) {
                target = channel_;
                print("<span class='marker'>" + Bullet_ + "</span> ");
            } else {
                error("No current target.");
                return;
            }
            println("<span class='actnOrigin'>" + connection_.getNick() + "</span> <span class='msgAction'>" + args + "</span>");
            connection_.send("PRIVMSG", target, '\u0001' + "ACTION " + args + '\u0001');
        } else if (cmd.equals("QUERY")) {
            if (args != null && isChannel(args)) {
                error("Cannot /query a channel.");
                return;
            }
            query(args);
        } else if (cmd.equals("JOIN")) {
            if (connection_.getChannels().containsKey(toLower(args)))
                redirect(args);
            connection_.send("JOIN", args);
        } else if (cmd.equals("PART")) {
            connection_.send("PART", args == null ? channel_ : args);
        } else if (cmd.equals("PING")) {
            connection_.send("PRIVMSG", args, '\u0001' + "PING " + new Date().getTime() + '\u0001');
        } else if (
            cmd.equals("WHOIS") ||
            cmd.equals("NICK") ||
            cmd.equals("NAMES") ||
            cmd.equals("WHO") ||
            cmd.equals("QUIT") ||
            cmd.equals("MOTD") ||
            cmd.equals("AWAY") ||
            cmd.equals("LIST") ||
            cmd.equals("LINKS")
        ) {
            if (args == null)
                connection_.send(cmd);
            else
                connection_.send(cmd, args);
        } else if (cmd.equals("QUOTE")) {
            connection_.send(args);
        } else {
            println("<span class='msgError'>" + Prefix_ + " Unrecognized Command: " + cmd + "</span>");
        }

        connection_.flush();
    }

    public void autoJoin(String[] channels) {
        if (channels == null)
            return;

        for (int i = 0; i < channels.length; ++i)
            if (!channels[i].trim().equals("")) {
                println(Prefix_ + " Auto-joining: <span class='channel'>" + channels[i] + "</span>");
                channel_ = channels[i];
            }

        if (channel_ != null)
            redirect(channel_);
    }

    public void onLeave(String channel) {
        if (!channel_.equalsIgnoreCase(channel))
            return;

        Map<String, Connection.Channel> channels = connection_.getChannels();
        redirect(channels.isEmpty() ? null : channels.keySet().iterator().next());
    }

    public void error(String data) {
        println("<span class='msgError'>" + Prefix_ + " Error: " + data + "</span>");
        flush();
    }
}
// }}}

}
