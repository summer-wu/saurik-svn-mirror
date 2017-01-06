pkg:setup
pkg:configure --with-openssl="$(PKG_DEST_ openssl)/usr" --with-sound=no
make
pkg:install
