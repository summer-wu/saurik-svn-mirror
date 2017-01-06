if [ "${CFG_DEFINE___FREEBSD__+@}" ]; then
    mak:build *
    hi-ccxx-flags -DAPI_HAS_FREEBSD
fi
