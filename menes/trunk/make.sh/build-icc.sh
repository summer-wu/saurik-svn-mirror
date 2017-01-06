if [[ ${CFG_LO_CFLAG__PTHREAD+@} ]]; then
    lo-ccxx-flags -pthread
    lo-ld-flags -pthread
fi

if [[ -z ${CFG_SYMBOL___CXA_ATEXIT+@} ]]; then
    mak:build "$menes/menes/crt/cxa-atexit.cpp"
fi

if [[ -z ${CFG_SYMBOL___DSO_HANDLE+@} ]]; then
    mak:build "$menes/menes/crt/dso-handle.cpp"
fi

#hi-ld-flags -lstdc++
#lo-ld-flags -nostdlib

CFG_AR='ar'
CFG_AR_CLASS='ar'

CFG_LD="$CFG_CXX"
CFG_LD_CLASS="$CFG_CXX_CLASS"
