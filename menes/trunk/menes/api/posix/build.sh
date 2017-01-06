if [ "${CFG_DEFINE__POSIX_VERSION+@}" ]; then
    mak:build *
    hi-ccxx-flags -DAPI_HAS_POSIX
fi
