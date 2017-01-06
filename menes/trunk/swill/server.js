var alert = function (message) {
    Packages.java.lang.System.out.println(message);
};

var html = Packages.com.saurik.html;
var io = Packages.com.saurik.io;
var irc = Packages.com.saurik.irc;
var jetty = Packages.org.mortbay.jetty;

var serve = function (host, port, handle) {
    var handler = new jetty.Handler({
        handle: handle
    });

    var server = new jetty.Server(); {
        var connector = new jetty.bio.SocketConnector(); {
            if (host != null)
                connector.setHost(host);
            connector.setPort(port);
        }

        server.addConnector(connector);
        server.setHandler(handler);
    };

    server.start();
};
