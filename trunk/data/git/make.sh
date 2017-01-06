pkg:setup
export PATH=/apl/tel/path:$PATH
pkg:configure ac_cv_c_c99_format=yes ac_cv_fread_reads_directories=no ac_cv_snprintf_returns_bogus=yes --without-tcltk
make
pkg:install NO_INSTALL_HARDLINKS=yes
pkg: rm -f /usr/lib/perl/5.10.1/perllocal.pod
