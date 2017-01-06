if [ "${CFG_INCLUDE_WINSOCK_H+@}" ]; then
    mak:build *

    hi-ccxx-flags -DAPI_HAS_WINSOCK
    hi-ld-flags -lws2_32
fi
