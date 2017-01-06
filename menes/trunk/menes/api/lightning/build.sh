if [ "${CFG_INCLUDE_LIGHTNING_H+@}" ]; then
    mak:build *
    hi-ccxx-flags -DAPI_HAS_LIGHTNING
fi
