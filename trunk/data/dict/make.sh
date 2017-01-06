pkg:setup
pkg:configure ac_cv_type_wint_t=yes ac_cv_prog_AR=$(which ${PKG_TARG}-ar)
make AR=${PKG_TARG}-ar
pkg:install
