if [ "${CFG_EXECUTABLE_PG_CONFIG+@}" -a -z "${CFG_DEFINE__WIN32+@}" ]; then
    mak:build *

    c++:gcc-flags hi-ccxx-flags "-I$(pg_config --includedir)"
    c++:gcc-flags hi-ld-flags "-L$(pg_config --libdir)" -lpq
fi
