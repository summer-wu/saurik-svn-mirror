pkg:setup
autoconf
pkg:configure ac_cv_func_setpgrp_void=yes rb_cv_binary_elf=no rb_cv_stack_grow_dir=-1 rb_cv_bsd_signal=yes ac_cv_type_getgroups=gid_t ac_cv_c_stack_direction=-1 ac_cv_func_memcmp_working=yes --enable-wide-getaddrinfo
make
cd ext/iconv
touch iconv.c
make LOCAL_LIBS=-liconv
cd ../..
pkg:install
pkg: rm -rf /usr/share/ri
pkg: rm -f /usr/lib/libruby-static.a
pkg: rm -f /usr/lib/libruby.1.9.1-static.a
