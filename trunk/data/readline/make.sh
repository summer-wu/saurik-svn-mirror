pkg:setup
autoconf
pkg:configure ac_cv_func_strcoll_works=yes bash_cv_func_sigsetjmp=present bash_cv_func_ctype_nonascii=no bash_cv_must_reinstall_sighandlers=no bash_cv_func_strcoll_broken=yes
make CFLAGS='-O2 -mthumb'
pkg:install
pkg: ln -s libreadline.5.2.dylib /usr/lib/libreadline.5.dylib
pkg: ln -s libreadline.6.0.dylib /usr/lib/libreadline.5.2.dylib
pkg: ln -s libhistory.5.2.dylib /usr/lib/libhistory.5.dylib
pkg: ln -s libhistory.6.0.dylib /usr/lib/libhistory.5.2.dylib
