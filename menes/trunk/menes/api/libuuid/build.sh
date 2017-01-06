if [ "${CFG_INCLUDE_UUID_UUID_H+@}" ]; then
    mak:build *
    hi-ccxx-flags -DAPI_HAS_LIBUUID

    if [ "${CFG_LINK_UUID+@}" ]; then
        hi-ld-flags -luuid
    fi
fi
