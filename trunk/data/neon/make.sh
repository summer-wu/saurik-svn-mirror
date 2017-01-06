pkg:setup
autoconf
pkg:configure --with-ssl
#--with-expat="${PKG_ROOT}/usr/lib/libexpat.la"
make
pkg:install
