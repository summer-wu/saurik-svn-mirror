if [ "${CFG_LINK_SSL+@}" ]; then
    mak:build *

    hi-ccxx-flags -DAPI_HAS_OPENSSL
    hi-ld-flags -lssl
fi
