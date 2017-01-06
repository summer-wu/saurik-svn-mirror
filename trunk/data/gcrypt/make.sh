pkg:setup
pkg:autoconf
pkg:configure --with-gpg-error-prefix="$(PKG_DEST_ libgpg-error)/usr"
make
pkg:install
