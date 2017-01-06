package com.saurik;

import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;

public class url {

public static String Escape(String url) {
    try {
        return URLEncoder.encode(url, "UTF-8");
    } catch (UnsupportedEncodingException e) {
        throw new RuntimeException("how in the hell do you not have utf-8 installed?!?");
    }
}

}
