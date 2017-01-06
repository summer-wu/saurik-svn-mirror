if [ "${CFG_TEST__R__DEV_RANDOM+@}" ]; then
    mak:build *
    hi-ccxx-flags -DAPI_HAS_DEVRANDOM
fi
