pkg:setup
Xprefix="$(PKG_DEST_ openssl)/usr" pkg:configure
make
pkg:install
