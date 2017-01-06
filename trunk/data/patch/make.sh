pkg:setup
mv config.sub config.sub_
cp -af /usr/share/misc/config.sub .
touch -r config.sub_ config.sub
rm -f m4/largefile.m4
touch m4/largefile.m4
rpl AM_C_ jm_C_ * -R
rpl jm_AC_ jm_ * -R
cat m4/* >aclocal.m4
autoconf
pkg:configure ac_cv_func_setmode=no
make
make install prefix="${PKG_DEST}/usr"
