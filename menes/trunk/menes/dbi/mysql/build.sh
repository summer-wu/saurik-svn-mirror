if [ "${CFG_EXECUTABLE_MYSQL_CONFIG+@}" -a -z "${CFG_DEFINE__WIN32+@}" ]; then
    mak:build *

    IFS=$' \t\n'
    c++:gcc-flags hi-ccxx-flags $(mysql_config --include)
    c++:gcc-flags hi-ld-flags $(mysql_config --libs_r)
fi
