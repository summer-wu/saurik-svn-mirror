if [ "${CFG_EXECUTABLE_APR_CONFIG+@}" ]; then
    apr_config=config
elif [ "${CFG_EXECUTABLE_APR_1_CONFIG+@}" ]; then
    apr_config=1-config
fi

if [ "${apr_config+@}" ]; then
    mak:build *

    hi-ccxx-flags -DAPI_HAS_APR

    IFS=$' \t\n'
    c++:gcc-flags hi-ccxx-flags $(apr-${apr_config} --includes) $(apu-${apr_config} --includes)

    IFS=$' \t\n'
    c++:gcc-flags hi-ld-flags $(apr-${apr_config} --link-ld --libs) $(apu-${apr_config} --link-ld --libs)
fi
