pkg:setup
autoconf
CFLAGS="-DSVN_NEON_0_25 -DSVN_NEON_0_26" pkg:configure --enable-maintainer-mode --disable-keychain --with-neon="$(PKG_DEST_ neon)/usr" --with-apr="$(PKG_WORK_ apr)/apr-1.2.12" --with-apr-util="$(PKG_WORK_ apr-util)/apr-util-1.2.12"
make
pkg:install
