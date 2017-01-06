pkg:setup
autoconf
pkg:configure --with-ksba-prefix="$(PKG_DEST_ libksba)/usr" --with-libassuan-prefix="$(PKG_DEST_ libassuan)/usr" --with-pth-prefix="$(PKG_DEST_ gnupth)/usr" --sysconfdir=/etc
make
pkg:install
