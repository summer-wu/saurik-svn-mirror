if [ "${CFG_INCLUDE_DLFCN_H+@}" ]; then
    mak:build *
    hi-ccxx-flags -DAPI_HAS_LIBDL

    if [ "${CFG_LINK_DL+@}" ]; then
        hi-ld-flags -ldl
    fi
fi
