if [ "${CFG_SYMBOL___CXA_ATEXIT-@}" ]; then
    mak:build cxa-atexit.cpp
fi

if [ "${CFG_SYMBOL___DSO_HANDLE-@}" ]; then
    mak:build dso-handle.cpp
fi
