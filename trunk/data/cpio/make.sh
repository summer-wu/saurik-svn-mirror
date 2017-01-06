pkg:setup
pkg:configure ac_cv_func_obstack=no
make
pkg:install
pkg: rm -f /usr/libexec/rmt
