if [ "${CFG_DEFINE___MACH__+@}" ]; then
    mak:build *
    hi-ccxx-flags -DAPI_HAS_MACH_O
fi
