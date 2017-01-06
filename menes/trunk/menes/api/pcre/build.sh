if [ "${CFG_EXECUTABLE_PCRE_CONFIG+@}" -o "${CFG_DEFINE__WIN32+@}" ]; then
    mak:build *

    c++:gcc-flags hi-ccxx-flags -DAPI_HAS_PCRE

    if [ "${CFG_DEFINE__WIN32+@}" ]; then
        hi-ccxx-flags "-I$menes/pcre" -DPCRE_STATIC
        hi-ld-flags "-L$menes/pcre"
    else
        IFS=$' \t\n'
        c++:gcc-flags hi-ccxx-flags $(pcre-config --cflags)
    fi

    # XXX: this adds /usr/lib to -L
    # c++:gcc-flags hi-ld-flags $(pcre-config --libs)
    c++:gcc-flags hi-ld-flags -lpcre
fi
