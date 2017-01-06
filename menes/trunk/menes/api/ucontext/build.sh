if [ "${CFG_SYMBOL_MAKECONTEXT+@}" ]; then
    mak:build *
    hi-ccxx-flags -DAPI_HAS_UCONTEXT
    API_PROVIDES_FIBER=
fi
