pkg:setup
pkg:configure --with-libgcrypt-prefix="$(PKG_DEST_ gcrypt)/usr" --with-included-libtasn1
make
pkg:install
