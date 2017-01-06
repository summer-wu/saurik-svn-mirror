Packages.org.mozilla.javascript.Context.getCurrentContext().getWrapFactory().setJavaPrimitiveWrap(false);

function _assert(expr) {
    if (!expr)
        throw "_assert(" + expr + ")";
}

function toInteger(value) {
    return Packages.java.lang.Integer.valueOf(value);
}

function toLong(value) {
    return Packages.java.lang.Long.valueOf(value);
}
