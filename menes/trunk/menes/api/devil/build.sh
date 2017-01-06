if [ "${CFG_LINK_IL+@}" ]; then
    mak:build *

    hi-ccxx-flags -DAPI_HAS_DEVIL
    hi-ld-flags -lIL
fi
