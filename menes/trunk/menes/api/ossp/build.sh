if [ "${CFG_INCLUDE_OSSP_UUID_H+@}" ]; then
    mak:build *
    hi-ccxx-flags -DAPI_HAS_OSSP
    hi-ld-flags -lossp-uuid
fi
