if [ "${CFG_DEFINE__WIN32+@}" ]; then
    mak:build *

    hi-ccxx-flags -DAPI_HAS_WIN32
    hi-ld-flags -ladvapi32 -ldbghelp -lkernel32 -loleaut32 -lpsapi -lrpcrt4 -luser32

    API_PROVIDES_FIBER=
fi
