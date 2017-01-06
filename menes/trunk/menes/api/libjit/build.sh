if [ "${CFG_LINK_JIT+@}" ]; then
    mak:build *

    hi-ccxx-flags -DAPI_HAS_LIBJIT
    hi-ld-flags -ljit
fi
