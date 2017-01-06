pkg:setup
autoconf
pkg:configure --with-pth-prefix="$(PKG_DEST_ gnupth)/usr"
make
pkg:install
