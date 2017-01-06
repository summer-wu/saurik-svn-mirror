if [ "${CFG_INCLUDE_PTHREAD_H+@}" ]; then
    mak:build *
    hi-ccxx-flags -DAPI_HAS_PTHREADS
fi
