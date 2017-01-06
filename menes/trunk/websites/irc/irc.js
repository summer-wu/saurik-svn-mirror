var buffer = "";

function Socket() {
}

Socket.prototype.send = function(data) {
    socket_.send(data);
}

function send(line) {
    print(line);
}

function UserHost(userhost) {
    var at = userhost.lastIndexOf("@");
    if (at == -1)
        if (userhost.indexOf(".") != -1)
            this.host = userhost;
        else
            this.nick = userhost;
    else {
        this.host = userhost.substring(at + 1);

        var bang = userhost.indexOf("!");
        if (bang == -1)
            this.nick = userhost.substring(0, at);
        else {
            this.nick = userhost.substring(0, bang);
            this.ident = userhost.substring(bang + 1, at);
        }
    }
}

UserHost.prototype.toString = function() {
    if (!this.nick)
        return this.host;

    var value = this.nick;
    if (this.ident)
        value += "!" + this.ident;
    if (this.host)
        value += "@" + this.host;
    return value;
}

function Command(line) {
    var begin = 0;
    var end = line.indexOf(" ");

    if (line.substring(0, 1) == ":") {
        end = line.indexOf(" ", begin);

        if (end == -1) {
            this.source = new UserHost(line.substring(1));
            return;
        } else {
            this.source = new UserHost(line.substring(begin + 1, end));
            begin = end + 1;
            end = line.indexOf(" ", begin);
        }
    }

    this.args = new Array();

    if (end == -1)
        this.args.push(line);
    else {
        this.args.push(line.substring(begin, end));
        begin = end + 1;

        while (true)
            if (line.substring(begin, begin + 1) == ":") {
                this.args.push(line.substring(begin + 1));
                break;
            } else if ((end = line.indexOf(" ", begin)) != -1) {
                this.args.push(line.substring(begin, end));
                begin = end + 1;
            } else {
                this.args.push(line.substring(begin));
                break;
            }
    }
}

function process(command) {
    print(command.source.toString() + "%" + command.args);
}

var socket = new Socket();
socket.AddListener(function(data) {
    buffer += data;

    while (true) {
        var newline = buffer.indexOf("\n");
        if (newline == -1)
            return;

        line = buffer.substring(0, newline);
        buffer = buffer.substring(newline + 1);

        if (line.length == 0)
            continue;
        else if (line.substring(0, 5) == "PING ")
            socket.SendAll("PONG " + line.substring(5));
        else process(new Command(line));
    }
});

socket.Connect("irc.saurik.com", 6667);

//receive("!~saurik@saurik.com PRIVMSG #404 :hello\n:irc.saurik.com MODE #404 +o saurik\nPING :321312\n:bob!");
//receive("bob@bob.com PRIVMSG #404 :nifty ass\n:bork@saurik.com\ndsa");
