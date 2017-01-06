if [ "${CFG_LINK_Z+@}" ]; then
    mak:build *

    hi-ccxx-flags -DAPI_HAS_ZLIB

    if [ "${CFG_DEFINE__WIN32+@}" ]; then
        hi-ld-flags "-L$menes/zlib" -lzlibwapi
    else
        hi-ld-flags -lz
    fi
fi
