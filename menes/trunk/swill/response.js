var _w = null;

var _o = function (data) {
    if (data != null)
        _w.print(data);
};

var send = function (path) {
    var file = new Packages.java.io.FileInputStream(path);

    try {
        io.splice(file, response.getOutputStream());
    } finally {
        file.close();
    }
};
