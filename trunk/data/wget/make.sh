pkg:setup
pkg:configure --with-ssl=openssl --with-libssl-prefix="$(PKG_DEST_ openssl)"
make
pkg:install
