pkg:setup
pkg:configure --disable-dependency-tracking --disable-largefile
make AR="${PKG_TARG}-ar" CFLAGS='-O2 -mthumb'
pkg:install
pkg:bin tar
pkg: mkdir -p /usr/bin
ln -s /bin/tar "${PKG_DEST}/usr/bin/tar"
