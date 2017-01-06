pkg:setup
pkg:configure
make
make install prefix="${PKG_DEST}/usr"
pkg: ln -sf BitchX-1.1-final /usr/bin/BitchX
