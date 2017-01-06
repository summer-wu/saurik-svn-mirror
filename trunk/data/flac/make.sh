pkg:setup
pkg:configure --with-ogg="$(PKG_DEST_ ogg)"
make
pkg:install
