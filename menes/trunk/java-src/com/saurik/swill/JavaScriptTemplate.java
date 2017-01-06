package com.saurik.swill;

public class JavaScriptTemplate
    implements Template
{

public String getStringBlockHeader() {
    return
        "+(function(){" +
        "var _t=_o,_s=\"\";" +
        "_o=function(v){" +
            "_s+=v;" +
        "};" +
        "_o(\"";
}

public String getStringBlockFooter() {
        return "\");" +
        "_o=_t;" +
        "return _s;" +
        "})()+";
}

}
