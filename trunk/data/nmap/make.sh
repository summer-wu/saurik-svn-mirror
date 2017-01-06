pkg:setup
autoconf
cd libdnet-stripped
autoconf
cd ../ncat
autoconf
cd ..
pkg:configure --without-liblua --without-python --without-zenmap --enable-static=yes --with-liblua=no
make AR="${PKG_TARG}-ar"
pkg:install
