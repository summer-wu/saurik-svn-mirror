request.setCharacterEncoding("UTF-8");

function getStringParameter(name) {
    var value = request.getParameter(name);
    return value == null || value.equals("") ? null : value;
}

function getIntegerParameter(name) {
    var value = getStringParameter(name);
    return value == null ? null : toInteger(value);
}

function getLongParameter(name) {
    var value = getStringParameter(name);
    return value == null ? null : toLong(value);
}

function getLongArrayParameter(name) {
    var value = getStringParameter(name);
    return value == null ? null : value.split(",").map(
        function (value) { return toLong(value); }
    );
}
